  .syntax unified

  .text

  .global default_handler
  .type default_handler, STT_FUNC
default_handler:
  b shutdown_privileged

  .macro add_handler symbol_name:req
    .word \symbol_name
    .weak \symbol_name
    .thumb_set \symbol_name, default_handler
  .endm

  .section .vector_table_ex, "a"
vector_table_ex:
  .word main_stack_base // defined in linker script
  add_handler reset_handler
  add_handler NMI_Handler
  add_handler HardFault_Handler
  add_handler MemManage_Handler
  add_handler BusFault_Handler
  add_handler UsageFault_Handler
  add_handler architecture_reserved_handler
  add_handler architecture_reserved_handler
  add_handler architecture_reserved_handler
  add_handler architecture_reserved_handler
  add_handler SVC_Handler
  add_handler DebugMon_Handler
  add_handler architecture_reserved_handler
  add_handler PendSV_Handler
  add_handler SysTick_Handler
  add_handler WWDG_IRQHandler
  add_handler PVD_AVD_IRQHandler                /* PVD/AVD through EXTI Line detection */
  add_handler TAMP_STAMP_IRQHandler             /* Tamper and TimeStamps through the EXTI line */
  add_handler RTC_WKUP_IRQHandler               /* RTC Wakeup through the EXTI line */
  add_handler FLASH_IRQHandler                  /* FLASH                        */
  add_handler RCC_IRQHandler                    /* RCC                          */
  add_handler EXTI0_IRQHandler                  /* EXTI Line0                   */
  add_handler EXTI1_IRQHandler                  /* EXTI Line1                   */
  add_handler EXTI2_IRQHandler                  /* EXTI Line2                   */
  add_handler EXTI3_IRQHandler                  /* EXTI Line3                   */
  add_handler EXTI4_IRQHandler                  /* EXTI Line4                   */
  add_handler DMA1_Stream0_IRQHandler           /* DMA1 Stream 0                */
  add_handler DMA1_Stream1_IRQHandler           /* DMA1 Stream 1                */
  add_handler DMA1_Stream2_IRQHandler           /* DMA1 Stream 2                */
  add_handler DMA1_Stream3_IRQHandler           /* DMA1 Stream 3                */
  add_handler DMA1_Stream4_IRQHandler           /* DMA1 Stream 4                */
  add_handler DMA1_Stream5_IRQHandler           /* DMA1 Stream 5                */
  add_handler DMA1_Stream6_IRQHandler           /* DMA1 Stream 6                */
  add_handler ADC_IRQHandler                    /* ADC1, ADC2 and ADC3s         */
  add_handler FDCAN1_IT0_IRQHandler             /* FDCAN1 interrupt line 0      */
  add_handler FDCAN2_IT0_IRQHandler             /* FDCAN2 interrupt line 0      */
  add_handler FDCAN1_IT1_IRQHandler             /* FDCAN1 interrupt line 1      */
  add_handler FDCAN2_IT1_IRQHandler             /* FDCAN2 interrupt line 1      */
  add_handler EXTI9_5_IRQHandler                /* External Line[9:5]s          */
  add_handler TIM1_BRK_IRQHandler               /* TIM1 Break interrupt         */
  add_handler TIM1_UP_IRQHandler                /* TIM1 Update interrupt        */
  add_handler TIM1_TRG_COM_IRQHandler           /* TIM1 Trigger and Commutation interrupt */
  add_handler TIM1_CC_IRQHandler                /* TIM1 Capture Compare         */
  add_handler TIM2_IRQHandler                   /* TIM2                         */
  add_handler TIM3_IRQHandler                   /* TIM3                         */
  add_handler TIM4_IRQHandler                   /* TIM4                         */

  // Reference:
  // Table 147 - STM32H757 Reference manual (RM0399)
  // Section B1.5 - ARMv7-M Architecture Reference Manual
  .section .vector_table, "a"
