/**
********************************************************************************
*
* @file:    adi_bms_utility.c
*
* @brief:   This file contains utility functions implementation.
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
/*! \addtogroup BMS_Driver
*  @{
*/

/*! \addtogroup Driver_Utility
*  @{
*/

/*!<
This documentation provides details about BMS driver APIs and their usage.
Using the BMS Driver Application can:
- Read/Write the configuration registers of the BMS devices stacked in daisy chaining.
- Send commands and Read the Cell Voltages, Aux Voltages and Status registers (Sum of cells, Internal Die temperature etc.)
*/

/*============= I N C L U D E S =============*/
/*============== D E F I N E S ===============*/
/*============= E X T E R N A L S ============*/
/*============= E N U M E R A T O R S ============*/

#include "adBms2950GenericType.h"
#include "bms_test.h"

/**
*******************************************************************************
* Function: spiSendCmd
* @brief Send command over SPI
*
* @details This function send bms command in spi line
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *ic      cell_asic_2950 structure pointer
*
* @param [in]  tx_cmd   Tx command byte array
*
* @return None
*
*******************************************************************************
*/
void spiSendCmd2950(uint8_t tIC, cell_asic_2950 *ic, uint8_t tx_cmd[2])
{
  uint8_t cmd[4];
  uint16_t cmd_pec;
  cmd[0] = tx_cmd[0];
  cmd[1] =  tx_cmd[1];
  cmd_pec = Pec15_Calc(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);
  /* SRST & RSTCC cmd not increment command counter */
  if((cmd[1] == 0x2E)|| (cmd[1] == 0x27)) { }
  else { adBms2950_IncCalCmdCounter(tIC, ic); }
#ifdef ISOSPI_WAKEUP  
  adBmsWakeupIc(tIC);
#endif
  adBmsCsLow();
  spiWriteBytes(4, &cmd[0]);
  adBmsCsHigh();
  
  if(tx_cmd == SRST)
  {
    Delay_us(RegulatorStartupTime_us);
  }  
}

