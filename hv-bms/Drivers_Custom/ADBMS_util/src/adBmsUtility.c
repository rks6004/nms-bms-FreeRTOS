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

#include "adBmsUtility.h"
#include "mcuWrapper.h"


/*!<**************************************** BMS Driver APIs definitions ********************************************/

/*!<
* @brief Precomputed CRC15 Table
*/
const uint16_t Crc15Table[256] =
{
  0x0000,0xc599, 0xceab, 0xb32, 0xd8cf, 0x1d56, 0x1664, 0xd3fd, 0xf407, 0x319e, 0x3aac,
  0xff35, 0x2cc8, 0xe951, 0xe263, 0x27fa, 0xad97, 0x680e, 0x633c, 0xa6a5, 0x7558, 0xb0c1,
  0xbbf3, 0x7e6a, 0x5990, 0x9c09, 0x973b, 0x52a2, 0x815f, 0x44c6, 0x4ff4, 0x8a6d, 0x5b2e,
  0x9eb7, 0x9585, 0x501c, 0x83e1, 0x4678, 0x4d4a, 0x88d3, 0xaf29, 0x6ab0, 0x6182, 0xa41b,
  0x77e6, 0xb27f, 0xb94d, 0x7cd4, 0xf6b9, 0x3320, 0x3812, 0xfd8b, 0x2e76, 0xebef, 0xe0dd,
  0x2544, 0x2be, 0xc727, 0xcc15, 0x98c, 0xda71, 0x1fe8, 0x14da, 0xd143, 0xf3c5, 0x365c,
  0x3d6e, 0xf8f7,0x2b0a, 0xee93, 0xe5a1, 0x2038, 0x7c2, 0xc25b, 0xc969, 0xcf0, 0xdf0d,
  0x1a94, 0x11a6, 0xd43f, 0x5e52, 0x9bcb, 0x90f9, 0x5560, 0x869d, 0x4304, 0x4836, 0x8daf,
  0xaa55, 0x6fcc, 0x64fe, 0xa167, 0x729a, 0xb703, 0xbc31, 0x79a8, 0xa8eb, 0x6d72, 0x6640,
  0xa3d9, 0x7024, 0xb5bd, 0xbe8f, 0x7b16, 0x5cec, 0x9975, 0x9247, 0x57de, 0x8423, 0x41ba,
  0x4a88, 0x8f11, 0x57c, 0xc0e5, 0xcbd7, 0xe4e, 0xddb3, 0x182a, 0x1318, 0xd681, 0xf17b,
  0x34e2, 0x3fd0, 0xfa49, 0x29b4, 0xec2d, 0xe71f, 0x2286, 0xa213, 0x678a, 0x6cb8, 0xa921,
  0x7adc, 0xbf45, 0xb477, 0x71ee, 0x5614, 0x938d, 0x98bf, 0x5d26, 0x8edb, 0x4b42, 0x4070,
  0x85e9, 0xf84, 0xca1d, 0xc12f, 0x4b6, 0xd74b, 0x12d2, 0x19e0, 0xdc79, 0xfb83, 0x3e1a, 0x3528,
  0xf0b1, 0x234c, 0xe6d5, 0xede7, 0x287e, 0xf93d, 0x3ca4, 0x3796, 0xf20f, 0x21f2, 0xe46b, 0xef59,
  0x2ac0, 0xd3a, 0xc8a3, 0xc391, 0x608, 0xd5f5, 0x106c, 0x1b5e, 0xdec7, 0x54aa, 0x9133, 0x9a01,
  0x5f98, 0x8c65, 0x49fc, 0x42ce, 0x8757, 0xa0ad, 0x6534, 0x6e06, 0xab9f, 0x7862, 0xbdfb, 0xb6c9,
  0x7350, 0x51d6, 0x944f, 0x9f7d, 0x5ae4, 0x8919, 0x4c80, 0x47b2, 0x822b, 0xa5d1, 0x6048, 0x6b7a,
  0xaee3, 0x7d1e, 0xb887, 0xb3b5, 0x762c, 0xfc41, 0x39d8, 0x32ea, 0xf773, 0x248e, 0xe117, 0xea25,
  0x2fbc, 0x846, 0xcddf, 0xc6ed, 0x374, 0xd089, 0x1510, 0x1e22, 0xdbbb, 0xaf8, 0xcf61, 0xc453,
  0x1ca, 0xd237, 0x17ae, 0x1c9c, 0xd905, 0xfeff, 0x3b66, 0x3054, 0xf5cd, 0x2630, 0xe3a9, 0xe89b,
  0x2d02, 0xa76f, 0x62f6, 0x69c4, 0xac5d, 0x7fa0, 0xba39, 0xb10b, 0x7492, 0x5368, 0x96f1, 0x9dc3,
  0x585a, 0x8ba7, 0x4e3e, 0x450c, 0x8095
};

