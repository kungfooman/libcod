#include "gsc_memory.hpp"

#if COMPILE_MEMORY == 0
	#warning gsc_memory.cpp is not compiled
#else


int gsc_memory_malloc()
{
	int bytes;
	
	int helper = 0;
	helper += stackGetParamInt(1, &bytes);
	
	printf_hide("GANZ NORMALES ASD");
	
	
	#if DEBUG_MEMORY
	printf("gsc_memory_malloc(bytes=%d)\n", bytes);
	#endif
	
	if (helper != 1)
	{
		printf_hide("scriptengine> wrongs args for gsc_memory_malloc(bytes);\n");
		return stackReturnInt(0);
	}

	int ret = (int) malloc(bytes);
	return stackReturnInt(ret);
}

int gsc_memory_free()
{
	int memory;
	
	int helper = 0;
	helper += stackGetParamInt(1, &memory);
	
	#if DEBUG_MEMORY
	printf("gsc_memory_free(memory=%d)\n", memory);
	#endif
	
	if (helper != 1)
	{
		printf_hide("scriptengine> wrongs args for gsc_memory_free(memory);\n");
		return stackReturnInt(0);
	}

	free((void*)memory);
	return stackReturnInt(0);
}

int gsc_memory_int_get()
{
	int memory;
	
	int helper = 0;
	helper += stackGetParamInt(1, &memory);
	
	#if DEBUG_MEMORY
	printf("gsc_memory_int_get(memory=%d)\n", memory);
	#endif
	
	if (helper != 1)
	{
		printf_hide("scriptengine> wrongs args for gsc_memory_int_get(memory);\n");
		return stackReturnInt(0);
	}

	int ret = *(int*)memory;
	return stackReturnInt(ret);
}
int gsc_memory_int_set()
{
	int memory;
	int value;
	
	int helper = 0;
	helper += stackGetParamInt(1, &memory);
	helper += stackGetParamInt(2, &value);
	
	#if DEBUG_MEMORY
	printf("gsc_memory_int_set(memory=%d, value=%d)\n", memory, value);
	#endif
	
	if (helper != 2)
	{
		printf_hide("scriptengine> wrongs args for gsc_memory_int_set(memory, value);\n");
		return stackReturnInt(0);
	}

	*(int*)memory = value;
	return stackReturnInt(1);
}
int gsc_memory_memset()
{
	int memory;
	int value;
	int bytes;
	
	int helper = 0;
	helper += stackGetParamInt(1, &memory);
	helper += stackGetParamInt(2, &value);
	helper += stackGetParamInt(3, &bytes);
	
	#if DEBUG_MEMORY
	printf("gsc_memory_memset(memory=%d, value=%d, bytes=%d)\n", memory, value, bytes);
	#endif
	
	if (helper != 3)
	{
		printf_hide("scriptengine> wrongs args for gsc_memory_memset(memory, value, bytes);\n");
		return stackReturnInt(0);
	}

	memset((void*)memory, value, bytes);
	return stackReturnInt(1);
}

#endif