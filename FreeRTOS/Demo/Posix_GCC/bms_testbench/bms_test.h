#include "bms_util.h"

#ifdef TESTBENCH
    #ifndef BMS_TEST_TYPE
        #define BMS_TEST_TYPE NORMAL_DISCHARGE
    #endif    
#endif

typedef enum {
    NORMAL = 0,
    SEGMENT_OV,
    SEGMENT_UV,
    CURRENT_SPIKE,
    OVERHEAT,
    SIGNAL_INTERFERENCE,
    ALL_TESTS
} bms_test_type;

typedef enum  {
    VOLT_NORMAL = 0,
    VOLT_OV,
    VOLT_UV
} VOLTAGE_STATE;

typedef enum {
    TEMP_NORMAL = 0,
    TEMP_OT,
    TEMP_UT
} TEMP_STATE;

typedef enum {
    CURR_NORMAL = 0,
    CURR_OC,
    CURR_UC
} CURRENT_STATE;

typedef enum {
    PEC_NORMAL = 0,
    PEC_SLIGHT_INTEFERENCE,
    PEC_HEAVY_INTERFERENCE
} PEC_STATE;

VOLTAGE_STATE voltage_testing;
TEMP_STATE temp_testing;
CURRENT_STATE current_testing;
PEC_STATE pec_testing; 

void testbench_init(void);