#include "gsc_memory.hpp"

#if COMPILE_MEMORY == 0
	#warning gsc_memory.cpp is not compiled
#else

void gsc_memory_malloc() {
	int bytes;
	
	if ( ! stackGetParams("i", &bytes)) {
		printf("scriptengine> wrongs args for gsc_memory_malloc(bytes);\n");
		stackPushUndefined();
		return;
	}
	#if DEBUG_MEMORY
	printf("gsc_memory_malloc(bytes=%d)\n", bytes);
	#endif

	stackReturnInt((int) malloc(bytes));
}

void gsc_memory_free() {
	int memory;

	if ( ! stackGetParams("i", &memory)) {
		printf("scriptengine> wrongs args for gsc_memory_free(memory);\n");
		stackPushUndefined();
		return;
	}
	#if DEBUG_MEMORY
	printf("gsc_memory_free(memory=%d)\n", memory);
	#endif

	free((void*)memory);
	stackReturnInt(0);
}

void gsc_memory_int_get() {
	int memory;

	if ( ! stackGetParams("i", &memory)) {
		printf("scriptengine> wrongs args for gsc_memory_int_get(memory);\n");
		stackPushUndefined();
		return;
	}
	#if DEBUG_MEMORY
	printf("gsc_memory_int_get(memory=%d)\n", memory);
	#endif

	stackReturnInt(*(int*)memory);
}

void gsc_memory_int_set() {
	int memory, value;

	if ( ! stackGetParams("ii", &memory, &value)) {
		printf("scriptengine> wrongs args for gsc_memory_int_set(memory, value);\n");
		stackPushUndefined();
		return;
	}
	#if DEBUG_MEMORY
	printf("gsc_memory_int_set(memory=%d, value=%d)\n", memory, value);
	#endif
	
	*(int*)memory = value;
	stackReturnInt(1);
}

void gsc_memory_memset() {
	int memory, value, bytes;

	if ( ! stackGetParams("iii", &memory, &value, &bytes)) {
		printf("scriptengine> wrongs args for gsc_memory_memset(memory, value, bytes);\n");
		stackPushUndefined();
		return;
	}
	#if DEBUG_MEMORY
	printf("gsc_memory_memset(memory=%d, value=%d, bytes=%d)\n", memory, value, bytes);
	#endif

	memset((void*)memory, value, bytes);
	stackReturnInt(1);
}

