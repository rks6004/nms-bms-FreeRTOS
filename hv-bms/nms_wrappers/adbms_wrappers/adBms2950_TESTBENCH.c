#include "adBms2950_TESTBENCH.h"
#include <stdint.h>

extern CURRENT_STATE current_testing;

void adBms2950_read_acc_ivbat_testbench(uint8_t tIC, cell_asic_2950 *ic, DATA_TYPE type) {
    for (int curr_ic = 0; curr_ic < tIC; curr_ic++) {
         
    }
    
}

uint32_t register_value_from_current(float current, cell_asic_2950 *ic) {
    return ((uint32_t)(((current * 0.00005) / 1e-6) * (4 * (1 + ic->tx_cfga.acci)))) & 0xFFFFFF;
}
