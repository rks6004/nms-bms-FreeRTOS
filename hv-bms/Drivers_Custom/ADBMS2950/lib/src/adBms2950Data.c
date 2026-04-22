/**
********************************************************************************
*
* @file:    adi_bms_2950data.c
*
* @brief:   This file contains 2950 data function implementation.
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

/*! \addtogroup Data_Management
 *  @{
 */

#include "adBms2950Data.h"
#include "adBms2950GenericType.h"
#ifndef TESTBENCH
#include "mcuWrapper.h"
#endif
/**
*******************************************************************************
* Function: adBms2950_Adi1
* @brief ADI1 Command.
*
* @details Send ADI1 command to start Current1 ADC Conversion.
*
* Parameters:
* @param [in]  RD      Enum type Read bit
*
* @param [in]  CONT    Enum type continuous measurement bit
*
* @param [in]  ow      Enum type open wire bit
*
* @return None
*
*******************************************************************************
*/
// void adBms2950_Adi1(RD rd, CONT cont, OW ow)
void adBms2950_Adi1(uint8_t tIC, cell_asic_2950 *ic, RD rd, OPT opt, cmd_description *cmd_des)
{
  uint8_t cmd[2];
  cmd[0] = 0x02 + rd;
  //  cmd[1] = (cont<<7) + (ow & 0x03) + 0x60;
  cmd[1] = ((opt & 0x08) << 4) + ((opt & 0x04) << 2) + (opt & 0x03) + 0x60; //(CC position)7- (OPT position)3 = 4 (Shift)

  spiSendCmd2950(tIC, &ic[0], cmd);
}

/**
*******************************************************************************
* Function: adBms2950_Adi2
* @brief ADI2 Command.
*
* @details Send ADI2 command to start Current2 ADC Conversion.
*
* Parameters:
* @param [in]  cont     Enum type continuous measurement bit
*
* @param [in]  ow       Enum type open wire
*
* @return None
*
*******************************************************************************
*/
// void adBms2950_Adi2(CONT cont, OW ow)
void adBms2950_Adi2(uint8_t tIC, cell_asic_2950 *ic, OPT opt, cmd_description *cmd_des)
{
  uint8_t cmd[2];
  cmd[0] = 0x01;
  //  cmd[1] = (cont<<7) + (ow &0x03) + 0x68;
  cmd[1] = ((opt & 0x08) << 4) + ((opt & 0x04) << 2) + (opt & 0x03) + 0x68; //(CC position)7- (OPT position)3 = 4 (Shift)
  spiSendCmd2950(tIC, &ic[0], cmd);
}

/**
*******************************************************************************
* Function: adBms2950_Adv
* @brief ADV Command.
*
* @details Send ADV command to start Voltage ADC Conversion.
*
* Parameters:
* @param [in]  ow      Enum type open wire bits
*
* @param [in]  vch     Enum type open VCH channel
*
* @return None
*
*******************************************************************************
*/
// void adBms2950_Adv(OW ow, VCH vch)
void adBms2950_Adv(uint8_t tIC, cell_asic_2950 *ic, OW ow, VCH vch)
{
  uint8_t cmd[2];
  cmd[0] = 0x04;
  cmd[1] = (ow << 6) + 0x30 + (vch & 0x0F);
  spiSendCmd2950(tIC, &ic[0], cmd);
}

/**
*******************************************************************************
* Function: adBms2950_Adaux
* @brief ADAUX Command.
*
* @details Send ADAUX command to start aux ADC Conversion.
*
* Parameters:
* @param [in]  ach      Enum type ACH channel
*
* @return None
*
*******************************************************************************
*/
// void adBms2950_Adaux(ACH ach)
//  replaced by spiSendCmd2950(tIC, &ic[0], tIC, &ic[0], sADX);
#if 0
void adBms2950_Adaux(uint8_t tIC, cell_asic_2950 *ic, ACH ach, cmd_description *cmd_des)
{
  uint8_t cmd[2];
  cmd[0] = 0x05;
  cmd[1] = 0x30 + (ach & 0x03);
  spiSendCmd2950(tIC, &ic[0], cmd);
}
#endif

/**
*******************************************************************************
* Function: adBms2950_Stcomm
* @brief Send command to Start I2C/SPI Communication.
*
* @details Send command to start I2C/SPI communication.
*
* @return None
*
*******************************************************************************
*/
// void adBms2950_Stcomm(void)
void adBms2950_Stcomm(uint8_t tIC, cell_asic_2950 *ic)
{
  adBms2950_IncCalCmdCounter(tIC, ic);
  adBmsCsLow();
  spiWriteBytes(13, &STCOMM[0]);
  adBmsCsHigh();
}

/**
*******************************************************************************
* Function: adBms2950ConfigA_Gpio
* @brief Config Gpio Pull High/Low.
*
* @details This function Set configuration gpio as pull high/Low.
*
* Parameters:
*
* @param [in]  gpio        Enum type GPIO Pin.
*
* @param [in]  set_clr     Enum type gpio set (Low or High).
*
* @return Gpio_value
*
*******************************************************************************
*/
uint8_t adBms2950ConfigA_Gpio(GPIO gpio, CFGA_GPIO set_clr)
{
  uint8_t gpoivalue;
  if (set_clr == GPIO_SET)
  {
    gpoivalue = (1 << gpio);
  }
  else
  {
    gpoivalue = (0 << gpio);
  }
  return (gpoivalue);
}

/**
*******************************************************************************
* Function: adBms2950ConfigA_Gpo
* @brief Set config GPO_PIN Pull High/Low.
*
* @details This function Set configuration gpo as pull high/Low.
*
* Parameters:
*
* @param [in]  gpo      Enum type GPO_PIN Pin.
*
* @param [in]  set_clr     Enum type gpo set (Low or High).
*
* @return Gpo_value
*
*******************************************************************************
*/
uint8_t adBms2950ConfigA_Gpo(GPO_PIN gpo, CFGA_GPO set_clr)
{
  uint8_t gpovalue;
  if (set_clr == GPO_SET)
  {
    gpovalue = (1 << gpo);
  }
  else
  {
    gpovalue = (0 << gpo);
  }
  return (gpovalue);
}

/**
*******************************************************************************
* Function: adBms2950ParseConfiga
* @brief Parse the recived Configuration register A data
*
* @details This function Parse the recived Configuration register A data.
*
* Parameters:
*
* @param [in]  tIC                     Total IC
*
* @param [in]  *ic                      cell_asic_2950 ic structure pointer
*
* @param [in]  *data                    Data pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950ParseConfiga(uint8_t tIC, cell_asic_2950 *ic, uint8_t *data)
{
  uint8_t address = 0;
  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    memcpy(&ic[curr_ic].configa.rx_data[0], &data[address], RX_DATA); /* dst , src , size */
    address = ((curr_ic + 1) * (RX_DATA));

    ic[curr_ic].rx_cfga.vs1 = (ic[curr_ic].configa.rx_data[0] & 0x03);
    ic[curr_ic].rx_cfga.vs2 = (ic[curr_ic].configa.rx_data[0] & 0x0C) >> 2;
    ic[curr_ic].rx_cfga.vs3 = (ic[curr_ic].configa.rx_data[0] & 0x10) >> 4;
    ic[curr_ic].rx_cfga.vs4 = (ic[curr_ic].configa.rx_data[0] & 0x20) >> 5;
    ic[curr_ic].rx_cfga.vs5 = (ic[curr_ic].configa.rx_data[0] & 0x40) >> 6;
    ic[curr_ic].rx_cfga.ocen = (ic[curr_ic].configa.rx_data[0] & 0x80) >> 7;

    ic[curr_ic].rx_cfga.injosc = (ic[curr_ic].configa.rx_data[1] & 0x03);
    ic[curr_ic].rx_cfga.injmon = (ic[curr_ic].configa.rx_data[1] & 0x0C) >> 2;
    ic[curr_ic].rx_cfga.injts = (ic[curr_ic].configa.rx_data[1] & 0x10) >> 4;
    ic[curr_ic].rx_cfga.injecc = (ic[curr_ic].configa.rx_data[1] & 0x40) >> 6;
    ic[curr_ic].rx_cfga.injtm = (ic[curr_ic].configa.rx_data[1] & 0x80) >> 7;

    ic[curr_ic].rx_cfga.vs6 = (ic[curr_ic].configa.rx_data[2] & 0x01);
    ic[curr_ic].rx_cfga.vs7 = (ic[curr_ic].configa.rx_data[2] & 0x02) >> 1;
    ic[curr_ic].rx_cfga.vs8 = (ic[curr_ic].configa.rx_data[2] & 0x04) >> 2;
    ic[curr_ic].rx_cfga.vs9 = (ic[curr_ic].configa.rx_data[2] & 0x08) >> 3;
    ic[curr_ic].rx_cfga.vs10 = (ic[curr_ic].configa.rx_data[2] & 0x10) >> 4;
    ic[curr_ic].rx_cfga.soak = (ic[curr_ic].configa.rx_data[2] & 0xE0) >> 5;

    ic[curr_ic].rx_cfga.gpo1c = (ic[curr_ic].configa.rx_data[3] & 0x01);
    ic[curr_ic].rx_cfga.gpo2c = (ic[curr_ic].configa.rx_data[3] & 0x02) >> 1;
    ic[curr_ic].rx_cfga.gpo3c = (ic[curr_ic].configa.rx_data[3] & 0x04) >> 2;
    ic[curr_ic].rx_cfga.gpo4c = (ic[curr_ic].configa.rx_data[3] & 0x08) >> 3;
    ic[curr_ic].rx_cfga.gpo5c = (ic[curr_ic].configa.rx_data[3] & 0x10) >> 4;
    ic[curr_ic].rx_cfga.gpo6c = (ic[curr_ic].configa.rx_data[3] & 0x60) >> 5;

    ic[curr_ic].rx_cfga.gpo1od = (ic[curr_ic].configa.rx_data[4] & 0x01);
    ic[curr_ic].rx_cfga.gpo2od = (ic[curr_ic].configa.rx_data[4] & 0x02) >> 1;
    ic[curr_ic].rx_cfga.gpo3od = (ic[curr_ic].configa.rx_data[4] & 0x04) >> 2;
    ic[curr_ic].rx_cfga.gpo4od = (ic[curr_ic].configa.rx_data[4] & 0x08) >> 3;
    ic[curr_ic].rx_cfga.gpo5od = (ic[curr_ic].configa.rx_data[4] & 0x10) >> 4;
    ic[curr_ic].rx_cfga.gpo6od = (ic[curr_ic].configa.rx_data[4] & 0x20) >> 5;
    ic[curr_ic].rx_cfga.gpio1fe = (ic[curr_ic].configa.rx_data[4] & 0x40) >> 6;
    ic[curr_ic].rx_cfga.spi3w = (ic[curr_ic].configa.rx_data[4] & 0x80) >> 7;

    ic[curr_ic].rx_cfga.acci = (ic[curr_ic].configa.rx_data[5] & 0x07);
    ic[curr_ic].rx_cfga.commbk = (ic[curr_ic].configa.rx_data[5] & 0x08) >> 3;
    ic[curr_ic].rx_cfga.refup = (ic[curr_ic].configa.rx_data[5] & 0x10) >> 4;
    ic[curr_ic].rx_cfga.snapst = (ic[curr_ic].configa.rx_data[5] & 0x20) >> 5;
    ic[curr_ic].rx_cfga.vb1mux = (ic[curr_ic].configa.rx_data[5] & 0x40) >> 6;
    ic[curr_ic].rx_cfga.vb2mux = (ic[curr_ic].configa.rx_data[5] & 0x80) >> 7;
  }
}

