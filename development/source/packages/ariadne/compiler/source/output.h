// This file is part of incalang.

// incalang is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.

// incalang is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with Foobar; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// ===========================================================================
//	output.h				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ARIADNE_OUTPUT_H
#define ARIADNE_OUTPUT_H

//#pragma once

#include "compiler.h"
#include "output_stream_t.h"

BEGIN_COMPILER_NAMESPACE

struct pos_t {
	void*				block;
	long				offset;
};

void
out_free_unused_memory();

void
out_open_inline();

void*
out_close_inline();

void
out_open_stream(
	output_stream_t*	stream );

void
out_close_stream();

void
out_open_string(
	istring&			string );
	
void
out_close_string();

void
out_byte(
	int					val );

void
out_word(
	unsigned int		val );

void
out_long(
	unsigned long		val );

void
out_wide(
	u64					val );
	
void
out_float(
	float				val );

void
out_double(
	double				val );
	
void
out_string(
	const char*			s );
	
void
out_block(
	const u08*			data,
	size_t				size );

void
out_inline(
	void*				data );

unsigned long
out_tell();
	
void
out_tell_pos(
	pos_t*				pos );

void
out_seek_pos(
	const pos_t*		pos );

END_COMPILER_NAMESPACE

#endif

