#include <stdio.h>
#include <stdlib.h>

#include "read_file.h"

int read_file(unsigned char **output, size_t *size, const char *name) {
    FILE *file;
    file = fopen(name, "rb");
    if (file == NULL) {
        printf("cannot open file: %s\n", name);
        return -1; // Return an error code if the file cannot be opened
    }

    fseek(file,  0, SEEK_END);
    *size = ftell(file);
    rewind(file);

    // Allocate memory for the buffer
    *output = (unsigned char*) malloc(*size +  1);
    if (*output == NULL) {
        printf("cannot allocate memory for file buffer\n");
        fclose(file);
        return -1; // Return an error code if memory allocation fails
    }

    // Set the last byte of the buffer to '\0' to make it a valid C string
    (*output)[*size] = '\0';

    // Read the file into the buffer
    fread(*output, sizeof(unsigned char), *size, file);
    fclose(file);

    return  0; // Return success
}