/**
*******************************************************************************
* Function: adBms2950ReadData
* @brief Adbms Read Data From Bms ic.
*
* @details This function sends the command read payload, parse data put it in structure and check pec error.(Ex: RDCFGA)
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *ic      cell_asic_2950 stucture pointer
*
* @param [in]  cmd_arg   command bytes
*
* @param [in]  DATA_TYPE   Enum type of resistor
*
* @param [in]  REG_GRP   Enum type of resistor group
*
* @return None
*
*******************************************************************************
*/
void adBms2950ReadData(uint8_t tIC, cell_asic_2950 *ic, uint8_t cmd_arg[2], DATA_TYPE type, REG_GRP group)
{
  #ifndef TESTBENCH
  uint16_t rBuff_size;
  uint8_t regData_size;
  if(group == GRP_ALL)
  {
    if(type == Rdalli){rBuff_size = RDALLI_SIZE; regData_size = RDALLI_SIZE;}
    else if(type == Rdalla){rBuff_size = RDALLA_SIZE; regData_size = RDALLA_SIZE;}
    else if(type == Rdallc){rBuff_size = RDALLC_SIZE; regData_size = RDALLC_SIZE;}
    else if(type == Rdallv){rBuff_size = RDALLV_SIZE; regData_size = RDALLV_SIZE;}
    else if(type == Rdallr){rBuff_size = RDALLR_SIZE; regData_size = RDALLR_SIZE;}
    else if(type == Rdallx){rBuff_size = RDALLX_SIZE; regData_size = RDALLX_SIZE;}  
    else
    {
      printf("Read All cmd wrong type select");
    }
  }
  else{rBuff_size = (tIC * RX_DATA); regData_size = RX_DATA;}
  uint8_t *read_buffer, *pec_error, *cmd_count;
  read_buffer = (uint8_t *)calloc(rBuff_size, sizeof(uint8_t));
  pec_error = (uint8_t *)calloc(tIC, sizeof(uint8_t));
  cmd_count = (uint8_t *)calloc(tIC, sizeof(uint8_t));
  if((pec_error == NULL) || (cmd_count == NULL) || (read_buffer == NULL))
  {
    printf("Failed to allocate memory");
    exit(0);
  }
  else
  {
    spiReadData(tIC, &cmd_arg[0], &read_buffer[0], &pec_error[0], &cmd_count[0], regData_size);
    switch (type)
    {
    case Conf:
      adBms2950ParseConfig(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.cfgr_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
        ic[cic].cccrc.cc_error  = adBms2950_CCErrorCheck(ic[cic].cccrc.cmd_cntr, ic[cic].cccrc.cal_cmd_cntr);
      }
      break;

    case GPV1:
      adBms2950V1ParseData(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.vr_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
        ic[cic].cccrc.cc_error  = adBms2950_CCErrorCheck(ic[cic].cccrc.cmd_cntr, ic[cic].cccrc.cal_cmd_cntr);
      }
      break;

    case GPV2:
      adBms2950V2ParseData(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.rvr_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
        ic[cic].cccrc.cc_error  = adBms2950_CCErrorCheck(ic[cic].cccrc.cmd_cntr, ic[cic].cccrc.cal_cmd_cntr);
      }
      break;

    case Cr:
      adBms2950ParseCurrentRegData(tIC, ic, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.cr_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
        ic[cic].cccrc.cc_error  = adBms2950_CCErrorCheck(ic[cic].cccrc.cmd_cntr, ic[cic].cccrc.cal_cmd_cntr);
      }
      break;

    case Vbat:
      adBms2950ParseVbatRegData(tIC, ic, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.vbat_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
        ic[cic].cccrc.cc_error  = adBms2950_CCErrorCheck(ic[cic].cccrc.cmd_cntr, ic[cic].cccrc.cal_cmd_cntr);
      }
      break;

    case Ivbat:
      adBms2950ParseIVbatRegData(tIC, ic, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.ivbat_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
        ic[cic].cccrc.cc_error  = adBms2950_CCErrorCheck(ic[cic].cccrc.cmd_cntr, ic[cic].cccrc.cal_cmd_cntr);
      }
      break;

    case Oc:
      adBms2950ParseOcRegData(tIC, ic, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.oc_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
        ic[cic].cccrc.cc_error  = adBms2950_CCErrorCheck(ic[cic].cccrc.cmd_cntr, ic[cic].cccrc.cal_cmd_cntr);
      }
      break;

    case AccCr:
      adBms2950ParseAccCurrentRegData(tIC, ic, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.avgcr_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
        ic[cic].cccrc.cc_error  = adBms2950_CCErrorCheck(ic[cic].cccrc.cmd_cntr, ic[cic].cccrc.cal_cmd_cntr);
      }
      break;

    case AccVbat:
      adBms2950ParseAccVbatRegData(tIC, ic, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.avgvbat_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
        ic[cic].cccrc.cc_error  = adBms2950_CCErrorCheck(ic[cic].cccrc.cmd_cntr, ic[cic].cccrc.cal_cmd_cntr);
      }
      break;

    case AccIvbat:
      adBms2950ParseAccIVbatRegData(tIC, ic, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.avgivbat_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
        ic[cic].cccrc.cc_error  = adBms2950_CCErrorCheck(ic[cic].cccrc.cmd_cntr, ic[cic].cccrc.cal_cmd_cntr);
      }
      break;
      
    case aux:
      adBms2950ParseAux(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.aux_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
        ic[cic].cccrc.cc_error  = adBms2950_CCErrorCheck(ic[cic].cccrc.cmd_cntr, ic[cic].cccrc.cal_cmd_cntr);
      }
      break;
      
    case Flag:
      adBms2950ParseFlag(tIC, ic, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.flag_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
        ic[cic].cccrc.cc_error  = adBms2950_CCErrorCheck(ic[cic].cccrc.cmd_cntr, ic[cic].cccrc.cal_cmd_cntr);
      }
      break;

    case status:
      adBms2950ParseStatus(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.stat_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
        ic[cic].cccrc.cc_error  = adBms2950_CCErrorCheck(ic[cic].cccrc.cmd_cntr, ic[cic].cccrc.cal_cmd_cntr);
      }
      break;

    case comm:
      adBms2950ParseComm(tIC, ic, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.comm_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
        ic[cic].cccrc.cc_error  = adBms2950_CCErrorCheck(ic[cic].cccrc.cmd_cntr, ic[cic].cccrc.cal_cmd_cntr);
      }
      break;

    case SID:
      adBms2950ParseSID(tIC, ic, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.sid_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
        ic[cic].cccrc.cc_error  = adBms2950_CCErrorCheck(ic[cic].cccrc.cmd_cntr, ic[cic].cccrc.cal_cmd_cntr);
      }
      break;
      
    case Rdalli:
      adBms2950ParseRdalli(tIC, ic, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.rdalli_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
        ic[cic].cccrc.cc_error  = adBms2950_CCErrorCheck(ic[cic].cccrc.cmd_cntr, ic[cic].cccrc.cal_cmd_cntr);
      }
      break;

    case Rdalla:
      adBms2950ParseRdalla(tIC, ic, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.rdalla_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
        ic[cic].cccrc.cc_error  = adBms2950_CCErrorCheck(ic[cic].cccrc.cmd_cntr, ic[cic].cccrc.cal_cmd_cntr);
      }
      break;     

    case Rdallc:
      adBms2950ParseRdallc(tIC, ic, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.rdallc_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
        ic[cic].cccrc.cc_error  = adBms2950_CCErrorCheck(ic[cic].cccrc.cmd_cntr, ic[cic].cccrc.cal_cmd_cntr);
      }
      break;

    case Rdallv:
      adBms2950ParseRdallv(tIC, ic, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.rdallv_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
        ic[cic].cccrc.cc_error  = adBms2950_CCErrorCheck(ic[cic].cccrc.cmd_cntr, ic[cic].cccrc.cal_cmd_cntr);
      }
      break; 

  case Rdallr:
      adBms2950ParseRdallr(tIC, ic, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.rdallr_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
        ic[cic].cccrc.cc_error  = adBms2950_CCErrorCheck(ic[cic].cccrc.cmd_cntr, ic[cic].cccrc.cal_cmd_cntr);
      }
      break;
      
    case Rdallx:
      adBms2950ParseRdallx(tIC, ic, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.rdallr_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
        ic[cic].cccrc.cc_error  = adBms2950_CCErrorCheck(ic[cic].cccrc.cmd_cntr, ic[cic].cccrc.cal_cmd_cntr);
      }
      break;

    default:
      break;
    }
  }
  free(read_buffer);
  free(pec_error);
  free(cmd_count);
  #else

  #endif
}
/**
*******************************************************************************
* Function: adBms2950WriteData
* @brief Adbms Write Data into Bms ic.
*
* @details This function write the data to DUT. (EX: WRCFGA)
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *ic      cell_asic_2950 stucture pointer
*
* @param [in]  cmd_arg   command bytes
*
* @param [in]  DATA_TYPE   Enum type of resistor
*
* @param [in]  REG_GRP   Enum type of resistor group
*
* @return None
*
*******************************************************************************
*/
void adBms2950WriteData(uint8_t tIC, cell_asic_2950 *ic, uint8_t cmd_arg[2], DATA_TYPE type, REG_GRP group)
{
  uint8_t data_len = TX_DATA, write_size = (TX_DATA * tIC);
  uint8_t *write_buffer = (uint8_t *)calloc(write_size, sizeof(uint8_t));
  if(write_buffer == NULL)
  {
    printf("Failed to allocate write_buffer array memory");
    exit(0);
  }
  else
  {
    switch (type)
    {
    case Conf:
      switch (group)
      {
      case GRP_A:
        adBms2950CreateConfiga(tIC, &ic[0]);
        for (uint8_t cic = 0; cic < tIC; cic++)
        {
          if ( ic[cic].cccrc.cal_cmd_cntr  == 63)
          {
            ic[cic].cccrc.cal_cmd_cntr  = 0;
          }
          ++ ic[cic].cccrc.cal_cmd_cntr ;
          for (uint8_t data = 0; data < data_len; data++)
          {
            write_buffer[(cic * data_len) + data] = ic[cic].configa.tx_data[data];
          }
        }
        break;
      case GRP_B:
        adBms2950CreateConfigb(tIC, &ic[0]);
        for (uint8_t cic = 0; cic < tIC; cic++)
        {
          if ( ic[cic].cccrc.cal_cmd_cntr  == 63)
          {
            ic[cic].cccrc.cal_cmd_cntr  = 0;
          }
          ++ ic[cic].cccrc.cal_cmd_cntr ;
          for (uint8_t data = 0; data < data_len; data++)
          {
            write_buffer[(cic * data_len) + data] = ic[cic].configb.tx_data[data];
          }
        }
        break;
      }
      break;

    case comm:
      adBms2950CreateComm(tIC, &ic[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        if ( ic[cic].cccrc.cal_cmd_cntr  == 63)
        {
          ic[cic].cccrc.cal_cmd_cntr  = 0;
        }
        ++ ic[cic].cccrc.cal_cmd_cntr ;
        for (uint8_t data = 0; data < data_len; data++)
        {
          write_buffer[(cic * data_len) + data] = ic[cic].com.tx_data[data];
        }
      }
      break;

    case clrflag:
      //adBms2950CreateClrflagData(tIC, &ic[0]);
      ic[0].clrflag.tx_data[0] = 0xFF;
      ic[0].clrflag.tx_data[1] = 0xFF;
      ic[0].clrflag.tx_data[2] = 0xFF;
      ic[0].clrflag.tx_data[3] = 0xFF;
      ic[0].clrflag.tx_data[4] = 0xFF;
      ic[0].clrflag.tx_data[5] = 0xFF;
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        if ( ic[cic].cccrc.cal_cmd_cntr  == 63)
        {
          ic[cic].cccrc.cal_cmd_cntr  = 0;
        }
        ++ ic[cic].cccrc.cal_cmd_cntr ;
        for (uint8_t data = 0; data < data_len; data++)
        {
          write_buffer[(cic * data_len) + data] = ic[cic].clrflag.tx_data[data];
        }
      }
      break;
    case Clrflag_OC:
      //adBms2950CreateClrflagData(tIC, &ic[0]);
      ic[0].clrflag.tx_data[0] = 0x0B;
      ic[0].clrflag.tx_data[1] = 0x0B;
      ic[0].clrflag.tx_data[2] = 0x00;
      ic[0].clrflag.tx_data[3] = 0x00;
      ic[0].clrflag.tx_data[4] = 0x00;
      ic[0].clrflag.tx_data[5] = 0x00;
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        if ( ic[cic].cccrc.cal_cmd_cntr  == 63)
        {
          ic[cic].cccrc.cal_cmd_cntr  = 0;
        }
        ++ ic[cic].cccrc.cal_cmd_cntr ;
        for (uint8_t data = 0; data < data_len; data++)
        {
          write_buffer[(cic * data_len) + data] = ic[cic].clrflag.tx_data[data];
        }
      }
      break;

    default:
      printf("I DON'T KNOW THIS DATA_TYPE");
      break;
    }
  }