#include <vector>
struct binarybuffer {
	int address;
	int pos;
	std::vector<char *> *strings;
};
void gsc_binarybuffer_new() {
	int address;
	if ( ! stackGetParams("i", &address)) {
		printf("scriptengine> wrongs args for binarybuffer_new(address);\n");
		stackPushUndefined();
		return;
	}
	struct binarybuffer *bb = (struct binarybuffer *)malloc(sizeof(struct binarybuffer));
	bb->address = address;
	bb->pos = 0;
	bb->strings = new std::vector<char *>();
	stackPushInt((int)bb);
}
void gsc_binarybuffer_free() {
	struct binarybuffer *bb;
	if ( ! stackGetParams("i", &bb)) {
		printf("scriptengine> wrongs args for binarybuffer_free(binarybuffer);\n");
		stackPushUndefined();
		return;
	}
	for (std::vector<char *>::const_iterator i = bb->strings->begin(); i != bb->strings->end(); i++)
		free(*i);
	delete bb->strings;
	free(bb);
	stackReturnInt(1);
}
void gsc_binarybuffer_seek() {
	struct binarybuffer *bb;
	int pos;
	if ( ! stackGetParams("ii", &bb, &pos)) {
		printf("scriptengine> wrongs args for binarybuffer_seek(binarybuffer, pos);\n");
		stackPushUndefined();
		return;
	}
	bb->pos = pos;
	stackReturnInt(1);
}
void gsc_binarybuffer_write() {
	struct binarybuffer *bb;
	char *type;
	if ( ! stackGetParams("is", &bb, &type)) {
		printf("scriptengine> wrongs args for binarybuffer_write(binarybuffer, type [, value] );\n");
		stackPushUndefined();
		return;
	}
	switch (type[0]) {
		case 'i': {
			int tmp_int;
			stackGetParamInt(2, &tmp_int);
			*(int *)(bb->address + bb->pos) = tmp_int;
			#if DEBUG_MEMORY
			printf("type=i bb->address=%.8p + %d = %d\n", bb->address, bb->pos, *(int *)(bb->address + bb->pos));
			#endif
			bb->pos += 4;
			break;
		}
		case 'f': {
			float tmp_float;
			stackGetParamFloat(2, &tmp_float);
			*(float *)(bb->address + bb->pos) = tmp_float;
			#if DEBUG_MEMORY
			printf("type=f bb->address=%.8p + %d = %f\n", bb->address, bb->pos, *(float *)(bb->address + bb->pos));
			#endif
			bb->pos += 4;
			break;
		}
		case 'd': {
			float tmp_float;
			stackGetParamFloat(2, &tmp_float);
			*(double *)(bb->address + bb->pos) = (double)tmp_float;
			#if DEBUG_MEMORY
			printf("type=d bb->address=%.8p + %d = %f\n", bb->address, bb->pos, *(double *)(bb->address + bb->pos));
			#endif
			bb->pos += 8;
			break;
		}
		case 's': {
			char *tmp_str;
			stackGetParamString(2, &tmp_str);
			char *copy = (char *)malloc(strlen(tmp_str) + 1);
			strcpy(copy, tmp_str);
			bb->strings->push_back(copy);
			*(char **)(bb->address + bb->pos) = copy;
			#if DEBUG_MEMORY
			printf("type=s bb->address=%.8p + %d = %s\n", bb->address, bb->pos, *(char **)(bb->address + bb->pos));
			#endif
			bb->pos += 4;
			break;
		}
		case 'c': {
			char *tmp_str;
			stackGetParamString(2, &tmp_str);
			*(char *)(bb->address + bb->pos) = tmp_str[0];
			#if DEBUG_MEMORY
			printf("type=c bb->address=%.8p + %d = %c\n", bb->address, bb->pos, *(char *)(bb->address + bb->pos));
			#endif
			bb->pos += 1;
			break;
		}
		case 'v': {
			float tmp_vector[3];
			stackGetParamVector(2, tmp_vector);
			*(float *)(bb->address + bb->pos + 0) = tmp_vector[0];
			*(float *)(bb->address + bb->pos + 4) = tmp_vector[1];
			*(float *)(bb->address + bb->pos + 8) = tmp_vector[2];
			#if DEBUG_MEMORY
			printf("type=v bb->address=%.8p + %d = (%f,%f,%f)\n", bb->address, bb->pos,
				*(float *)(bb->address + bb->pos + 0),
				*(float *)(bb->address + bb->pos + 4),
				*(float *)(bb->address + bb->pos + 8)
			);
			#endif
			bb->pos += 12;
			break;
		}
	}
	stackReturnInt(1);
}
void gsc_binarybuffer_read() {
	struct binarybuffer *bb;
	char *type;
	if ( ! stackGetParams("is", &bb, &type)) {
		printf("scriptengine> wrongs args for binarybuffer_read(binarybuffer, type);\n");
		stackPushUndefined();
		return;
	}
	switch (type[0]) {
		case 'i': {
			int tmp_int;
			tmp_int = *(int *)(bb->address + bb->pos);
			bb->pos += 4;
			stackPushInt(tmp_int);
			return;
		}
		case 'f': {
			float tmp_float;
			tmp_float = *(float *)(bb->address + bb->pos);
			bb->pos += 4;
			stackPushFloat(tmp_float);
			return;
		}
		case 'd': {
			float tmp_float;
			tmp_float = (float)*(double *)(bb->address + bb->pos);
			bb->pos += 8;
			stackPushFloat(tmp_float);
			return;
		}
		case 's': {
			char *tmp_str;
			tmp_str = *(char **)(bb->address + bb->pos);
			bb->pos += 4;
			stackPushString(tmp_str);
			return;
		}
		case 'c': {
			char tmp_str[2];
			tmp_str[0] = *(char *)(bb->address + bb->pos);
			tmp_str[1] = '\0';
			bb->pos += 1;
			stackPushString(tmp_str);
			return;
		}
		case 'v': {
			float *tmp_vector;
			tmp_vector = (float *)(bb->address + bb->pos + 0);
			bb->pos += 12;
			stackPushVector(tmp_vector);
			return;
		}
	}
	stackPushUndefined();
}
#endif
