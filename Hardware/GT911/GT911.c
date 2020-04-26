#include <stdio.h>
#include "GT911.h"
#include "SW_I2C.h"

GT911_Dev Dev_Now,Dev_Backup;

static void GT911_Reset_Sequence()
{
	GPIO_InitTypeDef GPIO_InitStruSW;
	
	GT911_RST_CLK();
	GT911_INT_CLK();
	
  GPIO_InitStruSW.Pin = GT911_RST_PIN;
  GPIO_InitStruSW.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruSW.Pull = GPIO_PULLUP;
  GPIO_InitStruSW.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(GT911_RST_PORT, &GPIO_InitStruSW);
	
	GPIO_InitStruSW.Pin = GT911_INT_PIN;
  HAL_GPIO_Init(GT911_INT_PORT, &GPIO_InitStruSW);
	
	HAL_GPIO_WritePin(GT911_RST_PORT,GT911_RST_PIN,GPIO_PIN_RESET);    
	HAL_Delay(100);
	HAL_GPIO_WritePin(GT911_INT_PORT,GT911_INT_PIN,GPIO_PIN_RESET);    
	HAL_Delay(100);
	HAL_GPIO_WritePin(GT911_RST_PORT,GT911_RST_PIN,GPIO_PIN_SET);    
	HAL_Delay(200);
	
	GPIO_InitStruSW.Pin = GT911_INT_PIN;
  GPIO_InitStruSW.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruSW.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GT911_INT_PORT, &GPIO_InitStruSW);
	
	HAL_Delay(100);
}

static uint8_t GT911_WR_Reg(uint16_t reg,uint8_t *buf,uint8_t len)
{
	uint8_t i;
	uint8_t ret=0;
	SW_I2C_Start();	
 	SW_I2C_Send_Byte(SW_CMD_WR);   
	SW_I2C_Wait_Ack();
	SW_I2C_Send_Byte(reg>>8);   	
	SW_I2C_Wait_Ack(); 	 										  		   
	SW_I2C_Send_Byte(reg&0XFF);   	
	SW_I2C_Wait_Ack();  
	for(i=0;i<len;i++)
	{	   
    SW_I2C_Send_Byte(buf[i]);  
		ret=SW_I2C_Wait_Ack();
		if(ret)break;  
	}
  SW_I2C_Stop();					
	return ret; 
}

static void GT911_RD_Reg(uint16_t reg,uint8_t *buf,uint8_t len)
{
	uint8_t i;
 	SW_I2C_Start();	
 	SW_I2C_Send_Byte(SW_CMD_WR);  
	SW_I2C_Wait_Ack();
 	SW_I2C_Send_Byte(reg>>8);   
	SW_I2C_Wait_Ack(); 	 										  		   
 	SW_I2C_Send_Byte(reg&0XFF);   	
	SW_I2C_Wait_Ack();  
 	SW_I2C_Stop();  
	
 	SW_I2C_Start();  	 	   
	SW_I2C_Send_Byte(SW_CMD_RD);     
	SW_I2C_Wait_Ack();	   
	for(i=0;i<len;i++)
	{	   
		buf[i]=SW_I2C_Read_Byte(i==(len-1)?0:1); 
	} 
  SW_I2C_Stop();   
}

static uint8_t GT911_ReadStatue(void)
{
	uint8_t buf[4];
	GT911_RD_Reg(GT911_PRODUSW_ID_REG, (uint8_t *)&buf[0], 3);
	GT911_RD_Reg(GT911_CONFIG_REG, (uint8_t *)&buf[3], 1);
	printf("TouchPad_ID:%c,%c,%c\r\nTouchPad_Config_Version:%2x\r\n",buf[0],buf[1],buf[2],buf[3]);
	return buf[3];
}

static uint16_t GT911_ReadFirmwareVersion(void)
{
	uint8_t buf[2];

	GT911_RD_Reg(GT911_FIRMWARE_VERSION_REG, buf, 2);

	printf("FirmwareVersion:%2x\r\n",(((uint16_t)buf[1] << 8) + buf[0]));
	return ((uint16_t)buf[1] << 8) + buf[0];
}

void GT911_Init(void)
{
	GT911_Reset_Sequence();

	SW_I2C_Init();
	
	GT911_ReadStatue();
	
	GT911_ReadFirmwareVersion();
}

bool GT911_isPressed(void)
{
	if(Dev_Now.Touch == 1){
		Dev_Now.Touch = 0;
		return true;
	}
	return false;
}

void GT911_X_Y(int16_t *x, int16_t *y)
{
	uint8_t buf[41];
  uint8_t Clearbuf = 0;
	
	GT911_RD_Reg(GT911_READ_XY_REG, buf, 1);		

	if ((buf[0]&0x80) == 0x00)
	{
		GT911_WR_Reg(GT911_READ_XY_REG, (uint8_t *)&Clearbuf, 1);
		HAL_Delay(10);
	}
	else
	{
		Dev_Now.TouchpointFlag = buf[0];
		Dev_Now.TouchCount = buf[0]&0x0f;
		if (Dev_Now.TouchCount > 5)
		{
			GT911_WR_Reg(GT911_READ_XY_REG, (uint8_t *)&Clearbuf, 1);
			return ;
		}
		GT911_RD_Reg(GT911_READ_XY_REG+1, &buf[1], Dev_Now.TouchCount*8);
		GT911_WR_Reg(GT911_READ_XY_REG, (uint8_t *)&Clearbuf, 1);
		
		Dev_Now.Touchkeytrackid[0] = buf[1];
		Dev_Now.X[0] = ((uint16_t)buf[3] << 8) + buf[2];
		Dev_Now.Y[0] = ((uint16_t)buf[5] << 8) + buf[4];
		Dev_Now.S[0] = ((uint16_t)buf[7] << 8) + buf[6];
		
		if(Dev_Now.X[0] < 1024 && Dev_Now.X[0] > 0){
			if(Dev_Now.Y[0] < 600 && Dev_Now.Y[0] > 0){
				*x = (int16_t)Dev_Now.X[0];
				*y = (int16_t)Dev_Now.Y[0];
				
				Dev_Backup.X[0] = Dev_Now.X[0];
				Dev_Backup.Y[0] = Dev_Now.Y[0];
			}
		}
		else
		{
				*x = (int16_t)Dev_Backup.X[0];
				*y = (int16_t)Dev_Backup.Y[0];
		}
	}
}


