#include STM32_HAL_H
#include "sdram.h"

static SDRAM_HandleTypeDef hsdram[1];
static FMC_SDRAM_CommandTypeDef Command;

#define REFRESH_COUNT ((uint32_t)0x0603) /* (100Mhz clock) */
#define FMC_SDRAM_TIMEOUT ((uint32_t)0xFFFF)

static void sdram_delay(uint32_t delay) {
  uint32_t tickstart;
  tickstart = HAL_GetTick();
  while ((HAL_GetTick() - tickstart) < delay) {
  }
}

static int sdram_init_sequence(void) {
  /* SDRAM initialization sequence */
  /* Step 1: Configure a clock configuration enable command */
  Command.CommandMode = FMC_SDRAM_DEVICE_CLK_ENABLE_CMD;
  Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
  Command.AutoRefreshNumber = 1;
  Command.ModeRegisterDefinition = 0;

  /* Send the command */
  if (HAL_SDRAM_SendCommand(&hsdram[0], &Command, FMC_SDRAM_TIMEOUT) !=
      HAL_OK) {
    return HAL_ERROR;
  }

  /* Step 2: Insert 100 us minimum delay */
  /* Inserted delay is equal to 1 ms due to systick time base unit (ms) */
  sdram_delay(1);

  /* Step 3: Configure a PALL (precharge all) command */
  Command.CommandMode = FMC_SDRAM_DEVICE_PALL_CMD;
  Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
  Command.AutoRefreshNumber = 1;
  Command.ModeRegisterDefinition = 0;

  /* Send the command */
  if (HAL_SDRAM_SendCommand(&hsdram[0], &Command, FMC_SDRAM_TIMEOUT) !=
      HAL_OK) {
    return HAL_ERROR;
  }

  /* Step 4: Configure a Refresh command */
  Command.CommandMode = FMC_SDRAM_DEVICE_AUTOREFRESH_MODE_CMD;
  Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
  Command.AutoRefreshNumber = 2;
  Command.ModeRegisterDefinition = 0;

  /* Send the command */
  if (HAL_SDRAM_SendCommand(&hsdram[0], &Command, FMC_SDRAM_TIMEOUT) !=
      HAL_OK) {
    return HAL_ERROR;
  }

  /* Step 5: Program the external memory mode register */
  uint32_t tmpmrd;

  /* Program the external memory mode register */
  tmpmrd = (uint32_t)FMC_SDRAM_DEVICE_BURST_LENGTH_1 |
           FMC_SDRAM_DEVICE_BURST_TYPE_SEQUENTIAL |
           FMC_SDRAM_DEVICE_CAS_LATENCY_2 |
           FMC_SDRAM_DEVICE_OPERATING_MODE_STANDARD |
           FMC_SDRAM_DEVICE_WRITEBURST_MODE_SINGLE;

  Command.CommandMode = FMC_SDRAM_DEVICE_LOAD_MODE_CMD;
  Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
  Command.AutoRefreshNumber = 1;
  Command.ModeRegisterDefinition = tmpmrd;

  /* Send the command */
  if (HAL_SDRAM_SendCommand(&hsdram[0], &Command, FMC_SDRAM_TIMEOUT) !=
      HAL_OK) {
    return HAL_ERROR;
  }
  /* Step 6: Set the refresh rate counter */
  if (HAL_SDRAM_ProgramRefreshRate(&hsdram[0], REFRESH_COUNT) != HAL_OK) {
    return HAL_ERROR;
  }
  return HAL_OK;
}

int sdram_init(void) {
  GPIO_InitTypeDef gpio_init_structure;

  /* Enable FMC clock */
  __HAL_RCC_FMC_CLK_ENABLE();

  /* Enable GPIOs clock */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();

  /* Common GPIO configuration */
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull = GPIO_PULLUP;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init_structure.Alternate = GPIO_AF12_FMC;

  /* GPIOD configuration */
  gpio_init_structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 |
                            GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;

  HAL_GPIO_Init(GPIOD, &gpio_init_structure);

  /* GPIOE configuration */
  gpio_init_structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_8 |
                            GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 |
                            GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |
                            GPIO_PIN_15;

  HAL_GPIO_Init(GPIOE, &gpio_init_structure);
  /* GPIOF configuration */
  gpio_init_structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                            GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_11 |
                            GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |
                            GPIO_PIN_15;

  HAL_GPIO_Init(GPIOF, &gpio_init_structure);
  /* GPIOG configuration */
  gpio_init_structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 |
                            GPIO_PIN_2 /*| GPIO_PIN_3 */ | GPIO_PIN_4 |
                            GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOG, &gpio_init_structure);

  /* GPIOH configuration */
  gpio_init_structure.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 |
                            GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 |
                            GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |
                            GPIO_PIN_15;

  HAL_GPIO_Init(GPIOH, &gpio_init_structure);

  /* GPIOI configuration */
  gpio_init_structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                            GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 |
                            GPIO_PIN_9 | GPIO_PIN_10;

  HAL_GPIO_Init(GPIOI, &gpio_init_structure);

  FMC_SDRAM_TimingTypeDef sdram_timing;

  /* SDRAM device configuration */
  hsdram[0].Instance = FMC_SDRAM_DEVICE;

  /* SDRAM handle configuration */
  hsdram[0].Init.SDBank = FMC_SDRAM_BANK2;
  hsdram[0].Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;
  hsdram[0].Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_12;
  hsdram[0].Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_32;
  hsdram[0].Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram[0].Init.CASLatency = FMC_SDRAM_CAS_LATENCY_2;
  hsdram[0].Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram[0].Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;
  hsdram[0].Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
  hsdram[0].Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;

  /* Timing configuration for 100Mhz as SDRAM clock frequency (System clock is
   * up to 200Mhz) */
  sdram_timing.LoadToActiveDelay = 2;
  sdram_timing.ExitSelfRefreshDelay = 6;
  sdram_timing.SelfRefreshTime = 4;
  sdram_timing.RowCycleDelay = 6;
  sdram_timing.WriteRecoveryTime = 2;
  sdram_timing.RPDelay = 2;
  sdram_timing.RCDDelay = 2;

  /* SDRAM controller initialization */
  if (HAL_SDRAM_Init(&hsdram[0], &sdram_timing) != HAL_OK) {
    return HAL_ERROR;
  }

  sdram_init_sequence();

  return HAL_OK;
}
