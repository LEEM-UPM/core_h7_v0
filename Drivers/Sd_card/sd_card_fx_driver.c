#include "sd_card_fx_driver.h"

/* De FileX (fx_partition_offset_calculate.c), sin prototipo público */
UINT _fx_partition_offset_calculate(void *partition_sector, UINT partition,
                                    ULONG *partition_start,
                                    ULONG *partition_size);

static UINT _status(bool ok) { return ok ? FX_SUCCESS : FX_IO_ERROR; }

VOID sd_card_fx_driver(FX_MEDIA *media_ptr) {
  const sd_card_id_t id =
      (sd_card_id_t)(uintptr_t)media_ptr->fx_media_driver_info;
  UCHAR *buffer = media_ptr->fx_media_driver_buffer;

  switch (media_ptr->fx_media_driver_request) {
  case FX_DRIVER_INIT:
    /* La tarjeta la inicializa storage.c con sd_card_init() antes de abrir */
    media_ptr->fx_media_driver_status = _status(sd_card_is_ready(id));
    break;

  case FX_DRIVER_READ:
    media_ptr->fx_media_driver_status = _status(sd_card_read_blocks(
        id, buffer,
        media_ptr->fx_media_driver_logical_sector +
            media_ptr->fx_media_hidden_sectors,
        media_ptr->fx_media_driver_sectors));
    break;

  case FX_DRIVER_WRITE:
    media_ptr->fx_media_driver_status = _status(sd_card_write_blocks(
        id, buffer,
        media_ptr->fx_media_driver_logical_sector +
            media_ptr->fx_media_hidden_sectors,
        media_ptr->fx_media_driver_sectors));
    break;

  case FX_DRIVER_BOOT_READ: {
    /* El sector 0 puede ser el boot record (tarjeta sin particiones) o un MBR
     * (lo normal en tarjetas formateadas en el PC): entonces se lee el boot
     * record del inicio de la primera partición. */
    if (!sd_card_read_blocks(id, buffer, 0U, 1U)) {
      media_ptr->fx_media_driver_status = FX_IO_ERROR;
      break;
    }

    ULONG partition_start = 0U;
    ULONG partition_size = 0U;
    if (_fx_partition_offset_calculate(buffer, 0U, &partition_start,
                                       &partition_size) != FX_SUCCESS) {
      media_ptr->fx_media_driver_status = FX_IO_ERROR;
      break;
    }

    if ((partition_start != 0U) &&
        !sd_card_read_blocks(id, buffer, partition_start, 1U)) {
      media_ptr->fx_media_driver_status = FX_IO_ERROR;
      break;
    }
    media_ptr->fx_media_driver_status = FX_SUCCESS;
    break;
  }

  case FX_DRIVER_BOOT_WRITE:
    /* El boot record está al inicio de la partición, no en el MBR */
    media_ptr->fx_media_driver_status = _status(sd_card_write_blocks(
        id, buffer, media_ptr->fx_media_hidden_sectors, 1U));
    break;

  case FX_DRIVER_UNINIT:
  case FX_DRIVER_FLUSH:
  case FX_DRIVER_ABORT:
  case FX_DRIVER_RELEASE_SECTORS:
    /* No hay caché propia en el driver */
    media_ptr->fx_media_driver_status = FX_SUCCESS;
    break;

  default:
    media_ptr->fx_media_driver_status = FX_IO_ERROR;
    break;
  }
}
