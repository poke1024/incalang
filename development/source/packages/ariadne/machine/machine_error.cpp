// ===========================================================================
//	machine_error.cp		   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "machine_error.h"
#include "bytecode_func_t.h"
#include "fiber_t.h"

BEGIN_MACHINE_NAMESPACE

/*vm_runtime_error_t::vm_runtime_error_t(
	int					err,
	func_t*				func,
	long				offset ) :
	
	runtime_error( "vm error" ),
	
	m_err( err ),
	m_func( func ),
	m_offset( offset )
{
}

const char*
vm_runtime_error_t::what() const
{
	return error_to_string( m_err );
}

long
vm_runtime_error_t::where()
{
	bytecode_func_t*	bfunc;
	
	bfunc = dynamic_cast<bytecode_func_t*>( m_func );
	if( !bfunc )
		return -1;
		
	return bfunc->find_source_location( m_offset );
}

void
throw_vm_runtime_error(
	int					err,
	func_t*				func,
	long				offset )
{
	throw vm_runtime_error_t(
		err, func, offset );
}*/

void
error_string(
	const stop_state_t&	state,
	istring&			text )
{
	switch( state.err )
	{
		case ERR_NONE:
			text = "";
			break;
	
		case ERR_ILLEGAL_ACCESS:
			text = "illegal memory access";
			break;

		case ERR_ILLEGAL_STRING_INDEX:
			text = "illegal string index";
			break;

		case ERR_ILLEGAL_ARRAY_INDEX:
			text = "illegal array index";
			break;

		case ERR_ILLEGAL_PARAMETER:
			text = "illegal parameter";
			break;

		case ERR_STACK_OVERFLOW:
			text = "stack overflow";
			break;
			
		case ERR_OUT_OF_MEMORY:
			text = "out of memory";
			break;

		case ERR_NO_G2WORLD_SET:
			text = "no 2d graphics context set";
			break;
			
		case ERR_NO_G3WORLD_SET:
			text = "no 3d graphics context set";
			break;

		case ERR_ILLEGAL_PIXEL_FORMAT:
			text = "illegal pixel format";
			break;
			
		case ERR_EMPTY_PIXMAP:
			text = "trying to draw into an empty pixmap";
			break;

		case ERR_ARITHMETIC:
			text = "arithmetic error";
			break;

		case ERR_NON_RT_FUNCTION:
			text = "non real-time function called in real-time mode";
			break;

		case ERR_FILE_NOT_OPEN:
			text = "file not open";
			break;

		case ERR_IO:
			text = "io error";
			break;

		case ERR_UNKNOWN:
			text = "unknown error";
			break;

		case ERR_EXCEPTION:
			text = state.errtext;
			break;

		default:
			text = "internal error";
			break;
	}
	
#if ICARUS_SAFE && 0
	char				buf[ 64 + 1 ];
	
	isnprintf( buf, 64, " [%lx]",
		state.func->get_code_offset(
			state.code ) );
	text.append( buf );
#endif
}

END_MACHINE_NAMESPACE
