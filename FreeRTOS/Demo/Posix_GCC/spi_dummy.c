#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

/* Local includes. */

int test_stream_read(void* filepath) {
    printf("Using filepath: %s\n", filepath);
    // const char* filepath_stable = (char*)filepath;
    FILE* fptr; 
    fptr = fopen(filepath, "r");
    // printf("File at: %p\n", fptr);
    if (fptr != NULL) {
        size_t bytes_to_read = 20;
        char rptr[bytes_to_read];
        int ret = fread(rptr, sizeof(char), bytes_to_read, fptr);
        // printf("Successfully retrieved %d bytes from file.\n", (int)ret);
        int i = 0;
        char this_char = rptr[i];
        while (this_char != NULL && this_char != "\0" && i < bytes_to_read) {
            printf("Read: %s\n", &this_char);
            i += 2;
            this_char = rptr[i];
        }
        return 0;
    }
    else {
        printf("Error in file reading: %d\n", errno);
        return -1;
    }
}

