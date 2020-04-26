/**
 * @file lv_port_disp_templ.c
 *
 */

 /*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include "stm32746i_lcd.h"
#include <stdio.h>
#include "main.h"
#include "stm32f7xx_it.h"
#include "dma.h"
#include "dma2d.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
#if LV_USE_GPU
static void gpu_blend(lv_disp_drv_t * disp_drv, lv_color_t * dest, const lv_color_t * src, uint32_t length, lv_opa_t opa);
static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
        const lv_area_t * fill_area, lv_color_t color);
#endif

static void DMA_TransferComplete(DMA_HandleTypeDef *han);
static void DMA_TransferError(DMA_HandleTypeDef *han);
static void DMA2D_Config(void);
/**********************
 *  STATIC VARIABLES
 **********************/
__IO uint16_t * my_fb = (__IO uint16_t*) (0xD0200000);

static DMA_HandleTypeDef  DmaHandle;
static int32_t            x1_flush;
//static int32_t            y1_flush;
static int32_t            x2_flush;
static int32_t            y2_fill;
static int32_t            y_fill_act;
static const lv_color_t * buf_to_flush;
static lv_disp_t *our_disp = NULL;
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /* LittlevGL requires a buffer where it draws the objects. The buffer's has to be greater than 1 display row
     *
     * There are three buffering configurations:
     * 1. Create ONE buffer with some rows: 
     *      LittlevGL will draw the display's content here and writes it to your display
     * 
     * 2. Create TWO buffer with some rows: 
     *      LittlevGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LittlevGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     * 
     * 3. Create TWO screen-sized buffer: 
     *      Similar to 2) but the buffer have to be screen sized. When LittlevGL is ready it will give the
     *      whole frame to display. This way you only need to change the frame buffer's address instead of
     *      copying the pixels.
     * */
#if 1
    /* Example for 1) */
    static lv_disp_buf_t disp_buf_1;
    static lv_color_t buf1_1[LV_HOR_RES_MAX * 10];                      /*A buffer for 10 rows*/
    lv_disp_buf_init(&disp_buf_1, buf1_1, NULL, LV_HOR_RES_MAX * 10);   /*Initialize the display buffer*/
#endif

#if 0
    /* Example for 2) */
    static lv_disp_buf_t disp_buf_2;
    static lv_color_t buf2_1[LV_HOR_RES_MAX * 10];                        /*A buffer for 10 rows*/
    static lv_color_t buf2_2[LV_HOR_RES_MAX * 10];                        /*An other buffer for 10 rows*/
    lv_disp_buf_init(&disp_buf_2, buf2_1, buf2_2, LV_HOR_RES_MAX * 10);   /*Initialize the display buffer*/

#endif
#if 0
    /* Example for 3) */
    static lv_disp_buf_t disp_buf_3;
    static lv_color_t buf3_1[LV_HOR_RES_MAX * LV_VER_RES_MAX];            /*A screen sized buffer*/
    static lv_color_t buf3_2[LV_HOR_RES_MAX * LV_VER_RES_MAX];            /*An other screen sized buffer*/
    lv_disp_buf_init(&disp_buf_3, buf3_1, buf3_2, LV_HOR_RES_MAX * LV_VER_RES_MAX);   /*Initialize the display buffer*/
#endif

    /*-----------------------------------
     * Register the display in LittlevGL
     *----------------------------------*/

    lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = 1024;
    disp_drv.ver_res = 600;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    disp_drv.buffer = &disp_buf_1;

#if LV_USE_GPU
    /*Optionally add functions to access the GPU. (Only in buffered mode, LV_VDB_SIZE != 0)*/

    /*Blend two color array using opacity*/
    disp_drv.gpu_blend_cb = gpu_blend;

    /*Fill a memory array with a color*/
    disp_drv.gpu_fill_cb = gpu_fill;
#endif

    /*Finally register the driver*/
    our_disp = lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Initialize your display and the required peripherals. */
static void disp_init(void)
{
    /*You code here*/
		BSP_LCD_Init();
		HAL_Delay(200);
		BSP_LCD_SetLayerVisible(1,DISABLE);
		BSP_LCD_SelectLayer(0);
		
		uint32_t i;
    for(i = 0; i < (1024 * 600) ; i++)
    {
        my_fb[i] = 0;
    }
		
	  /*##-5- Select Callbacks functions called after Transfer complete and Transfer error */
    HAL_DMA_RegisterCallback(&hdma_memtomem_dma2_stream0, HAL_DMA_XFER_CPLT_CB_ID, DMA_TransferComplete);
    HAL_DMA_RegisterCallback(&hdma_memtomem_dma2_stream0, HAL_DMA_XFER_ERROR_CB_ID, DMA_TransferError);

    /*##-6- Configure NVIC for DMA transfer complete/error interrupts ##########*/
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
		
		DMA2D_Config();
}

/* Flush the content of the internal buffer the specific area on the display
 * You can use DMA or any hardware acceleration to do this operation in the background but
 * 'lv_disp_flush_ready()' has to be called when finished. */
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
//    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/

