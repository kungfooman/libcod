#ifndef _GSC_ASTAR_HPP_
#define _GSC_ASTAR_HPP_

#ifdef __cplusplus
extern "C" {
#endif

/* default stuff */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* gsc functions */
#include "gsc.hpp"


#if COMPILE_ASTAR == 1

// LOOOOOOOOOOOOOL
// i can leave this away and it compiles!
// BUT THEN EVERYTHING IS INTEGER!!!!
// SEARCHED SO LONG THE PROBLEM WHY FLOATES ARE MESSED UP
extern int GRAPH_new(); // 8
extern int GRAPH_add_edge(int graph, int first, int second); // 9
extern int GRAPH_add_vertex(int graph, float x, float y, float z); // 10
extern int GRAPH_delete(int graph); // 11
extern int GRAPH_build(int graph); // 12
extern int GRAPH_do_astar(int graph, int from, int to, int **outRet, int *outLen, float *outCost); // 13
extern int GRAPH_getNearestEdge(int graph, float *pos, int *out_wp1, int *out_wp2, float **out_pos); // number 15
extern int MATH_nearest_point_on_linesegment(float *from, float *to, float *toPoint, float *out_point);

#endif

int gsc_graph_new();
int gsc_graph_add_edge();
int gsc_graph_add_vertex();
int gsc_graph_delete();
int gsc_graph_build();
int gsc_graph_do_astar();
int gsc_graph_get_nearest_edge();

#ifdef __cplusplus
}
#endif

#endif