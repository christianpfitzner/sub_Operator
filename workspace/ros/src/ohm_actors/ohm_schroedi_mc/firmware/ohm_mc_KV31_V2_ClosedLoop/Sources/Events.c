/* ###################################################################
**     Filename    : Events.c
**     Project     : test45
**     Processor   : MKV31F512VLH12
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-03-31, 13:15, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file Events.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"

#ifdef __cplusplus
extern "C" {
#endif 

extern uint8_t g_BrdID;
extern int SS[8];
extern uint8_t g_State;
extern uint32_t g_framesTransfered;
extern uint32_t HW_SPI0;
static int value = 0;
extern uint8_t ReceiveBuffer[12];
uint8_t Data_Buffer[11] = {0};
int16_t ticks_ENC1 = 0;
int16_t ticks_ENC2 = 0;
extern uint16_t Enc_Ticks[8][2];
extern uint8_t MotSel[8][2];
float e_inte_MOT1 = 0;
float e_pre_MOT1 = 0;
float DAC_VAR = 13.423;
float e_inte_MOT2 = 0;
float e_pre_MOT2 = 0;
int16_t VAL_MOT1 = 0;
int16_t VAL_MOT2 = 0;
extern uint32_t HW_DAC0;
extern uint32_t HW_DAC1;
extern bool DirSel[8][2];
extern uint8_t SendBuffer[12];
float VAL_PT_OLD;
float Ts = 0.0;
float VAL_SET_OLD = 0.0;




/* User includes (#include below this line is not maintained by Processor Expert) */

/*
** ===================================================================
**     Interrupt handler : DAC0_IRQHandler
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void DAC0_IRQHandler(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Interrupt handler : DAC1_IRQHandler
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void DAC1_IRQHandler(void)
{
  /* Write your code here ... */
}

#ifdef MOT2_QD_IDX
/*
** ===================================================================
**     Interrupt handler : FTM2_IRQHandler
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void FTM2_IRQHandler(void)
{
  FTM_DRV_IRQHandler(MOT2_QD_IDX);
  /* Write your code here ... */
}
#else
  /* This IRQ handler is not used by MOT2_QD component. The purpose may be
   * that the component has been removed or disabled. It is recommended to 
   * remove this handler because Processor Expert cannot modify it according to 
   * possible new request (e.g. in case that another component uses this
   * interrupt vector). */
  #warning This IRQ handler is not used by MOT2_QD component.\
           It is recommended to remove this because Processor Expert cannot\
           modify it according to possible new request.
#endif

#ifdef MOT1_QD_IDX
/*
** ===================================================================
**     Interrupt handler : FTM1_IRQHandler
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void FTM1_IRQHandler(void)
{
  FTM_DRV_IRQHandler(MOT1_QD_IDX);
  /* Write your code here ... */
}
#else
  /* This IRQ handler is not used by MOT1_QD component. The purpose may be
   * that the component has been removed or disabled. It is recommended to 
   * remove this handler because Processor Expert cannot modify it according to 
   * possible new request (e.g. in case that another component uses this
   * interrupt vector). */
  #warning This IRQ handler is not used by MOT1_QD component.\
           It is recommended to remove this because Processor Expert cannot\
           modify it according to possible new request.
#endif

/*
** ===================================================================
**     Interrupt handler : ADC0_IRQHandler
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void ADC0_IRQHandler(void)
{
  /* Write your code here ... */
}

#ifdef dspiCom1_IDX
/*
** ===================================================================
**     Interrupt handler : dspiCom1_IRQHandler
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void dspiCom1_IRQHandler(void)
{
value = value^1;


uint32_t Checksum = 0;

#if dspiCom1_DMA_MODE
  #if FSL_FEATURE_SOC_EDMA_COUNT
  /* fsl_dspi_edma driver handler (dma mode) */
  DSPI_DRV_EdmaIRQHandler(dspiCom1_IDX);
  #else
  /* fsl_dspi_dma driver does not have handler.
     DMA callbacks are used instead.
   */
  #endif
#else
  /* fsl_dspi driver handler (interrupt mode) */
  DSPI_DRV_IRQHandler(dspiCom1_IDX);
#endif
  /* Write your code here ... */

  if((DSPI_DRV_EdmaSlaveGetTransferStatus(HW_SPI0, &g_framesTransfered) == kStatus_DSPI_Success))
{

	  for(uint8_t i = 0; i<=10; i++)
	  {
		  Checksum += ReceiveBuffer[i];
	  }
	  if((Checksum & 0x000000FF) == ReceiveBuffer[11])
	  {
		  for(uint8_t i = 0; i<=10; i++)
		  {
			  Data_Buffer[i] = ReceiveBuffer[i];
		  }
	  }
}
  VAL_MOT1 = (int16_t)((Data_Buffer[1] << 8) | Data_Buffer[2]);
  VAL_MOT2 = (int16_t)((Data_Buffer[3] << 8) | Data_Buffer[4]);

  g_State = 1;
}
#else
  /* This IRQ handler is not used by dspiCom1 component. The purpose may be
   * that the component has been removed or disabled. It is recommended to 
   * remove this handler because Processor Expert cannot modify it according to 
   * possible new request (e.g. in case that another component uses this
   * interrupt vector). */
  #warning This IRQ handler is not used by dspiCom1 component.\
           It is recommended to remove this because Processor Expert cannot\
           modify it according to possible new request.
