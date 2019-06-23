#ifndef CITYSIM_MATH_H_GUARD
#define CITYSIM_MATH_H_GUARD

#include <cmath>

#define FUZZY_COMPARE_DIFF 0.0000001f
static float fuzzyFloatCompare(float f1, float f2)
{
	return fabsf(f1-f2) < FUZZY_COMPARE_DIFF;
}

static float fuzzyIsNull(float f)
{
	return f <= FUZZY_COMPARE_DIFF && f >= -FUZZY_COMPARE_DIFF;
}

#endif /* CITYSIM_MATH_H_GUARD */
