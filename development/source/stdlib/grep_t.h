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
//	grep_t.h				   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "machine.h"

BEGIN_MACHINE_NAMESPACE

class grep_t {
public:
						grep_t();

	void				set_pattern(
							const char*					s,
							long						length,
							int							flags );

	bool				has_pattern(
							const char*					s,
							long						length,
							int							flags ) const;

	const char*			match(
							const char*					lbuf );

private:
	void				store(
							int							op );

	void				badpat(
							const char*					err,
							const char*					s,
							const char*					t );

	const char*			cclass(
							const char*					source,
							const char*					src );

	const char*			pmatch(
							const char*					lbuf,
							const char*					line,
							const char*					pattern );

	istring				m_pbuf;
	int					m_flags;
};

inline bool
grep_t::has_pattern(
	const char*					s,
	long						length,
	int							flags ) const
{
	return flags == m_flags && length == m_pbuf.length() &&
		memcmp( s, m_pbuf.data(), length * sizeof( char ) ) == 0;
}

END_MACHINE_NAMESPACE
