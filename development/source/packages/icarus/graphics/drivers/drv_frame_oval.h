// ===========================================================================
//	drv_frame_oval.h		   ©2002-2002 Bernhard Liebl. All rights reserved.
// ===========================================================================

if( x2 < x1 )
{
	long h = x2;
	x2 = x1;
	x1 = h;
}

if( y2 < y1 )
{
	long h = y2;
	y2 = y1;
	y1 = h;
}

const rect* clip = bmp->clip;
const long clip_left = clip->left;
const long clip_right = clip->right;
const long clip_top = clip->top;
const long clip_bottom = clip->bottom;

if( x2 < clip_left || x1 >= clip_right )
	return;
if( y2 < clip_top || y1 >= clip_bottom )
	return;

const long pitch = bmp->pitch;
u08* base_ptr = (u08*)bmp->pixels;

long irx = ( x2 - x1 ) / 2;
long iry = ( y2 - y1 ) / 2;
long x = x1 + irx;
long y = y1 + iry;
long alpha, beta;
long dy, dx;
long alpha2, alpha4;
long beta2, beta4;
long alphadx, betady;
long ddx, ddy, d;

if( irx < 0 || irx > 32767L || iry < 0 || iry > 32767L )
	return;

#define hline( x0, x1, y )												\
	if( y >= clip_top && y < clip_bottom ) {							\
		u08* pixel_ptr = base_ptr + y * pitch;							\
		if( x0 >= clip_left && x0 < clip_right )						\
			DRV_PUT_PIXEL(												\
				pixel_ptr + x0 * DRV_BYTES_PER_PIXEL, color );			\
		if( x1 >= clip_left && x1 < clip_right )						\
			DRV_PUT_PIXEL(												\
				pixel_ptr + x1 * DRV_BYTES_PER_PIXEL, color );			\
	}
	
long mx0, mx1, my0, my1;
	
#define e_start( _x0, _x1, y )											\
	{ long x0 = _x0; long x1 = _x1;	hline( x0, x1, y );					\
	mx0 = x0; mx1 = x1; my0 = my1 = y; }

#define e_yu()															\
	{ my0--; my1++; 													\
	hline( mx0, mx1, my0 ); hline( mx0, mx1, my1 );  }

#define e_xd()															\
	{ mx0++; mx1--; hline( mx0, mx1, my0 ); hline( mx0, mx1, my1 ); }

#define e_xdyu()														\
	{ mx0++; mx1--; my0--; my1++;										\
	hline( mx0, mx1, my0 ); hline( mx0, mx1, my1 ); }

/* Draw an ellipse with width irx and height iry                         */
/* from a routine by Tim Hogan in Dr. Dobb's Journal May '85 p.40        */
/* Improved by calculating increments incrementally, thus removing all   */
/* multiplies from the loops. These multiplies were very bad since they  */
/* were (long)*(long).                                                   */
/* Written Sept. 7, 1987 by J.D. McDonald (public domain)                */

/* e_start draws left and rightmost pixels on vertical centerline */
/* e_yu draws a pixel in right top quadrant one up from previous  */
/* e_xd draws a pixel in right top quadrant one left from previous*/
/* e_xdyu draws a pixel in right top quadrant up and left from    */
/* previous. e_yu, e_xd, and e_xdyu also draw the corresponding   */
/* pixels in the other three quadrants.                           */
/* c is the color                                                 */

beta = (long) irx *(long) irx;
alpha = (long) iry *(long) iry;

if (alpha == 0L)
	alpha = 1L;
if (beta == 0L)
	beta = 1L;

dy = 0;
dx = irx;
alpha2 = alpha << 1;
alpha4 = alpha2 << 1;
beta2 = beta << 1;
beta4 = beta2 << 1;
alphadx = alpha * dx;
betady = 0;
ddx = alpha4 * (1 - dx);
ddy = beta2 * 3;

d = alpha2 * ((long) (dx - 1) * dx) + alpha + beta2 * (1 - alpha);
e_start(x - dx, x + dx, y);

do {
	if (d >= 0) {
	    d += ddx;
	    dx--;
	    alphadx -= alpha;
	    ddx += alpha4;
	    e_xdyu();
	} else
	    e_yu();
	d += ddy;
	dy++;
	betady += beta;
	ddy += beta4;
} while (alphadx > betady);

d = beta2 * ((long) dy * (dy + 1)) + alpha2 * ((long) dx * (dx - 2) + 1) 
	+ beta * (1 - alpha2);
ddx = alpha2 * (3 - (dx << 1));
ddy = beta4 * (1 + dy);

do {
	if (d <= 0) {
	    d += ddy;
	    ddy += beta4;
	    dy++;
	    e_xdyu();
	} else
	    e_xd();
	d += ddx;
	ddx += alpha4;
	dx--;
} while (dx > 0);

#undef e_start
#undef e_yu
#undef e_xd
#undef e_xdyu
#undef hline
