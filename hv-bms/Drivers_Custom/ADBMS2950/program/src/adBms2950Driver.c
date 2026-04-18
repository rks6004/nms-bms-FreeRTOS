/**
********************************************************************************
*
* @file:    adbms_2950.c
*
* @brief:   This file contains the test cases implementation.
*
* @details:
*
*******************************************************************************
Copyright(c) 2020 Analog Devices, Inc. All Rights Reserved. This software is
proprietary & confidential to Analog Devices, Inc. and its licensors. By using
this software you agree to the terms of the associated Analog Devices License
Agreement.
*******************************************************************************
*/

#include "adBms2950Driver.h"

/**
 * @brief Header for functional test cases
 */
void adBms2950_Functional_header(uint8_t tIC, cell_asic_2950 *ic)
{
  uint8_t cic = 0;

  // 1. Wakeup
  adBmsWakeupIc(tIC);

  // 2. SRST
  spiSendCmd2950(tIC, &ic[0], SRST);   

  // 3. RSTCC
  adBms2950_reset_cmd_count(tIC, &ic[0]);  
  // Send RDCFGA to read back reset values of CFGA
  adBms2950ReadData(tIC, &ic[0], RDCFGA, Conf, GRP_A);  

  // 4. REFUP bit check; Read CFGA
  while (ic[0].rx_cfga.refup != 1)
  {
    // Send RDCFGA to read back REFUP in CFGA
    adBms2950ReadData(tIC, &ic[0], RDCFGA, Conf, GRP_A);      
  }

  // 5. Read SID
  adBms2950ReadData(tIC, &ic[0], RDSID, SID, GRP_NONE);

  // 6. Status Rev ID
  adBms2950ReadData(tIC, &ic[0], RDSTAT, status, GRP_NONE);

  // 7. Send CLRFLAG to Flag
  adBms2950_Configure_CLRflag_Data(cic, tIC, &ic[0]);   
  adBms2950WriteData(tIC, &ic[0], CLRFLAG, clrflag, GRP_NONE);

  // 8. Read Flag  - all bits should be cleared
  adBms2950ReadData(tIC, &ic[0], RDFLAG, Flag, GRP_FLAG_NOERR);

}

/**
 * @brief Reset command counter 
 */
void adBms2950_reset_cmd_count(uint8_t tIC, cell_asic_2950 *ic)
{
  //Wakeup
  adBmsWakeupIc(tIC); 
  //SRST
  spiSendCmd2950(tIC, &ic[0], SRST);
  //RSTCC
  spiSendCmd2950(tIC, &ic[0], RSTCC); 
  for(int cic=0;cic< tIC;cic++)
  {
    ic[cic].cccrc.cal_cmd_cntr =0;
  }
}

/**
 * @brief Helper function to configure clrflag register data
 */
void adBms2950_Configure_CLRflag_Data(uint8_t cic, uint8_t tIC, cell_asic_2950 *ic)
{
  ic[cic].clflag.vdruv              = CL_FLAG_SET;
  ic[cic].clflag.ocmm               = CL_FLAG_SET;
  ic[cic].clflag.oc3l               = CL_FLAG_SET;
  ic[cic].clflag.ocagd_clrm         = CL_FLAG_SET;
  ic[cic].clflag.ocal               = CL_FLAG_SET;
  ic[cic].clflag.oc1l               = CL_FLAG_SET;
  
  ic[cic].clflag.vdduv              = CL_FLAG_SET;
  ic[cic].clflag.noclk              = CL_FLAG_SET;
  ic[cic].clflag.refflt             = CL_FLAG_SET;
  ic[cic].clflag.ocbgd              = CL_FLAG_SET;
  ic[cic].clflag.ocbl               = CL_FLAG_SET;
  ic[cic].clflag.oc2l               = CL_FLAG_SET;
  
  ic[cic].clflag.vregov             = CL_FLAG_SET;
  ic[cic].clflag.vreguv             = CL_FLAG_SET;
  ic[cic].clflag.vdigov             = CL_FLAG_SET;
  ic[cic].clflag.vdiguv             = CL_FLAG_SET;
  ic[cic].clflag.sed1               = CL_FLAG_SET;
  ic[cic].clflag.med1               = CL_FLAG_SET;
  ic[cic].clflag.sed2               = CL_FLAG_SET;
  ic[cic].clflag.med2               = CL_FLAG_SET;
  
  ic[cic].clflag.vdel               = CL_FLAG_SET;
  ic[cic].clflag.vde                = CL_FLAG_SET;
  ic[cic].clflag.spiflt             = CL_FLAG_SET;
  ic[cic].clflag.reset              = CL_FLAG_SET;
  ic[cic].clflag.thsd               = CL_FLAG_SET;
  ic[cic].clflag.tmode              = CL_FLAG_SET;
  ic[cic].clflag.oscflt             = CL_FLAG_SET;
}



