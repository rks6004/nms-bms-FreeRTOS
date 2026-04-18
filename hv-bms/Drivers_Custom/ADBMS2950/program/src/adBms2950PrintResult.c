/**
********************************************************************************
*
* @file:    print_result.c
*
* @brief:   This file contains the print result function implementation.
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
/*! \addtogroup Main
*  @{
*/

/*! \addtogroup Print_Result
*  @{
*/

#include "adBms2950PrintResult.h"


/**
*******************************************************************************
* Function: printWriteConfig2950
* @brief Print write config A/B result.
*
* @details This function Print write config result into terminal.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *IC      cell_asic_2950 stucture pointer
*
* @param [in]  type     Enum type of resistor
*
* @param [in]  grp      Enum type of resistor group
*
* @return None
*
*******************************************************************************
*/
void printWriteConfig2950(uint8_t tIC, cell_asic_2950 *IC, DATA_TYPE type, REG_GRP grp)
{
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    printf("IC%d:\n", (ic+1));
    if(type == Conf)
    {
      if(grp == GRP_A)
      {
        printf("Write Config A:\n");
        printf("0x%X, ", IC[ic].configa.tx_data[0]);
        printf("0x%X, ", IC[ic].configa.tx_data[1]);
        printf("0x%X, ", IC[ic].configa.tx_data[2]);
        printf("0x%X, ", IC[ic].configa.tx_data[3]);
        printf("0x%X, ", IC[ic].configa.tx_data[4]);
        printf("0x%X\n\n", IC[ic].configa.tx_data[5]);
      }
      else if(grp == GRP_B)
      {
        printf("Write Config B:\n");
        printf("0x%X, ", IC[ic].configb.tx_data[0]);
        printf("0x%X, ", IC[ic].configb.tx_data[1]);
        printf("0x%X, ", IC[ic].configb.tx_data[2]);
        printf("0x%X, ", IC[ic].configb.tx_data[3]);
        printf("0x%X, ", IC[ic].configb.tx_data[4]);
        printf("0x%X\n\n", IC[ic].configb.tx_data[5]);
      }
      else if(grp == GRP_ALL)
      {
        printf("Write Config A:\n");
        printf("0x%X, ", IC[ic].configa.tx_data[0]);
        printf("0x%X, ", IC[ic].configa.tx_data[1]);
        printf("0x%X, ", IC[ic].configa.tx_data[2]);
        printf("0x%X, ", IC[ic].configa.tx_data[3]);
        printf("0x%X, ", IC[ic].configa.tx_data[4]);
        printf("0x%X\n\n", IC[ic].configa.tx_data[5]);

        printf("Write Config B:\n");
        printf("0x%X, ", IC[ic].configb.tx_data[0]);
        printf("0x%X, ", IC[ic].configb.tx_data[1]);
        printf("0x%X, ", IC[ic].configb.tx_data[2]);
        printf("0x%X, ", IC[ic].configb.tx_data[3]);
        printf("0x%X, ", IC[ic].configb.tx_data[4]);
        printf("0x%X\n\n", IC[ic].configb.tx_data[5]);

        printf("Write Config Comm:\n");
        printf("ICOM:\n");
        printf("0x%X, ", IC[ic].tx_comm.icomm[0]);
        printf("0x%X, ", IC[ic].tx_comm.icomm[1]);
        printf("0x%X, \n", IC[ic].tx_comm.icomm[2]);
        printf("FCOM:\n");
        printf("0x%X, ", IC[ic].tx_comm.fcomm[0]);
        printf("0x%X, ", IC[ic].tx_comm.fcomm[1]);
        printf("0x%X\n", IC[ic].tx_comm.fcomm[2]);
        printf("DATA:\n");
        printf("0x%X, ", IC[ic].tx_comm.data[0]);
        printf("0x%X, ", IC[ic].tx_comm.data[1]);
        printf("0x%X, \n\n", IC[ic].tx_comm.data[2]);
    
      }
      else{ printf("Wrong Register Group Select\n"); }
    }
  }
}

/**
*******************************************************************************
* Function: printReadConfig2950
* @brief Print Read config A/B result.
*
* @details This function Print read config result into IAR I/O terminal.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *ic      cell_asic_2950 stucture pointer
*
* @param [in]  DATA_TYPE     Enum type of resistor
*
* @param [in]  REG_GRP      Enum type of resistor group
*
* @return None
*
*******************************************************************************
*/
void printReadConfig2950(uint8_t tIC, cell_asic_2950 *IC, DATA_TYPE type, REG_GRP grp)
{
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    printf("IC%d:\n",(ic+1));
    if(type == Conf)
    {
      if(grp == GRP_A)
      {
        printf("Read Config A:\n");
        printf("0x%X, ", IC[ic].configa.rx_data[0]);
        printf("0x%X, ", IC[ic].configa.rx_data[1]);
        printf("0x%X, ", IC[ic].configa.rx_data[2]);
        printf("0x%X, ", IC[ic].configa.rx_data[3]);
        printf("0x%X, ", IC[ic].configa.rx_data[4]);
        printf("0x%X, ", IC[ic].configa.rx_data[5]);
        printf("CCount:%d, ",IC[ic].cccrc.cmd_cntr);
        printf("PECError:%d\n\n",IC[ic].cccrc.cfgr_pec);
      }
      else if(grp == GRP_B)
      {
        printf("Read Config B:\n");
        printf("0x%X, ", IC[ic].configb.rx_data[0]);
        printf("0x%X, ", IC[ic].configb.rx_data[1]);
        printf("0x%X, ", IC[ic].configb.rx_data[2]);
        printf("0x%X, ", IC[ic].configb.rx_data[3]);
        printf("0x%X, ", IC[ic].configb.rx_data[4]);
        printf("0x%X, ", IC[ic].configb.rx_data[5]);
        printf("CCount:%d, ",IC[ic].cccrc.cmd_cntr);
        printf("PECError:%d\n\n",IC[ic].cccrc.cfgr_pec);
      }
      else if(grp == GRP_ALL)
      {
        printf("Read Config A:\n");
        printf("0x%X, ", IC[ic].configa.rx_data[0]);
        printf("0x%X, ", IC[ic].configa.rx_data[1]);
        printf("0x%X, ", IC[ic].configa.rx_data[2]);
        printf("0x%X, ", IC[ic].configa.rx_data[3]);
        printf("0x%X, ", IC[ic].configa.rx_data[4]);
        printf("0x%X, \n", IC[ic].configa.rx_data[5]);
        // printf("CCount:%d, ",IC[ic].cccrc.cmd_cntr);
        // printf("PECError:%d\n",IC[ic].cccrc.cfgr_pec);

        printf("Read Config B:\n");
        printf("0x%X, ", IC[ic].configb.rx_data[0]);
        printf("0x%X, ", IC[ic].configb.rx_data[1]);
        printf("0x%X, ", IC[ic].configb.rx_data[2]);
        printf("0x%X, ", IC[ic].configb.rx_data[3]);
        printf("0x%X, ", IC[ic].configb.rx_data[4]);
        printf("0x%X, \n", IC[ic].configb.rx_data[5]);
        // printf("CCount:%d, ",IC[ic].cccrc.cmd_cntr);
        // printf("PECError:%d\n\n",IC[ic].cccrc.cfgr_pec);

        printf("Read Config Comm:\n");
        printf("ICOM:\n");
        printf("0x%X, ", IC[ic].rx_comm.icomm[0]);
        printf("0x%X, ", IC[ic].rx_comm.icomm[1]);
        printf("0x%X, \n", IC[ic].rx_comm.icomm[2]);
        printf("FCOM:\n");
        printf("0x%X, ", IC[ic].rx_comm.fcomm[0]);
        printf("0x%X, ", IC[ic].rx_comm.fcomm[1]);
        printf("0x%X\n", IC[ic].rx_comm.fcomm[2]);
        printf("DATA:\n");
        printf("0x%X, ", IC[ic].rx_comm.data[0]);
        printf("0x%X, ", IC[ic].rx_comm.data[1]);
        printf("0x%X, ", IC[ic].rx_comm.data[2]);

        printf("CCount:%d, ",IC[ic].cccrc.cmd_cntr);
        printf("PECError:%d\n\n",IC[ic].cccrc.cfgr_pec);
      }
      else{ printf("Wrong Register Group Select\n"); }
    }
  }
}

