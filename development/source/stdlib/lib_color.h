// ===========================================================================
//	lib_color.h				   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "machine.h"

BEGIN_MACHINE_NAMESPACE

bool
retrieve_color_by_name(
	const char*			name,
	u08*				out );

void
retrieve_color_by_index(
	long				index,
	u08*				out );

END_MACHINE_NAMESPACE
