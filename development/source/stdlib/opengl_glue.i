OPENGL_FUNCTION(glue_glAccum)
GLenum op=f.pop_int();
GLfloat value=f.pop_float();
(glAccum(op,value));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glAlphaFunc)
GLenum func=f.pop_int();
GLclampf ref=f.pop_float();
(glAlphaFunc(func,ref));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glAreTexturesResident)
GLsizei n=f.pop_int();
const GLuint* textures=(const GLuint*)f.pop_ptr();
GLboolean* residences=(GLboolean*)f.pop_ptr();
f.push_byte(glAreTexturesResident(n,textures,residences));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glArrayElement)
GLint i=f.pop_int();
(glArrayElement(i));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glBegin)
GLenum mode=f.pop_int();
(glBegin(mode));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glBindTexture)
GLenum target=f.pop_int();
GLuint texture=f.pop_int();
(glBindTexture(target,texture));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glBitmap)
GLsizei width=f.pop_int();
GLsizei height=f.pop_int();
GLfloat xorig=f.pop_float();
GLfloat yorig=f.pop_float();
GLfloat xmove=f.pop_float();
GLfloat ymove=f.pop_float();
const GLubyte* bitmap=(const GLubyte*)f.pop_ptr();
(glBitmap(width,height,xorig,yorig,xmove,ymove,bitmap));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glBlendFunc)
GLenum sfactor=f.pop_int();
GLenum dfactor=f.pop_int();
(glBlendFunc(sfactor,dfactor));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glCallList)
GLuint list=f.pop_int();
(glCallList(list));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glCallLists)
GLsizei n=f.pop_int();
GLenum type=f.pop_int();
const GLvoid* lists=(const GLvoid*)f.pop_ptr();
(glCallLists(n,type,lists));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glClear)
GLbitfield mask=f.pop_int();
(glClear(mask));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glClearAccum)
GLfloat red=f.pop_float();
GLfloat green=f.pop_float();
GLfloat blue=f.pop_float();
GLfloat alpha=f.pop_float();
(glClearAccum(red,green,blue,alpha));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glClearColor)
GLclampf red=f.pop_float();
GLclampf green=f.pop_float();
GLclampf blue=f.pop_float();
GLclampf alpha=f.pop_float();
(glClearColor(red,green,blue,alpha));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glClearDepth)
GLclampd depth=f.pop_double();
(glClearDepth(depth));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glClearIndex)
GLfloat c=f.pop_float();
(glClearIndex(c));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glClearStencil)
GLint s=f.pop_int();
(glClearStencil(s));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glClipPlane)
GLenum plane=f.pop_int();
const GLdouble* equation=(const GLdouble*)f.pop_ptr();
(glClipPlane(plane,equation));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor3b)
GLbyte red=f.pop_byte();
GLbyte green=f.pop_byte();
GLbyte blue=f.pop_byte();
(glColor3b(red,green,blue));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor3bv)
const GLbyte* v=(const GLbyte*)f.pop_ptr();
(glColor3bv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor3d)
GLdouble red=f.pop_double();
GLdouble green=f.pop_double();
GLdouble blue=f.pop_double();
(glColor3d(red,green,blue));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor3dv)
const GLdouble* v=(const GLdouble*)f.pop_ptr();
(glColor3dv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor3f)
GLfloat red=f.pop_float();
GLfloat green=f.pop_float();
GLfloat blue=f.pop_float();
(glColor3f(red,green,blue));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor3fv)
const GLfloat* v=(const GLfloat*)f.pop_ptr();
(glColor3fv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor3i)
GLint red=f.pop_int();
GLint green=f.pop_int();
GLint blue=f.pop_int();
(glColor3i(red,green,blue));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor3iv)
const GLint* v=(const GLint*)f.pop_ptr();
(glColor3iv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor3s)
GLshort red=f.pop_short();
GLshort green=f.pop_short();
GLshort blue=f.pop_short();
(glColor3s(red,green,blue));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor3sv)
const GLshort* v=(const GLshort*)f.pop_ptr();
(glColor3sv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor3ub)
GLubyte red=f.pop_byte();
GLubyte green=f.pop_byte();
GLubyte blue=f.pop_byte();
(glColor3ub(red,green,blue));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor3ubv)
const GLubyte* v=(const GLubyte*)f.pop_ptr();
(glColor3ubv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor3ui)
GLuint red=f.pop_int();
GLuint green=f.pop_int();
GLuint blue=f.pop_int();
(glColor3ui(red,green,blue));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor3uiv)
const GLuint* v=(const GLuint*)f.pop_ptr();
(glColor3uiv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor3us)
GLushort red=f.pop_short();
GLushort green=f.pop_short();
GLushort blue=f.pop_short();
(glColor3us(red,green,blue));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor3usv)
const GLushort* v=(const GLushort*)f.pop_ptr();
(glColor3usv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor4b)
GLbyte red=f.pop_byte();
GLbyte green=f.pop_byte();
GLbyte blue=f.pop_byte();
GLbyte alpha=f.pop_byte();
(glColor4b(red,green,blue,alpha));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor4bv)
const GLbyte* v=(const GLbyte*)f.pop_ptr();
(glColor4bv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor4d)
GLdouble red=f.pop_double();
GLdouble green=f.pop_double();
GLdouble blue=f.pop_double();
GLdouble alpha=f.pop_double();
(glColor4d(red,green,blue,alpha));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor4dv)
const GLdouble* v=(const GLdouble*)f.pop_ptr();
(glColor4dv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor4f)
GLfloat red=f.pop_float();
GLfloat green=f.pop_float();
GLfloat blue=f.pop_float();
GLfloat alpha=f.pop_float();
(glColor4f(red,green,blue,alpha));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor4fv)
const GLfloat* v=(const GLfloat*)f.pop_ptr();
(glColor4fv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor4i)
GLint red=f.pop_int();
GLint green=f.pop_int();
GLint blue=f.pop_int();
GLint alpha=f.pop_int();
(glColor4i(red,green,blue,alpha));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor4iv)
const GLint* v=(const GLint*)f.pop_ptr();
(glColor4iv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor4s)
GLshort red=f.pop_short();
GLshort green=f.pop_short();
GLshort blue=f.pop_short();
GLshort alpha=f.pop_short();
(glColor4s(red,green,blue,alpha));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor4sv)
const GLshort* v=(const GLshort*)f.pop_ptr();
(glColor4sv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor4ub)
GLubyte red=f.pop_byte();
GLubyte green=f.pop_byte();
GLubyte blue=f.pop_byte();
GLubyte alpha=f.pop_byte();
(glColor4ub(red,green,blue,alpha));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor4ubv)
const GLubyte* v=(const GLubyte*)f.pop_ptr();
(glColor4ubv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor4ui)
GLuint red=f.pop_int();
GLuint green=f.pop_int();
GLuint blue=f.pop_int();
GLuint alpha=f.pop_int();
(glColor4ui(red,green,blue,alpha));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor4uiv)
const GLuint* v=(const GLuint*)f.pop_ptr();
(glColor4uiv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor4us)
GLushort red=f.pop_short();
GLushort green=f.pop_short();
GLushort blue=f.pop_short();
GLushort alpha=f.pop_short();
(glColor4us(red,green,blue,alpha));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColor4usv)
const GLushort* v=(const GLushort*)f.pop_ptr();
(glColor4usv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColorMask)
GLboolean red=f.pop_byte();
GLboolean green=f.pop_byte();
GLboolean blue=f.pop_byte();
GLboolean alpha=f.pop_byte();
(glColorMask(red,green,blue,alpha));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColorMaterial)
GLenum face=f.pop_int();
GLenum mode=f.pop_int();
(glColorMaterial(face,mode));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glColorPointer)
GLint size=f.pop_int();
GLenum type=f.pop_int();
GLsizei stride=f.pop_int();
const GLvoid* pointer=(const GLvoid*)f.pop_ptr();
(glColorPointer(size,type,stride,pointer));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glCopyPixels)
GLint x=f.pop_int();
GLint y=f.pop_int();
GLsizei width=f.pop_int();
GLsizei height=f.pop_int();
GLenum type=f.pop_int();
(glCopyPixels(x,y,width,height,type));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glCopyTexImage1D)
GLenum target=f.pop_int();
GLint level=f.pop_int();
GLenum internalFormat=f.pop_int();
GLint x=f.pop_int();
GLint y=f.pop_int();
GLsizei width=f.pop_int();
GLint border=f.pop_int();
(glCopyTexImage1D(target,level,internalFormat,x,y,width,border));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glCopyTexImage2D)
GLenum target=f.pop_int();
GLint level=f.pop_int();
GLenum internalFormat=f.pop_int();
GLint x=f.pop_int();
GLint y=f.pop_int();
GLsizei width=f.pop_int();
GLsizei height=f.pop_int();
GLint border=f.pop_int();
(glCopyTexImage2D(target,level,internalFormat,x,y,width,height,border));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glCopyTexSubImage1D)
GLenum target=f.pop_int();
GLint level=f.pop_int();
GLint xoffset=f.pop_int();
GLint x=f.pop_int();
GLint y=f.pop_int();
GLsizei width=f.pop_int();
(glCopyTexSubImage1D(target,level,xoffset,x,y,width));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glCopyTexSubImage2D)
GLenum target=f.pop_int();
GLint level=f.pop_int();
GLint xoffset=f.pop_int();
GLint yoffset=f.pop_int();
GLint x=f.pop_int();
GLint y=f.pop_int();
GLsizei width=f.pop_int();
GLsizei height=f.pop_int();
(glCopyTexSubImage2D(target,level,xoffset,yoffset,x,y,width,height));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glCullFace)
GLenum mode=f.pop_int();
(glCullFace(mode));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glDeleteLists)
GLuint list=f.pop_int();
GLsizei range=f.pop_int();
(glDeleteLists(list,range));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glDeleteTextures)
GLsizei n=f.pop_int();
const GLuint* textures=(const GLuint*)f.pop_ptr();
(glDeleteTextures(n,textures));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glDepthFunc)
GLenum func=f.pop_int();
(glDepthFunc(func));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glDepthMask)
GLboolean flag=f.pop_byte();
(glDepthMask(flag));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glDepthRange)
GLclampd zNear=f.pop_double();
GLclampd zFar=f.pop_double();
(glDepthRange(zNear,zFar));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glDisable)
GLenum cap=f.pop_int();
(glDisable(cap));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glDisableClientState)
GLenum array=f.pop_int();
(glDisableClientState(array));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glDrawArrays)
GLenum mode=f.pop_int();
GLint first=f.pop_int();
GLsizei count=f.pop_int();
(glDrawArrays(mode,first,count));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glDrawBuffer)
GLenum mode=f.pop_int();
(glDrawBuffer(mode));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glDrawElements)
GLenum mode=f.pop_int();
GLsizei count=f.pop_int();
GLenum type=f.pop_int();
const GLvoid* indices=(const GLvoid*)f.pop_ptr();
(glDrawElements(mode,count,type,indices));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glDrawPixels)
GLsizei width=f.pop_int();
GLsizei height=f.pop_int();
GLenum format=f.pop_int();
GLenum type=f.pop_int();
const GLvoid* pixels=(const GLvoid*)f.pop_ptr();
(glDrawPixels(width,height,format,type,pixels));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glEdgeFlag)
GLboolean flag=f.pop_byte();
(glEdgeFlag(flag));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glEdgeFlagPointer)
GLsizei stride=f.pop_int();
const GLvoid* pointer=(const GLvoid*)f.pop_ptr();
(glEdgeFlagPointer(stride,(const GLboolean*)pointer));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glEdgeFlagv)
const GLboolean* flag=(const GLboolean*)f.pop_ptr();
(glEdgeFlagv(flag));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glEnable)
GLenum cap=f.pop_int();
(glEnable(cap));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glEnableClientState)
GLenum array=f.pop_int();
(glEnableClientState(array));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glEnd)
(glEnd());
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glEndList)
(glEndList());
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glEvalCoord1d)
GLdouble u=f.pop_double();
(glEvalCoord1d(u));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glEvalCoord1dv)
const GLdouble* u=(const GLdouble*)f.pop_ptr();
(glEvalCoord1dv(u));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glEvalCoord1f)
GLfloat u=f.pop_float();
(glEvalCoord1f(u));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glEvalCoord1fv)
const GLfloat* u=(const GLfloat*)f.pop_ptr();
(glEvalCoord1fv(u));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glEvalCoord2d)
GLdouble u=f.pop_double();
GLdouble v=f.pop_double();
(glEvalCoord2d(u,v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glEvalCoord2dv)
const GLdouble* u=(const GLdouble*)f.pop_ptr();
(glEvalCoord2dv(u));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glEvalCoord2f)
GLfloat u=f.pop_float();
GLfloat v=f.pop_float();
(glEvalCoord2f(u,v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glEvalCoord2fv)
const GLfloat* u=(const GLfloat*)f.pop_ptr();
(glEvalCoord2fv(u));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glEvalMesh1)
GLenum mode=f.pop_int();
GLint i1=f.pop_int();
GLint i2=f.pop_int();
(glEvalMesh1(mode,i1,i2));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glEvalMesh2)
GLenum mode=f.pop_int();
GLint i1=f.pop_int();
GLint i2=f.pop_int();
GLint j1=f.pop_int();
GLint j2=f.pop_int();
(glEvalMesh2(mode,i1,i2,j1,j2));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glEvalPoint1)
GLint i=f.pop_int();
(glEvalPoint1(i));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glEvalPoint2)
GLint i=f.pop_int();
GLint j=f.pop_int();
(glEvalPoint2(i,j));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glFeedbackBuffer)
GLsizei size=f.pop_int();
GLenum type=f.pop_int();
GLfloat* buffer=(GLfloat*)f.pop_ptr();
(glFeedbackBuffer(size,type,buffer));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glFinish)
(glFinish());
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glFlush)
(glFlush());
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glFogf)
GLenum pname=f.pop_int();
GLfloat param=f.pop_float();
(glFogf(pname,param));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glFogfv)
GLenum pname=f.pop_int();
const GLfloat* params=(const GLfloat*)f.pop_ptr();
(glFogfv(pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glFogi)
GLenum pname=f.pop_int();
GLint param=f.pop_int();
(glFogi(pname,param));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glFogiv)
GLenum pname=f.pop_int();
const GLint* params=(const GLint*)f.pop_ptr();
(glFogiv(pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glFrontFace)
GLenum mode=f.pop_int();
(glFrontFace(mode));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glFrustum)
GLdouble left=f.pop_double();
GLdouble right=f.pop_double();
GLdouble bottom=f.pop_double();
GLdouble top=f.pop_double();
GLdouble zNear=f.pop_double();
GLdouble zFar=f.pop_double();
(glFrustum(left,right,bottom,top,zNear,zFar));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGenLists)
GLsizei range=f.pop_int();
f.push_int(glGenLists(range));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGenTextures)
GLsizei n=f.pop_int();
GLuint* textures=(GLuint*)f.pop_ptr();
(glGenTextures(n,textures));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetBooleanv)
GLenum pname=f.pop_int();
GLboolean* params=(GLboolean*)f.pop_ptr();
(glGetBooleanv(pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetClipPlane)
GLenum plane=f.pop_int();
GLdouble* equation=(GLdouble*)f.pop_ptr();
(glGetClipPlane(plane,equation));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetDoublev)
GLenum pname=f.pop_int();
GLdouble* params=(GLdouble*)f.pop_ptr();
(glGetDoublev(pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetError)
f.push_int(glGetError());
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetFloatv)
GLenum pname=f.pop_int();
GLfloat* params=(GLfloat*)f.pop_ptr();
(glGetFloatv(pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetIntegerv)
GLenum pname=f.pop_int();
GLint* params=(GLint*)f.pop_ptr();
(glGetIntegerv(pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetLightfv)
GLenum light=f.pop_int();
GLenum pname=f.pop_int();
GLfloat* params=(GLfloat*)f.pop_ptr();
(glGetLightfv(light,pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetLightiv)
GLenum light=f.pop_int();
GLenum pname=f.pop_int();
GLint* params=(GLint*)f.pop_ptr();
(glGetLightiv(light,pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetMapdv)
GLenum target=f.pop_int();
GLenum query=f.pop_int();
GLdouble* v=(GLdouble*)f.pop_ptr();
(glGetMapdv(target,query,v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetMapfv)
GLenum target=f.pop_int();
GLenum query=f.pop_int();
GLfloat* v=(GLfloat*)f.pop_ptr();
(glGetMapfv(target,query,v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetMapiv)
GLenum target=f.pop_int();
GLenum query=f.pop_int();
GLint* v=(GLint*)f.pop_ptr();
(glGetMapiv(target,query,v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetMaterialfv)
GLenum face=f.pop_int();
GLenum pname=f.pop_int();
GLfloat* params=(GLfloat*)f.pop_ptr();
(glGetMaterialfv(face,pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetMaterialiv)
GLenum face=f.pop_int();
GLenum pname=f.pop_int();
GLint* params=(GLint*)f.pop_ptr();
(glGetMaterialiv(face,pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetPixelMapfv)
GLenum map=f.pop_int();
GLfloat* values=(GLfloat*)f.pop_ptr();
(glGetPixelMapfv(map,values));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetPixelMapuiv)
GLenum map=f.pop_int();
GLuint* values=(GLuint*)f.pop_ptr();
(glGetPixelMapuiv(map,values));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetPixelMapusv)
GLenum map=f.pop_int();
GLushort* values=(GLushort*)f.pop_ptr();
(glGetPixelMapusv(map,values));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetPointerv)
GLenum pname=f.pop_int();
GLvoid** params=(GLvoid**)f.pop_ptr();
(glGetPointerv(pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetPolygonStipple)
GLubyte* mask=(GLubyte*)f.pop_ptr();
(glGetPolygonStipple(mask));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetString)
GLenum name=f.pop_int();
f.push_ptr((void*)glGetString(name));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetTexEnvfv)
GLenum target=f.pop_int();
GLenum pname=f.pop_int();
GLfloat* params=(GLfloat*)f.pop_ptr();
(glGetTexEnvfv(target,pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetTexEnviv)
GLenum target=f.pop_int();
GLenum pname=f.pop_int();
GLint* params=(GLint*)f.pop_ptr();
(glGetTexEnviv(target,pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetTexGendv)
GLenum coord=f.pop_int();
GLenum pname=f.pop_int();
GLdouble* params=(GLdouble*)f.pop_ptr();
(glGetTexGendv(coord,pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetTexGenfv)
GLenum coord=f.pop_int();
GLenum pname=f.pop_int();
GLfloat* params=(GLfloat*)f.pop_ptr();
(glGetTexGenfv(coord,pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetTexGeniv)
GLenum coord=f.pop_int();
GLenum pname=f.pop_int();
GLint* params=(GLint*)f.pop_ptr();
(glGetTexGeniv(coord,pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetTexImage)
GLenum target=f.pop_int();
GLint level=f.pop_int();
GLenum format=f.pop_int();
GLenum type=f.pop_int();
GLvoid* pixels=(GLvoid*)f.pop_ptr();
(glGetTexImage(target,level,format,type,pixels));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetTexLevelParameterfv)
GLenum target=f.pop_int();
GLint level=f.pop_int();
GLenum pname=f.pop_int();
GLfloat* params=(GLfloat*)f.pop_ptr();
(glGetTexLevelParameterfv(target,level,pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetTexLevelParameteriv)
GLenum target=f.pop_int();
GLint level=f.pop_int();
GLenum pname=f.pop_int();
GLint* params=(GLint*)f.pop_ptr();
(glGetTexLevelParameteriv(target,level,pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetTexParameterfv)
GLenum target=f.pop_int();
GLenum pname=f.pop_int();
GLfloat* params=(GLfloat*)f.pop_ptr();
(glGetTexParameterfv(target,pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glGetTexParameteriv)
GLenum target=f.pop_int();
GLenum pname=f.pop_int();
GLint* params=(GLint*)f.pop_ptr();
(glGetTexParameteriv(target,pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glHint)
GLenum target=f.pop_int();
GLenum mode=f.pop_int();
(glHint(target,mode));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glIndexMask)
GLuint mask=f.pop_int();
(glIndexMask(mask));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glIndexPointer)
GLenum type=f.pop_int();
GLsizei stride=f.pop_int();
const GLvoid* pointer=(const GLvoid*)f.pop_ptr();
(glIndexPointer(type,stride,pointer));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glIndexd)
GLdouble c=f.pop_double();
(glIndexd(c));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glIndexdv)
const GLdouble* c=(const GLdouble*)f.pop_ptr();
(glIndexdv(c));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glIndexf)
GLfloat c=f.pop_float();
(glIndexf(c));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glIndexfv)
const GLfloat* c=(const GLfloat*)f.pop_ptr();
(glIndexfv(c));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glIndexi)
GLint c=f.pop_int();
(glIndexi(c));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glIndexiv)
const GLint* c=(const GLint*)f.pop_ptr();
(glIndexiv(c));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glIndexs)
GLshort c=f.pop_short();
(glIndexs(c));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glIndexsv)
const GLshort* c=(const GLshort*)f.pop_ptr();
(glIndexsv(c));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glIndexub)
GLubyte c=f.pop_byte();
(glIndexub(c));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glIndexubv)
const GLubyte* c=(const GLubyte*)f.pop_ptr();
(glIndexubv(c));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glInitNames)
(glInitNames());
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glInterleavedArrays)
GLenum format=f.pop_int();
GLsizei stride=f.pop_int();
const GLvoid* pointer=(const GLvoid*)f.pop_ptr();
(glInterleavedArrays(format,stride,pointer));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glIsEnabled)
GLenum cap=f.pop_int();
f.push_byte(glIsEnabled(cap));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glIsList)
GLuint list=f.pop_int();
f.push_byte(glIsList(list));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glIsTexture)
GLuint texture=f.pop_int();
f.push_byte(glIsTexture(texture));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glLightModelf)
GLenum pname=f.pop_int();
GLfloat param=f.pop_float();
(glLightModelf(pname,param));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glLightModelfv)
GLenum pname=f.pop_int();
const GLfloat* params=(const GLfloat*)f.pop_ptr();
(glLightModelfv(pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glLightModeli)
GLenum pname=f.pop_int();
GLint param=f.pop_int();
(glLightModeli(pname,param));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glLightModeliv)
GLenum pname=f.pop_int();
const GLint* params=(const GLint*)f.pop_ptr();
(glLightModeliv(pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glLightf)
GLenum light=f.pop_int();
GLenum pname=f.pop_int();
GLfloat param=f.pop_float();
(glLightf(light,pname,param));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glLightfv)
GLenum light=f.pop_int();
GLenum pname=f.pop_int();
const GLfloat* params=(const GLfloat*)f.pop_ptr();
(glLightfv(light,pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glLighti)
GLenum light=f.pop_int();
GLenum pname=f.pop_int();
GLint param=f.pop_int();
(glLighti(light,pname,param));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glLightiv)
GLenum light=f.pop_int();
GLenum pname=f.pop_int();
const GLint* params=(const GLint*)f.pop_ptr();
(glLightiv(light,pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glLineStipple)
GLint factor=f.pop_int();
GLushort pattern=f.pop_short();
(glLineStipple(factor,pattern));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glLineWidth)
GLfloat width=f.pop_float();
(glLineWidth(width));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glListBase)
GLuint base=f.pop_int();
(glListBase(base));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glLoadIdentity)
(glLoadIdentity());
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glLoadMatrixd)
const GLdouble* m=(const GLdouble*)f.pop_ptr();
(glLoadMatrixd(m));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glLoadMatrixf)
const GLfloat* m=(const GLfloat*)f.pop_ptr();
(glLoadMatrixf(m));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glLoadName)
GLuint name=f.pop_int();
(glLoadName(name));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glLogicOp)
GLenum opcode=f.pop_int();
(glLogicOp(opcode));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glMap1d)
GLenum target=f.pop_int();
GLdouble u1=f.pop_double();
GLdouble u2=f.pop_double();
GLint stride=f.pop_int();
GLint order=f.pop_int();
const GLdouble* points=(const GLdouble*)f.pop_ptr();
(glMap1d(target,u1,u2,stride,order,points));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glMap1f)
GLenum target=f.pop_int();
GLfloat u1=f.pop_float();
GLfloat u2=f.pop_float();
GLint stride=f.pop_int();
GLint order=f.pop_int();
const GLfloat* points=(const GLfloat*)f.pop_ptr();
(glMap1f(target,u1,u2,stride,order,points));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glMap2d)
GLenum target=f.pop_int();
GLdouble u1=f.pop_double();
GLdouble u2=f.pop_double();
GLint ustride=f.pop_int();
GLint uorder=f.pop_int();
GLdouble v1=f.pop_double();
GLdouble v2=f.pop_double();
GLint vstride=f.pop_int();
GLint vorder=f.pop_int();
const GLdouble* points=(const GLdouble*)f.pop_ptr();
(glMap2d(target,u1,u2,ustride,uorder,v1,v2,vstride,vorder,points));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glMap2f)
GLenum target=f.pop_int();
GLfloat u1=f.pop_float();
GLfloat u2=f.pop_float();
GLint ustride=f.pop_int();
GLint uorder=f.pop_int();
GLfloat v1=f.pop_float();
GLfloat v2=f.pop_float();
GLint vstride=f.pop_int();
GLint vorder=f.pop_int();
const GLfloat* points=(const GLfloat*)f.pop_ptr();
(glMap2f(target,u1,u2,ustride,uorder,v1,v2,vstride,vorder,points));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glMapGrid1d)
GLint un=f.pop_int();
GLdouble u1=f.pop_double();
GLdouble u2=f.pop_double();
(glMapGrid1d(un,u1,u2));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glMapGrid1f)
GLint un=f.pop_int();
GLfloat u1=f.pop_float();
GLfloat u2=f.pop_float();
(glMapGrid1f(un,u1,u2));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glMapGrid2d)
GLint un=f.pop_int();
GLdouble u1=f.pop_double();
GLdouble u2=f.pop_double();
GLint vn=f.pop_int();
GLdouble v1=f.pop_double();
GLdouble v2=f.pop_double();
(glMapGrid2d(un,u1,u2,vn,v1,v2));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glMapGrid2f)
GLint un=f.pop_int();
GLfloat u1=f.pop_float();
GLfloat u2=f.pop_float();
GLint vn=f.pop_int();
GLfloat v1=f.pop_float();
GLfloat v2=f.pop_float();
(glMapGrid2f(un,u1,u2,vn,v1,v2));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glMaterialf)
GLenum face=f.pop_int();
GLenum pname=f.pop_int();
GLfloat param=f.pop_float();
(glMaterialf(face,pname,param));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glMaterialfv)
GLenum face=f.pop_int();
GLenum pname=f.pop_int();
const GLfloat* params=(const GLfloat*)f.pop_ptr();
(glMaterialfv(face,pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glMateriali)
GLenum face=f.pop_int();
GLenum pname=f.pop_int();
GLint param=f.pop_int();
(glMateriali(face,pname,param));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glMaterialiv)
GLenum face=f.pop_int();
GLenum pname=f.pop_int();
const GLint* params=(const GLint*)f.pop_ptr();
(glMaterialiv(face,pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glMatrixMode)
GLenum mode=f.pop_int();
(glMatrixMode(mode));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glMultMatrixd)
const GLdouble* m=(const GLdouble*)f.pop_ptr();
(glMultMatrixd(m));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glMultMatrixf)
const GLfloat* m=(const GLfloat*)f.pop_ptr();
(glMultMatrixf(m));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glNewList)
GLuint list=f.pop_int();
GLenum mode=f.pop_int();
(glNewList(list,mode));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glNormal3b)
GLbyte nx=f.pop_byte();
GLbyte ny=f.pop_byte();
GLbyte nz=f.pop_byte();
(glNormal3b(nx,ny,nz));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glNormal3bv)
const GLbyte* v=(const GLbyte*)f.pop_ptr();
(glNormal3bv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glNormal3d)
GLdouble nx=f.pop_double();
GLdouble ny=f.pop_double();
GLdouble nz=f.pop_double();
(glNormal3d(nx,ny,nz));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glNormal3dv)
const GLdouble* v=(const GLdouble*)f.pop_ptr();
(glNormal3dv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glNormal3f)
GLfloat nx=f.pop_float();
GLfloat ny=f.pop_float();
GLfloat nz=f.pop_float();
(glNormal3f(nx,ny,nz));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glNormal3fv)
const GLfloat* v=(const GLfloat*)f.pop_ptr();
(glNormal3fv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glNormal3i)
GLint nx=f.pop_int();
GLint ny=f.pop_int();
GLint nz=f.pop_int();
(glNormal3i(nx,ny,nz));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glNormal3iv)
const GLint* v=(const GLint*)f.pop_ptr();
(glNormal3iv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glNormal3s)
GLshort nx=f.pop_short();
GLshort ny=f.pop_short();
GLshort nz=f.pop_short();
(glNormal3s(nx,ny,nz));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glNormal3sv)
const GLshort* v=(const GLshort*)f.pop_ptr();
(glNormal3sv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glNormalPointer)
GLenum type=f.pop_int();
GLsizei stride=f.pop_int();
const GLvoid* pointer=(const GLvoid*)f.pop_ptr();
(glNormalPointer(type,stride,pointer));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glOrtho)
GLdouble left=f.pop_double();
GLdouble right=f.pop_double();
GLdouble bottom=f.pop_double();
GLdouble top=f.pop_double();
GLdouble zNear=f.pop_double();
GLdouble zFar=f.pop_double();
(glOrtho(left,right,bottom,top,zNear,zFar));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glPassThrough)
GLfloat token=f.pop_float();
(glPassThrough(token));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glPixelMapfv)
GLenum map=f.pop_int();
GLsizei mapsize=f.pop_int();
const GLfloat* values=(const GLfloat*)f.pop_ptr();
(glPixelMapfv(map,mapsize,values));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glPixelMapuiv)
GLenum map=f.pop_int();
GLsizei mapsize=f.pop_int();
const GLuint* values=(const GLuint*)f.pop_ptr();
(glPixelMapuiv(map,mapsize,values));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glPixelMapusv)
GLenum map=f.pop_int();
GLsizei mapsize=f.pop_int();
const GLushort* values=(const GLushort*)f.pop_ptr();
(glPixelMapusv(map,mapsize,values));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glPixelStoref)
GLenum pname=f.pop_int();
GLfloat param=f.pop_float();
(glPixelStoref(pname,param));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glPixelStorei)
GLenum pname=f.pop_int();
GLint param=f.pop_int();
(glPixelStorei(pname,param));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glPixelTransferf)
GLenum pname=f.pop_int();
GLfloat param=f.pop_float();
(glPixelTransferf(pname,param));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glPixelTransferi)
GLenum pname=f.pop_int();
GLint param=f.pop_int();
(glPixelTransferi(pname,param));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glPixelZoom)
GLfloat xfactor=f.pop_float();
GLfloat yfactor=f.pop_float();
(glPixelZoom(xfactor,yfactor));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glPointSize)
GLfloat size=f.pop_float();
(glPointSize(size));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glPolygonMode)
GLenum face=f.pop_int();
GLenum mode=f.pop_int();
(glPolygonMode(face,mode));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glPolygonOffset)
GLfloat factor=f.pop_float();
GLfloat units=f.pop_float();
(glPolygonOffset(factor,units));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glPolygonStipple)
const GLubyte* mask=(const GLubyte*)f.pop_ptr();
(glPolygonStipple(mask));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glPopAttrib)
(glPopAttrib());
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glPopClientAttrib)
(glPopClientAttrib());
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glPopMatrix)
(glPopMatrix());
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glPopName)
(glPopName());
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glPrioritizeTextures)
GLsizei n=f.pop_int();
const GLuint* textures=(const GLuint*)f.pop_ptr();
const GLclampf* priorities=(const GLclampf*)f.pop_ptr();
(glPrioritizeTextures(n,textures,priorities));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glPushAttrib)
GLbitfield mask=f.pop_int();
(glPushAttrib(mask));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glPushClientAttrib)
GLbitfield mask=f.pop_int();
(glPushClientAttrib(mask));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glPushMatrix)
(glPushMatrix());
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glPushName)
GLuint name=f.pop_int();
(glPushName(name));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRasterPos2d)
GLdouble x=f.pop_double();
GLdouble y=f.pop_double();
(glRasterPos2d(x,y));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRasterPos2dv)
const GLdouble* v=(const GLdouble*)f.pop_ptr();
(glRasterPos2dv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRasterPos2f)
GLfloat x=f.pop_float();
GLfloat y=f.pop_float();
(glRasterPos2f(x,y));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRasterPos2fv)
const GLfloat* v=(const GLfloat*)f.pop_ptr();
(glRasterPos2fv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRasterPos2i)
GLint x=f.pop_int();
GLint y=f.pop_int();
(glRasterPos2i(x,y));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRasterPos2iv)
const GLint* v=(const GLint*)f.pop_ptr();
(glRasterPos2iv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRasterPos2s)
GLshort x=f.pop_short();
GLshort y=f.pop_short();
(glRasterPos2s(x,y));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRasterPos2sv)
const GLshort* v=(const GLshort*)f.pop_ptr();
(glRasterPos2sv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRasterPos3d)
GLdouble x=f.pop_double();
GLdouble y=f.pop_double();
GLdouble z=f.pop_double();
(glRasterPos3d(x,y,z));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRasterPos3dv)
const GLdouble* v=(const GLdouble*)f.pop_ptr();
(glRasterPos3dv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRasterPos3f)
GLfloat x=f.pop_float();
GLfloat y=f.pop_float();
GLfloat z=f.pop_float();
(glRasterPos3f(x,y,z));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRasterPos3fv)
const GLfloat* v=(const GLfloat*)f.pop_ptr();
(glRasterPos3fv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRasterPos3i)
GLint x=f.pop_int();
GLint y=f.pop_int();
GLint z=f.pop_int();
(glRasterPos3i(x,y,z));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRasterPos3iv)
const GLint* v=(const GLint*)f.pop_ptr();
(glRasterPos3iv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRasterPos3s)
GLshort x=f.pop_short();
GLshort y=f.pop_short();
GLshort z=f.pop_short();
(glRasterPos3s(x,y,z));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRasterPos3sv)
const GLshort* v=(const GLshort*)f.pop_ptr();
(glRasterPos3sv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRasterPos4d)
GLdouble x=f.pop_double();
GLdouble y=f.pop_double();
GLdouble z=f.pop_double();
GLdouble w=f.pop_double();
(glRasterPos4d(x,y,z,w));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRasterPos4dv)
const GLdouble* v=(const GLdouble*)f.pop_ptr();
(glRasterPos4dv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRasterPos4f)
GLfloat x=f.pop_float();
GLfloat y=f.pop_float();
GLfloat z=f.pop_float();
GLfloat w=f.pop_float();
(glRasterPos4f(x,y,z,w));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRasterPos4fv)
const GLfloat* v=(const GLfloat*)f.pop_ptr();
(glRasterPos4fv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRasterPos4i)
GLint x=f.pop_int();
GLint y=f.pop_int();
GLint z=f.pop_int();
GLint w=f.pop_int();
(glRasterPos4i(x,y,z,w));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRasterPos4iv)
const GLint* v=(const GLint*)f.pop_ptr();
(glRasterPos4iv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRasterPos4s)
GLshort x=f.pop_short();
GLshort y=f.pop_short();
GLshort z=f.pop_short();
GLshort w=f.pop_short();
(glRasterPos4s(x,y,z,w));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRasterPos4sv)
const GLshort* v=(const GLshort*)f.pop_ptr();
(glRasterPos4sv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glReadBuffer)
GLenum mode=f.pop_int();
(glReadBuffer(mode));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glReadPixels)
GLint x=f.pop_int();
GLint y=f.pop_int();
GLsizei width=f.pop_int();
GLsizei height=f.pop_int();
GLenum format=f.pop_int();
GLenum type=f.pop_int();
GLvoid* pixels=(GLvoid*)f.pop_ptr();
(glReadPixels(x,y,width,height,format,type,pixels));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRectd)
GLdouble x1=f.pop_double();
GLdouble y1=f.pop_double();
GLdouble x2=f.pop_double();
GLdouble y2=f.pop_double();
(glRectd(x1,y1,x2,y2));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRectdv)
const GLdouble* v1=(const GLdouble*)f.pop_ptr();
const GLdouble* v2=(const GLdouble*)f.pop_ptr();
(glRectdv(v1,v2));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRectf)
GLfloat x1=f.pop_float();
GLfloat y1=f.pop_float();
GLfloat x2=f.pop_float();
GLfloat y2=f.pop_float();
(glRectf(x1,y1,x2,y2));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRectfv)
const GLfloat* v1=(const GLfloat*)f.pop_ptr();
const GLfloat* v2=(const GLfloat*)f.pop_ptr();
(glRectfv(v1,v2));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRecti)
GLint x1=f.pop_int();
GLint y1=f.pop_int();
GLint x2=f.pop_int();
GLint y2=f.pop_int();
(glRecti(x1,y1,x2,y2));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRectiv)
const GLint* v1=(const GLint*)f.pop_ptr();
const GLint* v2=(const GLint*)f.pop_ptr();
(glRectiv(v1,v2));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRects)
GLshort x1=f.pop_short();
GLshort y1=f.pop_short();
GLshort x2=f.pop_short();
GLshort y2=f.pop_short();
(glRects(x1,y1,x2,y2));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRectsv)
const GLshort* v1=(const GLshort*)f.pop_ptr();
const GLshort* v2=(const GLshort*)f.pop_ptr();
(glRectsv(v1,v2));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRenderMode)
GLenum mode=f.pop_int();
f.push_int(glRenderMode(mode));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRotated)
GLdouble angle=f.pop_double();
GLdouble x=f.pop_double();
GLdouble y=f.pop_double();
GLdouble z=f.pop_double();
(glRotated(angle,x,y,z));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glRotatef)
GLfloat angle=f.pop_float();
GLfloat x=f.pop_float();
GLfloat y=f.pop_float();
GLfloat z=f.pop_float();
(glRotatef(angle,x,y,z));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glScaled)
GLdouble x=f.pop_double();
GLdouble y=f.pop_double();
GLdouble z=f.pop_double();
(glScaled(x,y,z));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glScalef)
GLfloat x=f.pop_float();
GLfloat y=f.pop_float();
GLfloat z=f.pop_float();
(glScalef(x,y,z));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glScissor)
GLint x=f.pop_int();
GLint y=f.pop_int();
GLsizei width=f.pop_int();
GLsizei height=f.pop_int();
(glScissor(x,y,width,height));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glSelectBuffer)
GLsizei size=f.pop_int();
GLuint* buffer=(GLuint*)f.pop_ptr();
(glSelectBuffer(size,buffer));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glShadeModel)
GLenum mode=f.pop_int();
(glShadeModel(mode));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glStencilFunc)
GLenum func=f.pop_int();
GLint ref=f.pop_int();
GLuint mask=f.pop_int();
(glStencilFunc(func,ref,mask));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glStencilMask)
GLuint mask=f.pop_int();
(glStencilMask(mask));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glStencilOp)
GLenum fail=f.pop_int();
GLenum zfail=f.pop_int();
GLenum zpass=f.pop_int();
(glStencilOp(fail,zfail,zpass));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord1d)
GLdouble s=f.pop_double();
(glTexCoord1d(s));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord1dv)
const GLdouble* v=(const GLdouble*)f.pop_ptr();
(glTexCoord1dv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord1f)
GLfloat s=f.pop_float();
(glTexCoord1f(s));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord1fv)
const GLfloat* v=(const GLfloat*)f.pop_ptr();
(glTexCoord1fv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord1i)
GLint s=f.pop_int();
(glTexCoord1i(s));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord1iv)
const GLint* v=(const GLint*)f.pop_ptr();
(glTexCoord1iv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord1s)
GLshort s=f.pop_short();
(glTexCoord1s(s));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord1sv)
const GLshort* v=(const GLshort*)f.pop_ptr();
(glTexCoord1sv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord2d)
GLdouble s=f.pop_double();
GLdouble t=f.pop_double();
(glTexCoord2d(s,t));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord2dv)
const GLdouble* v=(const GLdouble*)f.pop_ptr();
(glTexCoord2dv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord2f)
GLfloat s=f.pop_float();
GLfloat t=f.pop_float();
(glTexCoord2f(s,t));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord2fv)
const GLfloat* v=(const GLfloat*)f.pop_ptr();
(glTexCoord2fv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord2i)
GLint s=f.pop_int();
GLint t=f.pop_int();
(glTexCoord2i(s,t));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord2iv)
const GLint* v=(const GLint*)f.pop_ptr();
(glTexCoord2iv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord2s)
GLshort s=f.pop_short();
GLshort t=f.pop_short();
(glTexCoord2s(s,t));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord2sv)
const GLshort* v=(const GLshort*)f.pop_ptr();
(glTexCoord2sv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord3d)
GLdouble s=f.pop_double();
GLdouble t=f.pop_double();
GLdouble r=f.pop_double();
(glTexCoord3d(s,t,r));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord3dv)
const GLdouble* v=(const GLdouble*)f.pop_ptr();
(glTexCoord3dv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord3f)
GLfloat s=f.pop_float();
GLfloat t=f.pop_float();
GLfloat r=f.pop_float();
(glTexCoord3f(s,t,r));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord3fv)
const GLfloat* v=(const GLfloat*)f.pop_ptr();
(glTexCoord3fv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord3i)
GLint s=f.pop_int();
GLint t=f.pop_int();
GLint r=f.pop_int();
(glTexCoord3i(s,t,r));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord3iv)
const GLint* v=(const GLint*)f.pop_ptr();
(glTexCoord3iv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord3s)
GLshort s=f.pop_short();
GLshort t=f.pop_short();
GLshort r=f.pop_short();
(glTexCoord3s(s,t,r));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord3sv)
const GLshort* v=(const GLshort*)f.pop_ptr();
(glTexCoord3sv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord4d)
GLdouble s=f.pop_double();
GLdouble t=f.pop_double();
GLdouble r=f.pop_double();
GLdouble q=f.pop_double();
(glTexCoord4d(s,t,r,q));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord4dv)
const GLdouble* v=(const GLdouble*)f.pop_ptr();
(glTexCoord4dv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord4f)
GLfloat s=f.pop_float();
GLfloat t=f.pop_float();
GLfloat r=f.pop_float();
GLfloat q=f.pop_float();
(glTexCoord4f(s,t,r,q));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord4fv)
const GLfloat* v=(const GLfloat*)f.pop_ptr();
(glTexCoord4fv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord4i)
GLint s=f.pop_int();
GLint t=f.pop_int();
GLint r=f.pop_int();
GLint q=f.pop_int();
(glTexCoord4i(s,t,r,q));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord4iv)
const GLint* v=(const GLint*)f.pop_ptr();
(glTexCoord4iv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord4s)
GLshort s=f.pop_short();
GLshort t=f.pop_short();
GLshort r=f.pop_short();
GLshort q=f.pop_short();
(glTexCoord4s(s,t,r,q));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoord4sv)
const GLshort* v=(const GLshort*)f.pop_ptr();
(glTexCoord4sv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexCoordPointer)
GLint size=f.pop_int();
GLenum type=f.pop_int();
GLsizei stride=f.pop_int();
const GLvoid* pointer=(const GLvoid*)f.pop_ptr();
(glTexCoordPointer(size,type,stride,pointer));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexEnvf)
GLenum target=f.pop_int();
GLenum pname=f.pop_int();
GLfloat param=f.pop_float();
(glTexEnvf(target,pname,param));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexEnvfv)
GLenum target=f.pop_int();
GLenum pname=f.pop_int();
const GLfloat* params=(const GLfloat*)f.pop_ptr();
(glTexEnvfv(target,pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexEnvi)
GLenum target=f.pop_int();
GLenum pname=f.pop_int();
GLint param=f.pop_int();
(glTexEnvi(target,pname,param));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexEnviv)
GLenum target=f.pop_int();
GLenum pname=f.pop_int();
const GLint* params=(const GLint*)f.pop_ptr();
(glTexEnviv(target,pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexGend)
GLenum coord=f.pop_int();
GLenum pname=f.pop_int();
GLdouble param=f.pop_double();
(glTexGend(coord,pname,param));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexGendv)
GLenum coord=f.pop_int();
GLenum pname=f.pop_int();
const GLdouble* params=(const GLdouble*)f.pop_ptr();
(glTexGendv(coord,pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexGenf)
GLenum coord=f.pop_int();
GLenum pname=f.pop_int();
GLfloat param=f.pop_float();
(glTexGenf(coord,pname,param));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexGenfv)
GLenum coord=f.pop_int();
GLenum pname=f.pop_int();
const GLfloat* params=(const GLfloat*)f.pop_ptr();
(glTexGenfv(coord,pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexGeni)
GLenum coord=f.pop_int();
GLenum pname=f.pop_int();
GLint param=f.pop_int();
(glTexGeni(coord,pname,param));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexGeniv)
GLenum coord=f.pop_int();
GLenum pname=f.pop_int();
const GLint* params=(const GLint*)f.pop_ptr();
(glTexGeniv(coord,pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexImage1D)
GLenum target=f.pop_int();
GLint level=f.pop_int();
GLint internalformat=f.pop_int();
GLsizei width=f.pop_int();
GLint border=f.pop_int();
GLenum format=f.pop_int();
GLenum type=f.pop_int();
const GLvoid* pixels=(const GLvoid*)f.pop_ptr();
(glTexImage1D(target,level,internalformat,width,border,format,type,pixels));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexImage2D)
GLenum target=f.pop_int();
GLint level=f.pop_int();
GLint internalformat=f.pop_int();
GLsizei width=f.pop_int();
GLsizei height=f.pop_int();
GLint border=f.pop_int();
GLenum format=f.pop_int();
GLenum type=f.pop_int();
const GLvoid* pixels=(const GLvoid*)f.pop_ptr();
(glTexImage2D(target,level,internalformat,width,height,border,format,type,pixels));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexParameterf)
GLenum target=f.pop_int();
GLenum pname=f.pop_int();
GLfloat param=f.pop_float();
(glTexParameterf(target,pname,param));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexParameterfv)
GLenum target=f.pop_int();
GLenum pname=f.pop_int();
const GLfloat* params=(const GLfloat*)f.pop_ptr();
(glTexParameterfv(target,pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexParameteri)
GLenum target=f.pop_int();
GLenum pname=f.pop_int();
GLint param=f.pop_int();
(glTexParameteri(target,pname,param));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexParameteriv)
GLenum target=f.pop_int();
GLenum pname=f.pop_int();
const GLint* params=(const GLint*)f.pop_ptr();
(glTexParameteriv(target,pname,params));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexSubImage1D)
GLenum target=f.pop_int();
GLint level=f.pop_int();
GLint xoffset=f.pop_int();
GLsizei width=f.pop_int();
GLenum format=f.pop_int();
GLenum type=f.pop_int();
const GLvoid* pixels=(const GLvoid*)f.pop_ptr();
(glTexSubImage1D(target,level,xoffset,width,format,type,pixels));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTexSubImage2D)
GLenum target=f.pop_int();
GLint level=f.pop_int();
GLint xoffset=f.pop_int();
GLint yoffset=f.pop_int();
GLsizei width=f.pop_int();
GLsizei height=f.pop_int();
GLenum format=f.pop_int();
GLenum type=f.pop_int();
const GLvoid* pixels=(const GLvoid*)f.pop_ptr();
(glTexSubImage2D(target,level,xoffset,yoffset,width,height,format,type,pixels));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTranslated)
GLdouble x=f.pop_double();
GLdouble y=f.pop_double();
GLdouble z=f.pop_double();
(glTranslated(x,y,z));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glTranslatef)
GLfloat x=f.pop_float();
GLfloat y=f.pop_float();
GLfloat z=f.pop_float();
(glTranslatef(x,y,z));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glVertex2d)
GLdouble x=f.pop_double();
GLdouble y=f.pop_double();
(glVertex2d(x,y));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glVertex2dv)
const GLdouble* v=(const GLdouble*)f.pop_ptr();
(glVertex2dv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glVertex2f)
GLfloat x=f.pop_float();
GLfloat y=f.pop_float();
(glVertex2f(x,y));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glVertex2fv)
const GLfloat* v=(const GLfloat*)f.pop_ptr();
(glVertex2fv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glVertex2i)
GLint x=f.pop_int();
GLint y=f.pop_int();
(glVertex2i(x,y));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glVertex2iv)
const GLint* v=(const GLint*)f.pop_ptr();
(glVertex2iv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glVertex2s)
GLshort x=f.pop_short();
GLshort y=f.pop_short();
(glVertex2s(x,y));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glVertex2sv)
const GLshort* v=(const GLshort*)f.pop_ptr();
(glVertex2sv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glVertex3d)
GLdouble x=f.pop_double();
GLdouble y=f.pop_double();
GLdouble z=f.pop_double();
(glVertex3d(x,y,z));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glVertex3dv)
const GLdouble* v=(const GLdouble*)f.pop_ptr();
(glVertex3dv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glVertex3f)
GLfloat x=f.pop_float();
GLfloat y=f.pop_float();
GLfloat z=f.pop_float();
(glVertex3f(x,y,z));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glVertex3fv)
const GLfloat* v=(const GLfloat*)f.pop_ptr();
(glVertex3fv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glVertex3i)
GLint x=f.pop_int();
GLint y=f.pop_int();
GLint z=f.pop_int();
(glVertex3i(x,y,z));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glVertex3iv)
const GLint* v=(const GLint*)f.pop_ptr();
(glVertex3iv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glVertex3s)
GLshort x=f.pop_short();
GLshort y=f.pop_short();
GLshort z=f.pop_short();
(glVertex3s(x,y,z));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glVertex3sv)
const GLshort* v=(const GLshort*)f.pop_ptr();
(glVertex3sv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glVertex4d)
GLdouble x=f.pop_double();
GLdouble y=f.pop_double();
GLdouble z=f.pop_double();
GLdouble w=f.pop_double();
(glVertex4d(x,y,z,w));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glVertex4dv)
const GLdouble* v=(const GLdouble*)f.pop_ptr();
(glVertex4dv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glVertex4f)
GLfloat x=f.pop_float();
GLfloat y=f.pop_float();
GLfloat z=f.pop_float();
GLfloat w=f.pop_float();
(glVertex4f(x,y,z,w));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glVertex4fv)
const GLfloat* v=(const GLfloat*)f.pop_ptr();
(glVertex4fv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glVertex4i)
GLint x=f.pop_int();
GLint y=f.pop_int();
GLint z=f.pop_int();
GLint w=f.pop_int();
(glVertex4i(x,y,z,w));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glVertex4iv)
const GLint* v=(const GLint*)f.pop_ptr();
(glVertex4iv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glVertex4s)
GLshort x=f.pop_short();
GLshort y=f.pop_short();
GLshort z=f.pop_short();
GLshort w=f.pop_short();
(glVertex4s(x,y,z,w));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glVertex4sv)
const GLshort* v=(const GLshort*)f.pop_ptr();
(glVertex4sv(v));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glVertexPointer)
GLint size=f.pop_int();
GLenum type=f.pop_int();
GLsizei stride=f.pop_int();
const GLvoid* pointer=(const GLvoid*)f.pop_ptr();
(glVertexPointer(size,type,stride,pointer));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_glViewport)
GLint x=f.pop_int();
GLint y=f.pop_int();
GLsizei width=f.pop_int();
GLsizei height=f.pop_int();
(glViewport(x,y,width,height));
END_STDLIB_FUNCTION


