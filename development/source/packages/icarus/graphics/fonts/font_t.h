// ===========================================================================
//	font_t.h				   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "icarus.h"
#include "shared_t.h"

BEGIN_ICARUS_NAMESPACE

enum {
	text_style_normal							= 0,
	text_style_italic							= 1,
	text_style_bold								= 2,
	text_style_underlined						= 4,
	
	text_style_mask								= 7
};

typedef u16				text_style_t;

class input_stream_t;
class glyphs_t;

class font_t : public shared_t {
public:
						font_t(
							input_stream_t*		stream );
							
	glyphs_t*			glyphs(
							text_style_t		style ) const;
	
	static font_t*		load(
							const char*			path );
							
protected:
	virtual				~font_t();

	u32					m_ref_count;

	glyphs_t*			m_glyphs[ text_style_mask + 1 ];
};

END_ICARUS_NAMESPACE
