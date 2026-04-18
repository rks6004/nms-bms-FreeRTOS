//standard includes
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include "bms_test.h"

//Analog-Devices-related includes
#include "adBms6830Driver.h"
#include "adBms6830Data.h"

#include "adBms6830_TESTBENCH.h"

void adBms6830_init_config(uint8_t tIC, cell_asic_6830 *ic) {
    switch (VOLTAGE_TEST)
    {
    case
        /* code */
        break;
    
    default:
        break;
    }
    
    return;    

}

void spiReadData_testbench
(
uint8_t tIC,
uint8_t tx_cmd[2],
uint8_t *rx_data,
uint8_t *pec_error,
uint8_t *cmd_cntr,
uint8_t regData_size
)
{
    
    return;
}



