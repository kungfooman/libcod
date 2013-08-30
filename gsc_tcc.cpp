#include "gsc_tcc.hpp"

#if COMPILE_TCC != 0

int gsc_tcc_new()
{
	TCCState *s;
	
	#if DEBUG_TCC
	printf_hide("%s()\n", __FUNCTION__);
	#endif
	
	s = tcc_new();
	s->nostdlib = 1;
	return stackReturnInt((int) s);
}

int gsc_tcc_add_include_path()
{
	TCCState *s;
	char *pathname;
	
	int helper = 0;
	helper += stackGetParamInt(1, (int *)&s);
	helper += stackGetParamString(2, &pathname);
	
	#if DEBUG_TCC
	printf_hide("%s(TCCState *s=%.8p, char *pathname=\"%s\")\n", __FUNCTION__, s, pathname);
	#endif
	
	if (helper != 2)
	{
		printf_hide("scriptengine> wrongs args for %s(TCCState *s=%.8p, char *pathname=\"%s\")\n", __FUNCTION__, s, pathname);
		return stackPushUndefined();
	}

	int ret = (int) tcc_add_include_path(s, pathname);
	return stackReturnInt(ret);
}

#include <pthread.h>

TCCState *global_s;
char *global_filename;
int global_ret;
	
void *thread_for_tcc_add_file(void *arg)
{
	//global_ret = tcc_add_file(global_s, global_filename);
	
	TCCState *s;
	
	s = tcc_new();
	s->nostdlib = 1;
	tcc_add_include_path(s, "/usr/include");
	tcc_add_include_path(s, "include");
	//tcc_add_library(s, "c"); // libc
	tcc_add_file(s, "first.c");
	//tcc_run(s, 0, (char **)NULL);
	//tcc_delete(s);
	printf("test");
}
int tcc_add_file_thread(TCCState *s, char *filename)
{
	global_s = s;
	global_filename = filename;
	pthread_t thread1;
	
	
	
	
	pthread_attr_t settings;

	int stacksize;

	int ret = pthread_attr_init(&settings);
	if (ret != 0)
	{
		printf_hide("> [ERROR] pthread_attr_init() failed.\n");
		return 0;
	}

	stacksize = 1024*1024*20;

	ret = pthread_attr_setstacksize(&settings, stacksize);
	if (ret != 0)
	{
		printf_hide("> [ERROR] pthread_attr_setstacksize failed.\n");
		return 0;
	}

	printf_hide("> [INFO] Stack-Size set to %d Bytes (%.2f KB, %.2f MB)\n", stacksize, (float)(stacksize/1024), (float)((stacksize/1024)/1024));
	
	
	
	
	
	
	pthread_create (&thread1, &settings, thread_for_tcc_add_file, NULL);
	//pthread_join(thread1, NULL);
	return global_ret;
}

int gsc_tcc_add_file()
{
	TCCState *s;
	char *filename;
	
	int helper = 0;
	helper += stackGetParamInt(1, (int *)&s);
	helper += stackGetParamString(2, &filename);

	#if DEBUG_TCC
	printf_hide("%s(TCCState *s=%.8p, char *filename=\"%s\")\n", __FUNCTION__, s, filename);
	#endif
	
	if (helper != 2)
	{
		printf_hide("scriptengine> wrongs args for %s(TCCState *s=%.8p, char *filename=\"%s\")\n", __FUNCTION__, s, filename);
		return stackPushUndefined();
	}

	int ret = (int) tcc_add_file_thread(s, filename);
	return stackReturnInt(ret);
}

int gsc_tcc_run()
{
	TCCState *s;
	
	int helper = 0;
	helper += stackGetParamInt(1, (int *)&s);

	#if DEBUG_TCC
	printf_hide("%s(TCCState *s=%.8p)\n", __FUNCTION__, s);
	#endif
	
	if (helper != 1)
	{
		printf_hide("scriptengine> wrongs args for %s(TCCState *s=%.8p)\n", __FUNCTION__, s);
		return stackPushUndefined();
	}

	int ret = (int) tcc_run(s, 0, (char **)NULL);
	return stackReturnInt(ret);
}

int gsc_tcc_delete()
{
	TCCState *s;
	
	int helper = 0;
	helper += stackGetParamInt(1, (int *)&s);

	#if DEBUG_TCC
	printf_hide("%s(TCCState *s=%.8p)\n", __FUNCTION__, s);
	#endif
	
	if (helper != 1)
	{
		printf_hide("scriptengine> wrongs args for %s(TCCState *s=%.8p)\n", __FUNCTION__, s);
		return stackPushUndefined();
	}

	tcc_delete(s);
	return stackPushUndefined();
}
#endif