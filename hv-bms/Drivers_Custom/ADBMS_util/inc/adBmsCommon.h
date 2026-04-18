#ifndef __ADBMSCOMM_H
#define __ADBMSCOMM_H

#include "common.h"

#define TX_DATA_SIZE 6               /* Bms tx data byte                   */
#define RX_DATA_SIZE 8               /* Bms rx data byte                   */
#define RSID_SIZE 6
#define COMM_SIZE 3

/*!
*  \enum ERR
* ERR: Inject error is spi read out.
*/
/* Inject error is spi read out */
typedef enum  { WITHOUT_ERR = 0x0, WITH_ERR = 0x1 } ERR;

/*!
*  \enum RD
* RD: Read Device.
*/
typedef enum { RD_OFF = 0X0, RD_ON = 0X1} RD;
/*!
*  \enum CL FLAG
* CL FLAG: Fault clear bit set or clear enum
*/
typedef enum  { CL_FLAG_CLR = 0x0, CL_FLAG_SET = 0x1 } CL_FLAG;

/*!
*  \enum COMMBK
* COMMBK: Communication Break.
*/
typedef enum  { COMMBK_OFF = 0x0, COMMBK_ON = 0x1 } COMMBK;

/*!
*  \enum SNAPST
* SNAPST: Snapshot.
*/
typedef enum  { SNAP_OFF = 0x0, SNAP_ON = 0x1 } SNAPST;

/*!
*  \enum DIAGNOSTIC_TYPE
* DIAGNOSTIC_TYPE: type of diagnostic check
*/
typedef enum { OSC_MISMATCH = 0x0, SUPPLY_ERROR, THSD, FUSE_ED, FUSE_MED, TMODCHK} DIAGNOSTIC_TYPE;

/*!
*  \enum LOOP_MEASURMENT
* LOOP_MEASURMENT: enabled / disabled
*/
typedef enum { DISABLED = 0X0, ENABLED = 0X1} LOOP_MEASURMENT;

/*!
*  \enum RESULT
* RESULT: pass or fail
*/
typedef enum { FAIL = 0x0, PASS } RESULT ;

/*!
*  \enum GPO
* GPIO: GPO Pin Control.
*/
/* GPO Pin Control */
typedef enum  { GPO_CLR = 0x0, GPO_SET = 0x1 } CFGA_GPO;

/*!< Transmit byte and recived byte data structure */
typedef struct
{
  uint8_t tx_data[TX_DATA_SIZE];
  uint8_t rx_data[RX_DATA_SIZE];
} ic_register_;

/*!< ADBMS2950 SID Register Structure */
typedef struct
{
  uint8_t sid[RSID_SIZE];
} sid_;

/* ADBMS2950 COMM register Data structure*/
typedef struct
{
  uint8_t fcomm[COMM_SIZE];
  uint8_t icomm[COMM_SIZE];
  uint8_t data[COMM_SIZE];
} com_;

#endif