//    int32_t x;
//    int32_t y;
//    for(y = area->y1; y <= area->y2; y++) {
//        for(x = area->x1; x <= area->x2; x++) {
//            /* Put a pixel to the display. For example: */
//            /* put_px(x, y, *color_p)*/
//						BSP_LCD_PutPixel(x, y, color_p->full);
//            color_p++;
//        }
//    }
//		
//    /* IMPORTANT!!!
//     * Inform the graphics library that you are ready with the flushing*/
//    lv_disp_flush_ready(disp_drv);
	
		int32_t x1 = area->x1;
		int32_t x2 = area->x2;
		int32_t y1 = area->y1;
		int32_t y2 = area->y2;
			/*Return if the area is out the screen*/

		if(x2 < 0) return;
		if(y2 < 0) return;
		if(x1 > 1024 - 1) return;
		if(y1 > 600 - 1) return;

		/*Truncate the area to the screen*/
		int32_t act_x1 = x1 < 0 ? 0 : x1;
		int32_t act_y1 = y1 < 0 ? 0 : y1;
		int32_t act_x2 = x2 > 1024 - 1 ? 1024 - 1 : x2;
		int32_t act_y2 = y2 > 600 - 1 ? 600 - 1 : y2;

		x1_flush = act_x1;
//		y1_flush = act_y1;
		x2_flush = act_x2;
		y2_fill = act_y2;
		y_fill_act = act_y1;
		buf_to_flush = color_p;

		/*##-7- Start the DMA transfer using the interrupt mode #*/
		/* Configure the source, destination and buffer size DMA fields and Start DMA Stream transfer */
		/* Enable All the DMA interrupts */
		HAL_StatusTypeDef err;
		uint32_t length = (x2_flush - x1_flush + 1);
#if LV_COLOR_DEPTH == 24 || LV_COLOR_DEPTH == 32
		length *= 2; /* STM32 DMA uses 16-bit chunks so multiply by 2 for 32-bit color */
#endif
		err = HAL_DMA_Start_IT(&hdma_memtomem_dma2_stream0,(uint32_t)buf_to_flush, (uint32_t)&my_fb[y_fill_act * 1024 + x1_flush],
						 length);
		if(err != HAL_OK)
		{
				while(1);	/*Halt on error*/
		}
}


/*OPTIONAL: GPU INTERFACE*/
#if LV_USE_GPU

/* If your MCU has hardware accelerator (GPU) then you can use it to blend to memories using opacity
 * It can be used only in buffered mode (LV_VDB_SIZE != 0 in lv_conf.h)*/
static void gpu_blend(lv_disp_drv_t * disp_drv, lv_color_t * dest, const lv_color_t * src, uint32_t length, lv_opa_t opa)
{
//    /*It's an example code which should be done by your GPU*/
//    uint32_t i;
//    for(i = 0; i < length; i++) {
//        dest[i] = lv_color_mix(dest[i], src[i], opa);
//    }
	
	/*Wait for the previous operation*/
	HAL_DMA2D_PollForTransfer(&hdma2d, 100);
	hdma2d.Init.Mode = DMA2D_M2M_BLEND;
	/* DMA2D Initialization */
	if(HAL_DMA2D_Init(&hdma2d) != HAL_OK)
	{
		/* Initialization Error */
		while(1);
	}

	hdma2d.LayerCfg[1].InputAlpha = opa;
	HAL_DMA2D_ConfigLayer(&hdma2d, 1);
	HAL_DMA2D_BlendingStart(&hdma2d, (uint32_t) src, (uint32_t) dest, (uint32_t)dest, length, 1);
}

/* If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color
 * It can be used only in buffered mode (LV_VDB_SIZE != 0 in lv_conf.h)*/
static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
                    const lv_area_t * fill_area, lv_color_t color)
{
//    /*It's an example code which should be done by your GPU*/
//    int32_t x, y;
//    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/

//    for(y = fill_area->y1; y < fill_area->y2; y++) {
//        for(x = fill_area->x1; x < fill_area->x2; x++) {
//            dest_buf[x] = color;
//        }
//        dest_buf+=dest_width;    /*Go to the next line*/
//    }
		/*Wait for the previous operation*/
		HAL_DMA2D_PollForTransfer(&hdma2d, 100);

		hdma2d.Init.Mode         = DMA2D_R2M;
		/* DMA2D Initialization */
		if(HAL_DMA2D_Init(&hdma2d) != HAL_OK)
		{
		 /* Initialization Error */
		 while(1);
		}

		hdma2d.LayerCfg[1].InputAlpha = 0xff;
		HAL_DMA2D_ConfigLayer(&hdma2d, 1);

		lv_color_t * dest_buf_ofs = dest_buf;

		dest_buf_ofs += dest_width * fill_area->y1;
		dest_buf_ofs += fill_area->x1;
		lv_coord_t area_w = lv_area_get_width(fill_area);

		uint32_t i;
		for(i = fill_area->y1; i <= fill_area->y2; i++) {
			/*Wait for the previous operation*/
			HAL_DMA2D_PollForTransfer(&hdma2d, 100);
			HAL_DMA2D_BlendingStart(&hdma2d, (uint32_t) lv_color_to32(color), (uint32_t) dest_buf_ofs, (uint32_t)dest_buf_ofs, area_w, 1);
			dest_buf_ofs += dest_width;
		}
}