void init_opengl( machine_t& m ) {
class_t* rootclss = m.get_root_class();
register_native_function(
	m, rootclss,
	"glAccum;GLenum::;GLfloat::",
	glue_glAccum );
register_native_function(
	m, rootclss,
	"glAlphaFunc;GLenum::;GLclampf::",
	glue_glAlphaFunc );
register_native_function(
	m, rootclss,
	"glAreTexturesResident;GLsizei::;.GLuint::*;GLboolean::*",
	glue_glAreTexturesResident );
register_native_function(
	m, rootclss,
	"glArrayElement;GLint::",
	glue_glArrayElement );
register_native_function(
	m, rootclss,
	"glBegin;GLenum::",
	glue_glBegin );
register_native_function(
	m, rootclss,
	"glBindTexture;GLenum::;GLuint::",
	glue_glBindTexture );
register_native_function(
	m, rootclss,
	"glBitmap;GLsizei::;GLsizei::;GLfloat::;GLfloat::;GLfloat::;GLfloat::;.GLubyte::*",
	glue_glBitmap );
register_native_function(
	m, rootclss,
	"glBlendFunc;GLenum::;GLenum::",
	glue_glBlendFunc );
register_native_function(
	m, rootclss,
	"glCallList;GLuint::",
	glue_glCallList );
register_native_function(
	m, rootclss,
	"glCallLists;GLsizei::;GLenum::;.GLvoid::*",
	glue_glCallLists );
register_native_function(
	m, rootclss,
	"glClear;GLbitfield::",
	glue_glClear );
register_native_function(
	m, rootclss,
	"glClearAccum;GLfloat::;GLfloat::;GLfloat::;GLfloat::",
	glue_glClearAccum );
register_native_function(
	m, rootclss,
	"glClearColor;GLclampf::;GLclampf::;GLclampf::;GLclampf::",
	glue_glClearColor );
register_native_function(
	m, rootclss,
	"glClearDepth;GLclampd::",
	glue_glClearDepth );
register_native_function(
	m, rootclss,
	"glClearIndex;GLfloat::",
	glue_glClearIndex );
register_native_function(
	m, rootclss,
	"glClearStencil;GLint::",
	glue_glClearStencil );
register_native_function(
	m, rootclss,
	"glClipPlane;GLenum::;.GLdouble::*",
	glue_glClipPlane );
register_native_function(
	m, rootclss,
	"glColor3b;GLbyte::;GLbyte::;GLbyte::",
	glue_glColor3b );
register_native_function(
	m, rootclss,
	"glColor3bv;.GLbyte::*",
	glue_glColor3bv );
register_native_function(
	m, rootclss,
	"glColor3d;GLdouble::;GLdouble::;GLdouble::",
	glue_glColor3d );
register_native_function(
	m, rootclss,
	"glColor3dv;.GLdouble::*",
	glue_glColor3dv );
register_native_function(
	m, rootclss,
	"glColor3f;GLfloat::;GLfloat::;GLfloat::",
	glue_glColor3f );
register_native_function(
	m, rootclss,
	"glColor3fv;.GLfloat::*",
	glue_glColor3fv );
register_native_function(
	m, rootclss,
	"glColor3i;GLint::;GLint::;GLint::",
	glue_glColor3i );
register_native_function(
	m, rootclss,
	"glColor3iv;.GLint::*",
	glue_glColor3iv );
register_native_function(
	m, rootclss,
	"glColor3s;GLshort::;GLshort::;GLshort::",
	glue_glColor3s );
register_native_function(
	m, rootclss,
	"glColor3sv;.GLshort::*",
	glue_glColor3sv );
register_native_function(
	m, rootclss,
	"glColor3ub;GLubyte::;GLubyte::;GLubyte::",
	glue_glColor3ub );
register_native_function(
	m, rootclss,
	"glColor3ubv;.GLubyte::*",
	glue_glColor3ubv );
register_native_function(
	m, rootclss,
	"glColor3ui;GLuint::;GLuint::;GLuint::",
	glue_glColor3ui );
register_native_function(
	m, rootclss,
	"glColor3uiv;.GLuint::*",
	glue_glColor3uiv );
register_native_function(
	m, rootclss,
	"glColor3us;GLushort::;GLushort::;GLushort::",
	glue_glColor3us );
register_native_function(
	m, rootclss,
	"glColor3usv;.GLushort::*",
	glue_glColor3usv );
register_native_function(
	m, rootclss,
	"glColor4b;GLbyte::;GLbyte::;GLbyte::;GLbyte::",
	glue_glColor4b );
register_native_function(
	m, rootclss,
	"glColor4bv;.GLbyte::*",
	glue_glColor4bv );
register_native_function(
	m, rootclss,
	"glColor4d;GLdouble::;GLdouble::;GLdouble::;GLdouble::",
	glue_glColor4d );
register_native_function(
	m, rootclss,
	"glColor4dv;.GLdouble::*",
	glue_glColor4dv );
register_native_function(
	m, rootclss,
	"glColor4f;GLfloat::;GLfloat::;GLfloat::;GLfloat::",
	glue_glColor4f );
register_native_function(
	m, rootclss,
	"glColor4fv;.GLfloat::*",
	glue_glColor4fv );
register_native_function(
	m, rootclss,
	"glColor4i;GLint::;GLint::;GLint::;GLint::",
	glue_glColor4i );
register_native_function(
	m, rootclss,
	"glColor4iv;.GLint::*",
	glue_glColor4iv );
register_native_function(
	m, rootclss,
	"glColor4s;GLshort::;GLshort::;GLshort::;GLshort::",
	glue_glColor4s );
register_native_function(
	m, rootclss,
	"glColor4sv;.GLshort::*",
	glue_glColor4sv );
register_native_function(
	m, rootclss,
	"glColor4ub;GLubyte::;GLubyte::;GLubyte::;GLubyte::",
	glue_glColor4ub );
register_native_function(
	m, rootclss,
	"glColor4ubv;.GLubyte::*",
	glue_glColor4ubv );
register_native_function(
	m, rootclss,
	"glColor4ui;GLuint::;GLuint::;GLuint::;GLuint::",
	glue_glColor4ui );
register_native_function(
	m, rootclss,
	"glColor4uiv;.GLuint::*",
	glue_glColor4uiv );
register_native_function(
	m, rootclss,
	"glColor4us;GLushort::;GLushort::;GLushort::;GLushort::",
	glue_glColor4us );
register_native_function(
	m, rootclss,
	"glColor4usv;.GLushort::*",
	glue_glColor4usv );
register_native_function(
	m, rootclss,
	"glColorMask;GLboolean::;GLboolean::;GLboolean::;GLboolean::",
	glue_glColorMask );
register_native_function(
	m, rootclss,
	"glColorMaterial;GLenum::;GLenum::",
	glue_glColorMaterial );
register_native_function(
	m, rootclss,
	"glColorPointer;GLint::;GLenum::;GLsizei::;.GLvoid::*",
	glue_glColorPointer );
register_native_function(
	m, rootclss,
	"glCopyPixels;GLint::;GLint::;GLsizei::;GLsizei::;GLenum::",
	glue_glCopyPixels );
register_native_function(
	m, rootclss,
	"glCopyTexImage1D;GLenum::;GLint::;GLenum::;GLint::;GLint::;GLsizei::;GLint::",
	glue_glCopyTexImage1D );
register_native_function(
	m, rootclss,
	"glCopyTexImage2D;GLenum::;GLint::;GLenum::;GLint::;GLint::;GLsizei::;GLsizei::;GLint::",
	glue_glCopyTexImage2D );
register_native_function(
	m, rootclss,
	"glCopyTexSubImage1D;GLenum::;GLint::;GLint::;GLint::;GLint::;GLsizei::",
	glue_glCopyTexSubImage1D );
register_native_function(
	m, rootclss,
	"glCopyTexSubImage2D;GLenum::;GLint::;GLint::;GLint::;GLint::;GLint::;GLsizei::;GLsizei::",
	glue_glCopyTexSubImage2D );
register_native_function(
	m, rootclss,
	"glCullFace;GLenum::",
	glue_glCullFace );
register_native_function(
	m, rootclss,
	"glDeleteLists;GLuint::;GLsizei::",
	glue_glDeleteLists );
register_native_function(
	m, rootclss,
	"glDeleteTextures;GLsizei::;.GLuint::*",
	glue_glDeleteTextures );
register_native_function(
	m, rootclss,
	"glDepthFunc;GLenum::",
	glue_glDepthFunc );
register_native_function(
	m, rootclss,
	"glDepthMask;GLboolean::",
	glue_glDepthMask );
register_native_function(
	m, rootclss,
	"glDepthRange;GLclampd::;GLclampd::",
	glue_glDepthRange );
register_native_function(
	m, rootclss,
	"glDisable;GLenum::",
	glue_glDisable );
register_native_function(
	m, rootclss,
	"glDisableClientState;GLenum::",
	glue_glDisableClientState );
register_native_function(
	m, rootclss,
	"glDrawArrays;GLenum::;GLint::;GLsizei::",
	glue_glDrawArrays );
register_native_function(
	m, rootclss,
	"glDrawBuffer;GLenum::",
	glue_glDrawBuffer );
register_native_function(
	m, rootclss,
	"glDrawElements;GLenum::;GLsizei::;GLenum::;.GLvoid::*",
	glue_glDrawElements );
register_native_function(
	m, rootclss,
	"glDrawPixels;GLsizei::;GLsizei::;GLenum::;GLenum::;.GLvoid::*",
	glue_glDrawPixels );
register_native_function(
	m, rootclss,
	"glEdgeFlag;GLboolean::",
	glue_glEdgeFlag );
register_native_function(
	m, rootclss,
	"glEdgeFlagPointer;GLsizei::;.GLvoid::*",
	glue_glEdgeFlagPointer );
register_native_function(
	m, rootclss,
	"glEdgeFlagv;.GLboolean::*",
	glue_glEdgeFlagv );
register_native_function(
	m, rootclss,
	"glEnable;GLenum::",
	glue_glEnable );
register_native_function(
	m, rootclss,
	"glEnableClientState;GLenum::",
	glue_glEnableClientState );
register_native_function(
	m, rootclss,
	"glEnd",
	glue_glEnd );
register_native_function(
	m, rootclss,
	"glEndList",
	glue_glEndList );
register_native_function(
	m, rootclss,
	"glEvalCoord1d;GLdouble::",
	glue_glEvalCoord1d );
register_native_function(
	m, rootclss,
	"glEvalCoord1dv;.GLdouble::*",
	glue_glEvalCoord1dv );
register_native_function(
	m, rootclss,
	"glEvalCoord1f;GLfloat::",
	glue_glEvalCoord1f );
register_native_function(
	m, rootclss,
	"glEvalCoord1fv;.GLfloat::*",
	glue_glEvalCoord1fv );
register_native_function(
	m, rootclss,
	"glEvalCoord2d;GLdouble::;GLdouble::",
	glue_glEvalCoord2d );
register_native_function(
	m, rootclss,
	"glEvalCoord2dv;.GLdouble::*",
	glue_glEvalCoord2dv );
register_native_function(
	m, rootclss,
	"glEvalCoord2f;GLfloat::;GLfloat::",
	glue_glEvalCoord2f );
register_native_function(
	m, rootclss,
	"glEvalCoord2fv;.GLfloat::*",
	glue_glEvalCoord2fv );
register_native_function(
	m, rootclss,
	"glEvalMesh1;GLenum::;GLint::;GLint::",
	glue_glEvalMesh1 );
register_native_function(
	m, rootclss,
	"glEvalMesh2;GLenum::;GLint::;GLint::;GLint::;GLint::",
	glue_glEvalMesh2 );
register_native_function(
	m, rootclss,
	"glEvalPoint1;GLint::",
	glue_glEvalPoint1 );
register_native_function(
	m, rootclss,
	"glEvalPoint2;GLint::;GLint::",
	glue_glEvalPoint2 );
register_native_function(
	m, rootclss,
	"glFeedbackBuffer;GLsizei::;GLenum::;GLfloat::*",
	glue_glFeedbackBuffer );
register_native_function(
	m, rootclss,
	"glFinish",
	glue_glFinish );
register_native_function(
	m, rootclss,
	"glFlush",
	glue_glFlush );
register_native_function(
	m, rootclss,
	"glFogf;GLenum::;GLfloat::",
	glue_glFogf );
register_native_function(
	m, rootclss,
	"glFogfv;GLenum::;.GLfloat::*",
	glue_glFogfv );
register_native_function(
	m, rootclss,
	"glFogi;GLenum::;GLint::",
	glue_glFogi );
register_native_function(
	m, rootclss,
	"glFogiv;GLenum::;.GLint::*",
	glue_glFogiv );
register_native_function(
	m, rootclss,
	"glFrontFace;GLenum::",
	glue_glFrontFace );
register_native_function(
	m, rootclss,
	"glFrustum;GLdouble::;GLdouble::;GLdouble::;GLdouble::;GLdouble::;GLdouble::",
	glue_glFrustum );
register_native_function(
	m, rootclss,
	"glGenLists;GLsizei::",
	glue_glGenLists );
register_native_function(
	m, rootclss,
	"glGenTextures;GLsizei::;GLuint::*",
	glue_glGenTextures );
register_native_function(
	m, rootclss,
	"glGetBooleanv;GLenum::;GLboolean::*",
	glue_glGetBooleanv );
register_native_function(
	m, rootclss,
	"glGetClipPlane;GLenum::;GLdouble::*",
	glue_glGetClipPlane );
register_native_function(
	m, rootclss,
	"glGetDoublev;GLenum::;GLdouble::*",
	glue_glGetDoublev );
register_native_function(
	m, rootclss,
	"glGetError",
	glue_glGetError );
register_native_function(
	m, rootclss,
	"glGetFloatv;GLenum::;GLfloat::*",
	glue_glGetFloatv );
register_native_function(
	m, rootclss,
	"glGetIntegerv;GLenum::;GLint::*",
	glue_glGetIntegerv );
register_native_function(
	m, rootclss,
	"glGetLightfv;GLenum::;GLenum::;GLfloat::*",
	glue_glGetLightfv );
register_native_function(
	m, rootclss,
	"glGetLightiv;GLenum::;GLenum::;GLint::*",
	glue_glGetLightiv );
register_native_function(
	m, rootclss,
	"glGetMapdv;GLenum::;GLenum::;GLdouble::*",
	glue_glGetMapdv );
register_native_function(
	m, rootclss,
	"glGetMapfv;GLenum::;GLenum::;GLfloat::*",
	glue_glGetMapfv );
register_native_function(
	m, rootclss,
	"glGetMapiv;GLenum::;GLenum::;GLint::*",
	glue_glGetMapiv );
register_native_function(
	m, rootclss,
	"glGetMaterialfv;GLenum::;GLenum::;GLfloat::*",
	glue_glGetMaterialfv );
register_native_function(
	m, rootclss,
	"glGetMaterialiv;GLenum::;GLenum::;GLint::*",
	glue_glGetMaterialiv );
register_native_function(
	m, rootclss,
	"glGetPixelMapfv;GLenum::;GLfloat::*",
	glue_glGetPixelMapfv );
register_native_function(
	m, rootclss,
	"glGetPixelMapuiv;GLenum::;GLuint::*",
	glue_glGetPixelMapuiv );
register_native_function(
	m, rootclss,
	"glGetPixelMapusv;GLenum::;GLushort::*",
	glue_glGetPixelMapusv );
register_native_function(
	m, rootclss,
	"glGetPointerv;GLenum::;GLvoid::**",
	glue_glGetPointerv );
register_native_function(
	m, rootclss,
	"glGetPolygonStipple;GLubyte::*",
	glue_glGetPolygonStipple );
register_native_function(
	m, rootclss,
	"glGetString;GLenum::",
	glue_glGetString );
register_native_function(
	m, rootclss,
	"glGetTexEnvfv;GLenum::;GLenum::;GLfloat::*",
	glue_glGetTexEnvfv );
register_native_function(
	m, rootclss,
	"glGetTexEnviv;GLenum::;GLenum::;GLint::*",
	glue_glGetTexEnviv );
register_native_function(
	m, rootclss,
	"glGetTexGendv;GLenum::;GLenum::;GLdouble::*",
	glue_glGetTexGendv );
register_native_function(
	m, rootclss,
	"glGetTexGenfv;GLenum::;GLenum::;GLfloat::*",
	glue_glGetTexGenfv );
register_native_function(
	m, rootclss,
	"glGetTexGeniv;GLenum::;GLenum::;GLint::*",
	glue_glGetTexGeniv );
register_native_function(
	m, rootclss,
	"glGetTexImage;GLenum::;GLint::;GLenum::;GLenum::;GLvoid::*",
	glue_glGetTexImage );
register_native_function(
	m, rootclss,
	"glGetTexLevelParameterfv;GLenum::;GLint::;GLenum::;GLfloat::*",
	glue_glGetTexLevelParameterfv );
register_native_function(
	m, rootclss,
	"glGetTexLevelParameteriv;GLenum::;GLint::;GLenum::;GLint::*",
	glue_glGetTexLevelParameteriv );
register_native_function(
	m, rootclss,
	"glGetTexParameterfv;GLenum::;GLenum::;GLfloat::*",
	glue_glGetTexParameterfv );
register_native_function(
	m, rootclss,
	"glGetTexParameteriv;GLenum::;GLenum::;GLint::*",
	glue_glGetTexParameteriv );
register_native_function(
	m, rootclss,
	"glHint;GLenum::;GLenum::",
	glue_glHint );
register_native_function(
	m, rootclss,
	"glIndexMask;GLuint::",
	glue_glIndexMask );
register_native_function(
	m, rootclss,
	"glIndexPointer;GLenum::;GLsizei::;.GLvoid::*",
	glue_glIndexPointer );
register_native_function(
	m, rootclss,
	"glIndexd;GLdouble::",
	glue_glIndexd );
register_native_function(
	m, rootclss,
	"glIndexdv;.GLdouble::*",
	glue_glIndexdv );
register_native_function(
	m, rootclss,
	"glIndexf;GLfloat::",
	glue_glIndexf );
register_native_function(
	m, rootclss,
	"glIndexfv;.GLfloat::*",
	glue_glIndexfv );
register_native_function(
	m, rootclss,
	"glIndexi;GLint::",
	glue_glIndexi );
register_native_function(
	m, rootclss,
	"glIndexiv;.GLint::*",
	glue_glIndexiv );
register_native_function(
	m, rootclss,
	"glIndexs;GLshort::",
	glue_glIndexs );
register_native_function(
	m, rootclss,
	"glIndexsv;.GLshort::*",
	glue_glIndexsv );
register_native_function(
	m, rootclss,
	"glIndexub;GLubyte::",
	glue_glIndexub );
register_native_function(
	m, rootclss,
	"glIndexubv;.GLubyte::*",
	glue_glIndexubv );
register_native_function(
	m, rootclss,
	"glInitNames",
	glue_glInitNames );
register_native_function(
	m, rootclss,
	"glInterleavedArrays;GLenum::;GLsizei::;.GLvoid::*",
	glue_glInterleavedArrays );
register_native_function(
	m, rootclss,
	"glIsEnabled;GLenum::",
	glue_glIsEnabled );
register_native_function(
	m, rootclss,
	"glIsList;GLuint::",
	glue_glIsList );
register_native_function(
	m, rootclss,
	"glIsTexture;GLuint::",
	glue_glIsTexture );
register_native_function(
	m, rootclss,
	"glLightModelf;GLenum::;GLfloat::",
	glue_glLightModelf );
register_native_function(
	m, rootclss,
	"glLightModelfv;GLenum::;.GLfloat::*",
	glue_glLightModelfv );
register_native_function(
	m, rootclss,
	"glLightModeli;GLenum::;GLint::",
	glue_glLightModeli );
register_native_function(
	m, rootclss,
	"glLightModeliv;GLenum::;.GLint::*",
	glue_glLightModeliv );
register_native_function(
	m, rootclss,
	"glLightf;GLenum::;GLenum::;GLfloat::",
	glue_glLightf );
register_native_function(
	m, rootclss,
	"glLightfv;GLenum::;GLenum::;.GLfloat::*",
	glue_glLightfv );
register_native_function(
	m, rootclss,
	"glLighti;GLenum::;GLenum::;GLint::",
	glue_glLighti );
register_native_function(
	m, rootclss,
	"glLightiv;GLenum::;GLenum::;.GLint::*",
	glue_glLightiv );
register_native_function(
	m, rootclss,
	"glLineStipple;GLint::;GLushort::",
	glue_glLineStipple );
register_native_function(
	m, rootclss,
	"glLineWidth;GLfloat::",
	glue_glLineWidth );
register_native_function(
	m, rootclss,
	"glListBase;GLuint::",
	glue_glListBase );
register_native_function(
	m, rootclss,
	"glLoadIdentity",
	glue_glLoadIdentity );
register_native_function(
	m, rootclss,
	"glLoadMatrixd;.GLdouble::*",
	glue_glLoadMatrixd );
register_native_function(
	m, rootclss,
	"glLoadMatrixf;.GLfloat::*",
	glue_glLoadMatrixf );
register_native_function(
	m, rootclss,
	"glLoadName;GLuint::",
	glue_glLoadName );
register_native_function(
	m, rootclss,
	"glLogicOp;GLenum::",
	glue_glLogicOp );
register_native_function(
	m, rootclss,
	"glMap1d;GLenum::;GLdouble::;GLdouble::;GLint::;GLint::;.GLdouble::*",
	glue_glMap1d );
register_native_function(
	m, rootclss,
	"glMap1f;GLenum::;GLfloat::;GLfloat::;GLint::;GLint::;.GLfloat::*",
	glue_glMap1f );
register_native_function(
	m, rootclss,
	"glMap2d;GLenum::;GLdouble::;GLdouble::;GLint::;GLint::;GLdouble::;GLdouble::;GLint::;GLint::;.GLdouble::*",
	glue_glMap2d );
register_native_function(
	m, rootclss,
	"glMap2f;GLenum::;GLfloat::;GLfloat::;GLint::;GLint::;GLfloat::;GLfloat::;GLint::;GLint::;.GLfloat::*",
	glue_glMap2f );
register_native_function(
	m, rootclss,
	"glMapGrid1d;GLint::;GLdouble::;GLdouble::",
	glue_glMapGrid1d );
register_native_function(
	m, rootclss,
	"glMapGrid1f;GLint::;GLfloat::;GLfloat::",
	glue_glMapGrid1f );
register_native_function(
	m, rootclss,
	"glMapGrid2d;GLint::;GLdouble::;GLdouble::;GLint::;GLdouble::;GLdouble::",
	glue_glMapGrid2d );
register_native_function(
	m, rootclss,
	"glMapGrid2f;GLint::;GLfloat::;GLfloat::;GLint::;GLfloat::;GLfloat::",
	glue_glMapGrid2f );
register_native_function(
	m, rootclss,
	"glMaterialf;GLenum::;GLenum::;GLfloat::",
	glue_glMaterialf );
register_native_function(
	m, rootclss,
	"glMaterialfv;GLenum::;GLenum::;.GLfloat::*",
	glue_glMaterialfv );
register_native_function(
	m, rootclss,
	"glMateriali;GLenum::;GLenum::;GLint::",
	glue_glMateriali );
register_native_function(
	m, rootclss,
	"glMaterialiv;GLenum::;GLenum::;.GLint::*",
	glue_glMaterialiv );
register_native_function(
	m, rootclss,
	"glMatrixMode;GLenum::",
	glue_glMatrixMode );
register_native_function(
	m, rootclss,
	"glMultMatrixd;.GLdouble::*",
	glue_glMultMatrixd );
register_native_function(
	m, rootclss,
	"glMultMatrixf;.GLfloat::*",
	glue_glMultMatrixf );
register_native_function(
	m, rootclss,
	"glNewList;GLuint::;GLenum::",
	glue_glNewList );
register_native_function(
	m, rootclss,
	"glNormal3b;GLbyte::;GLbyte::;GLbyte::",
	glue_glNormal3b );
register_native_function(
	m, rootclss,
	"glNormal3bv;.GLbyte::*",
	glue_glNormal3bv );
register_native_function(
	m, rootclss,
	"glNormal3d;GLdouble::;GLdouble::;GLdouble::",
	glue_glNormal3d );
register_native_function(
	m, rootclss,
	"glNormal3dv;.GLdouble::*",
	glue_glNormal3dv );
register_native_function(
	m, rootclss,
	"glNormal3f;GLfloat::;GLfloat::;GLfloat::",
	glue_glNormal3f );
register_native_function(
	m, rootclss,
	"glNormal3fv;.GLfloat::*",
	glue_glNormal3fv );
register_native_function(
	m, rootclss,
	"glNormal3i;GLint::;GLint::;GLint::",
	glue_glNormal3i );
register_native_function(
	m, rootclss,
	"glNormal3iv;.GLint::*",
	glue_glNormal3iv );
register_native_function(
	m, rootclss,
	"glNormal3s;GLshort::;GLshort::;GLshort::",
	glue_glNormal3s );
register_native_function(
	m, rootclss,
	"glNormal3sv;.GLshort::*",
	glue_glNormal3sv );
register_native_function(
	m, rootclss,
	"glNormalPointer;GLenum::;GLsizei::;.GLvoid::*",
	glue_glNormalPointer );
register_native_function(
	m, rootclss,
	"glOrtho;GLdouble::;GLdouble::;GLdouble::;GLdouble::;GLdouble::;GLdouble::",
	glue_glOrtho );
register_native_function(
	m, rootclss,
	"glPassThrough;GLfloat::",
	glue_glPassThrough );
register_native_function(
	m, rootclss,
	"glPixelMapfv;GLenum::;GLsizei::;.GLfloat::*",
	glue_glPixelMapfv );
register_native_function(
	m, rootclss,
	"glPixelMapuiv;GLenum::;GLsizei::;.GLuint::*",
	glue_glPixelMapuiv );
register_native_function(
	m, rootclss,
	"glPixelMapusv;GLenum::;GLsizei::;.GLushort::*",
	glue_glPixelMapusv );
register_native_function(
	m, rootclss,
	"glPixelStoref;GLenum::;GLfloat::",
	glue_glPixelStoref );
register_native_function(
	m, rootclss,
	"glPixelStorei;GLenum::;GLint::",
	glue_glPixelStorei );
register_native_function(
	m, rootclss,
	"glPixelTransferf;GLenum::;GLfloat::",
	glue_glPixelTransferf );
register_native_function(
	m, rootclss,
	"glPixelTransferi;GLenum::;GLint::",
	glue_glPixelTransferi );
register_native_function(
	m, rootclss,
	"glPixelZoom;GLfloat::;GLfloat::",
	glue_glPixelZoom );
register_native_function(
	m, rootclss,
	"glPointSize;GLfloat::",
	glue_glPointSize );
register_native_function(
	m, rootclss,
	"glPolygonMode;GLenum::;GLenum::",
	glue_glPolygonMode );
register_native_function(
	m, rootclss,
	"glPolygonOffset;GLfloat::;GLfloat::",
	glue_glPolygonOffset );
register_native_function(
	m, rootclss,
	"glPolygonStipple;.GLubyte::*",
	glue_glPolygonStipple );
register_native_function(
	m, rootclss,
	"glPopAttrib",
	glue_glPopAttrib );
register_native_function(
	m, rootclss,
	"glPopClientAttrib",
	glue_glPopClientAttrib );
register_native_function(
	m, rootclss,
	"glPopMatrix",
	glue_glPopMatrix );
register_native_function(
	m, rootclss,
	"glPopName",
	glue_glPopName );
register_native_function(
	m, rootclss,
	"glPrioritizeTextures;GLsizei::;.GLuint::*;.GLclampf::*",
	glue_glPrioritizeTextures );
register_native_function(
	m, rootclss,
	"glPushAttrib;GLbitfield::",
	glue_glPushAttrib );
register_native_function(
	m, rootclss,
	"glPushClientAttrib;GLbitfield::",
	glue_glPushClientAttrib );
register_native_function(
	m, rootclss,
	"glPushMatrix",
	glue_glPushMatrix );
register_native_function(
	m, rootclss,
	"glPushName;GLuint::",
	glue_glPushName );
register_native_function(
	m, rootclss,
	"glRasterPos2d;GLdouble::;GLdouble::",
	glue_glRasterPos2d );
register_native_function(
	m, rootclss,
	"glRasterPos2dv;.GLdouble::*",
	glue_glRasterPos2dv );
register_native_function(
	m, rootclss,
	"glRasterPos2f;GLfloat::;GLfloat::",
	glue_glRasterPos2f );
register_native_function(
	m, rootclss,
	"glRasterPos2fv;.GLfloat::*",
	glue_glRasterPos2fv );
register_native_function(
	m, rootclss,
	"glRasterPos2i;GLint::;GLint::",
	glue_glRasterPos2i );
register_native_function(
	m, rootclss,
	"glRasterPos2iv;.GLint::*",
	glue_glRasterPos2iv );
register_native_function(
	m, rootclss,
	"glRasterPos2s;GLshort::;GLshort::",
	glue_glRasterPos2s );
register_native_function(
	m, rootclss,
	"glRasterPos2sv;.GLshort::*",
	glue_glRasterPos2sv );
register_native_function(
	m, rootclss,
	"glRasterPos3d;GLdouble::;GLdouble::;GLdouble::",
	glue_glRasterPos3d );
register_native_function(
	m, rootclss,
	"glRasterPos3dv;.GLdouble::*",
	glue_glRasterPos3dv );
register_native_function(
	m, rootclss,
	"glRasterPos3f;GLfloat::;GLfloat::;GLfloat::",
	glue_glRasterPos3f );
register_native_function(
	m, rootclss,
	"glRasterPos3fv;.GLfloat::*",
	glue_glRasterPos3fv );
register_native_function(
	m, rootclss,
	"glRasterPos3i;GLint::;GLint::;GLint::",
	glue_glRasterPos3i );
register_native_function(
	m, rootclss,
	"glRasterPos3iv;.GLint::*",
	glue_glRasterPos3iv );
register_native_function(
	m, rootclss,
	"glRasterPos3s;GLshort::;GLshort::;GLshort::",
	glue_glRasterPos3s );
register_native_function(
	m, rootclss,
	"glRasterPos3sv;.GLshort::*",
	glue_glRasterPos3sv );
register_native_function(
	m, rootclss,
	"glRasterPos4d;GLdouble::;GLdouble::;GLdouble::;GLdouble::",
	glue_glRasterPos4d );
register_native_function(
	m, rootclss,
	"glRasterPos4dv;.GLdouble::*",
	glue_glRasterPos4dv );
register_native_function(
	m, rootclss,
	"glRasterPos4f;GLfloat::;GLfloat::;GLfloat::;GLfloat::",
	glue_glRasterPos4f );
register_native_function(
	m, rootclss,
	"glRasterPos4fv;.GLfloat::*",
	glue_glRasterPos4fv );
register_native_function(
	m, rootclss,
	"glRasterPos4i;GLint::;GLint::;GLint::;GLint::",
	glue_glRasterPos4i );
register_native_function(
	m, rootclss,
	"glRasterPos4iv;.GLint::*",
	glue_glRasterPos4iv );
register_native_function(
	m, rootclss,
	"glRasterPos4s;GLshort::;GLshort::;GLshort::;GLshort::",
	glue_glRasterPos4s );
register_native_function(
	m, rootclss,
	"glRasterPos4sv;.GLshort::*",
	glue_glRasterPos4sv );
register_native_function(
	m, rootclss,
	"glReadBuffer;GLenum::",
	glue_glReadBuffer );
register_native_function(
	m, rootclss,
	"glReadPixels;GLint::;GLint::;GLsizei::;GLsizei::;GLenum::;GLenum::;GLvoid::*",
	glue_glReadPixels );
register_native_function(
	m, rootclss,
	"glRectd;GLdouble::;GLdouble::;GLdouble::;GLdouble::",
	glue_glRectd );
register_native_function(
	m, rootclss,
	"glRectdv;.GLdouble::*;.GLdouble::*",
	glue_glRectdv );
register_native_function(
	m, rootclss,
	"glRectf;GLfloat::;GLfloat::;GLfloat::;GLfloat::",
	glue_glRectf );
register_native_function(
	m, rootclss,
	"glRectfv;.GLfloat::*;.GLfloat::*",
	glue_glRectfv );
register_native_function(
	m, rootclss,
	"glRecti;GLint::;GLint::;GLint::;GLint::",
	glue_glRecti );
register_native_function(
	m, rootclss,
	"glRectiv;.GLint::*;.GLint::*",
	glue_glRectiv );
register_native_function(
	m, rootclss,
	"glRects;GLshort::;GLshort::;GLshort::;GLshort::",
	glue_glRects );
register_native_function(
	m, rootclss,
	"glRectsv;.GLshort::*;.GLshort::*",
	glue_glRectsv );
register_native_function(
	m, rootclss,
	"glRenderMode;GLenum::",
	glue_glRenderMode );
register_native_function(
	m, rootclss,
	"glRotated;GLdouble::;GLdouble::;GLdouble::;GLdouble::",
	glue_glRotated );
register_native_function(
	m, rootclss,
	"glRotatef;GLfloat::;GLfloat::;GLfloat::;GLfloat::",
	glue_glRotatef );
register_native_function(
	m, rootclss,
	"glScaled;GLdouble::;GLdouble::;GLdouble::",
	glue_glScaled );
register_native_function(
	m, rootclss,
	"glScalef;GLfloat::;GLfloat::;GLfloat::",
	glue_glScalef );
register_native_function(
	m, rootclss,
	"glScissor;GLint::;GLint::;GLsizei::;GLsizei::",
	glue_glScissor );
register_native_function(
	m, rootclss,
	"glSelectBuffer;GLsizei::;GLuint::*",
	glue_glSelectBuffer );
register_native_function(
	m, rootclss,
	"glShadeModel;GLenum::",
	glue_glShadeModel );
register_native_function(
	m, rootclss,
	"glStencilFunc;GLenum::;GLint::;GLuint::",
	glue_glStencilFunc );
register_native_function(
	m, rootclss,
	"glStencilMask;GLuint::",
	glue_glStencilMask );
register_native_function(
	m, rootclss,
	"glStencilOp;GLenum::;GLenum::;GLenum::",
	glue_glStencilOp );
register_native_function(
	m, rootclss,
	"glTexCoord1d;GLdouble::",
	glue_glTexCoord1d );
register_native_function(
	m, rootclss,
	"glTexCoord1dv;.GLdouble::*",
	glue_glTexCoord1dv );
register_native_function(
	m, rootclss,
	"glTexCoord1f;GLfloat::",
	glue_glTexCoord1f );
register_native_function(
	m, rootclss,
	"glTexCoord1fv;.GLfloat::*",
	glue_glTexCoord1fv );
register_native_function(
	m, rootclss,
	"glTexCoord1i;GLint::",
	glue_glTexCoord1i );
register_native_function(
	m, rootclss,
	"glTexCoord1iv;.GLint::*",
	glue_glTexCoord1iv );
register_native_function(
	m, rootclss,
	"glTexCoord1s;GLshort::",
	glue_glTexCoord1s );
register_native_function(
	m, rootclss,
	"glTexCoord1sv;.GLshort::*",
	glue_glTexCoord1sv );
register_native_function(
	m, rootclss,
	"glTexCoord2d;GLdouble::;GLdouble::",
	glue_glTexCoord2d );
register_native_function(
	m, rootclss,
	"glTexCoord2dv;.GLdouble::*",
	glue_glTexCoord2dv );
register_native_function(
	m, rootclss,
	"glTexCoord2f;GLfloat::;GLfloat::",
	glue_glTexCoord2f );
register_native_function(
	m, rootclss,
	"glTexCoord2fv;.GLfloat::*",
	glue_glTexCoord2fv );
register_native_function(
	m, rootclss,
	"glTexCoord2i;GLint::;GLint::",
	glue_glTexCoord2i );
register_native_function(
	m, rootclss,
	"glTexCoord2iv;.GLint::*",
	glue_glTexCoord2iv );
register_native_function(
	m, rootclss,
	"glTexCoord2s;GLshort::;GLshort::",
	glue_glTexCoord2s );
register_native_function(
	m, rootclss,
	"glTexCoord2sv;.GLshort::*",
	glue_glTexCoord2sv );
register_native_function(
	m, rootclss,
	"glTexCoord3d;GLdouble::;GLdouble::;GLdouble::",
	glue_glTexCoord3d );
register_native_function(
	m, rootclss,
	"glTexCoord3dv;.GLdouble::*",
	glue_glTexCoord3dv );
register_native_function(
	m, rootclss,
	"glTexCoord3f;GLfloat::;GLfloat::;GLfloat::",
	glue_glTexCoord3f );
register_native_function(
	m, rootclss,
	"glTexCoord3fv;.GLfloat::*",
	glue_glTexCoord3fv );
register_native_function(
	m, rootclss,
	"glTexCoord3i;GLint::;GLint::;GLint::",
	glue_glTexCoord3i );
register_native_function(
	m, rootclss,
	"glTexCoord3iv;.GLint::*",
	glue_glTexCoord3iv );
register_native_function(
	m, rootclss,
	"glTexCoord3s;GLshort::;GLshort::;GLshort::",
	glue_glTexCoord3s );
register_native_function(
	m, rootclss,
	"glTexCoord3sv;.GLshort::*",
	glue_glTexCoord3sv );
register_native_function(
	m, rootclss,
	"glTexCoord4d;GLdouble::;GLdouble::;GLdouble::;GLdouble::",
	glue_glTexCoord4d );
register_native_function(
	m, rootclss,
	"glTexCoord4dv;.GLdouble::*",
	glue_glTexCoord4dv );
register_native_function(
	m, rootclss,
	"glTexCoord4f;GLfloat::;GLfloat::;GLfloat::;GLfloat::",
	glue_glTexCoord4f );
register_native_function(
	m, rootclss,
	"glTexCoord4fv;.GLfloat::*",
	glue_glTexCoord4fv );
register_native_function(
	m, rootclss,
	"glTexCoord4i;GLint::;GLint::;GLint::;GLint::",
	glue_glTexCoord4i );
register_native_function(
	m, rootclss,
	"glTexCoord4iv;.GLint::*",
	glue_glTexCoord4iv );
register_native_function(
	m, rootclss,
	"glTexCoord4s;GLshort::;GLshort::;GLshort::;GLshort::",
	glue_glTexCoord4s );
register_native_function(
	m, rootclss,
	"glTexCoord4sv;.GLshort::*",
	glue_glTexCoord4sv );
register_native_function(
	m, rootclss,
	"glTexCoordPointer;GLint::;GLenum::;GLsizei::;.GLvoid::*",
	glue_glTexCoordPointer );
register_native_function(
	m, rootclss,
	"glTexEnvf;GLenum::;GLenum::;GLfloat::",
	glue_glTexEnvf );
register_native_function(
	m, rootclss,
	"glTexEnvfv;GLenum::;GLenum::;.GLfloat::*",
	glue_glTexEnvfv );
register_native_function(
	m, rootclss,
	"glTexEnvi;GLenum::;GLenum::;GLint::",
	glue_glTexEnvi );
register_native_function(
	m, rootclss,
	"glTexEnviv;GLenum::;GLenum::;.GLint::*",
	glue_glTexEnviv );
register_native_function(
	m, rootclss,
	"glTexGend;GLenum::;GLenum::;GLdouble::",
	glue_glTexGend );
register_native_function(
	m, rootclss,
	"glTexGendv;GLenum::;GLenum::;.GLdouble::*",
	glue_glTexGendv );
register_native_function(
	m, rootclss,
	"glTexGenf;GLenum::;GLenum::;GLfloat::",
	glue_glTexGenf );
register_native_function(
	m, rootclss,
	"glTexGenfv;GLenum::;GLenum::;.GLfloat::*",
	glue_glTexGenfv );
register_native_function(
	m, rootclss,
	"glTexGeni;GLenum::;GLenum::;GLint::",
	glue_glTexGeni );
register_native_function(
	m, rootclss,
	"glTexGeniv;GLenum::;GLenum::;.GLint::*",
	glue_glTexGeniv );
register_native_function(
	m, rootclss,
	"glTexImage1D;GLenum::;GLint::;GLint::;GLsizei::;GLint::;GLenum::;GLenum::;.GLvoid::*",
	glue_glTexImage1D );
register_native_function(
	m, rootclss,
	"glTexImage2D;GLenum::;GLint::;GLint::;GLsizei::;GLsizei::;GLint::;GLenum::;GLenum::;.GLvoid::*",
	glue_glTexImage2D );
register_native_function(
	m, rootclss,
	"glTexParameterf;GLenum::;GLenum::;GLfloat::",
	glue_glTexParameterf );
register_native_function(
	m, rootclss,
	"glTexParameterfv;GLenum::;GLenum::;.GLfloat::*",
	glue_glTexParameterfv );
register_native_function(
	m, rootclss,
	"glTexParameteri;GLenum::;GLenum::;GLint::",
	glue_glTexParameteri );
register_native_function(
	m, rootclss,
	"glTexParameteriv;GLenum::;GLenum::;.GLint::*",
	glue_glTexParameteriv );
register_native_function(
	m, rootclss,
	"glTexSubImage1D;GLenum::;GLint::;GLint::;GLsizei::;GLenum::;GLenum::;.GLvoid::*",
	glue_glTexSubImage1D );
register_native_function(
	m, rootclss,
	"glTexSubImage2D;GLenum::;GLint::;GLint::;GLint::;GLsizei::;GLsizei::;GLenum::;GLenum::;.GLvoid::*",
	glue_glTexSubImage2D );
register_native_function(
	m, rootclss,
	"glTranslated;GLdouble::;GLdouble::;GLdouble::",
	glue_glTranslated );
register_native_function(
	m, rootclss,
	"glTranslatef;GLfloat::;GLfloat::;GLfloat::",
	glue_glTranslatef );
register_native_function(
	m, rootclss,
	"glVertex2d;GLdouble::;GLdouble::",
	glue_glVertex2d );
register_native_function(
	m, rootclss,
	"glVertex2dv;.GLdouble::*",
	glue_glVertex2dv );
register_native_function(
	m, rootclss,
	"glVertex2f;GLfloat::;GLfloat::",
	glue_glVertex2f );
register_native_function(
	m, rootclss,
	"glVertex2fv;.GLfloat::*",
	glue_glVertex2fv );
register_native_function(
	m, rootclss,
	"glVertex2i;GLint::;GLint::",
	glue_glVertex2i );
register_native_function(
	m, rootclss,
	"glVertex2iv;.GLint::*",
	glue_glVertex2iv );
register_native_function(
	m, rootclss,
	"glVertex2s;GLshort::;GLshort::",
	glue_glVertex2s );
register_native_function(
	m, rootclss,
	"glVertex2sv;.GLshort::*",
	glue_glVertex2sv );
register_native_function(
	m, rootclss,
	"glVertex3d;GLdouble::;GLdouble::;GLdouble::",
	glue_glVertex3d );
register_native_function(
	m, rootclss,
	"glVertex3dv;.GLdouble::*",
	glue_glVertex3dv );
register_native_function(
	m, rootclss,
	"glVertex3f;GLfloat::;GLfloat::;GLfloat::",
	glue_glVertex3f );
register_native_function(
	m, rootclss,
	"glVertex3fv;.GLfloat::*",
	glue_glVertex3fv );
register_native_function(
	m, rootclss,
	"glVertex3i;GLint::;GLint::;GLint::",
	glue_glVertex3i );
register_native_function(
	m, rootclss,
	"glVertex3iv;.GLint::*",
	glue_glVertex3iv );
register_native_function(
	m, rootclss,
	"glVertex3s;GLshort::;GLshort::;GLshort::",
	glue_glVertex3s );
register_native_function(
	m, rootclss,
	"glVertex3sv;.GLshort::*",
	glue_glVertex3sv );
register_native_function(
	m, rootclss,
	"glVertex4d;GLdouble::;GLdouble::;GLdouble::;GLdouble::",
	glue_glVertex4d );
register_native_function(
	m, rootclss,
	"glVertex4dv;.GLdouble::*",
	glue_glVertex4dv );
register_native_function(
	m, rootclss,
	"glVertex4f;GLfloat::;GLfloat::;GLfloat::;GLfloat::",
	glue_glVertex4f );
register_native_function(
	m, rootclss,
	"glVertex4fv;.GLfloat::*",
	glue_glVertex4fv );
register_native_function(
	m, rootclss,
	"glVertex4i;GLint::;GLint::;GLint::;GLint::",
	glue_glVertex4i );
register_native_function(
	m, rootclss,
	"glVertex4iv;.GLint::*",
	glue_glVertex4iv );
register_native_function(
	m, rootclss,
	"glVertex4s;GLshort::;GLshort::;GLshort::;GLshort::",
	glue_glVertex4s );
register_native_function(
	m, rootclss,
	"glVertex4sv;.GLshort::*",
	glue_glVertex4sv );
register_native_function(
	m, rootclss,
	"glVertexPointer;GLint::;GLenum::;GLsizei::;.GLvoid::*",
	glue_glVertexPointer );
register_native_function(
	m, rootclss,
	"glViewport;GLint::;GLint::;GLsizei::;GLsizei::",
	glue_glViewport );
}