vector_table:
  .word main_stack_base // defined in linker script
  add_handler reset_handler
  add_handler NMI_Handler
  add_handler HardFault_Handler
  add_handler MemManage_Handler
  add_handler BusFault_Handler
  add_handler UsageFault_Handler
  add_handler architecture_reserved_handler
  add_handler architecture_reserved_handler
  add_handler architecture_reserved_handler
  add_handler architecture_reserved_handler
  add_handler SVC_Handler
  add_handler DebugMon_Handler
  add_handler architecture_reserved_handler
  add_handler PendSV_Handler
  add_handler SysTick_Handler
  add_handler WWDG_IRQHandler
  add_handler PVD_AVD_IRQHandler                /* PVD/AVD through EXTI Line detection */
  add_handler TAMP_STAMP_IRQHandler             /* Tamper and TimeStamps through the EXTI line */
  add_handler RTC_WKUP_IRQHandler               /* RTC Wakeup through the EXTI line */
  add_handler FLASH_IRQHandler                  /* FLASH                        */
  add_handler RCC_IRQHandler                    /* RCC                          */
  add_handler EXTI0_IRQHandler                  /* EXTI Line0                   */
  add_handler EXTI1_IRQHandler                  /* EXTI Line1                   */
  add_handler EXTI2_IRQHandler                  /* EXTI Line2                   */
  add_handler EXTI3_IRQHandler                  /* EXTI Line3                   */
  add_handler EXTI4_IRQHandler                  /* EXTI Line4                   */
  add_handler DMA1_Stream0_IRQHandler           /* DMA1 Stream 0                */
  add_handler DMA1_Stream1_IRQHandler           /* DMA1 Stream 1                */
  add_handler DMA1_Stream2_IRQHandler           /* DMA1 Stream 2                */
  add_handler DMA1_Stream3_IRQHandler           /* DMA1 Stream 3                */
  add_handler DMA1_Stream4_IRQHandler           /* DMA1 Stream 4                */
  add_handler DMA1_Stream5_IRQHandler           /* DMA1 Stream 5                */
  add_handler DMA1_Stream6_IRQHandler           /* DMA1 Stream 6                */
  add_handler ADC_IRQHandler                    /* ADC1, ADC2 and ADC3s         */
  add_handler FDCAN1_IT0_IRQHandler             /* FDCAN1 interrupt line 0      */
  add_handler FDCAN2_IT0_IRQHandler             /* FDCAN2 interrupt line 0      */
  add_handler FDCAN1_IT1_IRQHandler             /* FDCAN1 interrupt line 1      */
  add_handler FDCAN2_IT1_IRQHandler             /* FDCAN2 interrupt line 1      */
  add_handler EXTI9_5_IRQHandler                /* External Line[9:5]s          */
  add_handler TIM1_BRK_IRQHandler               /* TIM1 Break interrupt         */
  add_handler TIM1_UP_IRQHandler                /* TIM1 Update interrupt        */
  add_handler TIM1_TRG_COM_IRQHandler           /* TIM1 Trigger and Commutation interrupt */
  add_handler TIM1_CC_IRQHandler                /* TIM1 Capture Compare         */
  add_handler TIM2_IRQHandler                   /* TIM2                         */
  add_handler TIM3_IRQHandler                   /* TIM3                         */
  add_handler TIM4_IRQHandler                   /* TIM4                         */
  add_handler I2C1_EV_IRQHandler                /* I2C1 Event                   */
  add_handler I2C1_ER_IRQHandler                /* I2C1 Error                   */
  add_handler I2C2_EV_IRQHandler                /* I2C2 Event                   */
  add_handler I2C2_ER_IRQHandler                /* I2C2 Error                   */
  add_handler SPI1_IRQHandler                   /* SPI1                         */
  add_handler SPI2_IRQHandler                   /* SPI2                         */
  add_handler USART1_IRQHandler                 /* USART1                       */
  add_handler USART2_IRQHandler                 /* USART2                       */
  add_handler USART3_IRQHandler                 /* USART3                       */
  add_handler EXTI15_10_IRQHandler              /* External Line[15:10]s        */
  add_handler RTC_Alarm_IRQHandler              /* RTC Alarm (A and B) through EXTI Line */
  add_handler architecture_reserved_handler                                 /* Reserved                     */
  add_handler TIM8_BRK_TIM12_IRQHandler         /* TIM8 Break and TIM12         */
  add_handler TIM8_UP_TIM13_IRQHandler          /* TIM8 Update and TIM13        */
  add_handler TIM8_TRG_COM_TIM14_IRQHandler     /* TIM8 Trigger and Commutation and TIM14 */
  add_handler TIM8_CC_IRQHandler                /* TIM8 Capture Compare         */
  add_handler DMA1_Stream7_IRQHandler           /* DMA1 Stream7                 */
  add_handler FMC_IRQHandler                    /* FMC                          */
  add_handler SDMMC1_IRQHandler                 /* SDMMC1                       */
  add_handler TIM5_IRQHandler                   /* TIM5                         */
  add_handler SPI3_IRQHandler                   /* SPI3                         */
  add_handler UART4_IRQHandler                  /* UART4                        */
  add_handler UART5_IRQHandler                  /* UART5                        */
  add_handler TIM6_DAC_IRQHandler               /* TIM6 and DAC1&2 underrun errors */
  add_handler TIM7_IRQHandler                   /* TIM7                         */
  add_handler DMA2_Stream0_IRQHandler           /* DMA2 Stream 0                */
  add_handler DMA2_Stream1_IRQHandler           /* DMA2 Stream 1                */
  add_handler DMA2_Stream2_IRQHandler           /* DMA2 Stream 2                */
  add_handler DMA2_Stream3_IRQHandler           /* DMA2 Stream 3                */
  add_handler DMA2_Stream4_IRQHandler           /* DMA2 Stream 4                */
  add_handler ETH_IRQHandler                    /* Ethernet                     */
  add_handler ETH_WKUP_IRQHandler               /* Ethernet Wakeup through EXTI line */
  add_handler FDCAN_CAL_IRQHandler              /* FDCAN calibration unit interrupt  */
  add_handler CM7_SEV_IRQHandler                /* CM7 Send event interrupt for CM4  */
  add_handler CM4_SEV_IRQHandler                /* CM4 Send event interrupt for CM7  */
  add_handler architecture_reserved_handler                                 /* Reserved                     */
  add_handler architecture_reserved_handler                                 /* Reserved                     */
  add_handler DMA2_Stream5_IRQHandler           /* DMA2 Stream 5                */
  add_handler DMA2_Stream6_IRQHandler           /* DMA2 Stream 6                */
  add_handler DMA2_Stream7_IRQHandler           /* DMA2 Stream 7                */
  add_handler USART6_IRQHandler                 /* USART6                       */
  add_handler I2C3_EV_IRQHandler                /* I2C3 event                   */
  add_handler I2C3_ER_IRQHandler                /* I2C3 error                   */
  add_handler OTG_HS_EP1_OUT_IRQHandler         /* USB OTG HS End Point 1 Out   */
  add_handler OTG_HS_EP1_IN_IRQHandler          /* USB OTG HS End Point 1 In    */
  add_handler OTG_HS_WKUP_IRQHandler            /* USB OTG HS Wakeup through EXTI */
  add_handler OTG_HS_IRQHandler                 /* USB OTG HS                   */
  add_handler DCMI_IRQHandler                   /* DCMI                         */
  add_handler CRYP_IRQHandler                   /* CRYP crypto                  */
  add_handler HASH_RNG_IRQHandler               /* Hash and Rng                 */
  add_handler FPU_IRQHandler                    /* FPU                          */
  add_handler UART7_IRQHandler                  /* UART7                        */
  add_handler UART8_IRQHandler                  /* UART8                        */
  add_handler SPI4_IRQHandler                   /* SPI4                         */
  add_handler SPI5_IRQHandler                   /* SPI5                         */
  add_handler SPI6_IRQHandler                   /* SPI6                         */
  add_handler SAI1_IRQHandler                   /* SAI1                         */
  add_handler LTDC_IRQHandler                   /* LTDC                         */
  add_handler LTDC_ER_IRQHandler                /* LTDC error                   */
  add_handler DMA2D_IRQHandler                  /* DMA2D                        */
  add_handler SAI2_IRQHandler                   /* SAI2                         */
  add_handler QUADSPI_IRQHandler                /* QUADSPI                      */
  add_handler LPTIM1_IRQHandler                 /* LPTIM1                       */
  add_handler CEC_IRQHandler                    /* HDMI_CEC                     */
  add_handler I2C4_EV_IRQHandler                /* I2C4 Event                   */
  add_handler I2C4_ER_IRQHandler                /* I2C4 Error                   */
  add_handler SPDIF_RX_IRQHandler               /* SPDIF_RX                     */
  add_handler OTG_FS_EP1_OUT_IRQHandler         /* USB OTG FS End Point 1 Out   */
  add_handler OTG_FS_EP1_IN_IRQHandler          /* USB OTG FS End Point 1 In    */
  add_handler OTG_FS_WKUP_IRQHandler            /* USB OTG FS Wakeup through EXTI */
  add_handler OTG_FS_IRQHandler                 /* USB OTG FS                   */
  add_handler DMAMUX1_OVR_IRQHandler            /* DMAMUX1 Overrun interrupt    */
  add_handler HRTIM1_Master_IRQHandler          /* HRTIM Master Timer global Interrupt */
  add_handler HRTIM1_TIMA_IRQHandler            /* HRTIM Timer A global Interrupt */
  add_handler HRTIM1_TIMB_IRQHandler            /* HRTIM Timer B global Interrupt */
  add_handler HRTIM1_TIMC_IRQHandler            /* HRTIM Timer C global Interrupt */
  add_handler HRTIM1_TIMD_IRQHandler            /* HRTIM Timer D global Interrupt */
  add_handler HRTIM1_TIME_IRQHandler            /* HRTIM Timer E global Interrupt */
  add_handler HRTIM1_FLT_IRQHandler             /* HRTIM Fault global Interrupt   */
  add_handler DFSDM1_FLT0_IRQHandler            /* DFSDM Filter0 Interrupt        */
  add_handler DFSDM1_FLT1_IRQHandler            /* DFSDM Filter1 Interrupt        */
  add_handler DFSDM1_FLT2_IRQHandler            /* DFSDM Filter2 Interrupt        */
  add_handler DFSDM1_FLT3_IRQHandler            /* DFSDM Filter3 Interrupt        */
  add_handler SAI3_IRQHandler                   /* SAI3 global Interrupt          */
  add_handler SWPMI1_IRQHandler                 /* Serial Wire Interface 1 global interrupt */
  add_handler TIM15_IRQHandler                  /* TIM15 global Interrupt      */
  add_handler TIM16_IRQHandler                  /* TIM16 global Interrupt      */
  add_handler TIM17_IRQHandler                  /* TIM17 global Interrupt      */
  add_handler MDIOS_WKUP_IRQHandler             /* MDIOS Wakeup  Interrupt     */
  add_handler MDIOS_IRQHandler                  /* MDIOS global Interrupt      */
  add_handler JPEG_IRQHandler                   /* JPEG global Interrupt       */
  add_handler MDMA_IRQHandler                   /* MDMA global Interrupt       */
  add_handler DSI_IRQHandler                    /* DSI global Interrupt        */
  add_handler SDMMC2_IRQHandler                 /* SDMMC2 global Interrupt     */
  add_handler HSEM1_IRQHandler                  /* HSEM1 global Interrupt      */
  add_handler HSEM2_IRQHandler                  /* HSEM1 global Interrupt      */
  add_handler ADC3_IRQHandler                   /* ADC3 global Interrupt       */
  add_handler DMAMUX2_OVR_IRQHandler            /* DMAMUX Overrun interrupt    */
  add_handler BDMA_Channel0_IRQHandler          /* BDMA Channel 0 global Interrupt */
  add_handler BDMA_Channel1_IRQHandler          /* BDMA Channel 1 global Interrupt */
  add_handler BDMA_Channel2_IRQHandler          /* BDMA Channel 2 global Interrupt */
  add_handler BDMA_Channel3_IRQHandler          /* BDMA Channel 3 global Interrupt */
  add_handler BDMA_Channel4_IRQHandler          /* BDMA Channel 4 global Interrupt */
  add_handler BDMA_Channel5_IRQHandler          /* BDMA Channel 5 global Interrupt */
  add_handler BDMA_Channel6_IRQHandler          /* BDMA Channel 6 global Interrupt */
  add_handler BDMA_Channel7_IRQHandler          /* BDMA Channel 7 global Interrupt */
  add_handler COMP1_IRQHandler                  /* COMP1 global Interrupt     */
  add_handler LPTIM2_IRQHandler                 /* LP TIM2 global interrupt   */
  add_handler LPTIM3_IRQHandler                 /* LP TIM3 global interrupt   */
  add_handler LPTIM4_IRQHandler                 /* LP TIM4 global interrupt   */
  add_handler LPTIM5_IRQHandler                 /* LP TIM5 global interrupt   */
  add_handler LPUART1_IRQHandler                /* LP UART1 interrupt         */
  add_handler WWDG_RST_IRQHandler               /* Window Watchdog reset interrupt (exti_d2_wwdg_it, exti_d1_wwdg_it) */
  add_handler CRS_IRQHandler                    /* Clock Recovery Global Interrupt */
  add_handler ECC_IRQHandler                    /* ECC diagnostic Global Interrupt */
  add_handler SAI4_IRQHandler                   /* SAI4 global interrupt      */
  add_handler architecture_reserved_handler                                 /* Reserved                   */
  add_handler HOLD_CORE_IRQHandler              /* Hold core interrupt        */
  add_handler WAKEUP_PIN_IRQHandler             /* Interrupt for all 6 wake-up pins */

  .end