/**
*******************************************************************************
* Function: adBms2950ParseConfigb
* @brief Parse the recived Configuration register B data
*
* @details This function Parse the recived Configuration register B data.
*
* Parameters:
*
* @param [in]  tIC                     Total IC
*
* @param [in]  *ic                     cell_asic_2950 ic structure pointer
*
* @param [in]  *data                   Data pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950ParseConfigb(uint8_t tIC, cell_asic_2950 *ic, uint8_t *data)
{
  uint8_t address = 0;
  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    memcpy(&ic[curr_ic].configb.rx_data[0], &data[address], RX_DATA); /* dst , src , size */
    address = ((curr_ic + 1) * (RX_DATA));

    ic[curr_ic].rx_cfgb.oc1th = (ic[curr_ic].configb.rx_data[0] & 0x7F);
    ic[curr_ic].rx_cfgb.oc1ten = (ic[curr_ic].configb.rx_data[0] & 0x80) >> 7;

    ic[curr_ic].rx_cfgb.oc2th = (ic[curr_ic].configb.rx_data[1] & 0x7F);
    ic[curr_ic].rx_cfgb.oc2ten = (ic[curr_ic].configb.rx_data[1] & 0x80) >> 7;

    ic[curr_ic].rx_cfgb.oc3th = (ic[curr_ic].configb.rx_data[2] & 0x7F);
    ic[curr_ic].rx_cfgb.oc3ten = (ic[curr_ic].configb.rx_data[2] & 0x80) >> 7;

    ic[curr_ic].rx_cfgb.ocdgt = (ic[curr_ic].configb.rx_data[3] & 0x03);
    ic[curr_ic].rx_cfgb.ocdp = (ic[curr_ic].configb.rx_data[3] & 0x08) >> 3;
    ic[curr_ic].rx_cfgb.reften = (ic[curr_ic].configb.rx_data[3] & 0x20) >> 5;
    ic[curr_ic].rx_cfgb.octsel = (ic[curr_ic].configb.rx_data[3] & 0xC0) >> 6;

    ic[curr_ic].rx_cfgb.ocod = (ic[curr_ic].configb.rx_data[4] & 0x01);
    ic[curr_ic].rx_cfgb.oc1gc = (ic[curr_ic].configb.rx_data[4] & 0x02) >> 1;
    ic[curr_ic].rx_cfgb.oc2gc = (ic[curr_ic].configb.rx_data[4] & 0x04) >> 2;
    ic[curr_ic].rx_cfgb.oc3gc = (ic[curr_ic].configb.rx_data[4] & 0x08) >> 3;
    ic[curr_ic].rx_cfgb.ocmode = (ic[curr_ic].configb.rx_data[4] & 0x30) >> 4;
    ic[curr_ic].rx_cfgb.ocax = (ic[curr_ic].configb.rx_data[4] & 0x40) >> 6;
    ic[curr_ic].rx_cfgb.ocbx = (ic[curr_ic].configb.rx_data[4] & 0x80) >> 7;

    ic[curr_ic].rx_cfgb.diagsel = (ic[curr_ic].configb.rx_data[5] & 0x07);
    ic[curr_ic].rx_cfgb.gpio2eoc = (ic[curr_ic].configb.rx_data[5] & 0x08) >> 3;
    ic[curr_ic].rx_cfgb.gpio1c = (ic[curr_ic].configb.rx_data[5] & 0x10) >> 4;
    ic[curr_ic].rx_cfgb.gpio2c = (ic[curr_ic].configb.rx_data[5] & 0x20) >> 5;
    ic[curr_ic].rx_cfgb.gpio3c = (ic[curr_ic].configb.rx_data[5] & 0x40) >> 6;
    ic[curr_ic].rx_cfgb.gpio4c = (ic[curr_ic].configb.rx_data[5] & 0x80) >> 7;
  }
}

/**
*******************************************************************************
* Function: adBms2950ParseConfig
* @brief Parse the recived Configuration register A & B data
*
* @details This function Parse the recived Configuration register A & B data.
*
* Parameters:
*
* @param [in]  tIC                     Total IC
*
* @param [in]  *ic                     cell_asic_2950 ic structure pointer
*
* @param [in]  grp                     Enum type register group
*
* @param [in]  *data                   Data pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950ParseConfig(uint8_t tIC, cell_asic_2950 *ic, REG_GRP grp, uint8_t *data)
{
  switch (grp)
  {
  case GRP_A:
    adBms2950ParseConfiga(tIC, &ic[0], &data[0]);
    break;

  case GRP_B:
    adBms2950ParseConfigb(tIC, &ic[0], &data[0]);
    break;

  default:
    break;
  }
}

/**
*******************************************************************************
* Function: adBms2950ParseCurrentRegData
* @brief Parse Current Register data
*
* @details This function Parse the received current register data.
*
* Parameters:
*
* @param [in]  tIC                     Total IC
*
* @param [in]  *ic                     cell_asic_2950 ic structure pointer
*
* @param [in]  *i_data                Current data pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950ParseCurrentRegData(uint8_t tIC, cell_asic_2950 *ic, uint8_t *i_data)
{
  uint8_t address = 0;
  // uint8_t extension = 0; /* checks bit 7 */
  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    memcpy(&ic[curr_ic].reg.rx_data[0], &i_data[address], RX_DATA); /* dst , src , size */
    address = ((curr_ic + 1) * (RX_DATA));
    // extension = ic[curr_ic].reg.rx_data[2] & 0x80 ? 0xff:00; /* checks bit 7 */
    // ic[curr_ic].i.i1 = ic[curr_ic].reg.rx_data[0] + (ic[curr_ic].reg.rx_data[1] << 8) + (ic[curr_ic].reg.rx_data[2] << 16) + (extension << 24);
    ic[curr_ic].i.i1 = ic[curr_ic].reg.rx_data[0] + (ic[curr_ic].reg.rx_data[1] << 8) + (ic[curr_ic].reg.rx_data[2] << 16);
    // extension = ic[curr_ic].reg.rx_data[5] & 0x80 ? 0xff:00; /* checks bit 7 */
    // ic[curr_ic].i.i2 = ic[curr_ic].reg.rx_data[3] + (ic[curr_ic].reg.rx_data[4] << 8) + (ic[curr_ic].reg.rx_data[5] << 16) + (extension << 24);
    ic[curr_ic].i.i2 = ic[curr_ic].reg.rx_data[3] + (ic[curr_ic].reg.rx_data[4] << 8) + (ic[curr_ic].reg.rx_data[5] << 16);
  }
}