#endif  /*LV_USE_GPU*/

/**
  * @brief  DMA conversion complete callback
  * @note   This function is executed when the transfer complete interrupt
  *         is generated
  * @retval None
  */
static void DMA_TransferComplete(DMA_HandleTypeDef *han)
{
    y_fill_act ++;

    if(y_fill_act > y2_fill) {
        lv_disp_flush_ready(&our_disp->driver);
    } else {
    	uint32_t length = (x2_flush - x1_flush + 1);
        buf_to_flush += x2_flush - x1_flush + 1;
        /*##-7- Start the DMA transfer using the interrupt mode ####################*/
        /* Configure the source, destination and buffer size DMA fields and Start DMA Stream transfer */
        /* Enable All the DMA interrupts */
#if LV_COLOR_DEPTH == 24 || LV_COLOR_DEPTH == 32
        length *= 2; /* STM32 DMA uses 16-bit chunks so multiply by 2 for 32-bit color */
#endif
        if(HAL_DMA_Start_IT(han,(uint32_t)buf_to_flush, (uint32_t)&my_fb[y_fill_act * 1024 + x1_flush],
                            length) != HAL_OK)
        {
            while(1);	/*Halt on error*/
        }
    }
}

/**
  * @brief  DMA conversion error callback
  * @note   This function is executed when the transfer error interrupt
  *         is generated during DMA transfer
  * @retval None
  */
static void DMA_TransferError(DMA_HandleTypeDef *han)
{

}

/**
  * @brief  DMA2D Transfer completed callback
  * @param  hdma2d: DMA2D handle.
  * @note   This example shows a simple way to report end of DMA2D transfer, and
  *         you can add your own implementation.
  * @retval None
  */
static void DMA2D_TransferComplete(DMA2D_HandleTypeDef *hdma2d)
{

}

/**
  * @brief  DMA2D error callbacks
  * @param  hdma2d: DMA2D handle
  * @note   This example shows a simple way to report DMA2D transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
static void DMA2D_TransferError(DMA2D_HandleTypeDef *hdma2d)
{

}

/**
  * @brief DMA2D configuration.
  * @note  This function Configure the DMA2D peripheral :
  *        1) Configure the Transfer mode as memory to memory with blending.
  *        2) Configure the output color mode as RGB565 pixel format.
  *        3) Configure the foreground
  *          - first image loaded from FLASH memory
  *          - constant alpha value (decreased to see the background)
  *          - color mode as RGB565 pixel format
  *        4) Configure the background
  *          - second image loaded from FLASH memory
  *          - color mode as RGB565 pixel format
  * @retval None
  */
static void DMA2D_Config(void)
{
    /* Configure the DMA2D Mode, Color Mode and output offset */
    hdma2d.Init.Mode         = DMA2D_M2M_BLEND;
#if LV_COLOR_DEPTH == 16
    hdma2d.Init.ColorMode    = DMA2D_RGB565;
#elif LV_COLOR_DEPTH == 24 || LV_COLOR_DEPTH == 32
    hdma2d.Init.ColorMode    = DMA2D_ARGB8888;
#endif
    hdma2d.Init.OutputOffset = 0x0;

    /* DMA2D Callbacks Configuration */
    hdma2d.XferCpltCallback  = DMA2D_TransferComplete;
    hdma2d.XferErrorCallback = DMA2D_TransferError;

    /* Foreground Configuration */
    hdma2d.LayerCfg[1].AlphaMode = DMA2D_REPLACE_ALPHA;
    hdma2d.LayerCfg[1].InputAlpha = 0xFF;
#if LV_COLOR_DEPTH == 16
    hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_RGB565;
#elif LV_COLOR_DEPTH == 24 || LV_COLOR_DEPTH == 32
    hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
#endif

    hdma2d.LayerCfg[1].InputOffset = 0x0;

    /* Background Configuration */
    hdma2d.LayerCfg[0].AlphaMode = DMA2D_REPLACE_ALPHA;
    hdma2d.LayerCfg[0].InputAlpha = 0xFF;
#if LV_COLOR_DEPTH == 16
    hdma2d.LayerCfg[0].InputColorMode = DMA2D_INPUT_RGB565;
#elif LV_COLOR_DEPTH == 24 || LV_COLOR_DEPTH == 32
    hdma2d.LayerCfg[0].InputColorMode = DMA2D_INPUT_ARGB8888;
#endif
    hdma2d.LayerCfg[0].InputOffset = 0x0;

    hdma2d.Instance   = DMA2D;

    /* DMA2D Initialization */
    if(HAL_DMA2D_Init(&hdma2d) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }

    HAL_DMA2D_ConfigLayer(&hdma2d, 0);
    HAL_DMA2D_ConfigLayer(&hdma2d, 1);
}

#else /* Enable this file at the top */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;
#endif
