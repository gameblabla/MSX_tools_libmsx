#include <stdio.h>
#include <stdlib.h>

#define PALETTE_SIZE 16
#define COLOR_SIZE 2

typedef struct {
    unsigned char r, g, b;
    unsigned char transparent;
} Color;

void print_palette(Color palette[]) {
    printf("Palette:\n");
    for (int i = 0; i < PALETTE_SIZE; i++) {
        printf("%2d: %d %d %d (%s)\n", i, palette[i].r, palette[i].g, palette[i].b, palette[i].transparent ? "transparent" : "opaque");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        fprintf(stderr, "Error: unable to open file %s\n", argv[1]);
        return 1;
    }

    // Determine the size of the file
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // Read the color values from the end of the file
    unsigned char color_data[PALETTE_SIZE * COLOR_SIZE];
    fseek(file, -PALETTE_SIZE * COLOR_SIZE, SEEK_END);
    fread(color_data, COLOR_SIZE, PALETTE_SIZE, file);

    // Convert the color values to a palette
    Color palette[PALETTE_SIZE];
    for (int i = 0; i < PALETTE_SIZE; i++) {
        unsigned short color_value = (color_data[i * COLOR_SIZE + 1] << 8) | color_data[i * COLOR_SIZE];
		palette[i].r = ((color_value & 0x7C00) >> 10) * 255 / 31 / 36 * 7;
		palette[i].g = ((color_value & 0x03E0) >> 5) * 255 / 31 / 36 * 7;
		palette[i].b = (color_value & 0x001F) * 255 / 31 / 36 * 7;

        palette[i].transparent = (color_value & 0x8000) >> 15;
    }

    // Print the palette
    print_palette(palette);

    fclose(file);
    return 0;
}
