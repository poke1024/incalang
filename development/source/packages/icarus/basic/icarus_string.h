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
//	icarus_string.h		 	   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#ifndef ICARUS_STRING_H
#define ICARUS_STRING_H

//#pragma once

BEGIN_ICARUS_NAMESPACE

class istring {
public:
						istring();
						
						istring(
							const char*			s );

						istring(
							const char*			s,
							isize_t				length );

						istring(
							const istring&		s );
						
						~istring();
						
	isize_t				length() const;
						
	void				operator=(
							const char*			s );

	void				operator=(
							const istring&		s );
	
	void				assign(
							const char*			s );

	void				assign(
							const istring&		s );
	
	void				assign(
							const char*			s,
							isize_t				length );
	
	void				append(
							const char*			s );

	void				append(
							const istring&		s );

	void				append(
							const char*			s,
							isize_t				length );

	void				append(
							isize_t				count,
							char				c );
	
	void				insert(
							long				pos,
							const char*			s );

	void				insert(
							long				pos,
							const char*			s,
							isize_t				length );
	
	void				erase(
							long				from );
	
	istring				substr(
							long				from ) const;

	istring				substr(
							long				from,
							long				n ) const;
	
	long				rfind(
							const char*			pattern ) const;

	long				rfind(
							const char*			pattern,
							long				from ) const;
	
	void				reserve(
							isize_t				length );
							
	void				clear();
	
	const char*			c_str() const;
	
	const char*			data() const;
							
	char&				operator[](
							long				index ) const;
							
private:
	void				init();

	char*				m_data;
	isize_t				m_length;
	isize_t				m_alloc;
};

inline isize_t
istring::length() const
{
	return m_length;
}

inline const char*
istring::c_str() const
{
	return m_data;
}

inline const char*
istring::data() const
{
	return m_data;
}

inline char&
istring::operator[](
	long				index ) const
{
	if( index < 0 || index >= (long)m_length )
	{
#if ICARUS_DEBUG
		throw_icarus_error( "illegal string access" );
#endif
		static char		c;
		c = '\0';
		return c;
	}

	return m_data[ index ];
}

END_ICARUS_NAMESPACE

#endif

