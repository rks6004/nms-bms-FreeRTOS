#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>

#include "bms_test.h"
#include "main.h"

//Analog-Devices-related includes
#include "adBms2950Data.h"
#include "adBms2950Driver.h"

void adBms2950_read_acc_ivbat_testbench(uint8_t tIC, cell_asic_2950 *ic, DATA_TYPE type);



