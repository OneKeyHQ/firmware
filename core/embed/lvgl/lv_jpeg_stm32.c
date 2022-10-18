/**
 * @file lv_jpeg.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"
#if 1
#include STM32_HAL_H

#include "ff.h"
#include "irq.h"
#include "mipi_lcd.h"
#include "sdram.h"

/*********************
 *      DEFINES
 *********************/
#define CHUNK_SIZE_IN ((uint32_t)(4 * 1024))
#define CHUNK_SIZE_OUT ((uint32_t)(64 * 1024))
#define JPEG_BUFFER_EMPTY 0
#define JPEG_BUFFER_FULL 1

#define NB_INPUT_DATA_BUFFERS 2

typedef struct {
  volatile uint8_t State;
  uint8_t *DataBuffer;
  volatile uint32_t DataBufferSize;

} JPEG_Data_BufferTypeDef;

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t decoder_info(struct _lv_img_decoder_t *decoder, const void *src,
                             lv_img_header_t *header);
static lv_res_t decoder_open(lv_img_decoder_t *dec, lv_img_decoder_dsc_t *dsc);
static void decoder_close(lv_img_decoder_t *dec, lv_img_decoder_dsc_t *dsc);

/**********************
 *  STATIC VARIABLES
 **********************/

static JPEG_HandleTypeDef JPEG_Handle;
static JPEG_ConfTypeDef JPEG_Info;

static lv_fs_file_t f;
volatile uint32_t Jpeg_HWDecodingEnd = 0;
static uint32_t FrameBufferAddress;

static uint8_t JPEG_Data_InBuffer0[CHUNK_SIZE_IN];
static uint8_t JPEG_Data_InBuffer1[CHUNK_SIZE_IN];

static JPEG_Data_BufferTypeDef Jpeg_IN_BufferTab[NB_INPUT_DATA_BUFFERS] = {
    {JPEG_BUFFER_EMPTY, JPEG_Data_InBuffer0, 0},
    {JPEG_BUFFER_EMPTY, JPEG_Data_InBuffer1, 0}};