/**
*******************************************************************************
* Function: printDeviceSID2950
* @brief Print Device SID.
*
* @details This function Print Device SID into IAR I/O terminal.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *IC      cell_asic_2950 stucture pointer
*
* @param [in]  type     Enum type of resistor
*
* @return None
*
*******************************************************************************
*/
void printDeviceSID2950(uint8_t tIC, cell_asic_2950 *IC, DATA_TYPE type)
{
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    printf("IC%d:\n",(ic+1));
    if(type == SID)
    {
      printf("Read Device SID:\n");
      printf("0x%X, ", IC[ic].sid.sid[0]);
      printf("0x%X, ", IC[ic].sid.sid[1]);
      printf("0x%X, ", IC[ic].sid.sid[2]);
      printf("0x%X, ", IC[ic].sid.sid[3]);
      printf("0x%X, ", IC[ic].sid.sid[4]);
      printf("0x%X, ", IC[ic].sid.sid[5]);
      printf("CCount:%d,",IC[ic].cccrc.cmd_cntr);
      printf("PECError:%d\n\n",IC[ic].cccrc.sid_pec);
    }
    else{ printf("Wrong Register Type Select\n"); }
  }
}

/**
*******************************************************************************
* Function: printWriteCommData2950
* @brief Print Write Comm data.
*
* @details This function Print write comm data.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *IC      cell_asic_2950 stucture pointer
*
* @param [in]  type     Enum type of resistor
*
* @return None
*
*******************************************************************************
*/
void printWriteCommData2950(uint8_t tIC, cell_asic_2950 *IC, DATA_TYPE type)
{
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    printf("IC%d:\n",(ic+1));
    if(type == comm)
    {
      printf("Write Comm Data:\n");
      printf("0x%X, ", IC[ic].com.tx_data[0]);
      printf("0x%X, ", IC[ic].com.tx_data[1]);
      printf("0x%X, ", IC[ic].com.tx_data[2]);
      printf("0x%X, ", IC[ic].com.tx_data[3]);
      printf("0x%X, ", IC[ic].com.tx_data[4]);
      printf("0x%X\n\n", IC[ic].com.tx_data[5]);
    }
    else{ printf("Wrong Register Group Select\n"); }
  }
}

/**
*******************************************************************************
* Function: printReadCommData2950
* @brief Print Read Comm Data.
*
* @details This function print read comm data.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *IC      cell_asic_2950 stucture pointer
*
* @param [in]  type     Enum type of resistor
*
* @return None
*
*******************************************************************************
*/
void printReadCommData2950(uint8_t tIC, cell_asic_2950 *IC, DATA_TYPE type)
{
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    printf("IC%d:\n",(ic+1));
    if(type == comm)
    {
      printf("Read Comm Data:\n");
      printf("ICOM0:0x%X, ", IC[ic].rx_comm.icomm[0]);
      printf("ICOM1:0x%X, ", IC[ic].rx_comm.icomm[1]);
      printf("ICOM2:0x%X\n", IC[ic].rx_comm.icomm[2]);
      printf("FCOM0:0x%X, ", IC[ic].rx_comm.fcomm[0]);
      printf("FCOM1:0x%X, ", IC[ic].rx_comm.fcomm[1]);
      printf("FCOM2:0x%X\n", IC[ic].rx_comm.fcomm[2]);
      printf("DATA0:0x%X, ", IC[ic].rx_comm.data[0]);
      printf("DATA1:0x%X, ", IC[ic].rx_comm.data[1]);
      printf("DATA2:0x%X\n", IC[ic].rx_comm.data[2]);
      printf("CCount:%d,",IC[ic].cccrc.cmd_cntr);
      printf("PECError:%d\n\n",IC[ic].cccrc.comm_pec);
    }
    else{ printf("Wrong Register Type Select\n"); }
  }
}

/**
*******************************************************************************
* Function: printCr2950
* @brief Print Current Result.
*
* @details This function Print current result into terminal.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *ic      cell_asic_2950 stucture pointer
*
* @return None
*
*******************************************************************************
*/
void printCr2950(uint8_t tIC, cell_asic_2950 *IC)
{
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    printf("IC%d:\n",(ic+1));
    printf("Read Current:\n");
    printf("I1(V):%f, ", getCurrent2950(IC[ic].i.i1));
    printf("I2(V):%f\n", getCurrent2950(IC[ic].i.i2));
    printf("CCount:%d, ",IC[ic].cccrc.cmd_cntr);
    printf("PECError:%d\n\n",IC[ic].cccrc.cr_pec);
  }
}

