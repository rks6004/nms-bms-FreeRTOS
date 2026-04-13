#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

/* Local includes. */

int test_stream_read(void* filepath) {
    printf("Using filepath: %s\n", (char*)filepath);
    FILE* fptr; 
    fptr = fopen(filepath, "r");
    if (fptr != NULL) {
        size_t bytes_to_read = 20;
        size_t bytes_to_read_actual = bytes_to_read;
        char rptr[bytes_to_read_actual];
        int ret = fread(rptr, sizeof(char), bytes_to_read_actual, fptr);
        int i = 0;
        char this_char = rptr[i];
        while (this_char != EOF && i < bytes_to_read_actual) {
            printf("Read: %c\n", this_char);
            i += 2;
            this_char = rptr[i];
        }
        fclose(fptr);
        fptr = NULL;
        return 0;
    }
    else {
        printf("Error in file reading: %d\n", errno);
        return -1;
    }
}




