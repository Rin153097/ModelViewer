#include "Util.h"

#include <crtdbg.h>

bool Util::ApproxEqual(float left, float right, float epsilon)
{
	return (left > right - epsilon && left < right + epsilon);
}