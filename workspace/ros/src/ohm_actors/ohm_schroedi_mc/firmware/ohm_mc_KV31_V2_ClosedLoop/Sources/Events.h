/* ###################################################################
**     Filename    : Events.h
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
** @file Events.h
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         

#ifndef __Events_H
#define __Events_H
/* MODULE Events */

#include "fsl_device_registers.h"
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

#ifdef __cplusplus
extern "C" {
#endif 


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
void DAC0_IRQHandler(void);

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
void DAC1_IRQHandler(void);

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
void FTM2_IRQHandler(void);
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
void FTM1_IRQHandler(void);
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
void ADC0_IRQHandler(void);

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
void dspiCom1_IRQHandler(void);
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
void SS_PORTC_IRQHandler(void);

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
void pitTimer1_IRQHandler(void);
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

#endif 
/* ifndef __Events_H*/
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
