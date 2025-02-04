/* ###################################################################
**     Filename    : main.c
**     Project     : test45
**     Processor   : MKV31F512VLH12
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-03-31, 13:15, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/
/*!
**  @addtogroup main_module main module documentation
**  @{
*/
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "clockMan1.h"
#include "pin_mux.h"
#include "DAC_MOT2.h"
#include "DAC_MOT1.h"
#include "MOT_EN.h"
#include "MOT_RDY.h"
#include "MOT_DIR.h"
#include "MOT1_QD.h"
#include "MOT2_QD.h"
#include "MOT1_LED.h"
#include "MOT2_LED.h"
#include "BRD_SEL.h"
#include "REF_INT.h"
#include "BRD_OK.h"
#include "MOT_ADC.h"
#include "QD_SEL.h"
#include "dspiCom1.h"
#include "dmaController1.h"
#include "pitTimer1.h"
#include "osa1.h"
#include "fsl_dac_driver.h"
//#include "fsl_ftm.h"


#if CPU_INIT_CONFIG
  #include "Init_Config.h"
#endif
/* User includes (#include below this line is not maintained by Processor Expert) */

bool DirSel[8][2] = {{0,1},{1,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
bool QDSel[8][4] = {{0,0,1,1},{1,0,0,0},{0,1,0,1},{1,1,1,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};
bool CtrlSel[8][2] = {{0,0},{0,0},{1,1},{1,1},{0,0},{0,0},{0,0},{0,0}};
uint8_t MotSel[8][2] = {{0,1},{2,3},{7,4},{6,5},{0,0},{0,0},{0,0},{0,0}};
uint16_t Enc_Ticks[8][2]= {{500,500},{500,500},{512,512},{512,512},{0,0},{0,0},{0,0},{0,0}};
//int SS[8]={SS_0,SS_1,SS_2,SS_3,0,0,0,0};
uint8_t g_BrdID = 0;
uint8_t g_State = 0;
uint32_t g_framesTransfered = 12;
uint8_t SPI_Instance = 0;
uint8_t BufferLength = 12;
uint8_t SendBuffer[12] = {10,11,12,13,14,15,16,17,18,19,20,21};
uint8_t ReceiveBuffer[12] = {0};
extern uint8_t Data_Buffer[11];
extern uint16_t ticks_ENC1;
extern uint16_t ticks_ENC2;
uint32_t counter_MOT1 = 1;
uint32_t ENC1_ticks = 0;
uint32_t ENC2_ticks = 0;
uint32_t HW_SPI0 = 0;
uint32_t HW_DAC0 = 0;
uint32_t HW_DAC1 = 1;
uint32_t HW_QUAD1 = 1;
uint32_t HW_QUAD2 = 2;
static int value = 1;
static int ctr = 0;
extern float RPS_ENC1;
extern float RPS_ENC2;


//static void FTM_SetQuadDecoderModuloValue(FTM_Type *base, uint32_t startValue, uint32_t overValue)
//{
//    base->CNTIN = startValue;
//    base->MOD = overValue;
//}
//
//static uint32_t FTM_GetQuadDecoderCounterValue(FTM_Type *base)
//{
//    return base->CNTIN;
//}

uint8_t BRD_DIP()
{
	uint8_t value = 0;
	value = !(GPIO_DRV_ReadPinInput(SEL8));
	value = (value<<1) | !(GPIO_DRV_ReadPinInput(SEL4));
	value = (value<<1) | !(GPIO_DRV_ReadPinInput(SEL2));
	value = (value<<1) | !(GPIO_DRV_ReadPinInput(SEL1));
	return value;
}



/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  __disable_irq();
  /* Write your local variable definition here */
  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();

  /*** End of Processor Expert internal initialization.                    ***/


  while(true)
  {

  switch(g_State)
  {
  case 0:
	  g_BrdID = BRD_DIP();

	  GPIO_DRV_WritePinOutput(QD1_SEL0, (QDSel[g_BrdID][0]));
	  GPIO_DRV_WritePinOutput(QD1_SEL1, (QDSel[g_BrdID][1]));
	  GPIO_DRV_WritePinOutput(QD2_SEL0, (QDSel[g_BrdID][2]));
	  GPIO_DRV_WritePinOutput(QD2_SEL1, (QDSel[g_BrdID][3]));

	  GPIO_DRV_WritePinOutput(MOT1_LED_1, (MotSel[g_BrdID][0] & 0b00000001));
	  GPIO_DRV_WritePinOutput(MOT1_LED_2, (MotSel[g_BrdID][0] & 0b00000010));
	  GPIO_DRV_WritePinOutput(MOT1_LED_4, (MotSel[g_BrdID][0] & 0b00000100));
	  GPIO_DRV_WritePinOutput(MOT2_LED_1, (MotSel[g_BrdID][1] & 0b00000001));
	  GPIO_DRV_WritePinOutput(MOT2_LED_2, (MotSel[g_BrdID][1] & 0b00000010));
	  GPIO_DRV_WritePinOutput(MOT2_LED_4, (MotSel[g_BrdID][1] & 0b00000100));

	  FTM1_MOD = 0x0000FFFF;
	  FTM2_MOD = 0x0000FFFF;


	  FTM_HAL_SetSoftwareTriggerCmd(FTM1, true);
	  FTM_HAL_SetSoftwareTriggerCmd(FTM1, false);
	  FTM_HAL_SetSoftwareTriggerCmd(FTM2, true);
	  FTM_HAL_SetSoftwareTriggerCmd(FTM2, false);
	  __enable_irq();
	  g_State = 1;
	  break;
  case 1:
	  DSPI_DRV_EdmaSlaveTransfer(HW_SPI0,SendBuffer,ReceiveBuffer,BufferLength);

//	  g_State = 2;
	  break;
//  case 2:
//
//
//
//
//
//
//	  ENC1_ticks = FTM1_CNT;
//	  ENC2_ticks = FTM2_CNT;
//	  uint8_t ENC1_dir = FTM_HAL_GetQuadTimerOverflowDir(FTM1);
//	  uint8_t ENC2_dir = FTM_HAL_GetQuadTimerOverflowDir(FTM2);
//
//
//
//	  if (ENC1_dir == 1)
//	  {
//		  GPIO_DRV_WritePinOutput(MOT2_LED_1, true);
//	  }
//		  else
//		  {
//			  GPIO_DRV_WritePinOutput(MOT2_LED_1, false);
//		  }
//	  if (ENC2_dir == 1)
//	  {
//	  	  GPIO_DRV_WritePinOutput(MOT2_LED_2, true);
//	  }
//	  else
//	  {
//		  GPIO_DRV_WritePinOutput(MOT2_LED_2, false);
//	  }
//	  	  GPIO_DRV_WritePinOutput(MOT2_LED_4, (FTM1_CNT & 0x00000004));
//
//	  g_State = 1;
//	  break;

  }
  }
  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/




/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
