// ===========================================================================
//	spline.h		   		   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "machine.h"

BEGIN_MACHINE_NAMESPACE

float
spline( float x, long nknots, const float *knot );

float
spline( float x, long nknots, const float *knot, long spacing );

END_MACHINE_NAMESPACE
