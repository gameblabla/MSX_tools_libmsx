#include <stdio.h>
#include <stdint.h>

#define ARRAY_SIZE 16

int main(int argc, char *argv[]) {
	int c = 0;
    if (argc < 3) {
        fprintf(stderr, "Usage: %s filename output\n", argv[0]);
        return 1;
    }

	FILE *filetowrite = fopen(argv[2], "w");
	
    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        fprintf(stderr, "Error: unable to open file %s\n", argv[1]);
        return 1;
    }

    uint16_t data[ARRAY_SIZE];
    size_t bytes_read = fread(data, sizeof(uint16_t), ARRAY_SIZE, file);
    fclose(file);

    fprintf(filetowrite, "const unsigned short msx_palette[%d] = {\n", ARRAY_SIZE);
    for (int i = 0; i < bytes_read; i++) {
        uint16_t value = data[i];
        
        fprintf(filetowrite, "    0x%04X", value);
        if (i < 15) fprintf(filetowrite, ",");
		
		c++;
        if (c > 3) {
            fprintf(filetowrite, "\n");
            c = 0;
        }
    }
    fprintf(filetowrite, "};\n");

	fclose(filetowrite);

    return 0;
}
