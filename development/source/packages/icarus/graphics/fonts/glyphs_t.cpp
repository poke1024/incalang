// ===========================================================================
//	glyphs_t.cp				   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "glyphs_t.h"
#include "input_stream_t.h"
#include "grafport_t.h"

BEGIN_ICARUS_NAMESPACE

glyphs_t::glyphs_t(
	input_stream_t*		stream )
{
	m_ascent = stream->read_u16();
	m_descent = stream->read_u16();
	m_leading = stream->read_u16();
	m_height = m_ascent + m_descent;
		
	for( int i = 0; i < 256; i++ )
		m_chars[ i ] = 0;
		
	try {
		while( true )
		{
			int				index = stream->read_u08();
		
			if( index == '\0' )
				break;
		
			glyph_t*		node = new glyph_t;
			
			m_chars[ index ] = node;
			
			u32				advance = stream->read_u32();
			
			node->advance = advance / (f32)65536.0;

			node->region.deserialize( *stream );
		}
	} catch(...) {
		for( int i = 0; i < 256; i++ )
			if( m_chars[ i ] )
				delete m_chars[ i ];
		throw;
	}
}

glyphs_t::~glyphs_t()
{
	for( int i = 0; i < 256; i++ )
	{
		glyph_t*			node = m_chars[ i ];
		
		if( node )
			delete node;
	}
}

long
glyphs_t::clip_text(
	char*				text,
	long				text_len,
	f64*				text_width_ptr,
	f64					font_height ) const
{
	const f64			max_text_width = *text_width_ptr;
	f64					width;

	width = text_width_limit( text, text_len,
		font_height, max_text_width );

	if( width < 0 )
	{
		const char		ellipsis = 0xC9; //'É';
	
		int				split;
		int				ellipsis_width;
		
		ellipsis_width = text_width( &ellipsis, 1, font_height );

		for( split = ( text_len - 1 ) / 2; split > 0; split-- )
		{
			f64			delta;
		
			width = ellipsis_width;
			
			delta = text_width_limit( text, split,
				font_height, max_text_width - width );
			if( delta < 0 )
				continue;
			
			width += delta;
			if( width > max_text_width )
				continue;
			
			delta = text_width_limit( text + text_len - split,
				split, font_height, max_text_width - width );
			if( delta < 0 )
				continue;

			width += delta;
			if( width > max_text_width )
				continue;

			break;
		}
		
		if( !split )
		{
			*text_width_ptr = 0;
			return 0;
		}
		
		text[ split ] = ellipsis;
		std::memcpy( text + split + 1, text + text_len - split, split );
		text_len = 2 * split + 1;
	}

	*text_width_ptr = width;

	return text_len;
}

f64
glyphs_t::render_text(
	grafport_t*			port,
	f64					x,
	f64					y,
	const char*			text_buf,
	long				text_len,
	f64					text_height,
	u32					text_color ) const
{
	if( text_len <= 0 )
		return x;

	const f64			ratio = text_height / m_height;

	const f64			ascent = m_ascent * ratio;
	
	const char*			buf = text_buf;
	long				count = text_len;
	glyph_t* const*		table = m_chars;
		
	f64					fx = x;
	f64					fy = y - ascent;
	
	do {
		int				c = *(unsigned char*)buf++;
		glyph_t*		node = table[ c ];
		
		if( !node )
			continue;
	
		region_t*		region = &node->region;
		const rect&		rb = region->bounds();
						
		flt_rect		r;
			
		r.left = fx + rb.left * ratio;
		r.right = fx + rb.right * ratio;
		r.top = fy + rb.top * ratio;
		r.bottom = fy + rb.bottom * ratio;
			
		port->render_rgn( region, r, text_color );
		
		fx += node->advance * ratio;
		
	} while( --count );
	
	return fx;
}

f64
glyphs_t::text_width(
	const char*			text_buf,
	long				text_len,
	f64					text_height ) const
{
	if( text_len <= 0 )
		return 0;

	const f64			ratio = text_height / m_height;

	const char*			buf = text_buf;
	long				count = text_len;
	glyph_t* const*		table = m_chars;

	f64					width = 0;

	do {
		int				c = *(unsigned char*)buf++;
		glyph_t*		node = table[ c ];
		
		if( !node )
			continue;
		
		width += node->advance;
	} while( --count );
	
	return width * ratio;
}

long
glyphs_t::pixel_to_char(
	const char*			text_buf,
	long				text_len,
	f64*				offset_ptr,
	f64					text_height ) const
{
	if( text_len <= 0 )
	{
		*offset_ptr = 0;
		return 0;
	}

	const f64			ratio = text_height / m_height;

	const char*			buf = text_buf;
	long				count = text_len;
	glyph_t* const*		table = m_chars;

	f64					width = 0;
	const f64			offset = *offset_ptr / ratio;

	do {
		int				c = *(unsigned char*)buf++;
		glyph_t*		node = table[ c ];
		
		if( !node )
			continue;
		
		const f32		advance = node->advance;
		
		if( width + advance * 0.5 > offset )
		{
			*offset_ptr = width * ratio;
			return buf - text_buf - 1;
		}

		width += advance;		
		
	} while( --count );
	
	*offset_ptr = width * ratio;
	return text_len;
}

f64
glyphs_t::text_width_limit(
	const char*			text_buf,
	long				text_len,
	f64					text_height,
	f64					limit_width ) const
{
	if( !text_len )
		return 0;

	const f64			ratio = text_height / m_height;
	const f64			limit = limit_width / ratio;

	const char*			buf = text_buf;
	long				count = text_len;
	glyph_t* const*		table = m_chars;

	f32					width = 0;

	do {
		int				c = *(unsigned char*)buf++;
		glyph_t*		node = table[ c ];
		
		if( !node )
			continue;
		
		width += node->advance;
		
		if( width > limit )
			return -1;
	} while( --count );
	
	return width * ratio;
}

END_ICARUS_NAMESPACE