static uint32_t JPEG_IN_Read_BufferIndex = 0;
static uint32_t JPEG_IN_Write_BufferIndex = 0;
volatile uint32_t Input_Is_Paused = 0;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void HAL_JPEG_MspInit(JPEG_HandleTypeDef *hjpeg) {
  static MDMA_HandleTypeDef hmdmaIn;
  static MDMA_HandleTypeDef hmdmaOut;

  /* Enable JPEG clock */
  __HAL_RCC_JPGDECEN_CLK_ENABLE();

  /* Enable MDMA clock */
  __HAL_RCC_MDMA_CLK_ENABLE();

  // HAL_NVIC_SetPriority(JPEG_IRQn, 0x07, 0x00);
  NVIC_SetPriority(MDMA_IRQn, IRQ_PRI_JPEG);
  HAL_NVIC_EnableIRQ(JPEG_IRQn);

  /* Input MDMA */
  /* Set the parameters to be configured */
  hmdmaIn.Init.Priority = MDMA_PRIORITY_HIGH;
  hmdmaIn.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
  hmdmaIn.Init.SourceInc = MDMA_SRC_INC_BYTE;
  hmdmaIn.Init.DestinationInc = MDMA_DEST_INC_DISABLE;
  hmdmaIn.Init.SourceDataSize = MDMA_SRC_DATASIZE_BYTE;
  hmdmaIn.Init.DestDataSize = MDMA_DEST_DATASIZE_WORD;
  hmdmaIn.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;
  hmdmaIn.Init.SourceBurst = MDMA_SOURCE_BURST_32BEATS;
  hmdmaIn.Init.DestBurst = MDMA_DEST_BURST_16BEATS;
  hmdmaIn.Init.SourceBlockAddressOffset = 0;
  hmdmaIn.Init.DestBlockAddressOffset = 0;

  /*Using JPEG Input FIFO Threshold as a trigger for the MDMA*/
  hmdmaIn.Init.Request =
      MDMA_REQUEST_JPEG_INFIFO_TH; /* Set the MDMA HW trigger to JPEG Input FIFO
                                      Threshold flag*/
  hmdmaIn.Init.TransferTriggerMode = MDMA_BUFFER_TRANSFER;
  hmdmaIn.Init.BufferTransferLength =
      32; /*Set the MDMA buffer size to the JPEG FIFO threshold size i.e 32
             bytes (8 words)*/

  hmdmaIn.Instance = MDMA_Channel7;

  /* Associate the DMA handle */
  __HAL_LINKDMA(hjpeg, hdmain, hmdmaIn);

  /* DeInitialize the DMA Stream */
  HAL_MDMA_DeInit(&hmdmaIn);
  /* Initialize the DMA stream */
  HAL_MDMA_Init(&hmdmaIn);

  /* Output MDMA */
  /* Set the parameters to be configured */
  hmdmaOut.Init.Priority = MDMA_PRIORITY_VERY_HIGH;
  hmdmaOut.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
  hmdmaOut.Init.SourceInc = MDMA_SRC_INC_DISABLE;
  hmdmaOut.Init.DestinationInc = MDMA_DEST_INC_BYTE;
  hmdmaOut.Init.SourceDataSize = MDMA_SRC_DATASIZE_WORD;
  hmdmaOut.Init.DestDataSize = MDMA_DEST_DATASIZE_BYTE;
  hmdmaOut.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;
  hmdmaOut.Init.SourceBurst = MDMA_SOURCE_BURST_32BEATS;
  hmdmaOut.Init.DestBurst = MDMA_DEST_BURST_32BEATS;
  hmdmaOut.Init.SourceBlockAddressOffset = 0;
  hmdmaOut.Init.DestBlockAddressOffset = 0;

  /*Using JPEG Output FIFO Threshold as a trigger for the MDMA*/
  hmdmaOut.Init.Request =
      MDMA_REQUEST_JPEG_OUTFIFO_TH; /* Set the MDMA HW trigger to JPEG Output
                                       FIFO Threshold flag*/
  hmdmaOut.Init.TransferTriggerMode = MDMA_BUFFER_TRANSFER;
  hmdmaOut.Init.BufferTransferLength =
      32; /*Set the MDMA buffer size to the JPEG FIFO threshold size i.e 32
             bytes (8 words)*/

  hmdmaOut.Instance = MDMA_Channel6;
  /* DeInitialize the DMA Stream */
  HAL_MDMA_DeInit(&hmdmaOut);
  /* Initialize the DMA stream */
  HAL_MDMA_Init(&hmdmaOut);

  /* Associate the DMA handle */
  __HAL_LINKDMA(hjpeg, hdmaout, hmdmaOut);

  // HAL_NVIC_SetPriority(MDMA_IRQn, 0x08, 0x00);
  NVIC_SetPriority(MDMA_IRQn, IRQ_PRI_DMA);
  HAL_NVIC_EnableIRQ(MDMA_IRQn);
}

void HAL_JPEG_MspDeInit(JPEG_HandleTypeDef *hjpeg) {
  HAL_NVIC_DisableIRQ(MDMA_IRQn);

  /* DeInitialize the MDMA Stream */
  HAL_MDMA_DeInit(hjpeg->hdmain);

  /* DeInitialize the MDMA Stream */
  HAL_MDMA_DeInit(hjpeg->hdmaout);
}

void JPEG_IRQHandler(void) { HAL_JPEG_IRQHandler(&JPEG_Handle); }

void MDMA_IRQHandler() {
  HAL_MDMA_IRQHandler(JPEG_Handle.hdmain);
  HAL_MDMA_IRQHandler(JPEG_Handle.hdmaout);
}

uint32_t JPEG_InputHandler(JPEG_HandleTypeDef *hjpeg) {
  if (Jpeg_HWDecodingEnd == 0) {
    if (Jpeg_IN_BufferTab[JPEG_IN_Write_BufferIndex].State ==
        JPEG_BUFFER_EMPTY) {
      if (lv_fs_read(&f,
                     Jpeg_IN_BufferTab[JPEG_IN_Write_BufferIndex].DataBuffer,
                     CHUNK_SIZE_IN,
                     (uint32_t *)(&Jpeg_IN_BufferTab[JPEG_IN_Write_BufferIndex]
                                       .DataBufferSize)) == LV_FS_RES_OK) {
        Jpeg_IN_BufferTab[JPEG_IN_Write_BufferIndex].State = JPEG_BUFFER_FULL;
      } else {
        return 1;
      }

      if ((Input_Is_Paused == 1) &&
          (JPEG_IN_Write_BufferIndex == JPEG_IN_Read_BufferIndex)) {
        Input_Is_Paused = 0;
        HAL_JPEG_ConfigInputBuffer(
            hjpeg, Jpeg_IN_BufferTab[JPEG_IN_Read_BufferIndex].DataBuffer,
            Jpeg_IN_BufferTab[JPEG_IN_Read_BufferIndex].DataBufferSize);

        HAL_JPEG_Resume(hjpeg, JPEG_PAUSE_RESUME_INPUT);
      }

      JPEG_IN_Write_BufferIndex++;
      if (JPEG_IN_Write_BufferIndex >= NB_INPUT_DATA_BUFFERS) {
        JPEG_IN_Write_BufferIndex = 0;
      }
    }
    return 0;
  } else {
    return 1;
  }
}

