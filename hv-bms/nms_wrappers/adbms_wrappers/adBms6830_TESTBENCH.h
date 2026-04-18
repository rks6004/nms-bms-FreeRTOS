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

//Defines
#ifndef VOLTAGE_TEST
    #define VOLTAGE_TEST VOLT_NORMAL
#endif

#ifndef TEMP_TEST
    #define TEMP_TEST TEMP_NORMAL
#endif

#ifndef CURRENT_TEST
    #define CURRENT_TEST CURR_NORMAL
#endif

#ifndef PEC_TEST
    #define PEC_TEST PEC_NORMAL
#endif

enum VOLTAGE_STATE {
    VOLT_NORMAL = 0,
    VOLT_OV_BRIEF,
    VOLT_UV_BRIEF,
    VOLT_OV_CONSISTENT,
    VOLT_UV_CONSISTENT
};

enum TEMP_STATE {
    TEMP_NORMAL = 0,
    TEMP_OT_BRIEF,
    TEMP_UT_BRIEF,
    TEMP_OT_CONSISTENT,
    TEMP_UT_CONSISTENT
};

//may need to go in 2950 code instead
enum CURRENT_STATE {
    CURR_NORMAL = 0,
    CURR_OC_BRIEF,
    CURR_UC_BRIEF,
    CURR_OC_CONSISTENT,
    CURR_UC_CONSISTENT
};

enum PEC_STATE {
    PEC_NORMAL = 0,
    PEC_SLIGHT_INTEFERENCE,
    PEC_HEAVY_INTERFERENCE
};





void adBms6830_init_config_testbench(uint8_t tIC, cell_asic_6830 *ic);

void spiReadData_testbench(uint8_t tIC, uint8_t tx_cmd[2], 
                            uint8_t *rx_data, uint8_t *pec_error, 
                            uint8_t *cmd_cntr, uint8_t regData_size);

