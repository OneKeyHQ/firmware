static void erase_storage_code_progress(void) {
  flash_enter();
  // erase storage area
  for (int i = FLASH_STORAGE_SECTOR_FIRST; i <= FLASH_STORAGE_SECTOR_LAST;
       i++) {
#if ONEKEY_MINI
    layoutProgress("WIPING ... Please\nwait",
#else
    layoutProgress("WIPING ... Please wait",
#endif
                   1000 * (i - FLASH_STORAGE_SECTOR_FIRST) /
                       (FLASH_CODE_SECTOR_LAST - FLASH_STORAGE_SECTOR_FIRST));
    flash_erase_sector(i, FLASH_CR_PROGRAM_X32);
  }
  // erase code area
  for (int i = FLASH_CODE_SECTOR_FIRST; i <= FLASH_CODE_SECTOR_LAST; i++) {
#if ONEKEY_MINI
    layoutProgress("WIPING ... Please\nwait",
#else
    layoutProgress("WIPING ... Please wait",
#endif
                   1000 * (i - FLASH_STORAGE_SECTOR_FIRST) /
                       (FLASH_CODE_SECTOR_LAST - FLASH_STORAGE_SECTOR_FIRST));
    flash_erase_sector(i, FLASH_CR_PROGRAM_X32);
  }
  flash_exit();
}

static void erase_code_progress(void) {
  flash_enter();
  for (int i = FLASH_CODE_SECTOR_FIRST; i <= FLASH_CODE_SECTOR_LAST; i++) {
#if ONEKEY_MINI
    layoutProgress("PREPARING ... Please\nwait",
#else
    layoutProgress("PREPARING ... Please wait",
#endif
                   1000 * (i - FLASH_CODE_SECTOR_FIRST) /
                       (FLASH_CODE_SECTOR_LAST - FLASH_CODE_SECTOR_FIRST));
    flash_erase_sector(i, FLASH_CR_PROGRAM_X32);
  }
#if ONEKEY_MINI
  layoutProgress("INSTALLING ... Please\nwait", 0);
#else
  layoutProgress("INSTALLING ... Please wait", 0);
#endif
  flash_exit();
}

#if ONEKEY_MINI
static void erase_code_progress_ex(void) {
  for (int i = 0; i < SPI_FLASH_FIRMWARE_BLOCKS; i++) {
    layoutProgress("PREPARING ... Please\nwait",
                   1000 * i / SPI_FLASH_FIRMWARE_BLOCKS);
    w25qxx_erase_block((SPI_FLASH_FIRMWARE_ADDR_START / SPI_FLASH_BLOCK_SIZE) +
                       i);
  }
  layoutProgress("DOWNLOADING ... Please\nwait", 0);
}
#endif

#if !ONEKEY_MINI
static void erase_ble_code_progress(void) {
  flash_enter();
  for (int i = FLASH_BLE_SECTOR_FIRST; i <= FLASH_BLE_SECTOR_LAST; i++) {
    layoutProgress("PREPARING ... Please wait",
                   1000 * (i - FLASH_CODE_SECTOR_FIRST) /
                       (FLASH_CODE_SECTOR_LAST - FLASH_CODE_SECTOR_FIRST));
    flash_erase_sector(i, FLASH_CR_PROGRAM_X32);
  }
  layoutProgress("INSTALLING ... Please wait", 0);
  flash_exit();
}
#endif

#if !ONEKEY_MINI
static void erase_storage(void) {
  flash_enter();
  for (int i = FLASH_STORAGE_SECTOR_FIRST; i <= FLASH_STORAGE_SECTOR_LAST;
       i++) {
    flash_erase_sector(i, FLASH_CR_PROGRAM_X32);
  }
  flash_exit();
}
#endif