void HAL_JPEG_GetDataCallback(JPEG_HandleTypeDef *hjpeg,
                              uint32_t NbDecodedData) {
  if (NbDecodedData ==
      Jpeg_IN_BufferTab[JPEG_IN_Read_BufferIndex].DataBufferSize) {
    Jpeg_IN_BufferTab[JPEG_IN_Read_BufferIndex].State = JPEG_BUFFER_EMPTY;
    Jpeg_IN_BufferTab[JPEG_IN_Read_BufferIndex].DataBufferSize = 0;

    JPEG_IN_Read_BufferIndex++;
    if (JPEG_IN_Read_BufferIndex >= NB_INPUT_DATA_BUFFERS) {
      JPEG_IN_Read_BufferIndex = 0;
    }

    if (Jpeg_IN_BufferTab[JPEG_IN_Read_BufferIndex].State ==
        JPEG_BUFFER_EMPTY) {
      HAL_JPEG_Pause(hjpeg, JPEG_PAUSE_RESUME_INPUT);
      Input_Is_Paused = 1;
    } else {
      HAL_JPEG_ConfigInputBuffer(
          hjpeg, Jpeg_IN_BufferTab[JPEG_IN_Read_BufferIndex].DataBuffer,
          Jpeg_IN_BufferTab[JPEG_IN_Read_BufferIndex].DataBufferSize);
    }
  } else {
    HAL_JPEG_ConfigInputBuffer(
        hjpeg,
        Jpeg_IN_BufferTab[JPEG_IN_Read_BufferIndex].DataBuffer + NbDecodedData,
        Jpeg_IN_BufferTab[JPEG_IN_Read_BufferIndex].DataBufferSize -
            NbDecodedData);
  }
}

void HAL_JPEG_DataReadyCallback(JPEG_HandleTypeDef *hjpeg, uint8_t *pDataOut,
                                uint32_t OutDataLength) {
  /* Update JPEG encoder output buffer address*/
  FrameBufferAddress += OutDataLength;

  HAL_JPEG_ConfigOutputBuffer(hjpeg, (uint8_t *)FrameBufferAddress,
                              CHUNK_SIZE_OUT);
}

void HAL_JPEG_InfoReadyCallback(JPEG_HandleTypeDef *hjpeg,
                                JPEG_ConfTypeDef *pInfo) {}

void HAL_JPEG_ErrorCallback(JPEG_HandleTypeDef *hjpeg) {}

void HAL_JPEG_DecodeCpltCallback(JPEG_HandleTypeDef *hjpeg) {
  Jpeg_HWDecodingEnd = 1;
}

/**
 * Register the PNG decoder functions in LVGL
 */
