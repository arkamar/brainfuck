#include <stdio.h>
#include <stdlib.h>

struct instruction {
	unsigned char operator;
	unsigned operand;
};

#define PROGRAM_SIZE 4096
#define STACK_SIZE   32

static struct instruction * program;

void
print_stack(const unsigned char * stack) {
	int i;
	fputs("stack: ", stderr);
	for (i = 0; i < STACK_SIZE; i++)
		fprintf(stderr, "%02x ", stack[i]);
	fputc('\n', stderr);
}

void
print_program(const struct instruction * prg) {
	int i;
	fputs("program:\n", stderr);
	for (i = 0; prg->operator; prg++, i++) {
		if (prg->operator == '[' || prg->operator == ']')
			fprintf(stderr, "%03d %c %03d\n", i, prg->operator, prg->operand);
		else
			fprintf(stderr, "%03d %c\n", i, prg->operator);
	}
}

void
compile(const char * text) {
	static unsigned pc = 0, jmp_pc;
	unsigned * stack, * ptr;

	stack = calloc(STACK_SIZE, sizeof * stack);
	ptr = stack;

	for (const char * t = text; *t && pc < PROGRAM_SIZE; t++) {
		switch (*t) {
		case '+': program[pc].operator = *t; break;
		case '-': program[pc].operator = *t; break;
		case '<': program[pc].operator = *t; break;
		case '>': program[pc].operator = *t; break;
		case '.': program[pc].operator = *t; break;
		case ',': program[pc].operator = *t; break;
		case '[':
			program[pc].operator = *t;
			*ptr = pc;
			ptr++;
			break;
		case ']':
			ptr--;
			jmp_pc = *ptr;
			program[pc].operator = *t;
			program[pc].operand  = jmp_pc;
			program[jmp_pc].operand  = pc;
			break;
		default:
			fprintf(stderr, "unknown instruction %c\n", *t);
			pc--;
			break;
		}
		pc++;
	}

	print_program(program);
	free(stack);
}

void
execute(const struct instruction * program) {
	unsigned char * stack;
	unsigned char * ptr;
	unsigned pc = 0;

	stack = calloc(STACK_SIZE, sizeof * stack);
	ptr = stack;

	while (program[pc].operator) {
		print_stack(stack);
		switch (program[pc].operator) {
			case '+': ++*ptr; break;
			case '-': --*ptr; break;
			case '<': --ptr; break;
			case '>': ++ptr; break;
			case '.': putchar(*ptr); break;
			case ',':
				if (fread(ptr, 1, 1, stdin) == 0)
					*ptr = 0;
				break;
			case '[': if (!*ptr) { pc = program[pc].operand; } break;
			case ']': if (*ptr)  { pc = program[pc].operand; } break;
			default:
				fputs("something went wrogn\n", stderr);
				return;
		}
		pc++;
	}
	print_stack(stack);
	free(stack);
}

int
main(int argc, char * argv[]) {
	program = calloc(PROGRAM_SIZE, sizeof * program);
	compile(argv[1]);
	execute(program);
	free(program);
	return 0;
}
