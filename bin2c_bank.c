#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define MAX_FILENAME_LEN 256

long fsize(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        return -1;
    }
    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);
    fclose(fp);
    return size;
}

char* replace_dots_with_underscores(const char* str) {
    char* new_str = malloc(strlen(str) + 1); // allocate memory for new string
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == '.') {
            new_str[i] = '_';
        } else {
            new_str[i] = str[i];
        }
    }
    new_str[strlen(str)] = '\0'; // add null terminator to new string
    return new_str;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        printf("Usage: %s file1 bank1 file2 bank2\n", argv[0]);
        return 1;
    }

    char* files[2] = {argv[1], argv[3]};
    int banks[2] = {atoi(argv[2]), atoi(argv[4])};

    // Check that input files exist
    for (int i = 0; i < 2; i++) {
        if (access(files[i], F_OK) == -1) {
            printf("Error: File %s not found.\n", files[i]);
            return 1;
        }
    }

    // Generate output files
    char outs[2][MAX_FILENAME_LEN];
    for (int i = 0; i < 2; i++) {
        snprintf(outs[i], MAX_FILENAME_LEN, "%s.c", replace_dots_with_underscores(files[i]));
        printf("Generating %s...\n", outs[i]);
        FILE* fp = fopen(outs[i], "w");
        fprintf(fp, "#pragma codeseg BANK%d\n", banks[i]);
        fprintf(fp, "const unsigned char %s[%d] = {", replace_dots_with_underscores(files[i]), (int)fsize(files[i]));
        FILE* f = fopen(files[i], "rb");
        unsigned char buf[16];
        size_t nread;
        while ((nread = fread(buf, 1, sizeof buf, f)) > 0) {
            for (size_t j = 0; j < nread; j++) {
                if (j == 0) {
                    fprintf(fp, "\n    ");
                } else {
                    fprintf(fp, ", ");
                }
                fprintf(fp, "0x%02X", buf[j]);
                if ((j == nread - 1 && (nread % 16 != 0 || nread == sizeof buf)) || j == 15) {
                    fprintf(fp, ",");
                }
            }
        }
        fprintf(fp, "\n};\n");
        fclose(fp);
        fclose(f);
    }

    printf("Done.\n");

    return 0;
}