void lv_st_jpeg_init(void) {
  JPEG_Handle.Instance = JPEG;
  HAL_JPEG_Init(&JPEG_Handle);

  lv_img_decoder_t *dec = lv_img_decoder_create();
  lv_img_decoder_set_info_cb(dec, decoder_info);
  lv_img_decoder_set_open_cb(dec, decoder_open);
  lv_img_decoder_set_close_cb(dec, decoder_close);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Get info about a PNG image
 * @param src can be file name or pointer to a C array
 * @param header store the info here
 * @return LV_RES_OK: no error; LV_RES_INV: can't get the info
 */
static lv_res_t decoder_info(struct _lv_img_decoder_t *decoder, const void *src,
                             lv_img_header_t *header) {
  (void)decoder; /*Unused*/

  volatile uint32_t JpegProcessing_End = 0;
  Jpeg_HWDecodingEnd = 0;
  FrameBufferAddress = FMC_SDRAM_JPEG_OUTPUT_DATA_BUFFER_ADDRESS;

  lv_img_src_t src_type = lv_img_src_get_type(src); /*Get the source type*/

  /*If it's a PNG file...*/
  if (src_type == LV_IMG_SRC_FILE) {
    const char *fn = src;
    if (strncmp(lv_fs_get_ext(fn), "jpg", strlen("jpg")) == 0 ||
        strncmp(lv_fs_get_ext(fn), "jpeg", strlen("jpeg")) ==
            0) { /*Check the extension*/
      lv_fs_res_t res = lv_fs_open(&f, fn, LV_FS_MODE_RD);
      if (res != LV_FS_RES_OK) return LV_RES_INV;

      /* Read from JPG file and fill input buffers */
      for (uint32_t i = 0; i < NB_INPUT_DATA_BUFFERS; i++) {
        if (lv_fs_read(&f, Jpeg_IN_BufferTab[i].DataBuffer, CHUNK_SIZE_IN,
                       (uint32_t *)(&Jpeg_IN_BufferTab[i].DataBufferSize)) ==
            LV_FS_RES_OK) {
          Jpeg_IN_BufferTab[i].State = JPEG_BUFFER_FULL;
        } else {
          return LV_RES_INV;
        }
      }
      /* Start JPEG decoding with DMA method */
      HAL_JPEG_Decode_DMA(&JPEG_Handle, Jpeg_IN_BufferTab[0].DataBuffer,
                          Jpeg_IN_BufferTab[0].DataBufferSize,
                          (uint8_t *)FrameBufferAddress, CHUNK_SIZE_OUT);

      do {
        JpegProcessing_End = JPEG_InputHandler(&JPEG_Handle);
      } while (JpegProcessing_End == 0);
      HAL_JPEG_GetInfo(&JPEG_Handle, &JPEG_Info);

      lv_fs_close(&f);

      /*Save the data in the header*/
      header->always_zero = 0;
      header->cf = LV_IMG_CF_TRUE_COLOR;
      /*The width and height are stored in Big endian format so convert
      them to
       * little endian*/
      header->w = JPEG_Info.ImageWidth;
      header->h = JPEG_Info.ImageHeight;

      return LV_RES_OK;
    }
  }

  return LV_RES_INV; /*If didn't succeeded earlier then it's an error*/
}

/**
 * Open a PNG image and return the decided image
 * @param src can be file name or pointer to a C array
 * @param style style of the image object (unused now but certain formats might
 * use it)
 * @return pointer to the decoded image or `LV_IMG_DECODER_OPEN_FAIL` if failed
 */
static lv_res_t decoder_open(lv_img_decoder_t *decoder,
                             lv_img_decoder_dsc_t *dsc) {
  (void)decoder; /*Unused*/

  uint8_t *img_data = NULL;

  /*If it's a PNG file...*/
  if (dsc->src_type == LV_IMG_SRC_FILE) {
    const char *fn = dsc->src;
    if (strncmp(lv_fs_get_ext(fn), "jpg", strlen("jpg")) == 0 ||
        strncmp(lv_fs_get_ext(fn), "jpeg", strlen("jpeg")) ==
            0) { /*Check the extension*/

      img_data =
          lodepng_malloc(JPEG_Info.ImageWidth * JPEG_Info.ImageHeight * 2);
      dma2d_copy_ycbcr_to_rgb(
          (uint32_t *)FMC_SDRAM_JPEG_OUTPUT_DATA_BUFFER_ADDRESS,
          (uint32_t *)img_data, JPEG_Info.ImageWidth, JPEG_Info.ImageHeight,
          JPEG_Info.ChromaSubsampling);

      dsc->img_data = img_data;
      return LV_RES_OK; /*The image is fully decoded. Return with its pointer*/
    }
  }

  return LV_RES_INV; /*If not returned earlier then it failed*/
}

/**
 * Free the allocated resources
 */
static void decoder_close(lv_img_decoder_t *decoder,
                          lv_img_decoder_dsc_t *dsc) {
  LV_UNUSED(decoder); /*Unused*/
  if (dsc->img_data) {
    lodepng_free((uint8_t *)dsc->img_data);
    dsc->img_data = NULL;
  }
}

#endif
