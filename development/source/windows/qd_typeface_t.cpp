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
//	typeface_t.cp			   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "qd_typeface_t.h"

BEGIN_ICARUS_NAMESPACE

typeface_t::typeface_t()
{
	m_ascent = 0;
	m_descent = 0;
	m_leading = 0;
	m_height = 0;
}

typeface_t::~typeface_t()
{
}

void
typeface_t::set(
	const typeface_t*	typeface )
{
	m_ascent = typeface->m_ascent;
	m_descent = typeface->m_descent;
	m_leading = typeface->m_leading;
	m_height = typeface->m_height;
	memcpy(
		m_width, typeface->m_width,
		256 * sizeof( FLOAT ) );
}

f64
typeface_t::text_width(
	const char*			text_buf,
	long				text_len ) const
{
	if( text_len <= 0 )
		return 0;

	const char*			buf = text_buf;
	long				count = text_len;

	f64					width = 0;

	do {
		int				c = *(unsigned char*)buf++;
		const f32		advance = m_width[ c ];

		width += advance;
	} while( --count );
	
	return width;
}

long
typeface_t::pixel_to_char(
	const char*			text_buf,
	long				text_len,
	f64*				offset_ptr ) const
{
	if( text_len <= 0 )
	{
		*offset_ptr = 0;
		return 0;
	}

	const char*			buf = text_buf;
	long				count = text_len;

	f64					width = 0;
	const f64			offset = *offset_ptr;

	do {
		int				c = *(unsigned char*)buf++;
		const f32		advance = m_width[ c ];
				
		if( width + advance > offset )
		{
			*offset_ptr = width;
			return buf - text_buf - 1;
		}

		width += advance;		
		
	} while( --count );
	
	*offset_ptr = width;
	return text_len;
}

void
typeface_t::init(
	HDC					hdc )
{
	TEXTMETRIC			metric;

	if( not GetTextMetrics( hdc, &metric ) )
		throw_icarus_error( "cannot retrieve font metrics" );

	m_ascent = metric.tmAscent;
	m_descent = metric.tmDescent;
	m_leading = metric.tmExternalLeading;
	m_height = metric.tmHeight;

	/*if( not GetCharWidthFloat( hdc, 0, 255, m_width ) )
		throw_icarus_error( "could not retrieve char widths" );*/

	ABCFLOAT			abcfloat[ 256 ];

	GetCharABCWidthsFloat( hdc, 0, 255, abcfloat );

	for( int i = 0; i < 256; i++ )
		m_width[ i ] =
			abcfloat[ i ].abcfA +
			abcfloat[ i ].abcfB +
			abcfloat[ i ].abcfC;

	/*int					intwidth[ 256 ];
	if( not GetCharWidthI( hdc, 0, 256, NULL, intwidth ) )
		throw_icarus_error( "could not retrieve char widths" );

	for( int i = 0; i < 256; i++ )
		m_width[ i ] = intwidth[ i ];*/
}

END_ICARUS_NAMESPACE
