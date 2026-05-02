/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BMS_TEST_H
#define BMS_TEST_H

#include "bms_util.h"
#include "adBms2950Data.h"
#include "adBms6830Data.h"

#define IO_TIMEOUT pdMS_TO_TICKS(2000) //2 seconds
#define EVENT_TIMEOUT (TEST_STREAM_MAX_LENGTH_MS) + 500 //gives a 0.5s buffer from when largest trace ends 

//line size calculated from longest string being 999995 milliseconds (resolution up to 50 ms) for timestamp 
//and -999999 (or any arrangement of decimal point) for value -  17. Round up to nearest power of 2 -> 32
#define MAX_SIZE_PER_DATASTREAM_LINE 32

//test stream defines
#define TEST_STREAM_TIMING_RESOLUTION 50 //in ms
#define TEST_STREAM_MAX_LENGTH_MS 3000 //also in ms, = 2 min

//event defines for testbench
#define VOLTAGE_ERROR_BITS 0x01U
#define CURRENT_ERROR_BITS 0x02U
#define TEMP_ERROR_BITS 0x04U
#define PEC_ERROR_BITS 0x08U

#define TESTBENCH_DATASTREAMS_READY_BITS 0x01

#define NUM_TEST_DIMS 4 //V, C, T, PEC


typedef enum VOLTAGE_STATE {
    VOLT_NORMAL = 1,
    VOLT_OV,
    VOLT_UV
} VOLTAGE_STATE;

typedef enum TEMP_STATE {
    TEMP_NORMAL = 1,
    TEMP_OT,
} TEMP_STATE;

typedef enum CURRENT_STATE {
    CURR_NORMAL = 1,
    CURR_OC,
} CURRENT_STATE;

typedef enum PEC_STATE {
    PEC_NORMAL = 1,
    PEC_SLIGHT_INTERFERENCE,
    PEC_HEAVY_INTERFERENCE
} PEC_STATE;

typedef struct testbench_datastream {
    uint32_t* timestamps;
    int32_t* values;
} testbench_datastream;

typedef struct bms_test_setup 
{
    VOLTAGE_STATE voltage_testing;
    TEMP_STATE temp_testing;
    CURRENT_STATE current_testing;
    PEC_STATE pec_testing;
    bool charging_testing;
} bms_test_setup;

typedef struct emulated_adbms_6830
{
    VOLTAGE_STATE volt_behavior;
    TEMP_STATE temp_behavior;
    PEC_STATE signal_behavior;
    cell_asic_6830* ic_data;
    testbench_datastream* voltage_data;
    testbench_datastream* temp_data;
} emulated_adbms_6830;

typedef struct emulated_adbms_2950
{
    VOLTAGE_STATE volt_behavior;
    CURRENT_STATE current_behavior;
    PEC_STATE signal_behavior;
    cell_asic_2950* ic_data;
    testbench_datastream* current_data;
} emulated_adbms_2950;


void testbench_init(void);

void testbench_task(void* argument);

int get_vehicle_testbench_data(char* filepath, testbench_datastream* datastream);

#endif //header guard