/**
*******************************************************************************
* Function: adBms2950ParseVbatRegData
* @brief Parse Battery Voltage Register Data
*
* @details This function Parse the received battery voltage register data.
*
* Parameters:
*
* @param [in]  tIC                    Total IC
*
* @param [in]  *ic                    cell_asic_2950 ic structure pointer
*
* @param [in]  *vbat_data             Vbat data pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950ParseVbatRegData(uint8_t tIC, cell_asic_2950 *ic, uint8_t *vbat_data)
{
  uint8_t address = 0;
  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    memcpy(&ic[curr_ic].reg.rx_data[0], &vbat_data[address], RX_DATA); /* dst , src , size */
    address = ((curr_ic + 1) * (RX_DATA));
    ic[curr_ic].vbat.vbat1 = ic[curr_ic].reg.rx_data[2] + (ic[curr_ic].reg.rx_data[3] << 8);
    ic[curr_ic].vbat.vbat2 = ic[curr_ic].reg.rx_data[4] + (ic[curr_ic].reg.rx_data[5] << 8);
  }
}

/**
*******************************************************************************
* Function: adBms2950ParseIVbatRegData
* @brief Parse Current and Battery Voltage Register Data.
*
* @details This function Parse the received current and battery voltage register data.
*
* Parameters:
*
* @param [in]  tIC                    Total IC
*
* @param [in]  *ic                    cell_asic_2950 ic structure pointer
*
* @param [in]  *ivbat_data            ivbat data pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950ParseIVbatRegData(uint8_t tIC, cell_asic_2950 *ic, uint8_t *ivbat_data)
{
  uint8_t address = 0;
  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    memcpy(&ic[curr_ic].reg.rx_data[0], &ivbat_data[address], RX_DATA); /* dst , src , size */
    address = ((curr_ic + 1) * (RX_DATA));
    ic[curr_ic].ivbat.i1 = ic[curr_ic].reg.rx_data[0] + (ic[curr_ic].reg.rx_data[1] << 8) + (ic[curr_ic].reg.rx_data[2] << 16);
    ic[curr_ic].ivbat.vbat1 = ic[curr_ic].reg.rx_data[4] + (ic[curr_ic].reg.rx_data[5] << 8);
  }
}

/**
*******************************************************************************
* Function: adBms2950ParseOcRegData
* @brief Parse Overcurrent Register Data.
*
* @details This function Parse the received overcurrent adc register.
*
* Parameters:
*
* @param [in]  tIC                    Total IC
*
* @param [in]  *ic                    cell_asic_2950 ic structure pointer
*
* @param [in]  *oc_data               Oc data pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950ParseOcRegData(uint8_t tIC, cell_asic_2950 *ic, uint8_t *oc_data)
{

  uint8_t address = 0;
  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    memcpy(&ic[curr_ic].reg.rx_data[0], &oc_data[address], RX_DATA); /* dst , src , size */
    address = ((curr_ic + 1) * (RX_DATA));

#ifndef ADBMS2951
    ic[curr_ic].oc.oc1r = (ic[curr_ic].reg.rx_data[0] & 0xFF);
    ic[curr_ic].oc.oc2r = (ic[curr_ic].reg.rx_data[1] & 0xFF);
    ic[curr_ic].oc.oc3r = (ic[curr_ic].reg.rx_data[2] & 0xFF);
    ic[curr_ic].oc.refr = (ic[curr_ic].reg.rx_data[3] & 0xFF);
    ic[curr_ic].oc.oc3max = (ic[curr_ic].reg.rx_data[4] & 0xFF);
    ic[curr_ic].oc.oc3min = (ic[curr_ic].reg.rx_data[5] & 0xFF);
#else
    ic[curr_ic].oc.oc1r = 0x7F;
    ic[curr_ic].oc.oc2r = 0x7F;
    ic[curr_ic].oc.oc3r = 0x7F;
    ic[curr_ic].oc.refr = 0x7F;
    ic[curr_ic].oc.oc3max = 0x7F;
    ic[curr_ic].oc.oc3min = 0x80;
#endif
  }
}

/**
*******************************************************************************
* Function: adBms2950ParseAccCurrentRegData
* @brief Parse Accumulated Current Register Data.
*
* @details This function Parse the received accumulated current register data.
*
* Parameters:
*
* @param [in]  tIC                    Total IC
*
* @param [in]  *ic                    cell_asic_2950 ic structure pointer
*
* @param [in]  *iacc_data             Accumulated current data pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950ParseAccCurrentRegData(uint8_t tIC, cell_asic_2950 *ic, uint8_t *iacc_data)
{
  uint8_t address = 0;
  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    memcpy(&ic[curr_ic].reg.rx_data[0], &iacc_data[address], RX_DATA); /* dst , src , size */
    address = ((curr_ic + 1) * (RX_DATA));
    ic[curr_ic].iacc.i1acc = ic[curr_ic].reg.rx_data[0] + (ic[curr_ic].reg.rx_data[1] << 8) + (ic[curr_ic].reg.rx_data[2] << 16);
    ic[curr_ic].iacc.i2acc = ic[curr_ic].reg.rx_data[3] + (ic[curr_ic].reg.rx_data[4] << 8) + (ic[curr_ic].reg.rx_data[5] << 16);
  }
}

/**
*******************************************************************************
* Function: adBms2950ParseAccVbatRegData
* @brief Parse Accumulated Battery Voltage Register Data.
*
* @details This function Parse the received accumulated battery voltage register data from RDVBACC.
*
* Parameters:
*
* @param [in]  tIC                    Total IC
*
* @param [in]  *ic                    cell_asic_2950 ic structure pointer
*
* @param [in]  *accvbat_data          accvbat data pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950ParseAccVbatRegData(uint8_t tIC, cell_asic_2950 *ic, uint8_t *accvbat_data)
{
  uint8_t address = 0;
  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    memcpy(&ic[curr_ic].reg.rx_data[0], &accvbat_data[address], RX_DATA); /* dst , src , size */
    address = ((curr_ic + 1) * (RX_DATA));
    ic[curr_ic].vbacc.vb1acc = ic[curr_ic].reg.rx_data[0] + (ic[curr_ic].reg.rx_data[1] << 8) + (ic[curr_ic].reg.rx_data[2] << 16);
    ic[curr_ic].vbacc.vb2acc = ic[curr_ic].reg.rx_data[3] + (ic[curr_ic].reg.rx_data[4] << 8) + (ic[curr_ic].reg.rx_data[5] << 16);
  }
}

/**
*******************************************************************************
* Function: adBms2950ParseAccIVbatRegData
* @brief Parse Accumulated Current and Battery Voltage Register Data.
*
* @details This function Parse the received Accumulated current and battery voltage register data.
*
* Parameters:
*
* @param [in]  tIC                    Total IC
*
* @param [in]  *ic                    cell_asic_2950 ic structure pointer
*
* @param [in]  *accivbat_data         accivbat data pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950ParseAccIVbatRegData(uint8_t tIC, cell_asic_2950 *ic, uint8_t *accivbat_data)
{
  uint8_t address = 0;
  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    memcpy(&ic[curr_ic].reg.rx_data[0], &accivbat_data[address], RX_DATA); /* dst , src , size */
    address = ((curr_ic + 1) * (RX_DATA));
    ic[curr_ic].i_vbacc.i1acc = ic[curr_ic].reg.rx_data[0] + (ic[curr_ic].reg.rx_data[1] << 8) + (ic[curr_ic].reg.rx_data[2] << 16);
    ic[curr_ic].i_vbacc.vb1acc = ic[curr_ic].reg.rx_data[3] + (ic[curr_ic].reg.rx_data[4] << 8) + (ic[curr_ic].reg.rx_data[5] << 16);
  }
}

