# ASPID GNC — Estados, fases, telemetría y variables configurables

Referencia del código embebido generado para el STM32H7. Extraído del código
generado, no de la documentación del modelo.

---

## 1. Modos de funcionamiento — `SMOut.rocket_state` (`int32_T`)

| Valor | Modo | Qué significa |
|---:|---|---|
| `0` | `POWER_ON` | Recién encendido. El EKF se está alineando en la guía. Hace de standby: no hay un modo aparte. |
| `1` | `MAINTENANCE` | Banco de pruebas. El aerofreno se comanda a mano desde tierra. La pirotecnia queda inhibida. |
| `2` | `READY` | Armado en la guía, esperando lanzamiento. Detección de despegue activa. |
| `3` | `AIRBORNE` | Lanzamiento detectado. Estado terminal: no se sale nunca. |
| `4` | `ERROR` | Fallo detectado en tierra. |

### Transiciones

```
POWER_ON ──ENTER_MAINT──> MAINTENANCE ──EXIT_MAINT──> POWER_ON
POWER_ON ──ARM──> READY ──DISARM──> POWER_ON
READY    ──|f_b| > umbral durante 60 ms──> AIRBORNE
en tierra ──fallo o FORCE_ERROR──> ERROR ──CLEAR_ERROR──> POWER_ON
```

Dos comportamientos que conviene conocer:

- **`AIRBORNE` es terminal.** En vuelo nunca se cae a `ERROR`: si aparece un
  fallo solo se registra en `error_code` y el modo sigue siendo `AIRBORNE`.
- **La detección de lanzamiento también actúa desde `POWER_ON` y `ERROR`**,
  para que un cohete que salga de la guía sin haber llegado a `READY` vuele
  igualmente con el GNC. No actúa en `MAINTENANCE`.

---

## 2. Fases de vuelo — `GCOut.flight_phase` (`int32_T`)

Solo avanzan cuando `rocket_state == AIRBORNE`.

| Valor | Fase | Aerofreno | Pirotecnia |
|---:|---|---|---|
| `0` | `IDLE` | cerrado | inhibida |
| `1` | `BOOST` | bloqueado a 0 | inhibida |
| `2` | `COAST_UNCTRL` | cerrado | inhibida |
| `3` | `COAST_CTRL` | controlado 0..1 | inhibida |
| `4` | `APOGEE` | cerrado | esperando |
| `5` | `DROGUE` | cerrado | drogue disparado |
| `6` | `MAIN` | cerrado | main disparado |
| `7` | `LANDED` | cerrado | — |

### Condiciones de paso

| Transición | Condición |
|---|---|
| `IDLE → BOOST` | entrada en `AIRBORNE` |
| `BOOST → COAST_UNCTRL` | deceleración a lo largo de la velocidad > 2 m/s² durante 100 ms, y ≥1 s en BOOST |
| `COAST_UNCTRL → COAST_CTRL` | ≥1 s en la fase **y** altura ≥ `min_ctrl_alt_agl_m` **y** Mach ≤ 1,05 **y** vz ≥ 30 m/s |
| `COAST_CTRL → APOGEE` | vz ≤ 30 m/s |
| `APOGEE → DROGUE` | apogeo detectado + `drogue_delay_s` |
| `DROGUE → MAIN` | altura ≤ `main_deploy_alt_agl_m` y bajando (respaldo: 120 s tras el drogue) |
| `MAIN → LANDED` | \|v\| ≤ 2 m/s y altura ≤ 30 m durante 5 s |

El burnout no se detecta por aceleración baja: a Mach 1 el arrastre solo ya da
2-3 g. Se detecta por el signo de la aceleración proyectada sobre la velocidad.

---

## 3. Comandos de telemetría — `Bus_TelemetryCmd`

```c
typedef struct {
  uint8_T  cmd_id;
  real32_T cmd_param;
  uint8_T  cmd_valid;
} Bus_TelemetryCmd;
```

Entra en `StateMachine_U.SMIn.telemetry_cmd` y en
`GuidanceControl_U.GCIn.telemetry_cmd`.

| `cmd_id` | Comando | `cmd_param` | Efecto |
|---:|---|---|---|
| `0` | `NONE` | — | nada |
| `1` | `ENTER_MAINT` | — | POWER_ON → MAINTENANCE |
| `2` | `EXIT_MAINT` | — | MAINTENANCE → POWER_ON |
| `3` | `ARM` | — | POWER_ON → READY |
| `4` | `DISARM` | — | READY → POWER_ON |
| `5` | `CLEAR_ERROR` | — | ERROR → POWER_ON |
| `6` | `SET_AIRBRAKE` | `0.0`–`1.0` | mueve el aerofreno, solo en MAINTENANCE |
| `255` | `FORCE_ERROR` | — | fuerza ERROR, solo en tierra |

### Cómo enviarlos

```c
StateMachine_U.SMIn.telemetry_cmd.cmd_id    = 3;      /* ARM */
StateMachine_U.SMIn.telemetry_cmd.cmd_param = 0.0F;
StateMachine_U.SMIn.telemetry_cmd.cmd_valid = 1;

StateMachine_step();

StateMachine_U.SMIn.telemetry_cmd.cmd_valid = 0;      /* un solo ciclo */
```

`cmd_valid` tiene que ser un pulso de exactamente un ciclo. Si se queda a 1,
el comando se re-ejecuta a 100 Hz.

---

## 4. Variables configurables por el usuario

Son las únicas que salen como estructura global escribible. Se pueden cargar
de flash al arrancar o actualizar por telemetría sin regenerar código ni
reflashear. Todo lo demás está compilado como constante.

```c
/* Navigation.h */
extern Bus_NavTunables nav_tun;
typedef struct { real32_T init_euler_rad[3]; } Bus_NavTunables;

/* StateMachine.h */
extern Bus_SMTunables sm_tun;
typedef struct { real32_T launch_accel_g_thr; } Bus_SMTunables;

/* GuidanceControl.h */
extern Bus_GCTunables gc_tun;
typedef struct {
  real32_T apogee_target_agl_m;
  real32_T min_ctrl_alt_agl_m;
  real32_T main_deploy_alt_agl_m;
  real32_T drogue_delay_s;
} Bus_GCTunables;
```

| Variable | Por defecto | Unidad | Qué hace |
|---|---:|---|---|
| `nav_tun.init_euler_rad[0]` | `0.0` | rad | Roll de la guía de lanzamiento |
| `nav_tun.init_euler_rad[1]` | `1.4748` | rad | Elevación de la guía (84,5°) |
| `nav_tun.init_euler_rad[2]` | `0.0` | rad | Azimut de la guía, con signo negativo |
| `sm_tun.launch_accel_g_thr` | `3.0` | g | Fuerza específica que dispara AIRBORNE |
| `gc_tun.apogee_target_agl_m` | `3600.0` | m AGL | Apogeo objetivo del aerofreno |
| `gc_tun.min_ctrl_alt_agl_m` | `1200.0` | m AGL | Altura mínima para empezar a controlar |
| `gc_tun.main_deploy_alt_agl_m` | `300.0` | m AGL | Altura de apertura del main |
| `gc_tun.drogue_delay_s` | `1.0` | s | Retardo del drogue tras el apogeo |

No hay validación en tiempo de ejecución: quien escriba estas estructuras es
responsable de la coherencia de los valores.
