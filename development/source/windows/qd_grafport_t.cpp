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
//	grafport_t.cp			   ©2000-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "qd_grafport_t.h"

BEGIN_ICARUS_NAMESPACE

inline COLORREF
col2rgb(
	u32					color )
{
	return RGB(
		( ( color >> 0 ) & 0xff ),
		( ( color >> 8 ) & 0xff ),
		( ( color >> 16 ) & 0xff ) );
}

grafport_t::grafport_t(
	CDC*				hdc,
	const rect&			clip ) :
	
	m_hdc( hdc ),
	m_raw_clip_rect( clip )
{
	m_brush = new CBrush;
	m_hilite_text = false;
}

grafport_t::~grafport_t()
{
	delete m_brush;
}

void
grafport_t::fill_rect(
	long				x0,
	long				y0,
	long				x1,
	long				y1,
	u32					color )
{
	x0 -= m_origin.h;
	y0 -= m_origin.v;
	x1 -= m_origin.h;
	y1 -= m_origin.v;

	RECT				r;
	
	r.left = x0;
	r.top = y0;
	r.right = x1 + 1;
	r.bottom = y1 + 1;
	
	if( m_brush )
		delete m_brush;
	m_brush = new CBrush;

	m_brush->CreateSolidBrush( col2rgb( color ) );
	m_hdc->FillRect( &r, m_brush );
}

void
grafport_t::fill_oval(
	long				x0,
	long				y0,
	long				x1,
	long				y1,
	u32					color )
{
	x0 -= m_origin.h;
	y0 -= m_origin.v;
	x1 -= m_origin.h;
	y1 -= m_origin.v;
	
	COLORREF			wcol = col2rgb( color );
	HBRUSH				old_brush;

	old_brush = (HBRUSH)SelectObject(
		m_hdc->m_hDC, GetStockObject( DC_BRUSH ) );
	SetDCBrushColor( m_hdc->m_hDC, wcol );
	//m_hdc->SetDCPenColor( wcol );
	Ellipse( m_hdc->m_hDC, x0, y0, x1, y1 );
	SelectObject( m_hdc->m_hDC, old_brush );
}

void
grafport_t::vline(
	long				x0,
	long				y0,
	long				y1,
	u32					color )
{
	x0 -= m_origin.h;
	y0 -= m_origin.v;
	y1 -= m_origin.v;

	m_hdc->SetDCPenColor( col2rgb( color ) );

	m_hdc->MoveTo( x0, y0 );
	m_hdc->LineTo( x0, y1 - 1 );
}

f64
grafport_t::draw_text(
	f64					x,
	f64					y,
	const char*			text,
	long				length,
	const typeface_t*	typeface,
	u32					color )
{
	x -= m_origin.h;
	y -= m_origin.v;

	if( not m_hilite_text )
		m_hdc->SetTextColor( col2rgb( color ) );

	SetBkMode( m_hdc->m_hDC, TRANSPARENT );

	m_hdc->MoveTo( x, y - typeface->ascent() );
	m_hdc->SetTextAlign( TA_UPDATECP );
	m_hdc->TextOut( 0, 0, text, length );

	return m_hdc->GetCurrentPosition().x + m_origin.h;
}

u32
grafport_t::selection_color() const
{
	DWORD col = GetSysColor( COLOR_HIGHLIGHT );
	int r = GetRValue( col );
	int g = GetGValue( col );
	int b = GetBValue( col );
	return make_color( 0, r, g, b );

	//return make_color( 0, 0, 0, 255 );
}

CDC*
grafport_t::hdc() const
{
	return m_hdc;
}

void
grafport_t::set_origin(
	const point&		p )
{
	m_origin = p;
}

void
grafport_t::set_hilite_text(
	bool				hilite )
{
	if( hilite )
		m_hdc->SetTextColor( GetSysColor( COLOR_HIGHLIGHTTEXT ) );

	m_hilite_text = hilite;
}

END_ICARUS_NAMESPACE