/**
*******************************************************************************
* Function: adBms2950V1ParseData
* @brief Parse Voltage Register Data.
*
* @details This function Parse the received voltage register data.
*
* Parameters:
*
* @param [in]  tIC                    Total IC
*
* @param [in]  *ic                    cell_asic_2950 ic structure pointer
*
* @param [in]  grp                    Enum type register group
*
* @param [in]  *vr_data               Volatge reg. data pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950V1ParseData(uint8_t tIC, cell_asic_2950 *ic, REG_GRP grp, uint8_t *vr_data)
{
  uint8_t *data, data_size, address = 0;
  if (grp == GRP_ALL)
  {
    data_size = ALLVR_SIZE;
  }
  else
  {
    data_size = RX_DATA;
  }
  data = (uint8_t *)calloc(data_size, sizeof(uint8_t));
  if (data == NULL)
  {
    printf("Failed to allocate parse vr memory");
    exit(0);
  }
  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    memcpy(&data[0], &vr_data[address], data_size); /* dst , src , size */
    address = ((curr_ic + 1) * (data_size));
    switch (grp)
    {
    case GRP_A:                                               /* VR Register group A */
      ic[curr_ic].vr.v_codes[0] = (data[0] + (data[1] << 8)); // V1A
      ic[curr_ic].vr.v_codes[1] = (data[2] + (data[3] << 8)); // V2A
      ic[curr_ic].vr.v_codes[2] = (data[4] + (data[5] << 8)); // V3A
      break;

    case GRP_B:                                               /* VR Register group B */
      ic[curr_ic].vr.v_codes[3] = (data[0] + (data[1] << 8)); // V4A
      ic[curr_ic].vr.v_codes[4] = (data[2] + (data[3] << 8)); // V5A
      ic[curr_ic].vr.v_codes[5] = (data[4] + (data[5] << 8)); // V6A
      break;

    case GRP_C:                                               /* VR Register group C */
      ic[curr_ic].vr.v_codes[6] = (data[0] + (data[1] << 8)); // V7A
      ic[curr_ic].vr.v_codes[7] = (data[2] + (data[3] << 8)); // V8A
      ic[curr_ic].vr.v_codes[8] = (data[4] + (data[5] << 8)); // VREF2A
      break;

    case GRP_D:                                                /* VR Register group D */
      ic[curr_ic].vr.v_codes[9] = (data[0] + (data[1] << 8));  // V7A
      ic[curr_ic].vr.v_codes[10] = (data[2] + (data[3] << 8)); // V8A
      ic[curr_ic].vr.v_codes[11] = (data[4] + (data[5] << 8)); // V9B
      break;

    case GRP_ALL:                                               /* VR Register group ALL (RDALLV) */
      ic[curr_ic].vr.v_codes[0] = (data[0] + (data[1] << 8));   // V1A
      ic[curr_ic].vr.v_codes[1] = (data[2] + (data[3] << 8));   // V2A
      ic[curr_ic].vr.v_codes[2] = (data[4] + (data[5] << 8));   // V3A
      ic[curr_ic].vr.v_codes[3] = (data[6] + (data[7] << 8));   // V4A
      ic[curr_ic].vr.v_codes[4] = (data[8] + (data[9] << 8));   // V5A
      ic[curr_ic].vr.v_codes[5] = (data[10] + (data[11] << 8)); // V6A
      ic[curr_ic].vr.v_codes[6] = (data[12] + (data[13] << 8)); // V7A
      ic[curr_ic].vr.v_codes[7] = (data[14] + (data[15] << 8)); // V8A
      ic[curr_ic].vr.v_codes[8] = (data[16] + (data[17] << 8)); // V9B
      ic[curr_ic].vr.v_codes[9] = (data[18] + (data[19] << 8)); // V10B
      break;

    default:
      break;
    }
  }
  free(data);
}

/**
*******************************************************************************
* Function: adBms2950V2ParseData
* @brief Parse Voltage Register Data.
*
* @details This function Parse the received Redundant voltage register data.
*
* Parameters:
*
* @param [in]  tIC                    Total IC
*
* @param [in]  *ic                    cell_asic_2950 ic structure pointer
*
* @param [in]  grp                    Enum type register group
*
* @param [in]  *redvr_data            Redundant volatge reg. data pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950V2ParseData(uint8_t tIC, cell_asic_2950 *ic, REG_GRP grp, uint8_t *redvr_data)
{
  uint8_t *data, data_size, address = 0;
  if (grp == GRP_ALL)
  {
    data_size = ALLREDVR_SIZE;
  }
  else
  {
    data_size = RX_DATA;
  }
  data = (uint8_t *)calloc(data_size, sizeof(uint8_t));
  if (data == NULL)
  {
    printf("Failed to allocate parse redvr memory");
    exit(0);
  }
  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    memcpy(&data[0], &redvr_data[address], data_size); /* dst , src , size */
    address = ((curr_ic + 1) * (data_size));
    switch (grp)
    {
    case GRP_A:                                                   /* RedVR Register group A */
      ic[curr_ic].rvr.redv_codes[0] = (data[0] + (data[1] << 8)); // V1B
      ic[curr_ic].rvr.redv_codes[1] = (data[2] + (data[3] << 8)); // V2B
      ic[curr_ic].rvr.redv_codes[2] = (data[4] + (data[5] << 8)); // V3B
      break;

    case GRP_B:                                                   /* RedVR Register group B */
      ic[curr_ic].rvr.redv_codes[3] = (data[0] + (data[1] << 8)); // V4B
      ic[curr_ic].rvr.redv_codes[4] = (data[2] + (data[3] << 8)); // V5B
      ic[curr_ic].rvr.redv_codes[5] = (data[4] + (data[5] << 8)); // V6B
      break;

    case GRP_C:                                                   /* RedVR Register group C */
      ic[curr_ic].rvr.redv_codes[6] = (data[0] + (data[1] << 8)); // V9B
      ic[curr_ic].rvr.redv_codes[7] = (data[2] + (data[3] << 8)); // V10B
      ic[curr_ic].rvr.redv_codes[8] = (data[4] + (data[5] << 8)); // VREF2B
      break;

    case GRP_D:                                                    /* RedVR Register group D */
      ic[curr_ic].rvr.redv_codes[9] = (data[0] + (data[1] << 8));  // V10B
      ic[curr_ic].rvr.redv_codes[10] = (data[2] + (data[3] << 8)); // VREF2A
      ic[curr_ic].rvr.redv_codes[11] = (data[4] + (data[5] << 8)); // VREF2B
      break;

    case GRP_E:                                                    /* RedVR Register group D */
      ic[curr_ic].rvr.redv_codes[12] = (data[0] + (data[1] << 8)); // V10B
      break;

    case GRP_ALL:                                                   /* RedVR Register group ALL (RDALLR) */
      ic[curr_ic].rvr.redv_codes[0] = (data[0] + (data[1] << 8));   // V1B
      ic[curr_ic].rvr.redv_codes[1] = (data[2] + (data[3] << 8));   // V2B
      ic[curr_ic].rvr.redv_codes[2] = (data[4] + (data[5] << 8));   // V3B
      ic[curr_ic].rvr.redv_codes[3] = (data[6] + (data[7] << 8));   // V4B
      ic[curr_ic].rvr.redv_codes[4] = (data[8] + (data[9] << 8));   // V5B
      ic[curr_ic].rvr.redv_codes[5] = (data[10] + (data[11] << 8)); // V6B
      ic[curr_ic].rvr.redv_codes[6] = (data[12] + (data[13] << 8)); // V7A
      ic[curr_ic].rvr.redv_codes[7] = (data[14] + (data[15] << 8)); // V8A
      ic[curr_ic].rvr.redv_codes[8] = (data[16] + (data[17] << 8)); // V9B
      ic[curr_ic].rvr.redv_codes[9] = (data[18] + (data[19] << 8)); // V10B
      break;

    default:
      break;
    }
  }
  free(data);
}

/**
*******************************************************************************
* Function: adBms2950ParseAuxA
* @brief Parse status A register data
*
* @details This function Parse the recived status A register data.
*
* Parameters:
*
* @param [in]  tIC                     Total IC
*
* @param [in]  *ic                     cell_asic_2950 ic structure pointer
*
* @param [in]  *data                   data pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950ParseAuxA(uint8_t tIC, cell_asic_2950 *ic, uint8_t *data)
{
  uint8_t address = 0;
  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    memcpy(&ic[curr_ic].axa.rx_data[0], &data[address], RX_DATA); /* dst , src , size */
    address = ((curr_ic + 1) * (RX_DATA));
    ic[curr_ic].auxa.vref1p25 = (ic[curr_ic].axa.rx_data[0] | (ic[curr_ic].axa.rx_data[1] << 8));
    ic[curr_ic].auxa.tmp1 = (ic[curr_ic].axa.rx_data[2] | (ic[curr_ic].axa.rx_data[3] << 8));
    ic[curr_ic].auxa.vreg = (ic[curr_ic].axa.rx_data[4] | (ic[curr_ic].axa.rx_data[5] << 8));
  }
}

/**
*******************************************************************************
* Function: adBms2950ParseAuxB
* @brief Parse status B register data
*
* @details This function Parse the recived status B register data.
*
* Parameters:
*
* @param [in]  tIC                     Total IC
*
* @param [in]  *ic                     cell_asic_2950 ic structure pointer
*
* @param [in]  *data                   data pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950ParseAuxB(uint8_t tIC, cell_asic_2950 *ic, uint8_t *data)
{
  uint8_t address = 0;
  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    memcpy(&ic[curr_ic].axb.rx_data[0], &data[address], RX_DATA); /* dst , src , size */
    address = ((curr_ic + 1) * (RX_DATA));

    ic[curr_ic].auxb.vdd = (ic[curr_ic].axb.rx_data[0] | (ic[curr_ic].axb.rx_data[1] << 8));
    ic[curr_ic].auxb.vdig = (ic[curr_ic].axb.rx_data[2] | (ic[curr_ic].axb.rx_data[3] << 8));
    ic[curr_ic].auxb.epad = (ic[curr_ic].axb.rx_data[4] | (ic[curr_ic].axb.rx_data[5] << 8));
  }
}

/**
*******************************************************************************
* Function: adBms2950ParseAuxC
* @brief Parse status D register data
*
* @details This function Parse the recived status D register data.
*
* Parameters:
*
* @param [in]  tIC                     Total IC
*
* @param [in]  *ic                     cell_asic_2950 ic structure pointer
*
* @param [in]  *data                   data pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950ParseAuxC(uint8_t tIC, cell_asic_2950 *ic, uint8_t *data)
{
  uint8_t address = 0;
  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    memcpy(&ic[curr_ic].axc.rx_data[0], &data[address], RX_DATA); /* dst , src , size */
    address = ((curr_ic + 1) * (RX_DATA));

    ic[curr_ic].auxc.vdiv = (ic[curr_ic].axc.rx_data[0] | (ic[curr_ic].axc.rx_data[1] << 8));
    ic[curr_ic].auxc.tmp2 = (ic[curr_ic].axc.rx_data[2] | (ic[curr_ic].axc.rx_data[3] << 8));
    ic[curr_ic].auxc.osccnt = (ic[curr_ic].axc.rx_data[5]);
  }
}

