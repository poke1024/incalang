// ===========================================================================
//	typeface_t.h			   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "font_t.h"

BEGIN_ICARUS_NAMESPACE

class glyphs_t;

class typeface_t {
public:
						typeface_t();
						
						typeface_t(
							font_t*				font,
							f64					height,
							text_style_t		style = 0 );
							
	void				set(
							const typeface_t*	typeface );

	void				set_font(
							font_t*				font );

	void				set_height(
							f64					height );
	
	void				set_style(
							text_style_t		style );
	
	font_t*				font() const;
	
	f64					height() const;
	
	text_style_t		style() const;
	
	f64					ascent() const;

	f64					descent() const;

	f64					leading() const;
	
	f64					text_width(
							const char*			text_buf,
							long				text_len ) const;

	long				clip_text(
							char*				text_buf,
							long				text_len,
							f64*				text_width_ptr ) const;

	long				pixel_to_char(
							const char*			text_buf,
							long				text_len,
							f64*				offset_ptr ) const;
	
protected:
	friend class		grafport_t;
	
	f64					render_text(
							grafport_t*			port,
							f64					x,
							f64					y,
							const char*			text_buf,
							long				text_len,
							u32					text_color ) const;
	
private:
	void				update_glyphs();
	
	void				update_info();

	font_t*				m_font;
	f64					m_height;
	text_style_t		m_style;
	
	f64					m_ascent;
	f64					m_descent;
	f64					m_leading;
	
	glyphs_t*			m_glyphs;
};

// ---------------------------------------------------------------------------

inline font_t*
typeface_t::font() const
{
	return m_font;
}
	
inline f64
typeface_t::height() const
{
	return m_height;
}
	
inline text_style_t
typeface_t::style() const
{
	return m_style;
}

inline f64
typeface_t::ascent() const
{
	return m_ascent;
}

inline f64
typeface_t::descent() const
{
	return m_descent;
}

inline f64
typeface_t::leading() const
{
	return m_leading;
}

END_ICARUS_NAMESPACE
