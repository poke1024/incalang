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
//	scrap_t.cp	 			   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "scrap_t.h"
#include "os_core.h"
#include "osglue.h"

#include "text_clip_t.h"

BEGIN_OSWRAP_NAMESPACE

clip_t::~clip_t()
{
}

// ---------------------------------------------------------------------------

static scrap_t*			s_instance = 0;

scrap_t::scrap_t() :

	m_clips( 0 )
{
}

scrap_t::~scrap_t()
{
	clear();
}

void
scrap_t::clear()
{
	node_t*				node = m_clips;
	node_t*				next;
	
	while( node )
	{
		next = node->next;
		delete node->clip;
		delete node;
		node = next;
	}
}

clip_t*
scrap_t::put(
	clip_t*				clip )
{
	node_t*				node = m_clips;
	clip_tag_t			tag = clip->tag();
	u64					timestamp = os_time_millis();
	
	istring				text;
	
	clip->to_text( text );
	
	if( text.length() > 0 )
		to_pasteboard( text.data(), text.length() );
	
	while( node )
	{
		if( node->tag == tag )
		{
			delete node->clip;
			node->clip = clip;
			node->timestamp = timestamp;
			return clip;
		}
		
		node = node->next;
	}
	
	node = new node_t;
	node->next = m_clips;
	node->tag = tag;
	node->clip = clip;
	node->timestamp = timestamp;
	
	m_clips = node;
	
	return clip;
}

clip_t*
scrap_t::get(
	clip_tag_t			tag )
{
	node_t*				node = m_clips;
	
	while( node )
	{
		if( node->tag == tag )
			return node->clip;
			
		node = node->next;
	}
	
	return 0;
}

clip_t*
scrap_t::get(
	const clip_tag_t*	tags,
	int					count )
{
	const char*			text = from_pasteboard();

	node_t*				node = m_clips;
	node_t*				cand = 0;
	
	while( node )
	{
		clip_tag_t		tag = node->tag;
		bool			found = false;
	
		for( int i = 0; i < count; i++ )
			if( tag == tags[ i ] )
			{
				found = true;
				break;
			}
	
		if( found )
		{
			if( cand == 0 )
				cand = node;
			else if( node->timestamp > cand->timestamp )
				cand = node;
		}
			
		node = node->next;
	}
	
	if( cand )
	{
		istring			block;
	
		cand->clip->to_text( block );
		if( istrcmp( block.c_str(), text ) == 0 )
			return cand->clip;
	}
	
	if( text && text[ 0 ] )
	{
		istring			s;
		long			length;
		
		s = text;
		length = s.length();
		
		for( long i = 0; i < length; i++ )
			if( s[ i ] == '\r' )
				s[ i ] = '\n';
	
		return put( new text_clip_t( s.data(), length) );
	}
	
	return 0;
}

scrap_t*
scrap_t::instance()
{
	if( not s_instance )
		s_instance = new scrap_t;

	return s_instance;
}

END_OSWRAP_NAMESPACE
