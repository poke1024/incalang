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
//	polygon_rasterizer_t.h	   ©2001-2001 Bernhard Liebl. All rights reserved.
// ===========================================================================

#pragma once

#include "icarus.h"

BEGIN_ICARUS_NAMESPACE

class region_t;
struct edge_t;

class polygon_rasterizer_t {
public:
						polygon_rasterizer_t(
							region_t&			region );

	void				rasterize(
							long				point_count,
							const point*		points );
protected:
	void				prepare_edge_table(
							long				n );

	void				fill_edges(
							long				n,
							const point*		points );

	edge_t*				update_active(
							edge_t*				active,
							long				cury );

	void				draw_runs(
							edge_t*				active,
							long				cury );

	region_t&			m_region;
	long				m_miny;
};

END_ICARUS_NAMESPACE