/**
*******************************************************************************
* Function: printVoltages2950
* @brief Print Voltages.
*
* @details This function Print Voltages into terminal.
*
* Parameters:
* @param [in]	tIC    Total IC
*
* @param [in]  *IC    cell_asic_2950 stucture pointer
*
* @param [in]  type    Enum type of resistor group
*
* @return None
*
*******************************************************************************
*/
void printVoltages2950(uint8_t tIC, cell_asic_2950 *IC, DATA_TYPE type)
{
  float voltage;
  uint16_t temp;
  uint8_t channel;

  if((type == GPV1)) { channel = VR_SIZE; }
  else if (type == GPV2) { channel = RVR_SIZE; }

  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    printf("IC%d: \n", (ic+1));
    if(type == GPV1)
    {
      for(uint8_t index = 0; index < 8; index++)
      {
        temp = IC[ic].vr.v_codes[index];
        voltage = getVoltage2950(temp);
        printf("V%dA = %7.4fV, ", (index+1), voltage); 
      } 
      printf("VREF2A = %7.4fV, ", getVoltage2950(IC[ic].vr.v_codes[8])); 
      printf("V7A = %7.4fV, ", getVoltage2950(IC[ic].vr.v_codes[9])); 
      printf("V8A = %7.4fV, ", getVoltage2950(IC[ic].vr.v_codes[10])); 
      printf("V9B = %7.4fV, ", getVoltage2950(IC[ic].vr.v_codes[11])); 
      printf("CCount:%d, ", IC[ic].cccrc.cmd_cntr);
      printf("PECError:%d", IC[ic].cccrc.vr_pec);
    } /* if(type == GPV1)*/
    else if(type == GPV2)
    {
      for(uint8_t index = 0; index < 6; index++)
      {
        temp = IC[ic].rvr.redv_codes[index];
        voltage = getVoltage2950(temp);
        printf("V%dB = %7.4fV, ", (index+1), voltage); 
      }
      printf("V9B = %7.4fV, ", getVoltage2950(IC[ic].rvr.redv_codes[6])); 
      printf("V10B = %7.4fV, ", getVoltage2950(IC[ic].rvr.redv_codes[7])); 
      printf("VREF2B = %7.4fV, ", getVoltage2950(IC[ic].rvr.redv_codes[8])); 
      printf("V10B = %7.4fV, ", getVoltage2950(IC[ic].rvr.redv_codes[9])); 
      printf("VREF2A = %7.4fV, ", getVoltage2950(IC[ic].rvr.redv_codes[10])); 
      printf("VREF2B = %7.4fV, ", getVoltage2950(IC[ic].rvr.redv_codes[11])); 
      printf("V10B = %7.4fV, ", getVoltage2950(IC[ic].rvr.redv_codes[12])); 
      printf("CCount:%d,", IC[ic].cccrc.cmd_cntr);
      printf("PECError:%d", IC[ic].cccrc.rvr_pec);
    } /* (type == GPV2) */
    else { printf("Wrong Register Group Select\n"); }
    printf("\n\n");
  } /* for(... ic < tIC ...) */
}

/**
*******************************************************************************
* Function: printVbat2950
* @brief Print VBAT Result.
*
* @details This function Print the VBAT result into terminal.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *ic      cell_asic_2950 stucture pointer
*
* @return None
*
*******************************************************************************
*/
void printVbat2950(uint8_t tIC, cell_asic_2950 *IC)
{
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    printf("IC%d:\n",(ic+1));
    printf("Read VBAT:\n");
    printf("VBAT1: %f, ", getVoltage2950(IC[ic].vbat.vbat1));
    printf("VBAT2: %f, ", getVoltage2950(IC[ic].vbat.vbat2));
    printf("CCount:%d,",IC[ic].cccrc.cmd_cntr);
    printf("PECError:%d\n\n",IC[ic].cccrc.vbat_pec);
  }
}

/**
*******************************************************************************
* Function: printIvbat2950
* @brief Print IVBAT result.
*
* @details This function Print the IVBAT result into terminal.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *ic      cell_asic_2950 stucture pointer
*
* @return None
*
*******************************************************************************
*/
void printIvbat2950(uint8_t tIC, cell_asic_2950 *IC)
{
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    printf("IC%d:\n",(ic+1));
    printf("Read IVBAT:\n");
    printf("I1: %f, ", getCurrent2950(IC[ic].ivbat.i1));
    printf("VBAT1: %f, ", getVoltage2950(IC[ic].ivbat.vbat1));
    printf("CCount: %d, ",IC[ic].cccrc.cmd_cntr);
    printf("PECError: %d\n\n",IC[ic].cccrc.ivbat_pec);
  }
}

/**
*******************************************************************************
* Function: printAccVbat2950
* @brief Print VBATACC result.
*
* @details This function Print the accumulated result into terminal.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *ic      cell_asic_2950 stucture pointer
*
* @return None
*
*******************************************************************************
*/
void printAccVbat2950(uint8_t tIC, cell_asic_2950 *IC)
{
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    printf("IC%d:\n",(ic+1));
    printf("Read AccVbat:\n");
    printf("VB1ACC :%fV, ", getAccVbat2950(IC[ic].vbacc.vb1acc));
    printf("VB2ACC: %fV, ", getAccVbat2950(IC[ic].vbacc.vb2acc));
    printf("CCount:%d,",IC[ic].cccrc.cmd_cntr);
    printf("PECError:%d\n\n",IC[ic].cccrc.avgvbat_pec);
  }
}

/**
*******************************************************************************
* Function: printAvgAccIVbat2950
* @brief Print I and VBAT results.
*
* @details This function prints the I and VBAT result by taking the average of VBATACC/ACCN
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *ic      cell_asic_2950 stucture pointer
*
* @return None
*
*******************************************************************************
*/
void printAvgAccIVbat2950(uint8_t tIC, cell_asic_2950 *IC)
{
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    printf("IC%d:\n",(ic+1));
    printf("Battery Voltage and Current:\n");
    printf("VBAT1 :%fV, ", getAvgAccVbat2950(IC[ic].vbacc.vb1acc, (uint32_t)(4 * (1 + IC[ic].tx_cfga.acci))));
    //printf("VBAT2 :%fV, ", getAvgAccVbat2950(IC[ic].vbacc.vb2acc, (uint32_t)(4 * (1 + IC[ic].tx_cfga.acci))));
    printf("I1 :%fA, ", getAvgAccCurrent2950(IC[ic].iacc.i1acc, (uint32_t)(4 * (1 + IC[ic].tx_cfga.acci))));
    //printf("I2 :%fA, ", getAvgAccCurrent2950(IC[ic].iacc.i2acc, (uint32_t)(4 * (1 + IC[ic].tx_cfga.acci))));
    printf("CCount:%d,",IC[ic].cccrc.cmd_cntr);
    printf("PECError:%d\n\n",IC[ic].cccrc.avgvbat_pec);
  }
}

/**
*******************************************************************************
* Function: getPackVoltage
* @brief return voltage of pack.
*
* @details This function returns the pack voltage
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *ic      cell_asic_2950 stucture pointer
*
* @return None
*
*******************************************************************************
*/
float getPackVoltage(uint8_t tIC, cell_asic_2950 *IC)
{

  //Create data variable
  float voltage = 0.0;

  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    voltage += getAvgAccVbat2950(IC[ic].vbacc.vb1acc, (uint32_t)(4 * (1 + IC[ic].tx_cfga.acci)));
  }

  //Return average voltage
  return voltage / (float)tIC;
}

/**
*******************************************************************************
* Function: getPackCurrent
* @brief return current of pack.
*
* @details This function returns the pack current
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *ic      cell_asic_2950 stucture pointer
*
* @return None
*
*******************************************************************************
*/
float getPackCurrent(uint8_t tIC, cell_asic_2950 *IC)
{

  //Create data variable
  float current = 0.0;

  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    current += getAvgAccCurrent2950(IC[ic].iacc.i1acc, (uint32_t)(4 * (1 + IC[ic].tx_cfga.acci)));
  }

  //Return average current
  return current / (float)tIC;
}

