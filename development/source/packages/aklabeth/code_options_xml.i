// ===========================================================================
//	code_options_xml.i		   ©2001-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "element_t.h"
#include "type_element_t.h"

#define BEGIN_XML_NAMESPACE\
	namespace code_options_xml {

#define END_XML_NAMESPACE }

BEGIN_CODE_NAMESPACE

BEGIN_XML_NAMESPACE

USING_IXELMIX_NAMESPACE
using namespace std;

class root_t : public element_t {
public:
							root_t(
								options_t*			options );

	virtual element_t*		open(
								buffer_t&			name,
								attribute_t*		attributes );
								
private:
	options_t*				m_options;
};

class body_t : public element_t {
public:
							body_t(
								options_t*			options );
	
	virtual					~body_t();

	virtual element_t*		open(
								buffer_t&			name,
								attribute_t*		attributes );

private:
	options_t*				m_options;
	istring					m_comma;
	istring					m_semicolon;
};

// ---------------------------------------------------------------------------

root_t::root_t(
	options_t*				options ) :

	m_options( options )
{
}

element_t*
root_t::open(
	buffer_t&			name,
	attribute_t*		attributes )
{
	if( name == "body" && !attributes )
		return new body_t( m_options );
	
	return 0;
}

body_t::body_t(
	options_t*				options ) :

	m_options( options )
{
	options->space_operators = true;
	options->space_statements = false;
	options->space_brackets = true;
	options->space_parentheses = true;
	options->space_delimiters = true;
	options->space_clamps = false;
	options->space_pointer = true;
	options->bind_pointer_left = false;
	options->declaration_tabs = 0;
	options->access_modifier_tabs = -1;
	options->switch_tabs = 1;
	
	m_comma = ", ";
	m_semicolon = "; ";
}

body_t::~body_t()
{
	if( m_comma.length() <= 3 )
		istrcpy( m_options->comma, m_comma.c_str() );

	if( m_semicolon.length() <= 3 )
		istrcpy( m_options->semicolon, m_semicolon.c_str() );
}

element_t*
body_t::open(
	buffer_t&			name,
	attribute_t*		attributes )
{
	if( attributes )
		return 0;

	if( name == "space_operators" )
		return new bool_element_t( m_options->space_operators );

	if( name == "space_statements" )
		return new bool_element_t( m_options->space_statements );

	if( name == "space_brackets" )
		return new bool_element_t( m_options->space_brackets );

	if( name == "space_parentheses" )
		return new bool_element_t( m_options->space_parentheses );

	if( name == "space_delimiters" )
		return new bool_element_t( m_options->space_delimiters );

	if( name == "space_clamps" )
		return new bool_element_t( m_options->space_clamps );

	if( name == "space_pointer" )
		return new bool_element_t( m_options->space_pointer );

	if( name == "bind_pointer_left" )
		return new bool_element_t( m_options->bind_pointer_left );

	/*if( name == "declaration_tabs" )
		return new s32_element_t( m_options->declaration_tabs );

	if( name == "access_modifier_tabs" )
		return new s32_element_t( m_options->access_modifier_tabs );

	if( name == "switch_tabs" )
		return new s32_element_t( m_options->switch_tabs );*/

	if( name == "comma" )
		return new str_element_t( m_comma );

	if( name == "semicolon" )
		return new str_element_t( m_semicolon );
	
	return 0;
}

END_XML_NAMESPACE

END_CODE_NAMESPACE
