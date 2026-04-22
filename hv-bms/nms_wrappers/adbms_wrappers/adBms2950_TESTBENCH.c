#include "adBms2950_TESTBENCH.h"
#include "semphr.h"
#include "FreeRTOS.h"

#include <stdint.h>
#include <math.h>

static inline uint32_t pack_current_adjuster(uint32_t millis) {return (uint32_t)((5 * (float)(millis)) / 1000);}
static inline uint32_t pack_voltage_adjuster(uint32_t millis) {return (uint32_t)((5 * (float)(millis)) / 1000);}
static inline bool probability_check(int percent) {return rand() < ((percent * RAND_MAX) / 100);}


extern SemaphoreHandle_t ioMutexHandle;

extern emulated_adbms_2950 characteristic_2950[ADBMS_2950_IC_NUM];

uint32_t register_2950_value_from_current(float current, cell_asic_2950 *ic) {
    return ((uint32_t)(((current * 0.00005) / 1e-6) * (4 * (1 + ic[0].tx_cfga.acci)))) & 0xFFFFFF;
}

uint32_t register_2950_value_from_voltage(float voltage, cell_asic_2950 *ic) {
    return ((uint32_t)(((voltage / 396.6043956) / 100e-6) * (4 * (1 + ic[0].tx_cfga.acci)))) & 0xFFFFFF;
}

void adBms2950_read_acc_ivbat_testbench(uint8_t tIC, cell_asic_2950 *ic, DATA_TYPE type) {
    uint32_t current_timer = pdTICKS_TO_MS(xTaskGetTickCount());
    //through init'ing in main.c, all cell_asic_2950 and cell_asic_6830 structs are linked to their characteristic counterparts by reference, meaning 
    for (int curr_ic = 0; curr_ic < tIC; curr_ic++) {
        float base_current = fmax((MAX_CURRENT - 50 - pack_current_adjuster(current_timer)), 5); //beginning just shy of max deployment of charge
        if (type == AccCr) 
        {
            switch (characteristic_2950[curr_ic].current_behavior)
            {
            case CURR_NORMAL:
                //normal discharge represented as a slow, linearly decreasing current: a slow ramp down
                ic[curr_ic].iacc.i1acc = register_2950_value_from_current(base_current, ic);
                break;
            case CURR_OC:
                //represent OC as 30% probability during operation: could be from track layout (power on straight, loss of traction) or connection unreliability
                ic[curr_ic].iacc.i1acc = probability_check(30) ? register_2950_value_from_current((float)(MAX_CURRENT), ic) : register_2950_value_from_current(base_current, ic);
            default:
                xSemaphoreTake(ioMutexHandle, portMAX_DELAY);
                printf("Invalid current emulation state: %d\n", characteristic_2950[curr_ic].current_behavior);
                xSemaphoreGive(ioMutexHandle);
                exit(EXIT_FAILURE);
                break;
            }    
            switch (characteristic_2950[curr_ic].signal_behavior)
            {
            case PEC_NORMAL:
                //low probability of failing interference
                ic[curr_ic].cccrc.avgcr_pec = (uint8_t)(probability_check(3));
                break;
            case PEC_SLIGHT_INTEFERENCE:
                //slight probability of failing interference
                ic[curr_ic].cccrc.avgcr_pec = (uint8_t)(probability_check(15));
                break;
            case PEC_HEAVY_INTERFERENCE:
                //extrodinarily statistically significant probability of failing interference
                ic[curr_ic].cccrc.avgcr_pec = (uint8_t)(probability_check(40));
                break;
            default:
                xSemaphoreTake(ioMutexHandle, portMAX_DELAY);
                printf("Invalid PEC strength setting in emulation: %d\n", characteristic_2950[curr_ic].signal_behavior);
                xSemaphoreGive(ioMutexHandle);
                exit(EXIT_FAILURE);
                break;
            }
        }
        else if (type == AccVbat) {
            float base_voltage = fmax(((MAX_CELL_VOLTAGE * CELL_COUNT)) - pack_voltage_adjuster(current_timer), (MIN_CELL_VOLTAGE * CELL_COUNT));
            switch (characteristic_2950[curr_ic].volt_behavior)
            {
            case VOLT_NORMAL:
                //normal discharge represented as a consistently decreasing
                ic[curr_ic].vbacc.vb1acc = (uint32_t)(register_2950_value_from_voltage(base_voltage, ic));
                break;
            case VOLT_OV:
                //represent OV as 30% probability - likely would only happen during charging
                ic[curr_ic].vbacc.vb1acc = probability_check(30) ? (uint32_t)(register_2950_value_from_voltage(, ic)) : register_2950_value_from_voltage(MAX_CURRENT), ic);
            default:
                xSemaphoreTake(ioMutexHandle, portMAX_DELAY);
                printf("Invalid current emulation state: %d\n", characteristic_2950[curr_ic].current_behavior);
                xSemaphoreGive(ioMutexHandle);
                exit(EXIT_FAILURE);
                break;
            }    
            switch (characteristic_2950[curr_ic].signal_behavior)
            {
            case PEC_NORMAL:
                //low probability of failing interference
                ic[curr_ic].cccrc.avgvbat_pec = (uint8_t)(probability_check(3));
                break;
            case PEC_SLIGHT_INTEFERENCE:
                //slight probability of failing interference
                ic[curr_ic].cccrc.avgvbat_pec = (uint8_t)(probability_check(15));
                break;
            case PEC_HEAVY_INTERFERENCE:
                //extrodinarily statistically significant probability of failing interference
                ic[curr_ic].cccrc.avgvbat_pec = (uint8_t)(probability_check(40));
                break;
            default:
                xSemaphoreTake(ioMutexHandle, portMAX_DELAY);
                printf("Invalid PEC strength setting in emulation: %d\n", characteristic_2950[curr_ic].signal_behavior);
                xSemaphoreGive(ioMutexHandle);
                exit(EXIT_FAILURE);
                break;
            }
        }
        else {
            xSemaphoreTake(ioMutexHandle, portMAX_DELAY);
            printf("Unhandled 2950 polling type: %d\n", characteristic_2950[curr_ic].signal_behavior);
            xSemaphoreGive(ioMutexHandle);
            exit(EXIT_FAILURE);
        }
    }

}

