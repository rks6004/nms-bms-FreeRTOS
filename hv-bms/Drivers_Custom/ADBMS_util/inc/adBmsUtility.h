#ifndef __adBmsUtility_H
#define __adBmsUtility_H

#include "common.h"

/* PEC_Format*/
/*!
    \enum                PEC_Format                
    \brief               Enum PEC Format.
*/
typedef enum
{
  PEC10_WRITE,
  PEC10_READ,
  PEC10_READ256,
  PEC10_READ512,
  PEC10_WRITE2,
  PEC10_READ2,
}PEC_Format;

#define TX_DATA 6               /* Bms tx data byte                   */
#define RX_DATA 8               /* Bms rx data byte                   */

uint16_t Pec15_Calc(uint8_t len, uint8_t *data );
uint16_t pec10_calc(bool rx_cmd, int len, uint8_t *data);
uint16_t pec10_calc_modular(uint8_t * data, uint8_t PEC_Format);
uint16_t pec10_calc_int(uint16_t remainder, uint8_t bit);
void spiReadData(uint8_t tIC, uint8_t tx_cmd[2], uint8_t *rx_data, uint8_t *pec_error, uint8_t *cmd_cntr, uint8_t regData_size);
void spiWriteData(uint8_t tIC, uint8_t tx_cmd[2], uint8_t *data);

#endif