/**
*******************************************************************************
* Function: printAccIVbat2950
* @brief Print ACCIVBAT result.
*
* @details This function Print the accivbat result into terminal.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *ic      cell_asic_2950 stucture pointer
*
* @return None
*
*******************************************************************************
*/
void printAccIVbat2950(uint8_t tIC, cell_asic_2950 *IC)
{
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    printf("IC%d:\n",(ic+1));
    printf("Read AccIVbat:\n");
    printf("I1ACC: %f, ", getAccCurrent2950(IC[ic].i_vbacc.i1acc));
    printf("VB1ACC: %fV, ", getAccVbat2950(IC[ic].i_vbacc.vb1acc));
    printf("CCount:%d,",IC[ic].cccrc.cmd_cntr);
    printf("PECError:%d\n\n",IC[ic].cccrc.avgivbat_pec);
  }
}

/**
*******************************************************************************
* Function: printAccCr2950
* @brief Print IACC result.
*
* @details This function Print the accumulated result into terminal.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *ic      cell_asic_2950 stucture pointer
*
* @return None
*
*******************************************************************************
*/
void printAccCr2950(uint8_t tIC, cell_asic_2950 *IC)
{
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    printf("IC%d:\n",(ic+1));
    printf("Read AccCr:\n");
    printf("I1ACC: %fmV, ", getAccCurrent2950(IC[ic].iacc.i1acc));
    printf("I2ACC: %fmV, ", getAccCurrent2950(IC[ic].iacc.i2acc));
    printf("CCount:%d,",IC[ic].cccrc.cmd_cntr);
    printf("PECError:%d\n\n",IC[ic].cccrc.avgcr_pec);
  }
}

/**
*******************************************************************************
* Function: printOc2950
* @brief Print OC result.
*
* @details This function Print the oc result into terminal.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *ic      cell_asic_2950 stucture pointer
*
* @return None
*
*******************************************************************************
*/
void printOc2950(uint8_t tIC, cell_asic_2950 *IC)
{
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    printf("Read OCR:\n");
    printf("OC1R: %f(mV), ", getOverCurrent2950(IC[ic].oc.oc1r));
    printf("OC2R: %f(mV), ", getOverCurrent2950(IC[ic].oc.oc2r));
    printf("OC3R: %f(mV), ", getOverCurrent2950(IC[ic].oc.oc3r));
    printf("REFR: %f(mV), ", getOverCurrent2950(IC[ic].oc.refr));
    printf("OC3MAX: %f(mV), ", getOverCurrent2950(IC[ic].oc.oc3max));
    printf("OC3MIN: %f(mV), ", getOverCurrent2950(IC[ic].oc.oc3min));
    printf("CCount:%d, ",IC[ic].cccrc.cmd_cntr);
    printf("PECError:%d\n\n",IC[ic].cccrc.oc_pec);
  }
}

/**
*******************************************************************************
* Function: printRdallv2950
* @brief Print RDALLV result.
*
* @details This function Print the RDALLV result into terminal.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *ic      cell_asic_2950 stucture pointer
*
* @return None
*
*******************************************************************************
*/
void printRdallv2950(uint8_t tIC, cell_asic_2950 *IC)
{
  float voltage;

  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    printf("IC%d: \n",(ic+1));
    voltage = getVoltage2950(IC[ic].rdallv.v1);
    printf("V1A = %7.4fV \n", voltage);
    voltage = getVoltage2950(IC[ic].rdallv.v2);
    printf("V2A = %7.4fV \n", voltage);
    voltage = getVoltage2950(IC[ic].rdallv.v3);
    printf("V3A = %7.4fV \n", voltage);
    voltage = getVoltage2950(IC[ic].rdallv.v4);
    printf("V4A = %7.4fV \n", voltage);
    voltage = getVoltage2950(IC[ic].rdallv.v5);
    printf("V5A = %7.4fV \n", voltage);
    voltage = getVoltage2950(IC[ic].rdallv.v6);
    printf("V6A = %7.4fV \n", voltage);
    voltage = getVoltage2950(IC[ic].rdallv.v7);
    printf("V7A = %7.4fV \n", voltage);
    voltage = getVoltage2950(IC[ic].rdallv.v8);
    printf("V8A = %7.4fV \n", voltage);
    voltage = getVoltage2950(IC[ic].rdallv.v9);
    printf("V9B = %7.4fV \n", voltage);
    voltage = getVoltage2950(IC[ic].rdallv.v10);
    printf("V10B = %7.4fV \n", voltage);
  }
  printf("\n\n");
}

/**
*******************************************************************************
* Function: printRdallr2950
* @brief Print RDALLR result.
*
* @details This function Print the RDALLR result into terminal.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *ic      cell_asic_2950 stucture pointer
*
* @return None
*
*******************************************************************************
*/
void printRdallr2950(uint8_t tIC, cell_asic_2950 *IC)
{
  float voltage;

  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    printf("IC%d: \n",(ic+1));
    voltage = getVoltage2950(IC[ic].rdallr.v1);
    printf("V1B = %7.4fV \n", voltage);
    voltage = getVoltage2950(IC[ic].rdallr.v2);
    printf("V2B = %7.4fV \n", voltage);
    voltage = getVoltage2950(IC[ic].rdallr.v3);
    printf("V3B = %7.4fV \n", voltage);
    voltage = getVoltage2950(IC[ic].rdallr.v4);
    printf("V4B = %7.4fV \n", voltage);
    voltage = getVoltage2950(IC[ic].rdallr.v5);
    printf("V5B = %7.4fV \n", voltage);
    voltage = getVoltage2950(IC[ic].rdallr.v6);
    printf("V6B = %7.4fV \n", voltage);
    // voltage = getVoltage2950(IC[ic].rdallr.v7);
    // printf("V7B = %7.4fV \n", voltage);
    // voltage = getVoltage2950(IC[ic].rdallr.v8);
    // printf("V8B = %7.4fV \n", voltage);
    // voltage = getVoltage2950(IC[ic].rdallr.v9);
    // printf("V9B = %7.4fV \n", voltage);
    // voltage = getVoltage2950(IC[ic].rdallr.v10);
    // printf("V10B = %7.4fV \n", voltage);
  }
  printf("\n\n");
}

/**
*******************************************************************************
* Function: printRdallx2950
* @brief Print RDALLX result.
*
* @details This function Print the RDALLX result into terminal.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *ic      cell_asic_2950 stucture pointer
*
* @return None
*
*******************************************************************************
*/
void printRdallx2950(uint8_t tIC, cell_asic_2950 *IC)
{
  float voltage;

  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    printf("IC%d: \n",(ic+1));
    voltage = getVoltage2950(IC[ic].rdallx.vref2A);
    printf("VREF2A = %fV \n", voltage);
    voltage = getVoltage2950(IC[ic].rdallx.vref2B);
    printf("VREF2B = %fV \n", voltage);
    voltage = getVoltage2950(IC[ic].rdallx.vref1p25);
    printf("VREF1P25 = %fV \n", voltage);
    printf("TMP1 = %fC \n", ((IC[ic].rdallx.tmp1 / 61.8) - 250));
    voltage = getVoltage2950(IC[ic].rdallx.vreg);
    printf("VREG = %fV \n", voltage);
    voltage = getVoltage2950(IC[ic].rdallx.vdd);
    printf("VDD = %fV \n", voltage);
    voltage = getVoltage2950(IC[ic].rdallx.vdig);
    printf("VDIG = %fV \n", voltage);
    voltage = getVoltage2950(IC[ic].rdallx.epad);
    printf("EPAD = %fV \n", voltage);
    voltage = getVoltage2950(IC[ic].rdallx.vdiv);
    printf("VDIV = %fV \n", voltage);
    printf("TMP2 = %fC \n", ((IC[ic].rdallx.tmp2 / 20.5)-267));
  }
  printf("\n\n");
}

