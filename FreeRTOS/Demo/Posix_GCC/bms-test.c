#include <stdio.h>
#include <pthread.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

/* Local includes. */
#include "console.h"

//overall function for bms main tester
int bms_test_main(void) {
    console_print("Starting the BMS test in specific code.\n");
    return 0;
}