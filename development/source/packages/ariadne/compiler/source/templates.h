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
//	templates.h				   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#include "compiler.h"
#include "type_t.h"
#include "class_t.h"

BEGIN_COMPILER_NAMESPACE

const int				MAXTPCOUNT = 16;

struct type_t;

struct template_map_t {
	type_t**			bind;
};

struct func_t;

struct functemp_map_t : template_map_t {
	functemp_map_t*		next;
	void*				code;
	func_t*				func;
};

struct class_t;

struct clsstemp_map_t : template_map_t {
	clsstemp_map_t*		next;
	type_t*				clss;
};

struct type_seq_t;

func_t*
instantiate_template_func_explicit(
	func_t*				func,
	type_seq_t*			tmpltype,
	fileref_t*			filepos );

func_t*
instantiate_template_func_implicit(
	func_t*				func );

struct decl_t;

type_t*
instantiate_template_type(
	decl_t*				decl,
	type_seq_t*			tmpltype,
	fileref_t*			filepos );

struct tt_save_t {
	type_t*				addr;
	tt_type_t			data;
};

class tt_replacer_t {
public:
	void				replace(
							type_t**		bind,
							decl_t*			param );
	
	void				replace(
							type_t**		bind,
							scope_t*		scope );

	void				restore();

private:
	tt_save_t			m_save[ MAXTPCOUNT ];
	int					m_savecount;
};

struct tt_class_t : public class_t {
	type_t**			tpbind;
	scope_t*			tpscope;
};

END_COMPILER_NAMESPACE
