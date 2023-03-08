#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAX_FILES 100
#define MAX_FILENAME_LENGTH 256
#define MAX_LINE_LENGTH 1024

int main(int argc, char *argv[]) {
    DIR *dir;
    struct dirent *ent;
    char filenames[MAX_FILES][MAX_FILENAME_LENGTH];
    int num_files = 0;
    char tmp[MAX_FILENAME_LENGTH+128];
    
    if (argc < 2)
    {
		printf("C_files_to_header directory\nBy gameblabla and ChatGPT\n");
		return 1;
	}
    
    char *dir_name = argv[1];

    if ((dir = opendir(dir_name)) != NULL) {
        /* Get all file names in directory */
        while ((ent = readdir(dir)) != NULL) {
            if (strstr(ent->d_name, ".c")) {
                strcpy(filenames[num_files], ent->d_name);
                num_files++;
            }
        }
        closedir(dir);
    } else {
        /* could not open directory */
        perror("");
        return EXIT_FAILURE;
    }

    /* Generate header file */

    /* Generate header file */
    FILE *header_file = fopen("files_msx.h", "w");
    fprintf(header_file, "#ifndef FILES_MSX_H\n#define FILES_MSX_H\n\n");
    int num;
    
    for (int i = 0; i < num_files; i++) {
        char *filename = filenames[i];
        char array_name[MAX_FILENAME_LENGTH];
        strcpy(array_name, filename);
        strtok(array_name, ".");
        snprintf(tmp, sizeof(tmp), "%s/%s", argv[1], filename); 
        FILE *input_file = fopen(tmp, "r");
        if (input_file == NULL) {
            perror(tmp);
            printf("filename %s\n", tmp);
            return EXIT_FAILURE;
        }
        int array_size = -1;
        int array_bank = -1; // Added for storing bank number
        char line[MAX_LINE_LENGTH];
        while (fgets(line, MAX_LINE_LENGTH, input_file) != NULL) {
            if (strstr(line, "const unsigned char") && strstr(line, array_name)) {
                char *size_start = strstr(line, "[") + 1;
                char *size_end = strstr(line, "]");
                *size_end = '\0';
                array_size = atoi(size_start);
            } else if (strstr(line, "#pragma codeseg BANK")) {
                char *bank_start = strstr(line, "BANK") + strlen("BANK");
                array_bank = atoi(bank_start);
            }
        }
        fclose(input_file);
        if (array_size == -1) {
            fprintf(stderr, "Could not find array size for %s\n", array_name);
            return EXIT_FAILURE;
        }
        input_file = fopen(tmp, "r");
        if (input_file == NULL) {
            perror(tmp);
            return EXIT_FAILURE;
        }
        fprintf(header_file, "extern const unsigned char %s[%d];\n", array_name, array_size);
        fclose(input_file);

        // Extracts part before _part
        char tmp[128];
        snprintf(tmp, sizeof(tmp), "%s", array_name);
		char* pos = strrchr(tmp, '_');
		if (pos != NULL) {
			*pos = '\0'; // set the character after the last "_" to null terminator
			char* part_pos = strstr(tmp, "_part");
			num = atoi(part_pos + strlen("_part")); // Extracts number
			if (part_pos != NULL) {
				*part_pos = '\0'; // set the first character of "_part" to null terminator
			}
		}
		
		fprintf(header_file, "#define SIZE_%s_PART_%d %d\n", tmp, num, array_size);

		
		char *part_number = strstr(array_name, "_part");
        if (part_number != NULL && atoi(part_number+strlen("_part")) == 1) {
            *part_number = '\0';
            fprintf(header_file, "#define %s_BANK %d\n", array_name, array_bank);
        }
    }

	/* Generate part1/part2 arrays */
	fprintf(header_file, "\n");
	for (int i = 0; i < num_files; i++) {
		char *filename = filenames[i];
		char array_name[MAX_FILENAME_LENGTH];
		strcpy(array_name, filename);
		strtok(array_name, ".");
		char *part_number = strstr(array_name, "_part");
		if (part_number != NULL) {
			*part_number = '\0';
			part_number += strlen("_part");
			int part = atoi(part_number);
			if (part == 1) {
				char array_inside[2][MAX_FILENAME_LENGTH];
				snprintf(array_inside[0] ,sizeof(array_inside[0]), "%s_part%d_bin", array_name, part);
				snprintf(array_inside[1] ,sizeof(array_inside[1]), "%s_part%d_bin", array_name, part+1);
				fprintf(header_file, "const unsigned char* const %s[] = {\n", array_name);
				fprintf(header_file, "\t%s,\n\t%s\n};\n", array_inside[0], array_inside[1]);
			}
		}
	}

	fprintf(header_file, "#endif\n");
	fclose(header_file);

	return EXIT_SUCCESS;
}
