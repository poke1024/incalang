// ===========================================================================
//	glyphs_t.h				   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "region_t.h"

BEGIN_ICARUS_NAMESPACE

class input_stream_t;
class grafport_t;

struct glyph_t {
	region_t			region;
	f32					advance;
};

class glyphs_t {
protected:
	friend class		font_t;
	friend class		typeface_t;

						glyphs_t(
							input_stream_t*		stream );

	virtual				~glyphs_t();
							
	f64					ascent(
							f64					font_height ) const;

	f64					descent(
							f64					font_height ) const;

	f64					leading(
							f64					font_height ) const;
							
	long				clip_text(
							char*				text_buf,
							long				text_len,
							f64*				text_width_ptr,
							f64					font_height ) const;

	f64					text_width(
							const char*			text_buf,
							long				text_len,
							f64					text_height ) const;
							
	long				pixel_to_char(
							const char*			text_buf,
							long				text_len,
							f64*				offset_ptr,
							f64					text_height ) const;

	f64					render_text(
							grafport_t*			port,
							f64					x,
							f64					y,
							const char*			text_buf,
							long				text_len,
							f64					text_height,
							u32					text_color ) const;

	f64					text_width_limit(
							const char*			text_buf,
							long				text_len,
							f64					text_height,
							f64					limit_width ) const;

private:
	u16					m_ascent;
	u16					m_descent;
	u16					m_leading;
	u16					m_height;

	glyph_t*			m_chars[ 256 ];
};

inline f64
glyphs_t::ascent(
	f64					font_height ) const
{
	return ( font_height / m_height ) * m_ascent;
}

inline f64
glyphs_t::descent(
	f64					font_height ) const
{
	return ( font_height / m_height ) * m_descent;
}

inline f64
glyphs_t::leading(
	f64					font_height ) const
{
	return ( font_height / m_height ) * m_leading;
}

END_ICARUS_NAMESPACE
