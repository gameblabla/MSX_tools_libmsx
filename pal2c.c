#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 32

int main(int argc, char *argv[]) {
    FILE *input_file, *output_file;
    char buffer[BUFFER_SIZE];
    int read_count, i;

    // Open input file
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    input_file = fopen(argv[1], "rb");
    if (input_file == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", argv[1]);
        return 1;
    }

    // Move file pointer to end of file and read last BUFFER_SIZE bytes
    fseek(input_file, -BUFFER_SIZE, SEEK_END);
    read_count = fread(buffer, 1, BUFFER_SIZE, input_file);
    if (read_count != BUFFER_SIZE) {
        fprintf(stderr, "Error: Could not read last %d bytes of file %s\n", BUFFER_SIZE, argv[1]);
        return 1;
    }

    // Output C array to file
    output_file = fopen(argv[2], "w");
    if (output_file == NULL) {
        fprintf(stderr, "Error: Could not open file output.c for writing\n");
        return 1;
    }
    fprintf(output_file, "const char my_array[%d] = {\n", BUFFER_SIZE);
    for (i = 0; i < BUFFER_SIZE; i++) {
        fprintf(output_file, "0x%02x, ", (unsigned char) buffer[i]);
    }
    fprintf(output_file, "};\n");

    // Close input and output files
    fclose(input_file);
    fclose(output_file);

    return 0;
}