/**
*******************************************************************************
* Function: printGpV12950
* @brief Print GPV1 result.
*
* @details This function Print the GPV1 result into terminal.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *ic      cell_asic_2950 stucture pointer
*
* @return None
*
*******************************************************************************
*/
void printGpV12950(uint8_t tIC, cell_asic_2950 *IC)
{
  float voltage;
  uint16_t temp;
  uint8_t channel;

  channel = VR_SIZE;
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    printf("IC%d: \n",(ic+1));
    for(uint8_t index = 0; index < channel; index++)
    {
      temp = IC[ic].vr.v_codes[index];
      voltage = getVoltage2950(temp);
      if(index == 8) { printf("VREF2A = %7.4fV \n", voltage); }
      else { printf("V%2d = %7.4fV \n", (index+1), voltage); }
      if(index == (channel-1))
      {
        printf("CCount:%d,",IC[ic].cccrc.cmd_cntr);
        printf("PECError:%d",IC[ic].cccrc.vr_pec);
      }
    }
  }
  printf("\n\n");
}

/**
*******************************************************************************
* Function: printGpV22950
* @brief Print GPV2 result.
*
* @details This function Print the GPV2 result into terminal.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *ic      cell_asic_2950 stucture pointer
*
* @return None
*
*******************************************************************************
*/
void printGpV22950(uint8_t tIC, cell_asic_2950 *IC)
{
  float voltage;
  uint16_t temp;
  uint8_t channel;

  channel = RVR_SIZE;
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    printf("IC%d: \n",(ic+1));
    for(uint8_t index = 0; index < channel; index++)
    {
      temp = IC[ic].rvr.redv_codes[index];
      voltage = getVoltage2950(temp);
      if(index == 8) { printf("VREF2B = %7.4fV \n", voltage); }
      else if(index == 10) { printf("VREF2A = %7.4fV \n", voltage); }
      else if(index == 11) { printf("VREF2B = %7.4fV \n", voltage); }
      else { printf("V%2d = %7.4fV \n", (index+1), voltage); }
      if(index == (channel-1))
      {
        printf("CCount:%d,",IC[ic].cccrc.cmd_cntr);
        printf("PECError:%d",IC[ic].cccrc.vr_pec);
      }
    }
  }
  printf("\n\n");
}

/**
 *******************************************************************************
 * Function: printAux2950
 * @brief Print Aux ADC reg. result.
 *
 * @details This function Print Aux ADC reg. result into terminal.
 *
 * Parameters:
 * @param [in]	tIC      Total IC
 *
 * @param [in]  *IC      cell_asic_2950 stucture pointer
 *
 * @param [in]  type     Enum type of resistor
 *
 * @param [in]  grp      Enum type of resistor group
 *
 * @return None
 *
 *******************************************************************************
*/
#if 0
void printAux2950(uint8_t tIC, cell_asic_2950 *IC, DATA_TYPE type, REG_GRP grp)
{
  float voltage;
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    printf("IC%d:",(ic+1));
    if(type == aux)
    {
      if(grp == GRP_A)
      {
        printf("Aux A:\n");
        voltage = getVoltage2950(IC[ic].auxa.vref1p25);
        printf("VREF1P25:%fV, ", voltage);
        // printf("TMP1:%fC, ", (((IC[ic].auxa.tmp1 * 150e-6 )+ 1.5)/0.0075)-273);//check scaling //Sayani
        printf("TMP1:%fC, ", ((IC[ic].auxa.tmp1 / 61.8) - 250));
        voltage = getVoltage2950(IC[ic].auxa.vreg); 
        // printf("VREG:%fV\n", (voltage + 1.5));
        printf("VREG:%fV, ", (voltage * 0.24)); // voltage = VREG * 240microV
        printf("CCount:%d, ",IC[ic].cccrc.cmd_cntr);
        printf("PECError:%d\n\n",IC[ic].cccrc.aux_pec);
      }
      else if(grp == GRP_B)
      {
        printf("Aux B:\n");
        voltage = getVoltage2950(IC[ic].auxb.vdd);
        printf("VDD:%fV, ", voltage); // voltage = VDD * 1mV
        voltage = getVoltage2950(IC[ic].auxb.vdig);
        // printf("VDIG:%fV, ", voltage);
        printf("VDIG:%fV, ", (voltage * 0.24)); // voltage = VDIG * 240microV
        voltage = getVoltage2950(IC[ic].auxb.epad);
        printf("EPAD:%fV, ", voltage); // voltage = EPAD * 100microV
        printf("CCount:%d, ",IC[ic].cccrc.cmd_cntr);
        printf("PECError:%d\n\n",IC[ic].cccrc.aux_pec);
      }
      else if(grp == GRP_C)
      {
        printf("Aux C:\n");
        voltage = getVoltage2950(IC[ic].auxc.vdiv);
        printf("VDIV:%fV, ", voltage); // voltage = VDIV * 100microV
        // printf("TMP2:%fC, ", (((IC[ic].auxc.tmp2 * 150e-6 )+ 1.5)/0.0075)-273);//check scaling//Sayani
        printf("TMP2:%fC, ", ((IC[ic].auxc.tmp2)/20.5)-267); 
        printf("OSCCNT:0x%X, ", IC[ic].auxc.osccnt);
        printf("CCount:%d, ", IC[ic].cccrc.cmd_cntr);
        printf("PECError:%d\n\n", IC[ic].cccrc.aux_pec);
      }
      else if(grp == GRP_ALL)
      {
        printf("Aux A:\n");
        voltage = getVoltage2950(IC[ic].auxa.vref1p25);
        printf("VREF1P25:%fV, ", voltage); // voltage = VREF1P25 * 100microV
        // printf("TMP1:%fC, ", (((IC[ic].auxa.tmp1 * 150e-6 )+ 1.5)/0.0075)-273);//check scaling//Sayani
        printf("TMP1:%fC, ", ((IC[ic].auxa.tmp1 / 61.8) - 250));
        voltage = getVoltage2950(IC[ic].auxa.vreg);
        // printf("VREG:%fV\n", (voltage + 1.5));
        printf("VREG:%fV\n", (voltage * 0.24)); // voltage = VREG * 240microV
        
        printf("Aux B:\n");
        voltage = getVoltage2950(IC[ic].auxb.vdd);
        printf("VDD:%fV, ", voltage); // voltage = VDD * 1mV
        voltage = getVoltage2950(IC[ic].auxb.vdig);
        // printf("VDIG:%fV, ", voltage);        
        printf("VDIG:%fV, ", (voltage * 0.24)); // voltage = VDIG * 240microV
        voltage = getVoltage2950(IC[ic].auxb.epad);
        printf("EPAD:%fV\n", voltage); // voltage = EPAD * 100microV
        
        printf("Aux C:\n");
        voltage = getVoltage2950(IC[ic].auxc.vdiv);
        printf("VDIV:%fV, ", voltage); // voltage = VDIV * 100microV
        // printf("TMP2:%fC, ", (((IC[ic].auxc.tmp2 * 150e-6 )+ 1.5)/0.0075)-273);//check scaling//Sayani
        printf("TMP2:%fC, ", ((IC[ic].auxc.tmp2)/20.5)-267); 
        printf("OSCCNT:0x%X, ", IC[ic].auxc.osccnt);
        
        printf("CCount:%d, ", IC[ic].cccrc.cmd_cntr);
        printf("PECError:%d\n\n", IC[ic].cccrc.aux_pec);
      }
      else{ printf("Wrong Register Group Select\n"); }
    }
  }
}
#endif