/* Pre-computed CRC10 Table */
static const uint16_t crc10Table[256] =
    {
        0x000, 0x08f, 0x11e, 0x191, 0x23c, 0x2b3, 0x322, 0x3ad, 0x0f7, 0x078, 0x1e9, 0x166, 0x2cb, 0x244, 0x3d5, 0x35a,
        0x1ee, 0x161, 0x0f0, 0x07f, 0x3d2, 0x35d, 0x2cc, 0x243, 0x119, 0x196, 0x007, 0x088, 0x325, 0x3aa, 0x23b, 0x2b4,
        0x3dc, 0x353, 0x2c2, 0x24d, 0x1e0, 0x16f, 0x0fe, 0x071, 0x32b, 0x3a4, 0x235, 0x2ba, 0x117, 0x198, 0x009, 0x086,
        0x232, 0x2bd, 0x32c, 0x3a3, 0x00e, 0x081, 0x110, 0x19f, 0x2c5, 0x24a, 0x3db, 0x354, 0x0f9, 0x076, 0x1e7, 0x168,
        0x337, 0x3b8, 0x229, 0x2a6, 0x10b, 0x184, 0x015, 0x09a, 0x3c0, 0x34f, 0x2de, 0x251, 0x1fc, 0x173, 0x0e2, 0x06d,
        0x2d9, 0x256, 0x3c7, 0x348, 0x0e5, 0x06a, 0x1fb, 0x174, 0x22e, 0x2a1, 0x330, 0x3bf, 0x012, 0x09d, 0x10c, 0x183,
        0x0eb, 0x064, 0x1f5, 0x17a, 0x2d7, 0x258, 0x3c9, 0x346, 0x01c, 0x093, 0x102, 0x18d, 0x220, 0x2af, 0x33e, 0x3b1,
        0x105, 0x18a, 0x01b, 0x094, 0x339, 0x3b6, 0x227, 0x2a8, 0x1f2, 0x17d, 0x0ec, 0x063, 0x3ce, 0x341, 0x2d0, 0x25f,
        0x2e1, 0x26e, 0x3ff, 0x370, 0x0dd, 0x052, 0x1c3, 0x14c, 0x216, 0x299, 0x308, 0x387, 0x02a, 0x0a5, 0x134, 0x1bb,
        0x30f, 0x380, 0x211, 0x29e, 0x133, 0x1bc, 0x02d, 0x0a2, 0x3f8, 0x377, 0x2e6, 0x269, 0x1c4, 0x14b, 0x0da, 0x055,
        0x13d, 0x1b2, 0x023, 0x0ac, 0x301, 0x38e, 0x21f, 0x290, 0x1ca, 0x145, 0x0d4, 0x05b, 0x3f6, 0x379, 0x2e8, 0x267,
        0x0d3, 0x05c, 0x1cd, 0x142, 0x2ef, 0x260, 0x3f1, 0x37e, 0x024, 0x0ab, 0x13a, 0x1b5, 0x218, 0x297, 0x306, 0x389,
        0x1d6, 0x159, 0x0c8, 0x047, 0x3ea, 0x365, 0x2f4, 0x27b, 0x121, 0x1ae, 0x03f, 0x0b0, 0x31d, 0x392, 0x203, 0x28c,
        0x038, 0x0b7, 0x126, 0x1a9, 0x204, 0x28b, 0x31a, 0x395, 0x0cf, 0x040, 0x1d1, 0x15e, 0x2f3, 0x27c, 0x3ed, 0x362,
        0x20a, 0x285, 0x314, 0x39b, 0x036, 0x0b9, 0x128, 0x1a7, 0x2fd, 0x272, 0x3e3, 0x36c, 0x0c1, 0x04e, 0x1df, 0x150,
        0x3e4, 0x36b, 0x2fa, 0x275, 0x1d8, 0x157, 0x0c6, 0x049, 0x313, 0x39c, 0x20d, 0x282, 0x12f, 0x1a0, 0x031, 0x0be};
