/*******************************************************************************
Copyright (c) 2020 - Analog Devices Inc. All Rights Reserved.
This software is proprietary & confidential to Analog Devices, Inc.
and its licensor.
******************************************************************************
* @file:    Adbms6830GenericType.c
* @brief:   Generic function driver file
* @version: $Revision$
* @date:    $Date$
* Developed by: ADIBMS Software team, Bangalore, India
*****************************************************************************/
/*! \addtogroup BMS DRIVER
*  @{
*/

/*! @addtogroup GENERIC_TYPE GENERIC TYPE
*  @{
*
This documentation provides details about BMS driver APIs and their usage.
Using the BMS Driver Application can:
- Read/Write the configuration registers of the BMS devices stacked in daisy chaining.
- Send commands and Read the Cell Voltages, Aux Voltages and Status registers (Sum of cells, Internal Die temperature etc.)

*/
#include "adBms6830GenericType.h"

/**
*******************************************************************************
* Function: spiSendCmd
* @brief Send command in spi line
*
* @details This function send bms command in spi line
*	   
* Parameters:
* @param [in]	tx_cmd	Tx command bytes
*
* @return None
*
*******************************************************************************
*/
void spiSendCmd6830(uint8_t tx_cmd[2])
{
  uint8_t cmd[4];
  uint16_t cmd_pec;	
  cmd[0] = tx_cmd[0];
  cmd[1] =  tx_cmd[1];
  cmd_pec = Pec15_Calc(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);
  adBmsCsLow();
  spiWriteBytes(4, &cmd[0]);
  adBmsCsHigh();
}