/**
 *******************************************************************************
 * Function: printFlag2950
 * @brief Print flag reg. result.
 *
 * @details This function Print flag result into terminal.
 *
 * Parameters:
 * @param [in]	tIC      Total IC
 *
 * @param [in]  *IC      cell_asic_2950 stucture pointer
 *
 * @param [in]  type     Enum type of resistor
 *
 * @param [in]  grp      Enum type of resistor group
 *
 * @return None
 *
 *******************************************************************************
*/
void printFlag2950(uint8_t tIC, cell_asic_2950 *IC, DATA_TYPE type)
{
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    printf("IC%d:",(ic+1));
    if(type == Flag)
    {
      printf("\t Flag:");
#if 1    // to switch between bit wise/byte wise extraction of STATC values     
      printf("OC1L      :0x%X, ", IC[ic].flag.oc1l);
      printf("OC2L      :0x%X, ", IC[ic].flag.oc2l);
      printf("OC3L      :0x%X, ", IC[ic].flag.oc3l);
      printf("OCAL      :0x%X, ", IC[ic].flag.ocal);
      printf("OCBL      :0x%X, ", IC[ic].flag.ocbl);
      printf("OCAGD_CLRM:0x%X, ", IC[ic].flag.ocagd_clrm);
      printf("OCBGD     :0x%X, ", IC[ic].flag.ocbgd);
      printf("OCMM      :0x%X, ", IC[ic].flag.ocmm);      
      
      printf("I1CNT     :0x%X, ", IC[ic].flag.i1cnt);
      printf("I2CNT     :0x%X, ", IC[ic].flag.i2cnt);
      printf("I1PHA     :0x%X, ", IC[ic].flag.i1pha);
      
      printf("VDRUV     :0x%X, ", IC[ic].flag.vdruv);
      printf("VDDUV     :0x%X, ", IC[ic].flag.vdduv);
      printf("VREGOV    :0x%X, ", IC[ic].flag.vregov);
      printf("VREGUV    :0x%X, ", IC[ic].flag.vreguv);
      printf("VDIGOV    :0x%X, ", IC[ic].flag.vdigov);
      printf("VDIGUV    :0x%X, ", IC[ic].flag.vdiguv);
      
      printf("V1D       :0x%X, ", IC[ic].flag.v1d);
      printf("V2D       :0x%X, ", IC[ic].flag.v2d);
      printf("I1D       :0x%X, ", IC[ic].flag.i1d);
      printf("I2D       :0x%X, ", IC[ic].flag.i2d); 
      
      printf("VDE       :0x%X, ", IC[ic].flag.vde);
      printf("VDE1      :0x%X, ", IC[ic].flag.vdel);
           
      printf("SED1      :0x%X, ", IC[ic].flag.sed1);
      printf("SED2      :0x%X, ", IC[ic].flag.sed2);
      printf("MED1      :0x%X, ", IC[ic].flag.med1);
      printf("MED2      :0x%X, ", IC[ic].flag.med2);      

      printf("NOCLK     :0x%X, ", IC[ic].flag.noclk);
      printf("SPIFLT    :0x%X, ", IC[ic].flag.spiflt);
      printf("RESET     :0x%X, ", IC[ic].flag.reset);
      printf("THSD      :0x%X, ", IC[ic].flag.thsd);
      printf("TMODE     :0x%X, ", IC[ic].flag.tmode);
      printf("OSCFLT    :0x%X, ", IC[ic].flag.oscflt);
      printf("REFFLT    :0x%X\n", IC[ic].flag.refflt);
      
      printf("CCount:%d, ", IC[ic].cccrc.cmd_cntr);
      printf("PECError:%d\n\n", IC[ic].cccrc.flag_pec);
#else         
      printf("0x%X, ", IC[ic].flg.rx_data[0]);
      printf("0x%X, ", IC[ic].flg.rx_data[1]);
      printf("0x%X, ", IC[ic].flg.rx_data[2]);
      printf("0x%X, ", IC[ic].flg.rx_data[3]);
      printf("0x%X, ", IC[ic].flg.rx_data[4]);
      printf("0x%X\n\n", IC[ic].flg.rx_data[5]);
#endif 
    }
  }
}

/**
*******************************************************************************
* Function: printStatus2950
* @brief Print status reg. result.
*
* @details This function Print status result into terminal.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *IC      cell_asic_2950 stucture pointer
*
* @param [in]  type     Enum type of resistor
*
* @param [in]  grp      Enum type of resistor group
*
* @return None
*
*******************************************************************************
*/
void printStatus2950(uint8_t tIC, cell_asic_2950 *IC, DATA_TYPE type, REG_GRP grp)
{
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    printf("IC%d:",(ic+1));
    if(type == status)
    {
      printf("Status :\n");
      
      printf("OCAP     :0x%X, ", IC[ic].state.ocap );
      printf("OCBP     :0x%X, ", IC[ic].state.ocbp );
      printf("DER      :0x%X\n", IC[ic].state.der  );
      printf("I1CAL    :0x%X, ", IC[ic].state.i1cal);
      printf("I2CAL    :0x%X, ", IC[ic].state.i2cal);
      
      printf("GPO1L    :0x%X\n", IC[ic].state.gpo1l);
      printf("GPO2L    :0x%X, ", IC[ic].state.gpo2l);
      printf("GPO3L    :0x%X, ", IC[ic].state.gpo3l);
      printf("GPO4L    :0x%X\n", IC[ic].state.gpo4l);
      printf("GPO5L    :0x%X, ", IC[ic].state.gpo5l);
      printf("GPO6L    :0x%X, ", IC[ic].state.gpo6l);
      
      printf("GPO1H    :0x%X\n", IC[ic].state.gpo1h);
      printf("GPO2H    :0x%X, ", IC[ic].state.gpo2h);
      printf("GPO3H    :0x%X, ", IC[ic].state.gpo3h);
      printf("GPO4H    :0x%X\n", IC[ic].state.gpo4h);
      printf("GPO5H    :0x%X, ", IC[ic].state.gpo5h);
      printf("GPO6H    :0x%X, ", IC[ic].state.gpo6h);
      
      printf("GPIO1L   :0x%X, ", IC[ic].state.gpio1l);
      printf("GPIO2L   :0x%X\n", IC[ic].state.gpio2l);
      printf("GPIO3L   :0x%X, ", IC[ic].state.gpio3l);
      printf("GPIO4L   :0x%X, ", IC[ic].state.gpio4l);
      
      printf("REV      :0x%X\n", IC[ic].state.rev);    
    
      printf("CCount    :%d, ",  IC[ic].cccrc.cmd_cntr);
      printf("PECError:%d\n\n",  IC[ic].cccrc.stat_pec);   
    }
  }
}