/* Const 16 section end */


/**
*******************************************************************************
* Function: Pec15_Calc
* @brief CRC15 Pec Calculation Function
*
* @details This function calculates and return the CRC15 value
*
* Parameters:
* @param [in]	Len	Data length
*
* @param [in] *data    Data pointer
*
* @return CRC15_Value
*
*******************************************************************************
*/
uint16_t Pec15_Calc
(
	uint8_t len, /*!< Number of bytes that will be used to calculate a PEC */
	uint8_t *data /*!< Array of data that will be used to calculate  a PEC */
)
{
  uint16_t remainder,addr;
  remainder = 16u; /*!< initialize the PEC */
  for (uint8_t i = 0; i<len; i++) /*!< loops for each byte in data array */
  {
    addr = (((remainder>>7u)^data[i])&0xff);/*!< calculate PEC table address */
    remainder = ((remainder<<8u)^Crc15Table[addr]);
  }
  return(remainder*2u);/*!< The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2 */
}

/**
*******************************************************************************
* Function: Pec10_Calc
* @brief CRC10 Pec Calculation Function
*
* @details This function calculates and return the CRC15 value
*
* Parameters:
* @param [in]	Len	Data length
*
* @param [in] *data    Data pointer
*
* @return CRC10_Value
*
*******************************************************************************
*/
uint16_t pec10_calc(bool rx_cmd, int len, uint8_t *data)
{
  uint16_t remainder = 16u; /*!< PEC_SEED;   0000010000 */
  uint16_t polynom = 0x8F; /*!< x10 + x7 + x3 + x2 + x + 1 <- the CRC15 polynomial         100 1000 1111   48F */

  /*!< Perform modulo-2 division, a byte at a time. */
  for (uint8_t pbyte = 0; pbyte < len; ++pbyte)
  {
    /*!< Bring the next byte into the remainder. */
    remainder ^= (uint16_t)((uint16_t)data[pbyte] << 2u);
    /*!< Perform modulo-2 division, a bit at a time.*/
    for (uint8_t bit_ = 8u; bit_ > 0u; --bit_)
    {
      /*!< Try to divide the current data bit. */
      if ((remainder & 0x200) > 0u)/*!<equivalent to remainder & 2^14 simply check for MSB */
      {
        remainder = (uint16_t)((remainder << 1u));
        remainder = (uint16_t)(remainder ^ polynom);
      }
      else
      {
        remainder = (uint16_t)(remainder << 1u);
      }
    }
  }
  if (rx_cmd == true)
  {
    remainder ^= (uint16_t)(((uint16_t)data[len] & (uint8_t)0xFC) << 2u);
  }
  /*!< Perform modulo-2 division, a bit at a time */
  for (uint8_t bit_ = 6u; bit_ > 0u; --bit_)
  {
    /*!< Try to divide the current data bit */
    if ((remainder & 0x200) > 0u)/*!<equivalent to remainder & 2^14 simply check for MSB*/
    {
      remainder = (uint16_t)((remainder << 1u));
      remainder = (uint16_t)(remainder ^ polynom);
    }
    else
    {
      remainder = (uint16_t)((remainder << 1u));
    }
  }
  return ((uint16_t)(remainder & 0x3FFu));
}

