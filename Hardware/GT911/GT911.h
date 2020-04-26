#ifndef __GT911_H_
#define __GT911_H_

#include "stm32f7xx_hal.h"
#include <stdbool.h>
#include <stdint.h>
#define GT911_MAX_WIDTH		1024    	//Touchscreen pad max width
#define GT911_MAX_HEIGHT		600			//Touchscreen pad max height

#define SW_CMD_WR			0XBA					//д��������
#define SW_CMD_RD   	0XBB					//����������

#define SW_MAX_TOUCH    5						//���ݴ��������֧�ֵĵ���

#define GT911_COMMAND_REG   				0x8040	/* ʵʱ���� */
#define GT911_CONFIG_REG						0x8047	/* ���ò����Ĵ��� */
#define GT911_PRODUSW_ID_REG 				0x8140 	/* Product ID */
#define GT911_FIRMWARE_VERSION_REG  0x8144  /* �̼��汾�� */
#define GT911_READ_XY_REG 					0x814E	/* ����Ĵ��� */

#define GT911_RST_CLK()			__GPIOD_CLK_ENABLE()
#define GT911_RST_PORT			GPIOD
#define GT911_RST_PIN				GPIO_PIN_11

#define GT911_INT_CLK()			__GPIOD_CLK_ENABLE()
#define GT911_INT_PORT			GPIOD
#define GT911_INT_PIN				GPIO_PIN_7

typedef struct
{
	uint8_t Touch;
	uint8_t TouchpointFlag;
	uint8_t TouchCount;
 
	uint8_t Touchkeytrackid[SW_MAX_TOUCH];
	uint16_t X[SW_MAX_TOUCH];
	uint16_t Y[SW_MAX_TOUCH];
	uint16_t S[SW_MAX_TOUCH];
}GT911_Dev;

void GT911_Init(void);
bool GT911_isPressed(void);
void GT911_X_Y(int16_t *x, int16_t *y);

#endif /*__GT911_H_*/