/**
 *******************************************************************************
 * Function: printAux2950
 * @brief Print Aux ADC reg. result.
 *
 * @details This function Print Aux ADC reg. result into terminal.
 *
 * Parameters:
 * @param [in]	tIC      Total IC
 *
 * @param [in]  *IC      cell_asic_2950 stucture pointer
 *
 * @param [in]  type     Enum type of resistor
 *
 * @param [in]  grp      Enum type of resistor group
 *
 * @return None
 *
 *******************************************************************************
*/
void printAux2950(uint8_t tIC, cell_asic_2950 *IC, DATA_TYPE type, REG_GRP grp)
{
  float voltage;
  for(uint8_t ic = 0; ic < tIC; ic++)
  {
    printf("IC%d:",(ic+1));
    if(type == aux)
    {
      if(grp == GRP_A)
      {
        printf("Aux A:\n");
        voltage = getVoltage2950(IC[ic].auxa.vref1p25);
        printf("VREF1P25:%fV, ", voltage);
        // printf("TMP1:%fC, ", (((IC[ic].auxa.tmp1 * 150e-6 )+ 1.5)/0.0075)-273);//check scaling 
        printf("TMP1:%fC, ", ((IC[ic].auxa.tmp1 / 61.8) - 250));
        voltage = getVoltage2950(IC[ic].auxa.vreg); 
        printf("VREG:%fV, ", (voltage * 0.24)); // voltage = VREG * 240microV
        printf("CCount:%d, ",IC[ic].cccrc.cmd_cntr);
        printf("PECError:%d\n\n",IC[ic].cccrc.aux_pec);
      }
      else if(grp == GRP_B)
      {
        printf("Aux B:\n");
        voltage = getVoltage2950(IC[ic].auxb.vdd);
        printf("VDD:%fV, ", voltage); // voltage = VDD * 1mV
        voltage = getVoltage2950(IC[ic].auxb.vdig);
        printf("VDIG:%fV, ", (voltage * 0.24)); // voltage = VDIG * 240microV
        voltage = getVoltage2950(IC[ic].auxb.epad);
        printf("EPAD:%fV, ", voltage); // voltage = EPAD * 100microV
        printf("CCount:%d, ",IC[ic].cccrc.cmd_cntr);
        printf("PECError:%d\n\n",IC[ic].cccrc.aux_pec);
      }
      else if(grp == GRP_C)
      {
        printf("Aux C:\n");
        voltage = getVoltage2950(IC[ic].auxc.vdiv);
        printf("VDIV:%fV, ", voltage); // voltage = VDIV * 100microV
        // printf("TMP2:%fC, ", (((IC[ic].auxc.tmp2 * 150e-6 )+ 1.5)/0.0075)-273);//check scaling
        printf("TMP2:%fC, ", ((IC[ic].auxc.tmp2)/20.5)-267); 
        printf("OSCCNT:0x%X, ", IC[ic].auxc.osccnt);
        printf("CCount:%d, ", IC[ic].cccrc.cmd_cntr);
        printf("PECError:%d\n\n", IC[ic].cccrc.aux_pec);
      }
      else if(grp == GRP_ALL)
      {
        printf("Aux A:\n");
        voltage = getVoltage2950(IC[ic].auxa.vref1p25);
        printf("VREF1P25:%fV, ", voltage); // voltage = VREF1P25 * 100microV
        printf("TMP1:%fC, ", ((IC[ic].auxa.tmp1 / 61.8) - 250));
        voltage = getVoltage2950(IC[ic].auxa.vreg);
        printf("VREG:%fV\n", (voltage * 0.24)); // voltage = VREG * 240microV
        
        printf("Aux B:\n");
        voltage = getVoltage2950(IC[ic].auxb.vdd);
        printf("VDD:%fV, ", voltage); // voltage = VDD * 1mV
        voltage = getVoltage2950(IC[ic].auxb.vdig);
        printf("VDIG:%fV, ", (voltage * 0.24)); // voltage = VDIG * 240microV
        voltage = getVoltage2950(IC[ic].auxb.epad);
        printf("EPAD:%fV\n", voltage); // voltage = EPAD * 100microV
        
        printf("Aux C:\n");
        voltage = getVoltage2950(IC[ic].auxc.vdiv);
        printf("VDIV:%fV, ", voltage); // voltage = VDIV * 100microV
        printf("TMP2:%fC, ", ((IC[ic].auxc.tmp2)/20.5)-267); 
        printf("OSCCNT:0x%X, ", IC[ic].auxc.osccnt);
        
        printf("CCount:%d, ", IC[ic].cccrc.cmd_cntr);
        printf("PECError:%d\n\n", IC[ic].cccrc.aux_pec);
      }
      else{ printf("Wrong Register Group Select\n"); }
    }
  }
}

/**
*******************************************************************************
* Function: printMenu2950
* @brief Print Command Menu.
*
* @details This function print all command menu.
*
* @return None
*
*******************************************************************************
*/
void printMenu2950()
{
  printf("List of ADBMS2950 Command:\n");
  printf("Write and Read Configuration: 1                              |Read Device SID: 26                 \n");
  printf("Read Configuration: 2                                        |Soft Reset: 27                      \n");
  printf("Start ADI1 Conversion(Single Shot): 3                        |Reset cmd counter: 28               \n");
  printf("Start ADI2 Conversion(Single Shot): 4                        |SNAP(Stop Reg. updates): 29         \n");
  printf("Start ADI1 Conversion(Continuous): 5                         |UNSNAP(Resume Reg. updates): 30     \n");
  printf("Start ADI2 Conversion(Continuous): 6                         |Set Reset GPO Pins: 31              \n");
  printf("Start ADI1 Redundant Conversion(Single Shot): 7              |GPIO SPI Write to Slave: 32         \n");
  printf("Start ADI1 Redundant Conversion(Continuous): 8               |GPIO SPI Read from Slave: 33        \n");
  printf("Read CR, VBAT & IVBAT Registers(Single Shot): 9              |GPIO I2C Write to Slave: 34         \n");
  printf("Read CR, VBAT & IVBAT Registers(Continuous): 10              |GPIO I2C Read from Slave: 35        \n");
  printf("Read Overcurrent ADC Register(Single Shot): 11               |                                    \n");
  printf("Read Overcurrent ADC Register(Continuous): 12                |                                    \n");
  printf("Read Accumulated CR, VBAT & IVBAT Registers(Single Shot): 13     |                                    \n");
  printf("Read Accumulated CR, VBAT & IVBAT Registers(Continuous): 14      |                                    \n");
  printf("Read All CR and VBAT Voltage Registers(Single Shot): 15      |                                    \n");
  printf("Read All CR and VBAT Voltage Registers(Continuous): 16       |                                    \n");
  printf("Start ADV All Channel Conversion: 17                         |                                    \n");
  printf("Read All VR Registers(RDV commands): 18                      |                                    \n");
  printf("Read All RVR Registers(RDRVA & RDRVB commands): 19           |                                    \n");
  printf("Read All VR & RVR Registers(RDV, RDRVA & RDRVB commands): 20 |                                    \n");
  printf("Read Voltage Registers(RDAUXC & RDAUXD commands)(NA): 21     |                                    \n");
  printf("Start ADAUX All Channel Conversion: 22                       |                                    \n");
  printf("Read ADAUX Measurement: 23                                   |                                    \n");
  printf("Read Status C Register: 24                                   |                                    \n");
  printf("Read All Status Registers: 25                                |                                    \n");

  printf("\n");
  printf("Print '0' for menu\n");
  printf("Please enter command: \n");
  printf("\n\n");
}