/**
*******************************************************************************
* Function: adBms2950ParseAux
* @brief Parse AUX register data
*
* @details This function Parse the recived AUX register data.
*
* Parameters:
*
* @param [in]  tIC                     Total IC
*
* @param [in]  *ic                     cell_asic_2950 ic structure pointer
*
* @param [in]  *data                   data pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950ParseAux(uint8_t tIC, cell_asic_2950 *ic, REG_GRP grp, uint8_t *data)
{
  switch (grp)
  {
  case GRP_A: /* Status Register group A */
    adBms2950ParseAuxA(tIC, &ic[0], &data[0]);
    break;

  case GRP_B: /* Status Register group B */
    adBms2950ParseAuxB(tIC, &ic[0], &data[0]);
    break;

  case GRP_C: /* Status Register group C */
    adBms2950ParseAuxC(tIC, &ic[0], &data[0]);
    break;

  case GRP_ALL: /* Status Register group ALL */
    // TBD
    break;

  default:
    break;
  }
}

/**
*******************************************************************************
* Function: adBms2950ParseFlag
* @brief Parse status C register data
*
* @details This function Parse the recived status C register data.
*
* Parameters:
*
* @param [in]  tIC                     Total IC
*
* @param [in]  *ic                     cell_asic_2950 ic structure pointer
*
* @param [in]  *data                   data pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950ParseFlag(uint8_t tIC, cell_asic_2950 *ic, uint8_t *data)
{
  uint8_t address = 0;
  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    memcpy(&ic[curr_ic].flg.rx_data[0], &data[address], RX_DATA); /* dst , src , size */
    address = ((curr_ic + 1) * (RX_DATA));

    ic[curr_ic].flag.oc1l = (ic[curr_ic].flg.rx_data[0] & 0x01);
    ic[curr_ic].flag.ocal = (ic[curr_ic].flg.rx_data[0] & 0x02) >> 1;
    ic[curr_ic].flag.ocagd_clrm = (ic[curr_ic].flg.rx_data[0] & 0x04) >> 2;
    ic[curr_ic].flag.oc3l = (ic[curr_ic].flg.rx_data[0] & 0x08) >> 3;
    ic[curr_ic].flag.ocmm = (ic[curr_ic].flg.rx_data[0] & 0x10) >> 4;
    ic[curr_ic].flag.vdruv = (ic[curr_ic].flg.rx_data[0] & 0x20) >> 5;
    ic[curr_ic].flag.v1d = (ic[curr_ic].flg.rx_data[0] & 0x40) >> 6;
    ic[curr_ic].flag.i1d = (ic[curr_ic].flg.rx_data[0] & 0x80) >> 7;

    ic[curr_ic].flag.oc2l = (ic[curr_ic].flg.rx_data[1] & 0x01);
    ic[curr_ic].flag.ocbl = (ic[curr_ic].flg.rx_data[1] & 0x02) >> 1;
    ic[curr_ic].flag.ocbgd = (ic[curr_ic].flg.rx_data[1] & 0x04) >> 2;
    ic[curr_ic].flag.refflt = (ic[curr_ic].flg.rx_data[1] & 0x08) >> 3;
    ic[curr_ic].flag.noclk = (ic[curr_ic].flg.rx_data[1] & 0x10) >> 4;
    ic[curr_ic].flag.vdduv = (ic[curr_ic].flg.rx_data[1] & 0x20) >> 5;
    ic[curr_ic].flag.v2d = (ic[curr_ic].flg.rx_data[1] & 0x40) >> 6;
    ic[curr_ic].flag.i2d = (ic[curr_ic].flg.rx_data[1] & 0x80) >> 7;

    ic[curr_ic].flag.i1pha = (ic[curr_ic].flg.rx_data[3] & 0x03);
    ic[curr_ic].flag.i1cnt = (((ic[curr_ic].flg.rx_data[2] & 0x1F) << 6) | ((ic[curr_ic].flg.rx_data[3] & 0xFC) >> 2));
    ic[curr_ic].flag.i2cnt = (ic[curr_ic].flg.rx_data[2] & 0x0E) >> 5;

    ic[curr_ic].flag.med2 = (ic[curr_ic].flg.rx_data[4] & 0x01);
    ic[curr_ic].flag.sed2 = (ic[curr_ic].flg.rx_data[4] & 0x02) >> 1;
    ic[curr_ic].flag.med1 = (ic[curr_ic].flg.rx_data[4] & 0x04) >> 2;
    ic[curr_ic].flag.sed1 = (ic[curr_ic].flg.rx_data[4] & 0x08) >> 3;
    ic[curr_ic].flag.vdiguv = (ic[curr_ic].flg.rx_data[4] & 0x10) >> 4;
    ic[curr_ic].flag.vdigov = (ic[curr_ic].flg.rx_data[4] & 0x20) >> 5;

    ic[curr_ic].flag.vreguv = (ic[curr_ic].flg.rx_data[4] & 0x40) >> 6;
    ic[curr_ic].flag.vregov = (ic[curr_ic].flg.rx_data[4] & 0x80) >> 7;

    ic[curr_ic].flag.oscflt = (ic[curr_ic].flg.rx_data[5] & 0x01);
    ic[curr_ic].flag.tmode = (ic[curr_ic].flg.rx_data[5] & 0x02) >> 1;
    ic[curr_ic].flag.thsd = (ic[curr_ic].flg.rx_data[5] & 0x04) >> 2;
    ic[curr_ic].flag.reset = (ic[curr_ic].flg.rx_data[5] & 0x08) >> 3;
    ic[curr_ic].flag.spiflt = (ic[curr_ic].flg.rx_data[5] & 0x10) >> 4;
    ic[curr_ic].flag.vde = (ic[curr_ic].flg.rx_data[5] & 0x40) >> 6;
    ic[curr_ic].flag.vdel = (ic[curr_ic].flg.rx_data[5] & 0x80) >> 7;
  }
}

/**
*******************************************************************************
* Function: adBms2950ParseStatusE
* @brief Parse status E register data
*
* @details This function Parse the recived status E register data.
*
* Parameters:
*
* @param [in]  tIC                     Total IC
*
* @param [in]  *ic                     cell_asic_2950 ic structure pointer
*
* @param [in]  *data                   data pointer
*
* @return None
*
*******************************************************************************
*/
#if 0
void adBms2950ParseStatusE(uint8_t tIC, cell_asic_2950 *ic, uint8_t *data)
{
  uint8_t address = 0;
  for(uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    memcpy(&ic[curr_ic].ste.rx_data[0], &data[address], RX_DATA); /* dst , src , size */
    address = ((curr_ic+1) * (RX_DATA));
    ic[curr_ic].state.gpio   = ((ic[curr_ic].ste.rx_data[4] & 0x1E));
    ic[curr_ic].state.gpo   = ((ic[curr_ic].ste.rx_data[5] & 0x03) << 4 | ((ic[curr_ic].ste.rx_data[4] & 0xE0) >> 4) | (ic[curr_ic].ste.rx_data[4] & 0x01));
    ic[curr_ic].state.rev = ((ic[curr_ic].ste.rx_data[5] & 0xF0) >> 4);
  }
}
#endif

