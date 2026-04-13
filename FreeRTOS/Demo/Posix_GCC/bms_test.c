#include <stdio.h>
#include <pthread.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

/* Local includes. */
#include "console.h"
#include "stdlib.h"

/*  BMS Includes */


extern int test_stream_read(void* filepath);

int bms_file_reading_test(void) {
    console_print("Starting the BMS test in specific code.\n");
    int ret = test_stream_read("./read_test");
    return ret;
}

int bms_task_tester(void* data) {
    console_print("%d\n", *(int*)data);
    return 0;
}



int cell_voltage_test(void) {


}

