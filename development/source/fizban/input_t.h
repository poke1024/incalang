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
//	input_t.h				   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "fizban.h"
#include <dinput.h>

BEGIN_FIZBAN_NAMESPACE

class input_device_t {
public:
						input_device_t(
							LPDIRECTINPUT8		lpdi );

	virtual				~input_device_t();

protected:
	LPDIRECTINPUTDEVICE8	m_device;
};

// ---------------------------------------------------------------------------

class mouse_device_t : public input_device_t {
public:
						mouse_device_t(
							LPDIRECTINPUT8		lpdi,
							HWND				hWnd );

	void				update();

	LONG				x() const;

	LONG				y() const;

	LONG				z() const;

	bool				button(
							int					index ) const;

private:
	DIMOUSESTATE		m_state;
};

inline LONG
mouse_device_t::x() const
{
	return m_state.lX;
}

inline LONG
mouse_device_t::y() const
{
	return m_state.lY;
}

inline LONG
mouse_device_t::z() const
{
	return m_state.lZ;
}

inline bool
mouse_device_t::button(
	int					index ) const
{
	return ( m_state.rgbButtons[ index ] & 0x80 ) != 0;
}

// ---------------------------------------------------------------------------

class keyboard_device_t : public input_device_t {
public:
						keyboard_device_t(
							LPDIRECTINPUT8		lpdi,
							HWND				hWnd );

	void				update();

	void				flush();

	bool				key_down(
							int					index ) const;

private:
	UCHAR				m_state[ 256 ];
};

inline bool
keyboard_device_t::key_down(
	int					index ) const
{
	return ( m_state[ index ] & 0x80 ) != 0;
}

// ---------------------------------------------------------------------------

void
rt_input_init(
	HWND				hWnd );

void
rt_input_cleanup();

void
rt_input_enable();

void
rt_input_disable();

void
rt_input_update();

bool
rt_key_down(
	int					key );

long
rt_mouse_x();

long
rt_mouse_y();

long
rt_mouse_z();

bool
rt_mouse_button(
	int					button );

END_FIZBAN_NAMESPACE
