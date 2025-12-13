/*******************************************************************************
* File Name: vid.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_vid_H) /* Pins vid_H */
#define CY_PINS_vid_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "vid_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 vid__PORT == 15 && ((vid__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    vid_Write(uint8 value);
void    vid_SetDriveMode(uint8 mode);
uint8   vid_ReadDataReg(void);
uint8   vid_Read(void);
void    vid_SetInterruptMode(uint16 position, uint16 mode);
uint8   vid_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the vid_SetDriveMode() function.
     *  @{
     */
        #define vid_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define vid_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define vid_DM_RES_UP          PIN_DM_RES_UP
        #define vid_DM_RES_DWN         PIN_DM_RES_DWN
        #define vid_DM_OD_LO           PIN_DM_OD_LO
        #define vid_DM_OD_HI           PIN_DM_OD_HI
        #define vid_DM_STRONG          PIN_DM_STRONG
        #define vid_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define vid_MASK               vid__MASK
#define vid_SHIFT              vid__SHIFT
#define vid_WIDTH              1u

/* Interrupt constants */
#if defined(vid__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in vid_SetInterruptMode() function.
     *  @{
     */
        #define vid_INTR_NONE      (uint16)(0x0000u)
        #define vid_INTR_RISING    (uint16)(0x0001u)
        #define vid_INTR_FALLING   (uint16)(0x0002u)
        #define vid_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define vid_INTR_MASK      (0x01u) 
#endif /* (vid__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define vid_PS                     (* (reg8 *) vid__PS)
/* Data Register */
#define vid_DR                     (* (reg8 *) vid__DR)
/* Port Number */
#define vid_PRT_NUM                (* (reg8 *) vid__PRT) 
/* Connect to Analog Globals */                                                  
#define vid_AG                     (* (reg8 *) vid__AG)                       
/* Analog MUX bux enable */
#define vid_AMUX                   (* (reg8 *) vid__AMUX) 
/* Bidirectional Enable */                                                        
#define vid_BIE                    (* (reg8 *) vid__BIE)
/* Bit-mask for Aliased Register Access */
#define vid_BIT_MASK               (* (reg8 *) vid__BIT_MASK)
/* Bypass Enable */
#define vid_BYP                    (* (reg8 *) vid__BYP)
/* Port wide control signals */                                                   
#define vid_CTL                    (* (reg8 *) vid__CTL)
/* Drive Modes */
#define vid_DM0                    (* (reg8 *) vid__DM0) 
#define vid_DM1                    (* (reg8 *) vid__DM1)
#define vid_DM2                    (* (reg8 *) vid__DM2) 
/* Input Buffer Disable Override */
#define vid_INP_DIS                (* (reg8 *) vid__INP_DIS)
/* LCD Common or Segment Drive */
#define vid_LCD_COM_SEG            (* (reg8 *) vid__LCD_COM_SEG)
/* Enable Segment LCD */
#define vid_LCD_EN                 (* (reg8 *) vid__LCD_EN)
/* Slew Rate Control */
#define vid_SLW                    (* (reg8 *) vid__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define vid_PRTDSI__CAPS_SEL       (* (reg8 *) vid__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define vid_PRTDSI__DBL_SYNC_IN    (* (reg8 *) vid__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define vid_PRTDSI__OE_SEL0        (* (reg8 *) vid__PRTDSI__OE_SEL0) 
#define vid_PRTDSI__OE_SEL1        (* (reg8 *) vid__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define vid_PRTDSI__OUT_SEL0       (* (reg8 *) vid__PRTDSI__OUT_SEL0) 
#define vid_PRTDSI__OUT_SEL1       (* (reg8 *) vid__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define vid_PRTDSI__SYNC_OUT       (* (reg8 *) vid__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(vid__SIO_CFG)
    #define vid_SIO_HYST_EN        (* (reg8 *) vid__SIO_HYST_EN)
    #define vid_SIO_REG_HIFREQ     (* (reg8 *) vid__SIO_REG_HIFREQ)
    #define vid_SIO_CFG            (* (reg8 *) vid__SIO_CFG)
    #define vid_SIO_DIFF           (* (reg8 *) vid__SIO_DIFF)
#endif /* (vid__SIO_CFG) */

/* Interrupt Registers */
#if defined(vid__INTSTAT)
    #define vid_INTSTAT            (* (reg8 *) vid__INTSTAT)
    #define vid_SNAP               (* (reg8 *) vid__SNAP)
    
	#define vid_0_INTTYPE_REG 		(* (reg8 *) vid__0__INTTYPE)
#endif /* (vid__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_vid_H */


/* [] END OF FILE */
