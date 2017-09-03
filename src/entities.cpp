#include "entities.h"

double constants::correctParams(double param)
{
	if (abs(param) < abs(MINIMAL_VALUE))
		return 0;
	return param;
}