#endif

/*
** ===================================================================
**     Interrupt handler : SS_PORTC_IRQHandler
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void SS_PORTC_IRQHandler(void)
{
  /* Clear interrupt flag.*/
  PORT_HAL_ClearPortIntFlag(PORTC_BASE_PTR);
  /* Write your code here ... */
}

#ifdef pitTimer1_IDX
/*
** ===================================================================
**     Interrupt handler : pitTimer1_IRQHandler
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void pitTimer1_IRQHandler(void)
{
  /* Clear interrupt flag.*/
  PIT_HAL_ClearIntFlag(g_pitBase[pitTimer1_IDX], pitTimer1_CHANNEL);
  /* Write your code here ... */
  ticks_ENC1 = FTM1_CNT;
  FTM1_CNT = 0;
  ticks_ENC2 = FTM2_CNT;
  FTM2_CNT = 0;
  float RPS_ENC1 = (((float)ticks_ENC1/4.0)/(float)(Enc_Ticks[g_BrdID][0]))*1000.0;
  float RPS_ENC2 = (((float)ticks_ENC2/4.0)/(float)(Enc_Ticks[g_BrdID][1]))*1000.0;
  ///////////////////////////////////////////////////////////////////////////////////
  //Closed Loop 1ms dt
  bool MOT1_DIR_VAL = 0;
  bool MOT2_DIR_VAL = 0;
  if (VAL_SET_OLD != (float)VAL_MOT1)
	{
	  Ts = (abs((float)VAL_MOT1 - VAL_SET_OLD)) * (0.35 /270);
	  VAL_SET_OLD = VAL_MOT1;
	}
  float K = 1.0;
  float VAL_PT = VAL_PT_OLD +(K*(float)VAL_MOT1-VAL_PT_OLD)*(0.001/(Ts+0.001));
  VAL_PT_OLD = VAL_PT;
  float KP = 10.0;
  float KI = 150.0;
  //float KD = 0;
  float e_MOT1 = VAL_PT - RPS_ENC1;
  e_inte_MOT1 += e_MOT1 ;

  //float VAL_MOT1_SET = (KP * e_MOT1 + KI * e_inte_MOT1 * 0.001f + KD * ((e_MOT1 - e_pre_MOT1)/ 0.001f)) * DAC_VAR;
  float VAL_MOT1_SET = KP * e_MOT1 + KI * e_inte_MOT1 * 0.001f;
  e_pre_MOT1 = e_MOT1;
  if (VAL_MOT1_SET > 0)
  {
	  MOT1_DIR_VAL = 0;
  	  MOT2_DIR_VAL = 1;
  }
  else
  {
	  MOT1_DIR_VAL = 1;
  	  MOT2_DIR_VAL = 0;
  }
  /*if (abs(VAL_MOT1) <= 3)
  {
	  VAL_MOT1_SET = 0;
  }*/

  GPIO_DRV_WritePinOutput(MOT1_DIR, MOT1_DIR_VAL);
  GPIO_DRV_WritePinOutput(MOT2_DIR, MOT2_DIR_VAL);

  int16_t VAL_MOT1_SET_ABS = abs(VAL_MOT1_SET);
  if (VAL_MOT1_SET_ABS > 4095)
  {
	  VAL_MOT1_SET_ABS = 4095;
		  e_inte_MOT1 -= e_MOT1;

  }
  DAC_DRV_Output(HW_DAC1, VAL_MOT1_SET_ABS);
  DAC_DRV_Output(HW_DAC0, VAL_MOT1_SET_ABS);
  //GPIO_DRV_WritePinOutput(MOT1_EN, Data_Buffer[5] & 0b00000100);
  GPIO_DRV_WritePinOutput(MOT1_EN, 1);
  GPIO_DRV_WritePinOutput(MOT2_EN, 1);
  //GPIO_DRV_WritePinOutput(MOT2_EN, Data_Buffer[5] & 0b00001000);
  ///////////////////////////////////////////////////////////////////////////////////
  uint8_t LByte_ENC_MOT1 = (int16_t)RPS_ENC1;
  uint8_t Byte2_ENC_MOT1 = ((int16_t)RPS_ENC1)>> 8 ;
//  uint8_t LByte_ENC_MOT1 = VAL_MOT1_SET_ABS;
//    uint8_t Byte2_ENC_MOT1 = (VAL_MOT1_SET_ABS >> 8 );

  uint8_t LByte_ENC_MOT2 = (int16_t)e_MOT1;//(int16_t)RPS_ENC2;
  uint8_t Byte2_ENC_MOT2 = ((int16_t)e_MOT1 >> 8 );

  SendBuffer[1] = Byte2_ENC_MOT2;
  SendBuffer[2] = LByte_ENC_MOT2;
  SendBuffer[3] = Byte2_ENC_MOT1;
  SendBuffer[4] = LByte_ENC_MOT1;
  g_State = 1;
}
#else
  /* This IRQ handler is not used by pitTimer1 component. The purpose may be
   * that the component has been removed or disabled. It is recommended to 
   * remove this handler because Processor Expert cannot modify it according to 
   * possible new request (e.g. in case that another component uses this
   * interrupt vector). */
  #warning This IRQ handler is not used by pitTimer1 component.\
           It is recommended to remove this because Processor Expert cannot\
           modify it according to possible new request.
#endif

/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 


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
