#include "gsc_astar.hpp"

#if COMPILE_ASTAR == 1

int gsc_graph_new()
{
	int graph = GRAPH_new();
	//printf("graph=%d", graph);
	return stackReturnInt(graph);
}

int gsc_graph_add_edge() // int GRAPH_add_edge(int graph, int first, int second);
{
	int graph, first, second;
	
	int helper = 0;
	helper += stackGetParamInt(1, &graph);
	helper += stackGetParamInt(2, &first);
	helper += stackGetParamInt(3, &second);
	if (helper != 3)
	{
		printf("scriptengine> wrongs args for RAPH_add_edge(int graph, int first, int second);\n");
		return stackReturnInt(0);
	}
	
	printf("GRAPH_add_edge(graph=%d, first=%d, second=%d);\n", graph, first, second);
	GRAPH_add_edge(graph, first, second);
	return stackReturnInt(1);
}

int gsc_graph_add_vertex() // int GRAPH_add_vertex(int graph, float x, float y, float z);
{
	int graph;
	float x, y, z;
	int helper = 0;
	helper += stackGetParamInt(1, &graph);
	helper += stackGetParamFloat(2, &x);
	helper += stackGetParamFloat(3, &y);
	helper += stackGetParamFloat(4, &z);
	if (helper != 4)
	{
		printf("scriptengine> wrongs args for RAPH_add_vertex(int graph, float x, float y, float z);\n");
		return stackReturnInt(0);
	}
	GRAPH_add_vertex(graph, x, y, z);
	printf("GRAPH_add_vertex(graph=%d, x=%.2f, y=%.2f, z=%.2f);\n", graph, x, y, z);
	return stackReturnInt(1);
}

int gsc_graph_delete() // int GRAPH_delete(int graph);
{
	int graph;
	int helper = 0;
	helper += stackGetParamInt(1, &graph);
	if (helper != 1)
	{
		printf("scriptengine> wrongs args for: int GRAPH_delete(int graph)\n");
		return stackReturnInt(0);
	}
	GRAPH_delete(graph);
	printf("GRAPH_delete(graph=%d);\n", graph);
	return stackReturnInt(1);
}

int gsc_graph_build() // int GRAPH_build(int graph);
{
	int graph;
	int helper = 0;
	helper += stackGetParamInt(1, &graph);
	if (helper != 1)
	{
		printf("scriptengine> wrongs args for: int GRAPH_build(int graph)\n");
		return stackReturnInt(0);
	}
	GRAPH_build(graph);
	printf("GRAPH_build(graph=%d);\n", graph);
	return stackReturnInt(1);
}

int gsc_graph_do_astar() // int *GRAPH_do_astar(int graph, int from, int to, int **outRet, int *outLen);
{
	int graph, from, to;
	int helper = 0;
	helper += stackGetParamInt(1, &graph);
	helper += stackGetParamInt(2, &from);
	helper += stackGetParamInt(3, &to);
	if (helper != 3)
	{
		printf("scriptengine> wrongs args for: int GRAPH_do_astar(int graph, int from, int to)\n");
		return stackReturnInt(0);
	}
	int *outRet, outLen;
	float cost;
	int ret = GRAPH_do_astar(graph, from, to, &outRet, &outLen, &cost);
	if (ret == 0) // just do as everything would be normal (empty results)
	{
		outLen = 0;
		cost = 0;
	}
	//printf("cost=%.2f\n", cost);
	
	int i; // useable out of this comment for the real array-creation
	#ifdef VERBOSE_GRAPH
	printf("GRAPH_do_astar(graph=%d, from=%d, to=%d, &outRet, &outLen);\n", graph, from, to);
	printf("route: ");

	for (i=0; i<outLen; i++)
		printf("%d ", outRet[i]);
	printf("\n");
	
	#endif
	
	// todo? if no nodes, then return undefined... but how?
	
	
	/*
		word_8853240 = precache_string_sub_81101B4("key1");
		word_8853242 = precache_string_sub_81101B4("key2");
	*/
	
	ret = stackPushArray(); // this will be an array with [0] and [1]

	stackPushArray(); // [0] = array of nodes
	for (i=0; i<outLen; i++)
	{
		stackReturnInt(outRet[i]);
		stackPushArrayLast();
	}
	
	// dont forget to free memory! :)
	free(outRet);
	
	// LUULZ, dont forget to set those 4 bytes to 0! :D
	// forgot that... and since im just writting 2 bytes to it each, the other
	// bytes were random! in the start it was working, but later i wondering why it
	// didnt worked anymore! xD
	int key1 = 0; // precached string "key1"
	int key2 = 0;
	memcpy(&key1, (void *)0x08853240, 2); // jeah it workes!
	memcpy(&key2, (void *)0x08853242, 2);
	//printf("key1 = %.8x ", key1);
	//printf("key2 = %.8x\n", key2);
	stackSetKeyInArray(key1);
	
	
	//ret = stackPushArray(); // [1] = cost
	//stackPushFloat(cost);
	stackPushFloat(cost);
	//stackPushArrayLast();
	stackSetKeyInArray(key2);
	
	return ret;
	
	
	// todo: stackPushArray()
	// stackPushElement()
	// stackReturnArray();
	return stackReturnInt(1);
}

int gsc_graph_get_nearest_edge()
{
	int graph;
	float pos[3];
	int helper = 0;
	helper += stackGetParamInt(1, &graph);
	helper += stackGetParamVector(2, pos);
	if (helper != 2)
	{
		printf("scriptengine> wrongs args for: int GRAPH_getNearestEdge(graph, pos)\n");
		return stackReturnInt(0);
	}

	int wp1, wp2;
	GRAPH_getNearestEdge(graph, pos, &wp1, &wp2, NULL);
	
	// [0] == wp1 [1] == wp2
	int ret = stackPushArray(); // this will be an array with [0] and [1]
	
	
	int key1 = 0; // precached string "key1"
	int key2 = 0;
	memcpy(&key1, (void *)0x08853240, 2); // jeah it workes!
	memcpy(&key2, (void *)0x08853242, 2);

	stackPushInt(wp1);
	stackSetKeyInArray(key1);
	
	//ret = stackPushArray(); // [1] = cost
	//stackPushFloat(cost);
	stackPushInt(wp2);
	//stackPushArrayLast();
	stackSetKeyInArray(key2);
	
	return ret;
}

#endif