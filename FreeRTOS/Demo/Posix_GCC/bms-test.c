#include <stdio.h>
#include <pthread.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

/* Local includes. */
#include "console.h" 

extern int test_stream_read(void* filepath);

//overall function for bms main tester
int bms_test_main(void) {
    console_print("Starting the BMS test in specific code.\n");
    int ret = test_stream_read("./test_data_streams/read_test\0");
    return ret;
}

