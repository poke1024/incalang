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
//	code_collapse_tag_t.cp     ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "code_collapse_tag_t.h"
#include "code_line_t.h"
#include "code_stream_t.h"

BEGIN_CODE_NAMESPACE

collapse_tag_t::collapse_tag_t(
	tagid_t				tag ) :
	
	tag_t( tag )
{
	m_lines = 0;
	m_line_count = 0;
}

collapse_tag_t::~collapse_tag_t()
{
	cleanup();
}

void
collapse_tag_t::text(
	istring&			text )
{
#if 0
	char s[ 32 ];
	std::sprintf( s, "%ld", (long)ref_count() );
	text = "{ .";
	text.append( s );
	text.append( ". }" );
#else
	text = "{ ... }";
#endif
}

bool
collapse_tag_t::can_expand()
{
	return m_lines != 0;
}

void
collapse_tag_t::transfer_lines(
	line_t**			lines )
{
	if( m_lines == 0 )
		throw_icarus_error( "no lines left" );

	memcpy(
		lines,
		m_lines,
		m_line_count * sizeof( line_t* ) );
		
	delete[] m_lines;
	m_lines = 0;
}

void
collapse_tag_t::cleanup()
{
	if( m_lines )
	{
		long			count = m_line_count;
		line_t**		lines = m_lines;
		
		for( long i = 0; i < count; i++ )
			line_dispose( lines[ i ] );
	
		delete[] m_lines;

		m_line_count = 0;
		m_lines = 0;
	}
}

END_CODE_NAMESPACE
