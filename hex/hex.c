#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t get_size(FILE* f) {
	if (f == NULL) return -1;

	fseek(f, 0, SEEK_END);
	size_t ret = ftell(f);
	rewind(f);
	return ret;
}

char *replace(char* src, char old, char new) {
	char* tmp = src;

	while (*tmp != 0) {
		if (*tmp == old) *tmp = new;
		tmp++;
	}

	return src;
}

void write_str(const char* src) {
	char* tmp = _strdup(src), *tmp2 = replace(replace(replace(replace(replace(tmp, '\f', '.'), '\t', '.'), '\v', '.'), '\r', '.'), '\n', '.');

	printf("\t.");
	for (size_t i = 0; i < strlen(tmp2); ++i)
		printf("%c.", tmp2[i]);

	free(tmp);
	putchar('\n');
}

int main(int argc, char** argv) {
	FILE* f;
	
	if (argc != 2) {
		fprintf(stderr, "Error: no file has been referenced.\nUsage: hex <name of file to read>\n");
		return 1;
	}

#ifdef _WIN32
	errno_t err = fopen_s(&f, argv[1], "r");
	if (err) {
		errno = err;
		perror("Error opening file");
		return 1;
	}
#else
	f = fopen(argv[1], "r");
	if (ferror(f)) {
		perror("Error opening file");
		return 1;
	}
#endif

	unsigned char* tmp = calloc(0x11, sizeof(char));
	if (tmp == NULL) {
		printf("Error creating reading buffer\n");
		return 1;
	}

#ifdef _WIN32
	size_t read = fread_s(tmp, 0x10, sizeof(char), 0x10, f);
#else
	size_t read = fread(tmp, sizeof(char), 0x10, f);
#endif

	for (size_t i = 0; read; i += 0x10) {
		printf("%08zx\t", i);
		for (int j = 0; j < read; j++) {
			printf("%02x", tmp[j]);
			if (j != read - 1) putchar(' ');
		}

		if (read != 0x10) {
			int iters = 0x10 - read;
			for (int j = 0; j < iters; j++) {
				printf("  ");
				if (j != iters - 1) putchar(' ');
			}
		}
		write_str(tmp);
		memset(tmp, 0, 0x11);
		read = fread_s(tmp, 0x10, sizeof(char), 0x10, f);
	}

	free(tmp);

	if (ferror(f)) {
		perror("There was an error reading the file");
		return 1;
	}

	return 0;
}