/**
*******************************************************************************
* Function: printMsg2950
* @brief Print Message.
*
* @details This function print message into terminal.
* Parameters:
* @param [in]	msg    Message string
*
* @return None
*
*******************************************************************************
*/
void printMsg2950(char *msg)
{
#ifdef MBED
  pc.printf("%s\n", msg);
#else /* IAR */
  printf("%s\n", msg);
#endif /* MBED */
}

/**
*******************************************************************************
* Function: printPollAdcConvTime2950
* @brief Print Poll adc conversion Time.
*
* @details This function print poll adc conversion Time.
*
* @return None
*
*******************************************************************************
*/
void printPollAdcConvTime2950(int count)
{
#ifdef MBED
  pc.printf("Adc Conversion Time = %fms\n", (float)(count/1000.0));
#else /* IAR */
  printf("Adc Conversion Time = %fms\n", (float)(count/64000.0));
#endif /* MBED */
}

/**
*******************************************************************************
* Function: printResultCount2950
* @brief Print Result Count.
*
* @details This function print the continuous measurment result count.
*
* @return None
*
*******************************************************************************
*/
void printResultCount2950(int count)
{
#ifdef MBED
  pc.printf("Result Count:%d\n", (count+1));
#else /* IAR */
  printf("Result Count:%d\n", (count+1));
#endif /* MBED */
}

/**
*******************************************************************************
* Function: readUserInupt2950
* @brief Read command input & print into console.
*
* @details This function print the input command & print into console.
*
* @return None
*
*******************************************************************************
*/
void readUserInupt2950(int *user_command)
{
#ifdef MBED
    pc.scanf("%d", user_command);
    pc.printf("Entered cmd:%d\n", *user_command);
#else /* IAR */
    scanf("%d", user_command);
    printf("Entered cmd:%d\n", *user_command);
#endif /* MBED */
}

/**
*******************************************************************************
* Function: getVoltage2950
* @brief Get Voltage with multiplication factor.
*
* @details This function calculates the voltage.
*
* Parameters:
* @param [in]	data    Register value(uint16_t)
*
* @return Voltage(float)
*
*******************************************************************************
*/
float getVoltage2950(int data)
{
  float voltage;
  voltage = 100e-6 * (int16_t)data; /* Interpreting as 16-bit to be sure of length so signed works */
  return voltage * 396.6043956;
}

/**
*******************************************************************************
* Function: getCurrent2950
* @brief Get Current with multiplication factor.
*
* @details This function calculates the current.
*
* Parameters:
* @param [in]	data    Register value(int32_t)
*
* @return Current(float)
*
*******************************************************************************
*/
float getCurrent2950(uint32_t data)
{
  float current;
  // current = 1e-6 * (((int32_t)data << (32-24)) >> (32-24));
  current = 1e-6 * ((int32_t)(data << (32-24)) >> (32-24));
  // current = 1e-6 * ((int32_t)(data));
  return current / 0.00005;
}

/**
*******************************************************************************
* Function: getAccCurrent2950
* @brief Get Accumulated Current with multiplication factor.
*
* @details This function calculates the current.
*
* Parameters:
* @param [in]	data    Register value(uint32_t)
*
* @return Current(float)
*
*******************************************************************************
*/
float getAccCurrent2950(uint32_t data)
{
  float current;
  current = 1e-6 * 0.125 * ((int32_t)(data << (32-24)) >> (32-24));
  // current = 1e-6 * 0.125 * ((int32_t)(data << (32-18)) >> (32-18));
  return current;
}

/**
*******************************************************************************
* Function: getAccVbat2950
* @brief Get Accumulated VBAT with multiplication factor.
*
* @details This function calculates the acc vbat.
*
* Parameters:
* @param [in]	data    Register value(uint32_t)
*
* @return Current(float)
*
*******************************************************************************
*/
float getAccVbat2950(uint32_t data)
{
  float accvbat;
  accvbat = 100e-6 * 0.125 * ((int32_t)(data << (32-24)) >> (32-24));
  return accvbat;
}

/**
*******************************************************************************
* Function: getOverCurrent2950
* @brief Get Over Current with multiplication factor.
*
* @details This function calculates the over current.
*
* Parameters:
* @param [in]	data    Register value(uint8_t)
*
* @return Current(float)
*
*******************************************************************************
*/
float getOverCurrent2950(uint8_t data)
{
  float over_current;
//  over_current = 0.005 * ((int8_t)(data << (8-7)) >> (8-7));
  over_current =4.78e-3 * ((int8_t)(data)+0.5);
  return over_current;
}

/**
*******************************************************************************
* Function: getAvgAccVbat2950
* @brief Get Voltage with multiplication factor.
*
* @details This function calculates the battery voltage by taking acc vbat/accn
*
* Parameters:
* @param [in]	data    Register value(uint32_t)
* @param [in] accn    ACCN value (uint32_t)
*
* @return Current(float)
*
*******************************************************************************
*/
float getAvgAccVbat2950(uint32_t data, uint32_t accn)
{
  float accvbat;
  accvbat = 100e-6 * ((int32_t)(data << (32-24)) >> (32-24)) / accn;
  return accvbat * 396.6043956;
}

/**
*******************************************************************************
* Function: getAvgAccCurrent2950
* @brief Get Current with multiplication factor.
*
* @details This function calculates the current by taking acc current/accn.
*
* Parameters:
* @param [in]	data    Register value(int32_t)
*
* @return Current(float)
*
*******************************************************************************
*/
float getAvgAccCurrent2950(uint32_t data, uint32_t accn)
{
  float current;
  current = 1e-6 * ((int32_t)(data << (32-24)) >> (32-24)) / accn;
  // current = 1e-6 * 0.125 * ((int32_t)(data << (32-18)) >> (32-18));
  return current / 0.00005;
}

/** @}*/
/** @}*/