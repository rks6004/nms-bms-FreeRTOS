#include "adBms2950_TESTBENCH.h"
#include "semphr.h"
#include "FreeRTOS.h"
#include "event_groups.h"

#include <stdint.h>
#include <math.h>

static inline uint32_t pack_current_adjuster(uint32_t millis) {return (uint32_t)((5 * (float)(millis)) / 1000);}
static inline uint32_t pack_voltage_adjuster(uint32_t millis) {return (uint32_t)((5 * (float)(millis)) / 1000);}
static inline bool probability_check(int percent) {return rand() < ((percent * RAND_MAX) / 100);}

extern SemaphoreHandle_t ioMutexHandle;
extern EventGroupHandle_t charging_evt_id;

extern uint32_t testbench_current;

extern emulated_adbms_2950 characteristic_2950[ADBMS_2950_IC_NUM];

uint32_t register_2950_value_from_current(float current, cell_asic_2950 *ic) {
    return ((uint32_t)(((current * 0.00005) / 1e-6) * (4 * (1 + ic[0].tx_cfga.acci)))) & 0xFFFFFF;
}

uint32_t register_2950_value_from_voltage(float voltage, cell_asic_2950 *ic) {
    return ((uint32_t)(((voltage / 396.6043956) / 100e-6) * (4 * (1 + ic[0].tx_cfga.acci)))) & 0xFFFFFF;
}

void adBms2950_read_acc_ivbat_testbench(uint8_t tIC, cell_asic_2950 *ic, DATA_TYPE type) {
    uint32_t data_timer = pdTICKS_TO_MS(xTaskGetTickCount()) % TEST_STREAM_MAX_LENGTH_MS; //datastream will loop if not terminated when EOF of datastream reached
    uint32_t data_index = (data_timer / TEST_STREAM_TIMING_RESOLUTION);
    //through init'ing in main.c, all cell_asic_2950 and cell_asic_6830 structs are linked to their characteristic counterparts by reference 
    for (int curr_ic = 0; curr_ic < tIC; curr_ic++) {
        if (type == AccCr) 
        {
            int32_t curr_value = characteristic_2950[curr_ic].current_data->values[data_index];
            ic[curr_ic].iacc.i1acc = register_2950_value_from_current(((float)(curr_value)/1000.0f), ic);

            switch (characteristic_2950[curr_ic].signal_behavior)
            {
            case PEC_NORMAL:
                //low probability of failing interference
                ic[curr_ic].cccrc.avgcr_pec = (uint8_t)(probability_check(PEC_NORMAL_PROB));
                break;
            case PEC_SLIGHT_INTERFERENCE:
                //slight probability of failing interference
                ic[curr_ic].cccrc.avgcr_pec = (uint8_t)(probability_check(PEC_SLIGHT_PROB));
                break;
            case PEC_HEAVY_INTERFERENCE:
                //statistically significant probability of failing interference
                ic[curr_ic].cccrc.avgcr_pec = (uint8_t)(probability_check(PEC_HEAVY_PROB));
                break;
            default:
                xSemaphoreTake(ioMutexHandle, IO_TIMEOUT);
                printf("Invalid PEC strength setting in 2950 emulation, exiting application: %d\n", characteristic_2950[curr_ic].signal_behavior);
                xSemaphoreGive(ioMutexHandle);
                exit(EXIT_FAILURE);
                break;
            }
        }
        else {
            xSemaphoreTake(ioMutexHandle, IO_TIMEOUT);
            printf("Unhandled 2950 polling type, exiting application: %d\n", characteristic_2950[curr_ic].signal_behavior);
            xSemaphoreGive(ioMutexHandle);
            exit(EXIT_FAILURE);
        }
    }

}