/**
*******************************************************************************
* Function: pec10_calc_modular
* @brief calculate data pec
*
* @details This function is used to calculate Data PEC.
*
* @return Pec
*
*******************************************************************************
*/
uint16_t pec10_calc_modular(
    uint8_t * data, // Array of data that will be used to calculate a PEC
    uint8_t PEC_Format // use any of PEC10_...
)
{
    uint16_t remainder = 16u; // PEC_SEED;
    uint16_t len;
    switch (PEC_Format)
    {
    case PEC10_WRITE:
        data[6] = 0; // for write commands the command counter is all zero
        // step through
    case PEC10_READ:
        len = 6;
        break;
    case PEC10_READ256:
        len = 256;
        break;
    case PEC10_READ512:
        len = 512;
        break;
    case PEC10_WRITE2:
        data[2] = 0;
        // step through
    case PEC10_READ2:
        len = 2;
        break;
    default:
        return 0xFFFF;
        break;
    }
    //Perform modulo-2 division, a byte at a time.
    for (uint8_t pbyte = 0; pbyte < len; ++pbyte)
    {
        // Bring the next byte into the remainder.
        remainder ^= (uint16_t)(data[pbyte]) << 2u;
        remainder = pec10_calc_int(remainder, 8u);
    }
    // the last byte is different as it holds the 6-bit command counter
    // Note: for write commands, those bits are zero!
    remainder ^= (uint16_t)((data[len] & 0xFC) << 2u);
    remainder = pec10_calc_int(remainder, 6u);
    return ((uint16_t)(remainder & 0x3FF));
}

/**
*******************************************************************************
* Function: pec10_calc_int
* @brief calculate data pec
*
* @details This function is used to calculate Data PEC.
*
* @return Pec
*
*******************************************************************************
*/
uint16_t pec10_calc_int(uint16_t remainder, uint8_t bit)
{
  uint8_t PEC10_POLY = 0x8F;
    //Perform modulo-2 division, a bit at a time.
    //Perform modulo-2 division, a bit at a time.
    for (; bit > 0; --bit)
    {
        //Try to divide the current data bit.
        if ((remainder & 0x200u) > 0u)//equivalent to remainder & 2^14 simply check for MSB
        {
            remainder = (uint16_t)((remainder << 1u));
            remainder = (uint16_t)(remainder ^ PEC10_POLY);
        }
        else
        {
            remainder = (uint16_t)(remainder << 1u);
        }
    }
    return ((uint16_t)(remainder & 0x3FFu));
}

/**
*******************************************************************************
* Function: spiReadData
* @brief Spi Read Data, Lower Level function for adBms6830ReadData
*
* @details This function send command to DUT and read command corrospond data byte.
*
* Parameters:
* @param [in]	tIC     Total IC
*
* @param [in]  tx_cmd   Tx command bytes
*
* @param [in]  *rx_data Rx data pointer
*
* @param [in]  *pec_error Pec error pointer
*
* @param [in]  *cmd_cntr command counter pointer
*
* @return None
*
*******************************************************************************
*/
void spiReadData
(
uint8_t tIC,
uint8_t tx_cmd[2],
uint8_t *rx_data,
uint8_t *pec_error,
uint8_t *cmd_cntr,
uint8_t regData_size
)
{
  uint8_t *data, *copyArray, src_address = 0;
  uint16_t cmd_pec, received_pec, calculated_pec;
  uint8_t BYTES_IN_REG = regData_size;
  uint8_t RX_BUFFER = (regData_size * tIC);

  data = (uint8_t *)calloc(RX_BUFFER, sizeof(uint8_t));
  copyArray = (uint8_t *)calloc(BYTES_IN_REG, sizeof(uint8_t));
  if((data == NULL) || (copyArray == NULL))
  {
    printf("Failed to allocate spi read data memory");
    exit(0);
  }
  else
  {
    uint8_t cmd[4];
    cmd[0] = tx_cmd[0];
    cmd[1] = tx_cmd[1];
    cmd_pec = Pec15_Calc(2, cmd);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);
    
#ifdef ISOSPI_WAKEUP  
    adBmsWakeupIc(tIC);
#endif
    #ifndef TESTBENCH
      adBmsCsLow();
      spiWriteReadBytes(&cmd[0], &data[0], RX_BUFFER);                 /*!< Read the configuration data of all ICs on the daisy chain into readdata array */
      adBmsCsHigh();      
    #else
      //emulate spiWriteReadBytes functionality here???
    #endif
    for (uint8_t current_ic = 0; current_ic < tIC; current_ic++)     /*!< executes for each ic in the daisy chain and packs the data */
    {																																      /*!< Into the r_comm array as well as check the received data for any bit errors */
      // for (uint8_t current_byte = 0; current_byte < (BYTES_IN_REG-2); current_byte++)
      for (uint8_t current_byte = 0; current_byte < (BYTES_IN_REG); current_byte++)
      {
        rx_data[(current_ic*BYTES_IN_REG)+current_byte] = data[current_byte + (current_ic*BYTES_IN_REG)];
      }
      /*!< Get command counter value */
      cmd_cntr[current_ic] = (data[(current_ic * BYTES_IN_REG) + (BYTES_IN_REG - 2)] >> 2);
      /*!< Get received pec value from ic*/
      received_pec = (uint16_t)(((data[(current_ic * BYTES_IN_REG) + (BYTES_IN_REG - 2)] & 0x03) << 8) | data[(current_ic * BYTES_IN_REG) + (BYTES_IN_REG - 1)]);
      /*!< Copy each ic correspond data + pec value for calculate data pec */
      memcpy(&copyArray[0], &data[src_address], BYTES_IN_REG);
      src_address = ((current_ic+1) * (regData_size));
      /*!< Calculate data pec */
      calculated_pec = (uint16_t)pec10_calc(true, (BYTES_IN_REG-2), &copyArray[0]);
      // calculated_pec = (uint16_t)pec10_calc(false, (BYTES_IN_REG), &copyArray[0]);
      // printf("spiReadData() received_pec=0x%x, calculated_pec=0x%x\n", received_pec, calculated_pec);
      /*!< Match received pec with calculated pec */
      if (received_pec == calculated_pec){ pec_error[current_ic] = 0; }/*!< If no error is there value set to 0 */
      else{ pec_error[current_ic] = 1; }                               /*!< If error is there value set to 1 */
    }
  }
  free(data);
  free(copyArray);
}

