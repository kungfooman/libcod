#include "gsc_math.hpp"

void gsc_math_sqrt() {
	float x;
	if ( ! stackGetParams("f", &x)) {
		printf("scriptengine> wrong param for sqrt(float x)\n");
		stackPushUndefined();
		return;
	}
	stackPushFloat(sqrt(x));
}

void gsc_math_sqrtInv() {
	float x;
	if ( ! stackGetParams("f", &x)) {
		printf("scriptengine> wrong param for sqrtInv(float x)\n");
		stackPushUndefined();
		return;
	}
	// http://www.beyond3d.com/content/articles/8/
	float xhalf = 0.5f*x;
	int i = *(int*)&x;
	i = 0x5f3759df - (i>>1);
	x = *(float*)&i;
	x = x*(1.5f - xhalf*x*x);

	stackPushFloat(x);
}