/**
*******************************************************************************
* Function: adBms2950ParseStatus
* @brief Parse status register data
*
* @details This function Parse the recived status register data.
*
* Parameters:
*
* @param [in]  tIC                     Total IC
*
* @param [in]  *ic                     cell_asic_2950 ic structure pointer
*
* @param [in]  *data                   data pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950ParseStatus(uint8_t tIC, cell_asic_2950 *ic, REG_GRP grp, uint8_t *data)
{
  uint8_t address = 0;
  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    memcpy(&ic[curr_ic].ste.rx_data[0], &data[address], RX_DATA); /* dst , src , size */
    address = ((curr_ic + 1) * (RX_DATA));

    ic[curr_ic].state.ocap = (ic[curr_ic].ste.rx_data[0] & 0x01);
    ic[curr_ic].state.ocbp = (ic[curr_ic].ste.rx_data[0] & 0x02) >> 1;

    ic[curr_ic].state.der = (ic[curr_ic].ste.rx_data[1] & 0x03);
    ic[curr_ic].state.i1cal = (ic[curr_ic].ste.rx_data[1] & 0x40) >> 6;
    ic[curr_ic].state.i2cal = (ic[curr_ic].ste.rx_data[1] & 0x80) >> 7;

    ic[curr_ic].state.gpo5l = (ic[curr_ic].ste.rx_data[3] & 0x01);
    ic[curr_ic].state.gpo6l = (ic[curr_ic].ste.rx_data[3] & 0x02) >> 1;
    ic[curr_ic].state.gpo1h = (ic[curr_ic].ste.rx_data[3] & 0x04) >> 2;
    ic[curr_ic].state.gpo2h = (ic[curr_ic].ste.rx_data[3] & 0x08) >> 3;
    ic[curr_ic].state.gpo3h = (ic[curr_ic].ste.rx_data[3] & 0x10) >> 4;
    ic[curr_ic].state.gpo4h = (ic[curr_ic].ste.rx_data[3] & 0x20) >> 5;
    ic[curr_ic].state.gpo5h = (ic[curr_ic].ste.rx_data[3] & 0x40) >> 6;
    ic[curr_ic].state.gpo6h = (ic[curr_ic].ste.rx_data[3] & 0x80) >> 7;

    ic[curr_ic].state.gpio1l = (ic[curr_ic].ste.rx_data[4] & 0x01);
    ic[curr_ic].state.gpio2l = (ic[curr_ic].ste.rx_data[4] & 0x02) >> 1;
    ic[curr_ic].state.gpio3l = (ic[curr_ic].ste.rx_data[4] & 0x04) >> 2;
    ic[curr_ic].state.gpio4l = (ic[curr_ic].ste.rx_data[4] & 0x08) >> 3;
    ic[curr_ic].state.gpo1l = (ic[curr_ic].ste.rx_data[4] & 0x10) >> 4;
    ic[curr_ic].state.gpo2l = (ic[curr_ic].ste.rx_data[4] & 0x20) >> 5;
    ic[curr_ic].state.gpo3l = (ic[curr_ic].ste.rx_data[4] & 0x40) >> 6;
    ic[curr_ic].state.gpo4l = (ic[curr_ic].ste.rx_data[4] & 0x80) >> 7;

    ic[curr_ic].state.rev = ((ic[curr_ic].ste.rx_data[5] & 0xF0) >> 4);
  }
}

/**
*******************************************************************************
* Function: adBms2950ParseRdalli
* @brief Parse RDALLI data
*
* @details This function Parse the recived RDALLI data.
*
* Parameters:
*
* @param [in]  tIC                     Total IC
*
* @param [in]  *ic                     cell_asic_2950 ic structure pointer
*
* @param [in]  *data                   data pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950ParseRdalli(uint8_t tIC, cell_asic_2950 *ic, uint8_t *data)
{
  uint8_t *data1, data_size, address = 0;
  data_size = RDALLI_SIZE;
  data1 = (uint8_t *)calloc(data_size, sizeof(uint8_t));

  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    memcpy(&data1[0], &data[address], data_size); /* dst , src , size */
    address = ((curr_ic + 1) * (data_size));

    ic[curr_ic].rdalli.i1 = (data1[0] | (data1[1] << 8) | (data1[2] << 16));
    ic[curr_ic].rdalli.i2 = (data1[3] | (data1[4] << 8) | (data1[5] << 16));

    ic[curr_ic].rdalli.vb1 = (data1[6] | (data1[7] << 8));
    ic[curr_ic].rdalli.vb2 = (data1[8] | (data1[9] << 8));

    ic[curr_ic].rdalli.oc1r = data1[10];
    ic[curr_ic].rdalli.oc2r = data1[11];
    ic[curr_ic].rdalli.oc3r = data1[12];

    ic[curr_ic].rdalli.stat3 = data1[13];

    ic[curr_ic].rdalli.flag0 = data1[14];
    ic[curr_ic].rdalli.flag1 = data1[15];
    ic[curr_ic].rdalli.flag2 = data1[16];
    ic[curr_ic].rdalli.flag3 = data1[17];
    ic[curr_ic].rdalli.flag4 = data1[18];
    ic[curr_ic].rdalli.flag5 = data1[19];
  }
}

/**
*******************************************************************************
* Function: adBms2950ParseRdalla
* @brief Parse RDALLA data
*
* @details This function Parse the recived RDALLI data.
*
* Parameters:
*
* @param [in]  tIC                     Total IC
*
* @param [in]  *ic                     cell_asic_2950 ic structure pointer
*
* @param [in]  *data                   data pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950ParseRdalla(uint8_t tIC, cell_asic_2950 *ic, uint8_t *data)
{
  uint8_t *data1, data_size, address = 0;
  data_size = RDALLA_SIZE;
  data1 = (uint8_t *)calloc(data_size, sizeof(uint8_t));

  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    memcpy(&data1[0], &data[address], data_size); /* dst , src , size */
    address = ((curr_ic + 1) * (data_size));

    ic[curr_ic].rdalla.i1acc = (data1[0] | (data1[1] << 8) | (data1[2] << 16));
    ic[curr_ic].rdalla.i2acc = (data1[3] | (data1[4] << 8) | (data1[5] << 16));

    ic[curr_ic].rdalla.vb1acc = (data1[6] | (data1[7] << 8) | (data1[8] << 16));
    ic[curr_ic].rdalla.vb2acc = (data1[9] | (data1[10] << 8) | (data1[11] << 16));

    ic[curr_ic].rdalla.stat3 = data1[12];
    ic[curr_ic].rdalla.stat4 = data1[13];

    ic[curr_ic].rdalla.flag0 = data1[14];
    ic[curr_ic].rdalla.flag1 = data1[15];
    ic[curr_ic].rdalla.flag2 = data1[16];
    ic[curr_ic].rdalla.flag3 = data1[17];
    ic[curr_ic].rdalla.flag4 = data1[18];
    ic[curr_ic].rdalla.flag5 = data1[19];
  }
}

/**
*******************************************************************************
* Function: adBms2950ParseRdallc
* @brief Parse RDALLC data
*
* @details This function Parse the recived RDALLC data.
*
* Parameters:
*
* @param [in]  tIC                     Total IC
*
* @param [in]  *ic                     cell_asic_2950 ic structure pointer
*
* @param [in]  *data                   data pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950ParseRdallc(uint8_t tIC, cell_asic_2950 *ic, uint8_t *data)
{
  uint8_t *data1, data_size, address = 0;
  data_size = RDALLC_SIZE;
  data1 = (uint8_t *)calloc(data_size, sizeof(uint8_t));

  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    memcpy(&data1[0], &data[address], data_size); /* dst , src , size */
    address = ((curr_ic + 1) * (data_size));

    ic[curr_ic].rdallc.cfga0 = data1[0];
    ic[curr_ic].rdallc.cfga1 = data1[1];
    ic[curr_ic].rdallc.cfga2 = data1[2];
    ic[curr_ic].rdallc.cfga3 = data1[3];
    ic[curr_ic].rdallc.cfga4 = data1[4];
    ic[curr_ic].rdallc.cfga5 = data1[5];

    ic[curr_ic].rdallc.cfgb0 = data1[6];
    ic[curr_ic].rdallc.cfgb1 = data1[7];
    ic[curr_ic].rdallc.cfgb2 = data1[8];
    ic[curr_ic].rdallc.cfgb3 = data1[9];
    ic[curr_ic].rdallc.cfgb4 = data1[10];
    ic[curr_ic].rdallc.cfgb5 = data1[11];

    ic[curr_ic].rdallc.stat3 = data1[12];
    ic[curr_ic].rdallc.stat4 = data1[13];

    ic[curr_ic].rdallc.flag0 = data1[14];
    ic[curr_ic].rdallc.flag1 = data1[15];
    ic[curr_ic].rdallc.flag2 = data1[16];
    ic[curr_ic].rdallc.flag3 = data1[17];
    ic[curr_ic].rdallc.flag4 = data1[18];
    ic[curr_ic].rdallc.flag5 = data1[19];
  }
}

/**
*******************************************************************************
* Function: adBms2950ParseRdallv
* @brief Parse RDALLV data
*
* @details This function Parse the recived RDALLV data.
*
* Parameters:
*
* @param [in]  tIC                     Total IC
*
* @param [in]  *ic                     cell_asic_2950 ic structure pointer
*
* @param [in]  *data                   data pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950ParseRdallv(uint8_t tIC, cell_asic_2950 *ic, uint8_t *data)
{
  uint8_t *data1, data_size, address = 0;
  data_size = RDALLV_SIZE;
  data1 = (uint8_t *)calloc(data_size, sizeof(uint8_t));

  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    memcpy(&data1[0], &data[address], data_size); /* dst , src , size */
    address = ((curr_ic + 1) * (data_size));

    ic[curr_ic].rdallv.v1 = (data1[0] | (data1[1] << 8));
    ic[curr_ic].rdallv.v2 = (data1[2] | (data1[3] << 8));
    ic[curr_ic].rdallv.v3 = (data1[4] | (data1[5] << 8));
    ic[curr_ic].rdallv.v4 = (data1[6] | (data1[7] << 8));
    ic[curr_ic].rdallv.v5 = (data1[8] | (data1[9] << 8));
    ic[curr_ic].rdallv.v6 = (data1[10] | (data1[11] << 8));
    ic[curr_ic].rdallv.v7 = (data1[12] | (data1[13] << 8));
    ic[curr_ic].rdallv.v8 = (data1[14] | (data1[15] << 8));
    ic[curr_ic].rdallv.v9 = (data1[16] | (data1[17] << 8));
    ic[curr_ic].rdallv.v10 = (data1[18] | (data1[19] << 8));
  }
}

