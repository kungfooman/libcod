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

#endif
