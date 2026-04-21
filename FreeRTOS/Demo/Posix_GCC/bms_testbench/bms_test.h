#include "bms_util.h"
#include "adBms2950Data.h"
#include "adBms6830Data.h"

typedef enum bms_test_type {
    NORMAL_DISCHARGE = 0,
    NORMAL_CHARGE,
    SEGMENT_OV,
    SEGMENT_UV,
    CURRENT_SPIKE,
    OVERHEAT,
    SIGNAL_INTERFERENCE,
    ALL_TESTS
} bms_test_type;

typedef enum VOLTAGE_STATE {
    VOLT_NORMAL = 0,
    VOLT_OV,
    VOLT_UV
} VOLTAGE_STATE;

typedef enum CURRENT_STATE {
    TEMP_NORMAL = 0,
    TEMP_OT,
    TEMP_UT
} TEMP_STATE;

typedef enum {
    CURR_NORMAL = 0,
    CURR_OC,
    CURR_UC
} CURRENT_STATE;

typedef enum PEC_STATE {
    PEC_NORMAL = 0,
    PEC_SLIGHT_INTEFERENCE,
    PEC_HEAVY_INTERFERENCE
} PEC_STATE;

typedef struct emulated_adbms_6830
{
    VOLTAGE_STATE volt_behavior;
    TEMP_STATE temp_behavior;
    PEC_STATE signal_behavior;
    cell_asic_6830* ic_data;
} emulated_adbms_6830;

typedef struct emulated_adbms_2950
{
    VOLTAGE_STATE volt_behavior;
    CURRENT_STATE current_behavior;
    PEC_STATE signal_behavior;
    cell_asic_2950* ic_data;
} emulated_adbms_2950;

#ifdef TESTBENCH
    #ifndef BMS_TEST_TYPE
        bms_test_type test_type = NORMAL_DISCHARGE;
    #else
        bms_test_type test_type = BMS_TEST_TYPE;
    #endif    
#endif

void testbench_init(void);