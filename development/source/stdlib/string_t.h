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
//	string_t.h				   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "machine_t.h"

BEGIN_MACHINE_NAMESPACE

struct string_t {
	char*				text;
	u32					length;
};

void
string_create(
	machine_t&			m,
	string_t*			self,
	const char*			text,
	u32					length );

void
string_dispose(
	machine_t&			m,
	string_t*			self );

void
string_assign(
	machine_t&			m,
	string_t*			self,
	string_t*			what );

void
string_add(
	machine_t&			m,
	string_t*			r,
	string_t*			a,
	string_t*			b );

void
string_string(
	machine_t&			m,
	string_t*			r,
	string_t*			a,
	long				count );

void
string_upper(
	machine_t&			m,
	string_t*			r,
	string_t*			a );

void
string_lower(
	machine_t&			m,
	string_t*			r,
	string_t*			a );

void
string_left(
	machine_t&			m,
	string_t*			r,
	string_t*			a,
	long				count );

void
string_right(
	machine_t&			m,
	string_t*			r,
	string_t*			a,
	long				count );

void
string_mid(
	machine_t&			m,
	string_t*			r,
	string_t*			a,
	long				index );

void
string_mid(
	machine_t&			m,
	string_t*			r,
	string_t*			a,
	long				index,
	long				count );

long
string_instr(
	machine_t&			m,
	string_t*			self,
	string_t*			what,
	long				index );

long
string_grep(
	machine_t&			m,
	string_t*			self,
	string_t*			pattern,
	long				index,
	int					flags );

int
string_asc(
	machine_t&			m,
	string_t*			self );

double
string_val(
	machine_t&			m,
	string_t*			self );

s64
string_ival(
	machine_t&			/*m*/,
	string_t*			self );

int
string_valn(
	machine_t&			m,
	string_t*			self );

bool
string_equal(
	machine_t&			m,
	string_t*			a,
	string_t*			b );

int
string_compare(
	machine_t&			m,
	string_t*			a,
	string_t*			b );

const char*
string_get_c_str(
	string_t*			self );

END_MACHINE_NAMESPACE
