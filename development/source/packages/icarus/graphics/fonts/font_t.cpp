// ===========================================================================
//	font_t.cp				   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "font_t.h"
#include "glyphs_t.h"
#include "input_stream_t.h"
#include "file_stream_t.h"

BEGIN_ICARUS_NAMESPACE

font_t::font_t(
	input_stream_t*		stream )
{
	if( stream->read_u32() != 'ifnt' )
		throw_icarus_error( "font data expected" );

	if( stream->read_u16() != 0x0100 )
		throw_icarus_error( "unsupported font version" );	
	
	for( int i = 0; i <= text_style_mask; i++ )
		m_glyphs[ i ] = 0;
	
	int					num_styles = stream->read_u16();
	
	for( int i = 0; i < num_styles; i++ )
	{
		int				style = stream->read_u16();
		
		if( style < 0 || style > text_style_mask )
			throw_icarus_error( "invalid text style" );
		
		glyphs_t*		glyphs = new glyphs_t( stream );
		m_glyphs[ style ] = glyphs;
	}
	
	if( !m_glyphs[ text_style_normal ] )
		throw_icarus_error( "font has no normal text style" );
}

font_t::~font_t()
{
	for( int i = 0; i <= text_style_mask; i++ )
		if( m_glyphs[ i ] )
			delete m_glyphs[ i ];
}

font_t*
font_t::load(
	const char*			path )
{
	file_stream_t		stream( path, "rb" );

	return new font_t( &stream );
}

glyphs_t*
font_t::glyphs(
	text_style_t		style ) const
{
	if( style < 0 || style > text_style_mask )
		throw_icarus_error( "invalid text style" );

	glyphs_t*			glyphs = m_glyphs[ style ];
	
	if( !glyphs )
		glyphs = m_glyphs[ text_style_normal ];
		
	return glyphs;
}

END_ICARUS_NAMESPACE