/**
*******************************************************************************
* Function: spiWriteData
* @brief write data over SPI, Lower Level function for adBms6830WriteData
*
* @details This function write the data into bms ic
*
* Parameters:
* @param [in]	tIC      Total IC
*
* @param [in]  tx_cmd   Tx command bytes
*
* @param [in]  *data   Data pointer
*
* @return None
*
*******************************************************************************
*/
void spiWriteData
(
uint8_t tIC,
uint8_t tx_cmd[2],
uint8_t *data
)
{
  uint8_t BYTES_IN_REG = TX_DATA;
  uint8_t CMD_LEN = 4 + (RX_DATA * tIC);
  uint16_t data_pec, cmd_pec;
  uint8_t *cmd, copyArray[TX_DATA], src_address = 0;
  uint8_t cmd_index;
  cmd = (uint8_t *)calloc(CMD_LEN, sizeof(uint8_t));
  if(cmd == NULL)
  {
#ifdef MBED
    pc.printf(" Failed to allocate cmd array memory \n");
#else
    printf(" Failed to allocate cmd array memory \n");
#endif  
    exit(0);
  }
  else
  {
    cmd[0] = tx_cmd[0];
    cmd[1] = tx_cmd[1];
    cmd_pec = Pec15_Calc(2, cmd);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);
    cmd_index = 4;
    /*!< executes for each LTC68xx, this loops starts with the last IC on the stack */
    for (uint8_t current_ic = tIC; current_ic > 0; current_ic--)
    {
      src_address = ((current_ic-1) * TX_DATA);
      /*!< The first configuration written is received by the last IC in the daisy chain */
      for (uint8_t current_byte = 0; current_byte < BYTES_IN_REG; current_byte++)
      {
        cmd[cmd_index] = data[((current_ic-1)*6)+current_byte];
        cmd_index = cmd_index + 1;
      }
      /*!< Copy each ic correspond data + pec value for calculate data pec */
      memcpy(&copyArray[0], &data[src_address], TX_DATA); /*!< dst, src, size */
      /*!< calculating the PEC for each Ics configuration register data */
      data_pec = (uint16_t)pec10_calc_modular(&copyArray[0], PEC10_WRITE);
      // data_pec = (uint16_t)pec10_calc(true,BYTES_IN_REG, &copyArray[0]);  
      cmd[cmd_index] = (uint8_t)(data_pec >> 8);
      cmd_index = cmd_index + 1;
      cmd[cmd_index] = (uint8_t)data_pec;
      cmd_index = cmd_index + 1;
    }
    #ifndef TESTBENCH
    adBmsCsLow();
    spiWriteBytes( CMD_LEN, &cmd[0]);
    adBmsCsHigh();
    #endif
  }
  free(cmd);
}