/**
*******************************************************************************
* Function: adBms6830ReadData
* @brief Adbms Read Data From Bms ic. 
*
* @details This function send bms command, read payload data parse into function and check pec error.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *ic      cell_asic_6830 stucture pointer
*
* @param [in]  cmd_arg   command bytes
*			
* @param [in]  TYPE   Enum type of resistor  
*
* @param [in]  GRP   Enum type of resistor group
*	
* @return None 
*
*******************************************************************************
*/
void adBms6830ReadData(uint8_t tIC, cell_asic_6830 *ic, uint8_t cmd_arg[2], TYPE type, GRP group)
{
  uint16_t rBuff_size;
  uint8_t regData_size;
  if(group == ALL_GRP)
  {
    if(type == Rdcvall){rBuff_size = RDCVALL_SIZE; regData_size = RDCVALL_SIZE;}
    else if(type == Rdsall){rBuff_size = RDSALL_SIZE; regData_size = RDSALL_SIZE;}
    else if(type == Rdacsall){rBuff_size = RDACSALL_SIZE; regData_size = RDACSALL_SIZE;}
    else if(type == Rdacall){rBuff_size = RDACALL_SIZE; regData_size = RDACALL_SIZE;}
    else if(type == Rdfcall){rBuff_size = RDFCALL_SIZE; regData_size = RDFCALL_SIZE;}
    else if(type == Rdcsall){rBuff_size = RDCSALL_SIZE; regData_size = RDCSALL_SIZE;}
    else if(type == Rdasall){rBuff_size = RDASALL_SIZE; regData_size = RDASALL_SIZE;}
    else{printf("Read All cmd wrong type select \n");}
  }
  else{rBuff_size = (tIC * RX_DATA); regData_size = RX_DATA;}
  uint8_t *read_buffer, *pec_error, *cmd_count;
  read_buffer = (uint8_t *)calloc(rBuff_size, sizeof(uint8_t));
  pec_error = (uint8_t *)calloc(tIC, sizeof(uint8_t)); 
  cmd_count = (uint8_t *)calloc(tIC, sizeof(uint8_t));
  if((pec_error == NULL) || (cmd_count == NULL) || (read_buffer == NULL))
  {
    printf(" Failed to allocate memory \n");
    exit(0);
  }
  else
  {
    spiReadData(tIC, &cmd_arg[0], &read_buffer[0], &pec_error[0], &cmd_count[0], regData_size);   
    switch (type)
    {
    case Config:
      adBms6830ParseConfig(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {							
        ic[cic].cccrc.cfgr_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;
      
    case Cell:
      adBms6830ParseCell(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {							
        ic[cic].cccrc.cell_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];    
      }
      break;
      
    case AvgCell:
      adBms6830ParseAverageCell(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {							
        ic[cic].cccrc.acell_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;
      
    case S_volt:
      adBms6830ParseSCell(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {							
        ic[cic].cccrc.scell_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;
      
    case F_volt:
      adBms6830ParseFCell(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {							
        ic[cic].cccrc.fcell_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;
      
    case Aux:
      adBms6830ParseAux(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.aux_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;
      
    case RAux:
      adBms6830ParseRAux(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.raux_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;
      
    case Status:
      adBms6830ParseStatus(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.stat_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;
      
    case Comm:
      adBms6830ParseComm(tIC, ic, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.comm_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;
      
    case Pwm:
      adBms6830ParsePwm(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.pwm_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;
      
    case Sid:
      adBms6830ParseSID(tIC, ic, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.sid_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;
      
    case Rdcvall:
      /* 32 byte cell data + 2 byte pec */
      adBms6830ParseCell(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {							
        ic[cic].cccrc.cell_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;
      
    case Rdacall:
      /* 32 byte avg cell data + 2 byte pec */
      adBms6830ParseAverageCell(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {							
        ic[cic].cccrc.acell_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;
      
    case Rdsall:
      /* 32 byte scell volt data + 2 byte pec */
      adBms6830ParseSCell(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {							
        ic[cic].cccrc.scell_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;
      
    case Rdfcall:
      /* 32 byte fcell data + 2 byte pec */
      adBms6830ParseFCell(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {							
        ic[cic].cccrc.fcell_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;
      
    case Rdcsall:
      /* 64 byte + 2 byte pec = 32 byte cell data + 32 byte scell volt data */
      adBms6830ParseCell(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {							
        ic[cic].cccrc.cell_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      adBms6830ParseSCell(tIC, ic, group, &read_buffer[32]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {							
        ic[cic].cccrc.scell_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;
      
    case Rdacsall:
      /* 64 byte + 2 byte pec = 32 byte avg cell data + 32 byte scell volt data */
      adBms6830ParseAverageCell(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {							
        ic[cic].cccrc.acell_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      adBms6830ParseSCell(tIC, ic, group, &read_buffer[32]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {							
        ic[cic].cccrc.scell_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;
      
    case Rdasall:
      /* 68 byte + 2 byte pec: 
      24 byte gpio data + 20 byte Redundant gpio data +
      24 byte status A(6 byte), B(6 byte), C(4 byte), D(6 byte) & E(2 byte)
      */
      adBms6830ParseAux(tIC, ic, group, &read_buffer[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.aux_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      adBms6830ParseRAux(tIC, ic, group, &read_buffer[24]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.raux_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      adBms6830ParseStatus(tIC, ic, group, &read_buffer[44]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        ic[cic].cccrc.stat_pec = pec_error[cic];
        ic[cic].cccrc.cmd_cntr = cmd_count[cic];
      }
      break;
      
    default:
      break;
    }
  }
  free(read_buffer);
  free(pec_error); 
  free(cmd_count); 
}
/**
*******************************************************************************
* Function: adBms6830WriteData
* @brief Adbms Write Data into Bms ic. 
*
* @details This function write the data into bms ic.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *ic      cell_asic_6830 stucture pointer
*
* @param [in]  cmd_arg   command bytes
*			
* @param [in]  TYPE   Enum type of resistor  
*
* @param [in]  GRP   Enum type of resistor group
*	
* @return None 
*
*******************************************************************************
*/
void adBms6830WriteData(uint8_t tIC, cell_asic_6830 *ic, uint8_t cmd_arg[2], TYPE type, GRP group)
{	  
  uint8_t data_len = TX_DATA, write_size = (TX_DATA * tIC);
  uint8_t *write_buffer = (uint8_t *)calloc(write_size, sizeof(uint8_t));
  if(write_buffer == NULL)
  {
    printf(" Failed to allocate write_buffer array memory \n");
    exit(0);
  }
  else
  {
    switch (type)
    {	   
    case Config:	
      switch (group)
      {
      case A:
        adBms6830CreateConfiga(tIC, &ic[0]);
        for (uint8_t cic = 0; cic < tIC; cic++)
        {
          for (uint8_t data = 0; data < data_len; data++)
          {
            write_buffer[(cic * data_len) + data] = ic[cic].configa.tx_data[data];
          }
        }
        break;
      case B:
        adBms6830CreateConfigb(tIC, &ic[0]);
        for (uint8_t cic = 0; cic < tIC; cic++)
        {
          for (uint8_t data = 0; data < data_len; data++)
          {
            write_buffer[(cic * data_len) + data] = ic[cic].configb.tx_data[data];
          }		
        }
        break;
      }
      break;
      
    case Comm:
      adBms6830CreateComm(tIC, &ic[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        for (uint8_t data = 0; data < data_len; data++)
        {
          write_buffer[(cic * data_len) + data] = ic[cic].com.tx_data[data];
        }	
      }
      break;
      
    case Pwm:
      switch (group)
      {
      case A:
        adBms6830CreatePwma(tIC, &ic[0]);
        for (uint8_t cic = 0; cic < tIC; cic++)
        {
          for (uint8_t data = 0; data < data_len; data++)
          {
            write_buffer[(cic * data_len) + data] = ic[cic].pwma.tx_data[data];
          }	
        }
        break;   
      case B:
        adBms6830CreatePwmb(tIC, &ic[0]);
        for (uint8_t cic = 0; cic < tIC; cic++)
        {
          for (uint8_t data = 0; data < data_len; data++)
          {
            write_buffer[(cic * data_len) + data] = ic[cic].pwmb.tx_data[data];
          }	
        }
        break;
      }
      break;
      
    case Clrflag:	
      adBms6830CreateClrflagData(tIC, &ic[0]);
      for (uint8_t cic = 0; cic < tIC; cic++)
      {
        for (uint8_t data = 0; data < data_len; data++)
        {
          write_buffer[(cic * data_len) + data] = ic[cic].clrflag.tx_data[data];
        }
      }
      break;
      
    default:
      break;
    }
  }
  spiWriteData(tIC, cmd_arg, &write_buffer[0]);	
  free(write_buffer);
}

/**
*******************************************************************************
* Function: adBmsPollAdc
* @brief PLADC Command. 
*
* @details Send poll adc command and retun adc conversion count.
*
* Parameters:
*	
* @param [in]  tIC      Total IC 
*
* @param [in]  tx_cmd   Tx command byte 
*
* @return None 
*
*******************************************************************************
*/
uint32_t adBmsPollAdc(uint8_t tx_cmd[2])
{
  uint32_t conv_count = 0;
  uint8_t cmd[4];
  uint16_t cmd_pec;
  uint8_t read_data = 0x00;
  uint8_t SDO_Line = 0xFF;
  cmd[0] = tx_cmd[0];
  cmd[1] = tx_cmd[1];
  cmd_pec = Pec15_Calc(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);
  startTimer();
  adBmsCsLow();
  spiWriteBytes(4, &cmd[0]);
  do{
    spiReadBytes(1, &read_data);
  }while(!(read_data == SDO_Line));
  adBmsCsHigh();
  conv_count = getTimCount();
  stopTimer();
  return(conv_count);
}

/**
*******************************************************************************
* Function: adBms6830_Adcv
* @brief ADCV Command. 
*
* @details Send adcv command to start cell voltage conversion.
*
* Parameters:
*
* @param [in]  RD      Enum type Read bit
*
* @param [in]  CONT   Enum type continuous measurement bit 
*			
* @param [in]  DCP   Enum type discharge bit  
*
* @param [in]  RSTF   Enum type Reset filter
*	
* @param [in]  OW_C_S   Enum type open wire c/s
*
* @return None 
*
*******************************************************************************
*/
void adBms6830_Adcv
(
RD rd,
CONT cont,
DCP dcp,
RSTF rstf,
OW_C_S owcs
)
{
  uint8_t cmd[2];
  cmd[0] = 0x02 + rd;
  cmd[1] = (cont<<7)+(dcp<<4)+(rstf<<2)+(owcs & 0x03) + 0x60;
  spiSendCmd6830(cmd);
}

/**
*******************************************************************************
* Function: adBms6830_Adsv
* @brief ADSV Command. 
*
* @details Send s_adcv command to start cell voltage conversion.
*
* Parameters:
*
* @param [in]  cont    Enum type continuous measurement bit
*
* @param [in]  dcp    Enum type discharge bit
*			
* @param [in]  owcs   Enum type open wire c/s  
*
* @return None 
*
*******************************************************************************
*/
void adBms6830_Adsv
(
CONT cont,
DCP dcp,
OW_C_S owcs
)
{
  uint8_t cmd[2];
  cmd[0] = 0x01;
  cmd[1] = (cont<<7)+(dcp<<4)+(owcs &0x03) + 0x68;
  spiSendCmd6830(cmd);
}

/**
*******************************************************************************
* Function: adBms6830_Adax
* @brief ADAX Command. 
*
* @details Send Aux command to starts auxiliary conversion.
*
* Parameters:
*
* @param [in]  owcs    Enum type open wire c/s
*
* @param [in]  pup    Enum type Pull Down current during aux conversion
*			
* @param [in]  ch    Enum type gpio Channel selection  
*
* @return None 
*
*******************************************************************************
*/
void adBms6830_Adax
(
OW_AUX owaux, 							
PUP pup,
CH ch
)
{
  uint8_t cmd[2];
  cmd[0] = 0x04 + owaux;
  cmd[1] = (pup << 7) + (((ch >>4)&0x01)<<6) + (ch & 0x0F) + 0x10;
  spiSendCmd6830(cmd);
}
/**
*******************************************************************************
* Function: adBms6830_Adax2
* @brief ADAX2 Command. 
*
* @details Send Aux2 command to starts auxiliary conversion.
*
* Parameters:
*	
* @param [in]  ch    Enum type gpio Channel selection  
*
* @return None 
*
*******************************************************************************
*/
void adBms6830_Adax2
(
CH ch
)
{
  uint8_t cmd[2];
  cmd[0] = 0x04;
  cmd[1] = (ch & 0x0F);
  spiSendCmd6830(cmd);
}

/** @}*/
/** @}*/