#ifdef ISOSPI_WAKEUP  
  adBmsWakeupIc(tIC);
#endif
  spiWriteData(tIC, cmd_arg, &write_buffer[0]);
  free(write_buffer);
}

/**
*******************************************************************************
* Function: adBms2950PollAdc
* @brief PLADC Command.
*
* @details Send poll adc command and retun adc conversion count.
*
* Parameters:
*
* @param [in]  tIC      Total IC
*
* @param [in]  *ic      cell_asic_2950 structure pointer
*
* @param [in]  tx_cmd   Tx command byte
*
* @return Pladc_count(uint32_t)
*
*******************************************************************************
*/
uint32_t adBms2950PollAdc(uint8_t tIC, cell_asic_2950 *ic, uint8_t tx_cmd[2])
{
  volatile uint32_t pladc_count = 0;
  uint8_t cmd[4];
  uint16_t cmd_pec;
  uint8_t read_data = 0x00;
  uint8_t SDO_Line = 0xFF;
  cmd[0] = tx_cmd[0];
  cmd[1] = tx_cmd[1];
  cmd_pec = Pec15_Calc(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);
  adBms2950_IncCalCmdCounter(tIC, ic);  
  adBmsCsLow();
  spiWriteBytes(4, &cmd[0]);
  startTimer();
  do{
    spiReadBytes(1, &read_data);
  }while(!(read_data == SDO_Line));
  pladc_count = getTimCount();
  adBmsCsHigh();
  stopTimer(); 
  return(pladc_count);
}

/**
*******************************************************************************
* Function: adBms2950_CCErrorCheck
* @brief Command Counter Error Check
*
* @details Command counter error check.
*
* Parameters:
* @param [in]	ic_cc      Received command counter value
*
* @param [in]  cal_cc     Calculated command counter value
*
* @return cc_error(flag)
*
*******************************************************************************
*/
uint8_t adBms2950_CCErrorCheck(uint8_t ic_cc, uint8_t cal_cc)
{
  uint8_t cc_flag = 1;
   if(ic_cc  == cal_cc)
   {
     cc_flag = 0;
   }
   return cc_flag;
}

/** @}*/
/** @}*/