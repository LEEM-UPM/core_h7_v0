#include "gpio.h"
#include "main.h"
#include "stm32h7xx_hal_gpio.h"
#include "fdcan.h"
#include "ASPID_FDCAN_NETWORK.h"
#include "Code_V_1_1_2/GuidanceControl_ert_rtw/GuidanceControl.h"
#include "Code_V_1_1_2/Navigation_ert_rtw/Navigation.h"
#include "Code_V_1_1_2/StateMachine_ert_rtw/StateMachine.h"

extern void SystemClock_Config(void);
extern void PeriphCommonClock_Config(void);

/**
 * @brief  Unpacks incoming FDCAN frames from Avionics, RF, and Power boards 
 *         and feeds decoded sensor data into Navigation & State Machine input structures.
 */
void process_fdcan_frame(uint32_t identifier, const uint8_t *data, size_t len) {
    switch (identifier) {
        case ASPID_FDCAN_NETWORK_GNSS_OUT_FRAME_ID: {
            struct ASPID_FDCAN_NETWORK_gnss_out_t msg;
            if (ASPID_FDCAN_NETWORK_gnss_out_unpack(&msg, data, len) == 0) {
                // Populate Navigation GNSS inputs
                Navigation_U.NavIn.gnss.data_valid = 1;
            }
            break;
        }

        case ASPID_FDCAN_NETWORK_IMU_OUT_FRAME_ID: {
            struct ASPID_FDCAN_NETWORK_imu_out_t msg;
            if (ASPID_FDCAN_NETWORK_imu_out_unpack(&msg, data, len) == 0) {
                // Populate Navigation Low-G IMU inputs
                Navigation_U.NavIn.imu_lowg.data_valid = 1;
            }
            break;
        }

        case ASPID_FDCAN_NETWORK_IMU_HIGH_G_OUT_FRAME_ID: {
            struct ASPID_FDCAN_NETWORK_imu_high_g_out_t msg;
            if (ASPID_FDCAN_NETWORK_imu_high_g_out_unpack(&msg, data, len) == 0) {
                // Populate Navigation High-G IMU inputs
                Navigation_U.NavIn.imu_highg.data_valid = 1;
            }
            break;
        }

        case ASPID_FDCAN_NETWORK_CONFIG_OUT_FRAME_ID: {
            struct ASPID_FDCAN_NETWORK_config_out_t msg;
            if (ASPID_FDCAN_NETWORK_config_out_unpack(&msg, data, len) == 0) {
                // Populate RF Telemetry command into StateMachine & GuidanceControl
                StateMachine_U.SMIn.telemetry_cmd.cmd_valid = 1;
                GuidanceControl_U.GCIn.telemetry_cmd.cmd_valid = 1;
            }
            break;
        }

        default:
            break;
    }
}

/**
 * @brief  Executes the sequential GNC algorithm pipeline:
 *         1. Navigation -> estimates rocket state (Pos, Vel, Accel, Quat)
 *         2. State Machine -> evaluates flight state (Launch, Coast, Apogee, etc.)
 *         3. Guidance & Control -> calculates airbrake & pyro deployment commands
 */
void gnc_step_pipeline(void) {
    // 1. Step Navigation Model
    Navigation_step();

    // 2. Feed Navigation outputs into State Machine inputs
    for (int i = 0; i < 3; i++) {
        StateMachine_U.SMIn.pos_ned[i]    = Navigation_Y.NavOut.pos_ned[i];
        StateMachine_U.SMIn.vel_ned[i]    = Navigation_Y.NavOut.vel_ned[i];
        StateMachine_U.SMIn.accel_ned[i]  = Navigation_Y.NavOut.accel_ned[i];
        StateMachine_U.SMIn.accel_body[i] = Navigation_Y.NavOut.accel_body[i];
        StateMachine_U.SMIn.gyro_body[i]  = Navigation_Y.NavOut.gyro_body[i];
    }
    StateMachine_U.SMIn.gnss_age_s  = Navigation_Y.NavOut.gnss_age_s;
    StateMachine_U.SMIn.nav_valid   = Navigation_Y.NavOut.nav_valid;
    StateMachine_U.SMIn.timestamp_s = Navigation_Y.NavOut.timestamp_s;

    // 3. Step State Machine Model
    StateMachine_step();

    // 4. Feed Navigation & State Machine outputs into Guidance & Control inputs
    for (int i = 0; i < 3; i++) {
        GuidanceControl_U.GCIn.pos_ned[i]    = Navigation_Y.NavOut.pos_ned[i];
        GuidanceControl_U.GCIn.vel_ned[i]    = Navigation_Y.NavOut.vel_ned[i];
        GuidanceControl_U.GCIn.accel_ned[i]  = Navigation_Y.NavOut.accel_ned[i];
        GuidanceControl_U.GCIn.accel_body[i] = Navigation_Y.NavOut.accel_body[i];
    }
    GuidanceControl_U.GCIn.gnss_age_s   = Navigation_Y.NavOut.gnss_age_s;
    GuidanceControl_U.GCIn.nav_valid    = Navigation_Y.NavOut.nav_valid;
    GuidanceControl_U.GCIn.timestamp_s  = Navigation_Y.NavOut.timestamp_s;
    GuidanceControl_U.GCIn.rocket_state = StateMachine_Y.SMOut.rocket_state;

    // 5. Step Guidance & Control Model
    GuidanceControl_step();
}

/**
 * @brief  Packs and transmits Guidance & Control outputs over FDCAN to peripheral boards
 */
void gnc_send_outputs(void) {
    // Pack Airbrake command for Avionics (ID 0x04)
    struct ASPID_FDCAN_NETWORK_airbrake_out_t airbrake_msg;
    uint8_t airbrake_tx_buf[ASPID_FDCAN_NETWORK_AIRBRAKE_OUT_LENGTH];
    ASPID_FDCAN_NETWORK_airbrake_out_init(&airbrake_msg);
    airbrake_msg.airbrake_out_sig215 = (int32_t)GuidanceControl_Y.GCOut.airbrake_deg;
    ASPID_FDCAN_NETWORK_airbrake_out_pack(airbrake_tx_buf, &airbrake_msg, sizeof(airbrake_tx_buf));

    // Pack Pyro / Recovery deployment flags for Power Board (ID 0x06)
    struct ASPID_FDCAN_NETWORK_pyro_out_t pyro_msg;
    uint8_t pyro_tx_buf[ASPID_FDCAN_NETWORK_PYRO_OUT_LENGTH];
    ASPID_FDCAN_NETWORK_pyro_out_init(&pyro_msg);
    pyro_msg.pyro_out_sig75 = GuidanceControl_Y.GCOut.drogue_deploy_flag | (GuidanceControl_Y.GCOut.main_deploy_flag << 1);
    ASPID_FDCAN_NETWORK_pyro_out_pack(pyro_tx_buf, &pyro_msg, sizeof(pyro_tx_buf));
}

int main(void) {
    HAL_Init();
    SystemClock_Config();
    PeriphCommonClock_Config();

    MX_GPIO_Init();
    MX_FDCAN1_Init();
    MX_FDCAN2_Init();

    // Initialize Simulink Models
    Navigation_initialize();
    StateMachine_initialize();
    GuidanceControl_initialize();

    while (1) {
        // 1. Process FDCAN received frames (when frames arrive)
        // process_fdcan_frame(rx_header.Identifier, rx_data, rx_header.DataLength);

        // 2. Execute GNC pipeline
        gnc_step_pipeline();

        // 3. Send Output Commands
        gnc_send_outputs();
    }
}