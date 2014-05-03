#include "gsc_car.hpp"

#if COMPILE_CAR == 0
	#warning gsc_car.cpp is not compiled
#else

// /root/q3rally/q3rallysa/engine/code/game
// -I/root/q3rally/q3rallysa/
extern "C"
{
#include "engine/code/qcommon/q_shared.h"
#include "engine/code/game/bg_public.h"
#include "engine/code/game/bg_local.h"
}
#define DEBUG_CAR 1

void my_trace(trace_t *results, const vec3_t start,  const vec3_t mins,  const vec3_t maxs,  const vec3_t end, int passEntityNum, int contentMask)
{
	results->startsolid = qfalse;
	results->allsolid = qfalse;
	
	// needed: fraction, plane(plane.normal), endpos, surfaceFlags
	
	//trace 0 00000000
	//printf("trace %d %.8x\n", passEntityNum, contentMask);
}

qboolean G_FrictionCalc_(const carPoint_t *point, float *sCOF, float *kCOF)
{
	return qfalse;
}
int CG_PointContents_(const vec3_t point, int passEntityNum)
{
	return 0;
}

void init_pm()
{
	pm = (pmove_t *) malloc(sizeof(pmove_t));
	memset(pm, 0, sizeof(pmove_t));
	
	pm->ps = (playerState_t *) malloc(sizeof(playerState_t));
	memset(pm->ps, 0, sizeof(playerState_t));
	
	
	
	pm->ps->gravity = 800;
	pm->ps->viewangles[PITCH] = 0;
	pm->ps->viewangles[YAW] = 0;
	pm->ps->viewangles[ROLL] = 0;
	
	pm->ps->damageYaw = 0;
	pm->ps->damagePitch = 0;
	//pm->ps->damageRoll = 0;
	
	pm->ps->extra_eFlags = 0;
	
	//pm->cmd = (usercmd_t *) malloc(sizeof(usercmd_t));
	pm->cmd.upmove = 100.0;
	pm->cmd.forwardmove = 100.0;
	pm->cmd.upmove = 100.0;
	
	pm->ps->stats[STAT_HEALTH] = 100;
	
	pm->manualShift = qfalse;
	pm->controlMode = CT_MOUSE;
	
	pm->car_frontweight_dist = 0.5;
	pm->car_body_elasticity = 0.05;
	pm->car_IT_xScale = 1.0;
	pm->car_IT_yScale = 1.0;
	pm->car_IT_zScale = 1.0;
	
	pm->car_swaybar = 20;
	
	pm->car_shock_up = 12;
	pm->car_shock_down = 11;
	
	pm->car_wheel = 2400;
	pm->car_wheel_damp = 140;
	
	pm->car_friction_scale = 1.10;
	
	pm->trace = my_trace;
	pm->frictionFunc = G_FrictionCalc_;
	pm->pointcontents = CG_PointContents_;
	
	pm->pDebug = 1;
}

int gsc_car_new()
{
	float origin[3];
	float viewangles[3];
	float velocity[3];


	static int first = 1;
	if (first)
	{
		printf("init_pm();\n");
		init_pm();
		first = 0;
	}
	
	int helper = 0;
	helper += stackGetParamVector(1, origin);
	helper += stackGetParamVector(2, viewangles);
	helper += stackGetParamVector(3, velocity);
	
	#if DEBUG_CAR
	printf("gsc_car_new(origin=(%.2f,%.2f,%.2f), viewangles=(%.2f,%.2f,%.2f), velocity(%.2f,%.2f,%.2f))\n",
		origin[0], origin[1], origin[2],
		viewangles[0], viewangles[1], viewangles[2],
		velocity[0], velocity[1], velocity[2]
	);
	#endif
	
	if (helper != 3)
	{
		printf("scriptengine> wrongs args for gsc_car_new(origin, viewangles, velocity)\n");
		return stackPushUndefined();
	}
	
	car_t *car = (car_t *) malloc(sizeof(car_t));
	
	/*
	vec3_t origin = {100,100,100};
	vec3_t angles = {100,100,100};
	vec3_t velocity = {0,0,10};
	*/

	
	// instead of calling it myself, let PM_DriveMove do it
	//PM_InitializeVehicle(&car, origin, angles, velocity);
	//printf("car.initializeOnNextMove = %d\n", car.initializeOnNextMove);
	VectorCopy(origin, pm->ps->origin);
	VectorCopy(viewangles, pm->ps->viewangles);
	VectorCopy(velocity, pm->ps->velocity);
	/*VectorCopy(pm->ps->origin, origin);
	VectorCopy(pm->ps->viewangles, angles);
	VectorCopy(pm->ps->velocity, velocity);*/
	car->initializeOnNextMove = qtrue;

	// initialize it the first time (maybe i shall call it myself to prevent the first run)
	PM_DriveMove(car, 0.20, qfalse); // includeBodies
	
	
	int ret = (int) car;
	return stackReturnInt(ret);
}

extern "C" void PM_DebugDynamics(carBody_t *body, carPoint_t *points);

int gsc_car_update()
{
	int car_address;

	
	int helper = 0;
	helper += stackGetParamInt(1, &car_address);
	
	#if DEBUG_CAR
	printf("gsc_car_update(car=%d)\n", car_address);
	#endif
	
	if (helper != 1)
	{
		printf("scriptengine> wrongs args for gsc_car_update(car)\n");
		return stackPushUndefined();
	}
	
	car_t *car = (car_t *) car_address;
	
	PM_DriveMove(car, 0.05, qfalse); // includeBodies
	
	PM_DebugDynamics(&(car->tBody), &(car->tPoints[0]));
	
	int ret = stackPushArray();
	
	// ret[0] = origin
	stackPushVector(car->tBody.r);
	stackPushArrayLast();
	
	// ret[1] = forward
	stackPushVector(car->tBody.forward);
	stackPushArrayLast();
	
	return ret;
}

#endif