/**
*******************************************************************************
* Function: adBms2950ParseRdallr
* @brief Parse RDALLR data
*
* @details This function Parse the recived RDALLR data.
*
* Parameters:
*
* @param [in]  tIC                     Total IC
*
* @param [in]  *ic                     cell_asic_2950 ic structure pointer
*
* @param [in]  *data                   data pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950ParseRdallr(uint8_t tIC, cell_asic_2950 *ic, uint8_t *data)
{
  uint8_t *data1, data_size, address = 0;
  data_size = RDALLR_SIZE;
  data1 = (uint8_t *)calloc(data_size, sizeof(uint8_t));

  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    memcpy(&data1[0], &data[address], data_size); /* dst , src , size */
    address = ((curr_ic + 1) * (data_size));

    ic[curr_ic].rdallr.v1 = (data1[0] | (data1[1] << 8));
    ic[curr_ic].rdallr.v2 = (data1[2] | (data1[3] << 8));
    ic[curr_ic].rdallr.v3 = (data1[4] | (data1[5] << 8));
    ic[curr_ic].rdallr.v4 = (data1[6] | (data1[7] << 8));
    ic[curr_ic].rdallr.v5 = (data1[8] | (data1[9] << 8));
    ic[curr_ic].rdallr.v6 = (data1[10] | (data1[11] << 8));
    ic[curr_ic].rdallr.v7 = (data1[12] | (data1[13] << 8));
    ic[curr_ic].rdallr.v8 = (data1[14] | (data1[15] << 8));
    ic[curr_ic].rdallr.v9 = (data1[16] | (data1[17] << 8));
    ic[curr_ic].rdallr.v10 = (data1[18] | (data1[19] << 8));
  }
}

/**
*******************************************************************************
* Function: adBms2950ParseRdallx
* @brief Parse RDALLX data
*
* @details This function Parse the recived RDALLR data.
*
* Parameters:
*
* @param [in]  tIC                     Total IC
*
* @param [in]  *ic                     cell_asic_2950 ic structure pointer
*
* @param [in]  *data                   data pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950ParseRdallx(uint8_t tIC, cell_asic_2950 *ic, uint8_t *data)
{
  uint8_t *data1, data_size, address = 0;
  data_size = RDALLX_SIZE;
  data1 = (uint8_t *)calloc(data_size, sizeof(uint8_t));

  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    memcpy(&data1[0], &data[address], data_size); /* dst , src , size */
    address = ((curr_ic + 1) * (data_size));

    ic[curr_ic].rdallx.vref2A = (data1[0] | (data1[1] << 8));
    ic[curr_ic].rdallx.vref2B = (data1[2] | (data1[3] << 8));
    ic[curr_ic].rdallx.vref1p25 = (data1[4] | (data1[5] << 8));
    ic[curr_ic].rdallx.tmp1 = (data1[6] | (data1[7] << 8));
    ic[curr_ic].rdallx.vreg = (data1[8] | (data1[9] << 8));
    ic[curr_ic].rdallx.vdd = (data1[10] | (data1[11] << 8));
    ic[curr_ic].rdallx.vdig = (data1[12] | (data1[13] << 8));
    // ic[curr_ic].rdallx.gnd              = (data1[14] | (data1[15] << 8));
    ic[curr_ic].rdallx.epad = (data1[14] | (data1[15] << 8));
    ic[curr_ic].rdallx.vdiv = (data1[16] | (data1[17] << 8));
    ic[curr_ic].rdallx.tmp2 = (data1[18] | (data1[19] << 8));
  }
}

/**
*******************************************************************************
* Function: adBms2950ParseComm
* @brief Parse comm register
*
* @details This function Parse the recived comm register data.
*
* Parameters:
*
* @param [in]  tIC                     Total IC
*
* @param [in]  *ic                     cell_asic_2950 ic structure pointer
*
* @param [in]  *data                   data pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950ParseComm(uint8_t tIC, cell_asic_2950 *ic, uint8_t *data)
{
  uint8_t address = 0;
  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    memcpy(&ic[curr_ic].com.rx_data[0], &data[address], RX_DATA); /* dst , src , size */
    address = ((curr_ic + 1) * (RX_DATA));
    ic[curr_ic].rx_comm.icomm[0] = ((ic[curr_ic].com.rx_data[0] & 0xF0) >> 4);
    ic[curr_ic].rx_comm.fcomm[0] = (ic[curr_ic].com.rx_data[0] & 0x0F);
    ic[curr_ic].rx_comm.data[0] = (ic[curr_ic].com.rx_data[1]);
    ic[curr_ic].rx_comm.icomm[1] = ((ic[curr_ic].com.rx_data[2] & 0xF0) >> 4);
    ic[curr_ic].rx_comm.data[1] = (ic[curr_ic].com.rx_data[3]);
    ic[curr_ic].rx_comm.fcomm[1] = (ic[curr_ic].com.rx_data[2] & 0x0F);
    ic[curr_ic].rx_comm.icomm[2] = ((ic[curr_ic].com.rx_data[4] & 0xF0) >> 4);
    ic[curr_ic].rx_comm.data[2] = (ic[curr_ic].com.rx_data[5]);
    ic[curr_ic].rx_comm.fcomm[2] = (ic[curr_ic].com.rx_data[4] & 0x0F);
  }
}

/**
*******************************************************************************
* Function: adBms2950ParseSID
* @brief Parse SID register
*
* @details This function Parse the recived sid register data.
*
* Parameters:
*
* @param [in]  tIC                     Total IC
*
* @param [in]  *ic                     cell_asic_2950 ic structure pointer
*
* @param [in]  *data                   data pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950ParseSID(uint8_t tIC, cell_asic_2950 *ic, uint8_t *data)
{
  uint8_t address = 0;
  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    memcpy(&ic[curr_ic].rsid.rx_data[0], &data[address], RX_DATA); /* dst , src , size */
    address = ((curr_ic + 1) * (RX_DATA));
    ic[curr_ic].sid.sid[0] = ic[curr_ic].rsid.rx_data[0];
    ic[curr_ic].sid.sid[1] = ic[curr_ic].rsid.rx_data[1];
    ic[curr_ic].sid.sid[2] = ic[curr_ic].rsid.rx_data[2];
    ic[curr_ic].sid.sid[3] = ic[curr_ic].rsid.rx_data[3];
    ic[curr_ic].sid.sid[4] = ic[curr_ic].rsid.rx_data[4];
    ic[curr_ic].sid.sid[5] = ic[curr_ic].rsid.rx_data[5];
  }
}

