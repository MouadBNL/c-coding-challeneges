#include <stdio.h>
#include <stdbool.h>
#include <string.h>


typedef struct {
	char *path;
	union {
		struct {
			bool bytes;
			bool lines;
			bool words;
			bool chars;
		};
		int hasargs;
	};
} Args;

typedef struct {
	size_t bytes;
	size_t lines;
	size_t words;
	size_t chars;
} WcResult;

void args_parse(Args *args, int argc, char **argv) {
	args->path = NULL;
	for(int i = 1; i < argc; i++) {
		char *arg = argv[i];
		if(strncmp(arg, "-c", 2) == 0 || strncmp(arg, "--bytes", 7) == 0) {
			args->bytes = 1;
		}
		if(strncmp(arg, "-l", 2) == 0 || strncmp(arg, "--lines", 7) == 0) {
			args->lines = 2;
		}
		if(strncmp(arg, "-w", 2) == 0 || strncmp(arg, "--words", 7) == 0) {
			args->words = 1;
		}
		if(strncmp(arg, "-m", 2) == 0 || strncmp(arg, "--chars", 7) == 0) {
			args->chars = 1;
		}
		if(arg[0] == '-') {
			continue;
		}
		args->path = arg;
	}
}

bool iswspacechar(char byte) {
	return byte == ' ' || byte == '\n' || byte == '\t' || byte == '\f' || byte == '\v' || byte == '\r';
}

WcResult wc(FILE *file) {
	WcResult res = {
		.bytes = 0,
		.lines = 0,
		.words = 0,
	};
	char byte = 0, pbyte = 0;

	while((byte = fgetc(file)) != EOF) {
		res.bytes++;
		if(byte == '\n') {
			res.lines++;
		}
		if(iswspacechar(byte) && !iswspacechar(pbyte)) {
			res.words++;
		}
		if((byte & 127) == 0) {
			res.chars++;
		}

		pbyte = byte;
	}

	return res;
}

int main(int argc, char **argv) {

	Args args = { 0 };
	args_parse(&args, argc, argv);

	FILE *file = args.path == NULL ? stdin : fopen(args.path, "r");
	if(file == NULL) {
		fprintf(stderr, "Could not open the files %s.", args.path);
		goto free_file;
	}

	WcResult res = wc(file);

	printf(" ");
	if(args.lines || !args.hasargs) {
		printf("%4zu ", res.lines);
	}
	if(args.words || !args.hasargs) {
		printf("%4zu ", res.words);
	}
	if(args.bytes || !args.hasargs) {
		printf("%4zu ", res.bytes);
	}
	if(args.chars) {
		printf("%4zu ", res.chars);
	}


	printf("%s\n", args.path == NULL ? " " : args.path);

free_file:
	fclose(file);

	return 0;
}