/**
 * @brief Set configuration register A (CFGA). Refer to the data sheet
 *        Set configuration register B (CFGB). Refer to the data sheet
 */
void adBms2950_init_config(uint8_t tIC, cell_asic_2950 *ic)
{
  for(uint8_t cic = 0; cic < tIC; cic++)
  {
    //---------------------------- CFGA ------------------------------------------
    //CFGAR0
    /* Init config A */
    ic[cic].tx_cfga.vs1       = (VS)VSM_V4;
    ic[cic].tx_cfga.vs2       = (VS)VSM_V4;
    ic[cic].tx_cfga.vs3       = (VSB)VSMV_VREF1P25;      
    ic[cic].tx_cfga.vs4       = (VSB)VSMV_VREF1P25;
    ic[cic].tx_cfga.vs5       = (VSB)VSMV_VREF1P25;
    ic[cic].tx_cfga.ocen      = (OCEN)OC_ENABLE;   

    //CFGAR1
    /* Init config A */      
    ic[cic].tx_cfga.injosc    = (INJOSC)INJOSC1_OSC_FAST;
    ic[cic].tx_cfga.injmon    = (INJMON)INJMON3_OV_DE;
    ic[cic].tx_cfga.injts     = (INJTS)FORCE_THSD;
    ic[cic].tx_cfga.injecc    = (INJECC)FORCE_ECC;
    ic[cic].tx_cfga.injtm     = (INJTM)FORCE_TMODE;      

    //CFGAR2
    /* Init config A */
    ic[cic].tx_cfga.vs6       = (VSB)VSMV_VREF1P25;
    ic[cic].tx_cfga.vs7       = (VSB)VSMV_VREF1P25;
    ic[cic].tx_cfga.vs8       = (VSB)VSMV_VREF1P25;
    ic[cic].tx_cfga.vs9       = (VSB)VSMV_VREF1P25;
    ic[cic].tx_cfga.vs10      = (VSB)VSMV_VREF1P25;
    ic[cic].tx_cfga.soak      = (SOAK)SOAK_150ms;      

    //CFGAR3
    /* Init config A */      
    ic[cic].tx_cfga.gpo1c     = (GPOxC)PULLED_UP_TRISTATED;
    ic[cic].tx_cfga.gpo2c     = (GPOxC)PULLED_UP_TRISTATED;
    ic[cic].tx_cfga.gpo3c     = (GPOxC)PULLED_UP_TRISTATED;
    ic[cic].tx_cfga.gpo4c     = (GPOxC)PULLED_UP_TRISTATED;
    ic[cic].tx_cfga.gpo5c     = (GPOxC)PULLED_UP_TRISTATED;
    ic[cic].tx_cfga.gpo6c     = (GPOxC)PULLED_UP_TRISTATED;      

    //CFGAR4
    /* Init config A */      
    ic[cic].tx_cfga.gpo1od    = (GPOxOD_OPOD)OPEN_DRAIN;
    ic[cic].tx_cfga.gpo2od    = (GPOxOD_OPOD)PUSH_PULL;
    ic[cic].tx_cfga.gpo3od    = (GPOxOD_OPOD)OPEN_DRAIN;
    ic[cic].tx_cfga.gpo4od    = (GPOxOD_OPOD)OPEN_DRAIN;
    ic[cic].tx_cfga.gpo5od    = (GPOxOD_OPOD)OPEN_DRAIN;
    ic[cic].tx_cfga.gpo6od    = (GPOxOD_OPOD)OPEN_DRAIN; 
    ic[cic].tx_cfga.gpio1fe   = (GPIO1FE)FAULT_STATUS_ENABLE; 
    ic[cic].tx_cfga.spi3w     = (SPI3W)THREE_WIRE; 

    //CFGAR5
    /* Init config A */
    ic[cic].tx_cfga.acci      = (ACCI)ACCI_32;
    ic[cic].tx_cfga.commbk    = (COMMBK)COMMBK_OFF; //uncomment this if running in daisy chain as this will affect communication in daisy chain
    //      ic[cic].tx_cfga.snapst    = (SNAPST)SNAP_ON; //snapst is read bits, Just to check if this bits writes to register (commented in Rev B)
    ic[cic].tx_cfga.vb1mux    = (VBxMUX)DIFFERENTIAL;
    ic[cic].tx_cfga.vb2mux    = (VBxMUX)DIFFERENTIAL; 
    ic[cic].tx_cfga.refup     = 0x1;  

    //---------------------------- CFGB ------------------------------------------
    //CFGBR0
    /* Init config B */
    ic[cic].tx_cfgb.oc1th = 0x7F; //setting oc1th Max
    ic[cic].tx_cfgb.oc1ten = (OCxTEN_REFTEN)REFERENCE_INPUT;

    //CFGBR1
    /* Init config B */
    ic[cic].tx_cfgb.oc2th = 0x7F; //setting oc1th Max
    ic[cic].tx_cfgb.oc2ten = (OCxTEN_REFTEN)REFERENCE_INPUT;


    //CFGBR2
    /* Init config B */
    ic[cic].tx_cfgb.oc3th = 0x7F; //setting oc1th Max
    ic[cic].tx_cfgb.oc3ten = (OCxTEN_REFTEN)REFERENCE_INPUT;


    //CFGBR3
    /* Init config B */
    ic[cic].tx_cfgb.ocdgt     = (OCDGT)OCDGT3_7oo8; 
    ic[cic].tx_cfgb.ocdp      = (OCDP)OCDP1_FAST; 
    ic[cic].tx_cfgb.reften    = (OCxTEN_REFTEN)REFERENCE_INPUT; 
    ic[cic].tx_cfgb.octsel    = (OCTSEL)OCTSEL3_OCxADC_M293_REFADC_P20;


    //CFGBR4
    /* Init config B */
    ic[cic].tx_cfgb.ocod      = (GPOxOD_OPOD)OPEN_DRAIN; 
    ic[cic].tx_cfgb.oc1gc     = (OCxGC)GAIN_2;
    ic[cic].tx_cfgb.oc2gc     = (OCxGC)GAIN_2;
    ic[cic].tx_cfgb.oc3gc     = (OCxGC)GAIN_2;
    ic[cic].tx_cfgb.ocmode    = (OCMODE)OCMODE3_STATIC;
    ic[cic].tx_cfgb.ocax      = (OCABX)OCABX_ACTIVE_LOW;
    ic[cic].tx_cfgb.ocbx      = (OCABX)OCABX_ACTIVE_LOW;

    //CFGBR5
    /* Init config B */
    ic[cic].tx_cfgb.diagsel   = (DIAGSEL)DIAGSEL0_IAB_VBAT;
    ic[cic].tx_cfgb.gpio2eoc  = (GPIO2EOC)EOC_ENABLED;      
    ic[cic].tx_cfgb.gpio1c    = (GPIOxC)PULL_DOWN_OFF;
    ic[cic].tx_cfgb.gpio2c    = (GPIOxC)PULL_DOWN_OFF;
    ic[cic].tx_cfgb.gpio3c    = (GPIOxC)PULL_DOWN_OFF;
    ic[cic].tx_cfgb.gpio4c    = (GPIOxC)PULL_DOWN_OFF;   
  }
}

/**
 * @brief Read Battery Voltages and Current Registers Accumulators
 */
void adBms2950_read_acc_ivbat(uint8_t tIC, cell_asic_2950 *ic)
{
  // Read Current Accumulator Register Group
  adBms2950ReadData(tIC, &ic[0], RDIACC, AccCr, GRP_NONE);
  // Read Battery Voltage Accumulator Group
  adBms2950ReadData(tIC, &ic[0], RDVBACC, AccVbat, GRP_NONE); 
}