/**
*******************************************************************************
* Function: adBms2950CreateConfiga
* @brief Create the configation A write buffer
*
* @details This function create the configation A write buffer.
*
* Parameters:
*
* @param [in]  tIC                     Total IC
*
* @param [in]  *ic                     cell_asic_2950 ic structure pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950CreateConfiga(uint8_t tIC, cell_asic_2950 *ic)
{
  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    ic[curr_ic].configa.tx_data[0] = (((ic[curr_ic].tx_cfga.ocen & 0x01) << 7) | ((ic[curr_ic].tx_cfga.vs5 & 0x01) << 6) | ((ic[curr_ic].tx_cfga.vs4 & 0x01) << 5) | ((ic[curr_ic].tx_cfga.vs3 & 0x01) << 4) | ((ic[curr_ic].tx_cfga.vs2 & 0x03) << 2) | (ic[curr_ic].tx_cfga.vs1 & 0x03));
    ic[curr_ic].configa.tx_data[1] = (((ic[curr_ic].tx_cfga.injtm & 0x01) << 7) | ((ic[curr_ic].tx_cfga.injecc & 0x01) << 6) | ((ic[curr_ic].tx_cfga.injts & 0x01) << 4) | ((ic[curr_ic].tx_cfga.injmon & 0x03) << 2) | (ic[curr_ic].tx_cfga.injosc & 0x03));
    ic[curr_ic].configa.tx_data[2] = (((ic[curr_ic].tx_cfga.soak & 0x07) << 5) | ((ic[curr_ic].tx_cfga.vs10 & 0x01) << 4) | ((ic[curr_ic].tx_cfga.vs9 & 0x01) << 3) | ((ic[curr_ic].tx_cfga.vs8 & 0x01) << 2) | ((ic[curr_ic].tx_cfga.vs7 & 0x01) << 1) | (ic[curr_ic].tx_cfga.vs6 & 0x01));
    ic[curr_ic].configa.tx_data[3] = (((ic[curr_ic].tx_cfga.gpo6c & 0x03) << 5) | ((ic[curr_ic].tx_cfga.gpo5c & 0x01) << 4) | ((ic[curr_ic].tx_cfga.gpo4c & 0x01) << 3) | ((ic[curr_ic].tx_cfga.gpo3c & 0x01) << 2) | ((ic[curr_ic].tx_cfga.gpo2c & 0x01) << 1) | (ic[curr_ic].tx_cfga.gpo1c & 0x01)); // GPO1 is at position 0
    ic[curr_ic].configa.tx_data[4] = (((ic[curr_ic].tx_cfga.spi3w & 0x01) << 7) | ((ic[curr_ic].tx_cfga.gpio1fe & 0x01) << 6) | ((ic[curr_ic].tx_cfga.gpo6od & 0x01) << 5) | ((ic[curr_ic].tx_cfga.gpo5od & 0x01) << 4) | ((ic[curr_ic].tx_cfga.gpo4od & 0x01) << 3) | ((ic[curr_ic].tx_cfga.gpo3od & 0x01) << 2) | ((ic[curr_ic].tx_cfga.gpo2od & 0x01) << 1) | (ic[curr_ic].tx_cfga.gpo1od & 0x01));
    ic[curr_ic].configa.tx_data[5] = (((ic[curr_ic].tx_cfga.vb2mux & 0x01) << 7) | ((ic[curr_ic].tx_cfga.vb1mux & 0x01) << 6) | ((ic[curr_ic].tx_cfga.snapst & 0x01) << 5) | ((ic[curr_ic].tx_cfga.refup & 0x01) << 4) | ((ic[curr_ic].tx_cfga.commbk & 0x01) << 3) | (ic[curr_ic].tx_cfga.acci & 0x07));
  }
}

/**
*******************************************************************************
* Function: adBms2950CreateConfigb
* @brief Create the configation B write buffer
*
* @details This function create the configation B write buffer.
*
* Parameters:
*
* @param [in]  tIC                     Total IC
*
* @param [in]  *ic                     cell_asic_2950 ic structure pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950CreateConfigb(uint8_t tIC, cell_asic_2950 *ic)
{
  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    ic[curr_ic].configb.tx_data[0] = (((ic[curr_ic].tx_cfgb.oc1ten & 0x01) << 7) | (ic[curr_ic].tx_cfgb.oc1th & 0x7F));
    ic[curr_ic].configb.tx_data[1] = (((ic[curr_ic].tx_cfgb.oc2ten & 0x01) << 7) | (ic[curr_ic].tx_cfgb.oc2th & 0x7F));
    ic[curr_ic].configb.tx_data[2] = (((ic[curr_ic].tx_cfgb.oc3ten & 0x01) << 7) | (ic[curr_ic].tx_cfgb.oc3th & 0x7F));
    ic[curr_ic].configb.tx_data[3] = 0x00;
    ic[curr_ic].configb.tx_data[4] = 0x00;
    ic[curr_ic].configb.tx_data[5] = 0x00;
    ic[curr_ic].configb.tx_data[3] = (((ic[curr_ic].tx_cfgb.octsel & 0x03) << 6) | ((ic[curr_ic].tx_cfgb.reften & 0x01) << 5) | ((ic[curr_ic].tx_cfgb.ocdp & 0x01) << 3) | (ic[curr_ic].tx_cfgb.ocdgt & 0x03)); // GPO1 is at position 0
    ic[curr_ic].configb.tx_data[4] = (((ic[curr_ic].tx_cfgb.ocbx & 0x01) << 7) | ((ic[curr_ic].tx_cfgb.ocax & 0x01) << 6) | ((ic[curr_ic].tx_cfgb.ocmode & 0x03) << 4) | ((ic[curr_ic].tx_cfgb.oc3gc & 0x01) << 3) | ((ic[curr_ic].tx_cfgb.oc2gc & 0x01) << 2) | ((ic[curr_ic].tx_cfgb.oc1gc & 0x01) << 1) | (ic[curr_ic].tx_cfgb.ocod & 0x01));
    ic[curr_ic].configb.tx_data[5] = (((ic[curr_ic].tx_cfgb.gpio4c & 0x01) << 7) | ((ic[curr_ic].tx_cfgb.gpio3c & 0x01) << 6) | ((ic[curr_ic].tx_cfgb.gpio2c & 0x01) << 5) | ((ic[curr_ic].tx_cfgb.gpio1c & 0x01) << 4) | ((ic[curr_ic].tx_cfgb.gpio2eoc & 0x01) << 3) | (ic[curr_ic].tx_cfgb.diagsel & 0x07));
  }
}

/**
*******************************************************************************
* Function: adBms2950CreateClrflagData
* @brief Create the clear flag write buffer
*
* @details This function create the clear flag write buffer.
*
* Parameters:
*
* @param [in]  tIC                     Total IC
*
* @param [in]  *ic                     cell_asic_2950 ic structure pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950CreateClrflagData(uint8_t tIC, cell_asic_2950 *ic)
{
  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    ic[curr_ic].clrflag.tx_data[0] = (((ic[curr_ic].clflag.i1d & 0x01) << 7) | ((ic[curr_ic].clflag.v1d & 0x01) << 6) | ((ic[curr_ic].clflag.vdruv & 0x01) << 5) | ((ic[curr_ic].clflag.ocmm & 0x01) << 4) | ((ic[curr_ic].clflag.oc3l & 0x01) << 3) | ((ic[curr_ic].clflag.ocagd_clrm & 0x01) << 2) | ((ic[curr_ic].clflag.ocal & 0x01) << 2) | (ic[curr_ic].clflag.oc1l & 0x01));
    ic[curr_ic].clrflag.tx_data[1] = (((ic[curr_ic].clflag.i2d & 0x01) << 7) | ((ic[curr_ic].clflag.v2d & 0x01) << 6) | ((ic[curr_ic].clflag.vdduv & 0x01) << 5) | ((ic[curr_ic].clflag.noclk & 0x01) << 4) | ((ic[curr_ic].clflag.refflt & 0x01) << 3) | ((ic[curr_ic].clflag.ocbgd & 0x01) << 2) | ((ic[curr_ic].clflag.ocbl & 0x01) << 2) | (ic[curr_ic].clflag.oc2l & 0x01));
    ic[curr_ic].clrflag.tx_data[2] = (((ic[curr_ic].clflag.i2cnt & 0x07) << 5) | (ic[curr_ic].clflag.i1cnt & 0x07C0));
    ic[curr_ic].clrflag.tx_data[3] = (((ic[curr_ic].clflag.i1cnt & 0x003F) << 2) | (ic[curr_ic].clflag.i1pha & 0x03));
    ic[curr_ic].clrflag.tx_data[4] = (((ic[curr_ic].clflag.vregov & 0x01) << 7) | ((ic[curr_ic].clflag.vreguv & 0x01) << 6) | ((ic[curr_ic].clflag.vdigov & 0x01) << 5) | ((ic[curr_ic].clflag.vdiguv & 0x01) << 4) | ((ic[curr_ic].clflag.sed1 & 0x01) << 3) | ((ic[curr_ic].clflag.med1 & 0x01) << 2) | ((ic[curr_ic].clflag.sed2 & 0x01) << 2) | (ic[curr_ic].clflag.med2 & 0x01));
    ic[curr_ic].clrflag.tx_data[5] = (((ic[curr_ic].clflag.vdel & 0x01) << 7) | ((ic[curr_ic].clflag.vde & 0x01) << 6) | ((ic[curr_ic].clflag.spiflt & 0x01) << 4) | ((ic[curr_ic].clflag.reset & 0x01) << 3) | ((ic[curr_ic].clflag.thsd & 0x01) << 2) | ((ic[curr_ic].clflag.tmode & 0x01) << 2) | (ic[curr_ic].clflag.oscflt & 0x01));
  }
}

/**
*******************************************************************************
* Function: adBms2950_IncCalCmdCounter
* @brief Increment a Calculated Command Counter.
*
* @details Increment a calculated command counter.
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  *ic      cell_asic_2950 structure pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950_IncCalCmdCounter(uint8_t tIC, cell_asic_2950 *ic)
{
  for (uint8_t cic = 0; cic < tIC; cic++)
  {
    if (ic[cic].cccrc.cal_cmd_cntr == 63)
    {
      ic[cic].cccrc.cal_cmd_cntr = 0;
    }
    ++ic[cic].cccrc.cal_cmd_cntr;
  }
}

/**
*******************************************************************************
* Function: adBms2950CreateComm
* @brief Create the configation comm write buffer
*
* @details This function create the configation comm write buffer.
*
* Parameters:
*
* @param [in]  tIC                     Total IC
*
* @param [in]  *ic                     cell_asic_2950 ic structure pointer
*
* @return None
*
*******************************************************************************
*/
void adBms2950CreateComm(uint8_t tIC, cell_asic_2950 *ic)
{
  for (uint8_t curr_ic = 0; curr_ic < tIC; curr_ic++)
  {
    // please check these bit shifting operations for correct precedence
    ic[curr_ic].com.tx_data[0] = ((ic[curr_ic].tx_comm.icomm[0] & 0x0F) << 4 | (ic[curr_ic].tx_comm.fcomm[0] & 0x0F));
    ic[curr_ic].com.tx_data[1] = ((ic[curr_ic].tx_comm.data[0]));
    ic[curr_ic].com.tx_data[2] = ((ic[curr_ic].tx_comm.icomm[1] & 0x0F) << 4) | (ic[curr_ic].tx_comm.fcomm[1] & 0x0F);
    ic[curr_ic].com.tx_data[3] = ((ic[curr_ic].tx_comm.data[1]));
    ic[curr_ic].com.tx_data[4] = ((ic[curr_ic].tx_comm.icomm[2] & 0x0F) << 4 | (ic[curr_ic].tx_comm.fcomm[2] & 0x0F));
    ic[curr_ic].com.tx_data[5] = ((ic[curr_ic].tx_comm.data[2]));
  }
}

/** @}*/
/** @}*/