// ===========================================================================
//	typeface_t.cp			   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "typeface_t.h"
#include "glyphs_t.h"

BEGIN_ICARUS_NAMESPACE

typeface_t::typeface_t()
{
	m_font = 0;
	m_height = 0;
	m_style = 0;

	m_ascent = 0;
	m_descent = 0;
	m_leading = 0;

	m_glyphs = 0;
}

typeface_t::typeface_t(
	font_t*				font,
	f64					height,
	text_style_t		style )
{
	m_font = font;
	m_height = height;
	m_style = style;
	
	update_glyphs();
}

void
typeface_t::set(
	const typeface_t*	typeface )
{
	m_font = typeface->m_font;
	m_height = typeface->m_height;
	m_style = typeface->m_style;
	
	update_glyphs();
}

void
typeface_t::set_font(
	font_t*				font )
{
	if( m_font == font )
		return;

	if( m_font )
		m_font->unrefer();

	m_font = font;
	if( font )
		font->refer();

	update_glyphs();
}

void
typeface_t::set_height(
	f64					height )
{
	if( height == m_height )
		return;
		
	m_height = height;
		
	update_info();
}

void
typeface_t::set_style(
	text_style_t		style )
{
	if( style == m_style )
		return;
		
	m_style = style;
	
	update_glyphs();
}

f64
typeface_t::text_width(
	const char*			text_buf,
	long				text_len ) const
{
	glyphs_t*			glyphs = m_glyphs;
	
	if( !glyphs )
		return 0;
		
	return glyphs->text_width( text_buf, text_len, m_height );
}

long
typeface_t::clip_text(
	char*				text_buf,
	long				text_len,
	f64*				text_width_ptr ) const
{
	glyphs_t*			glyphs = m_glyphs;
	
	if( !glyphs )
		return 0;
		
	return glyphs->clip_text( text_buf, text_len,
		text_width_ptr, m_height );
}

long
typeface_t::pixel_to_char(
	const char*			text_buf,
	long				text_len,
	f64*				offset_ptr ) const
{
	glyphs_t*			glyphs = m_glyphs;
	
	if( !glyphs )
		return 0;
		
	return glyphs->pixel_to_char( text_buf, text_len,
		offset_ptr, m_height );
}

f64
typeface_t::render_text(
	grafport_t*			port,
	f64					x,
	f64					y,
	const char*			text_buf,
	long				text_len,
	u32					text_color ) const
{
	glyphs_t*			glyphs = m_glyphs;
	
	if( !glyphs )
		return x;
		
	return glyphs->render_text( port, x, y, text_buf,
		text_len, m_height, text_color );
}

void
typeface_t::update_glyphs()
{
	if( m_font )
		m_glyphs = m_font->glyphs( m_style );
	else
		m_glyphs = 0;
	
	update_info();
}

void
typeface_t::update_info()
{
	glyphs_t*			glyphs = m_glyphs;

	if( !glyphs )
	{
		m_ascent = 0;
		m_descent = 0;
		m_leading = 0;
	}
	else
	{
		const int		height = m_height;
	
		m_ascent = glyphs->ascent( height );
		m_descent = glyphs->descent( height );
		m_leading = glyphs->leading( height );
	}
}

END_ICARUS_NAMESPACE
