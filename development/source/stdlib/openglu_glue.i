OPENGL_FUNCTION(glue_gluBeginCurve)
GLUnurbs* nurb=(GLUnurbs*)f.pop_ptr();
(gluBeginCurve(nurb));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluBeginPolygon)
GLUtesselator* tess=(GLUtesselator*)f.pop_ptr();
(gluBeginPolygon(tess));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluBeginSurface)
GLUnurbs* nurb=(GLUnurbs*)f.pop_ptr();
(gluBeginSurface(nurb));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluBeginTrim)
GLUnurbs* nurb=(GLUnurbs*)f.pop_ptr();
(gluBeginTrim(nurb));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluBuild1DMipmaps)
GLenum target=f.pop_int();
GLint internalFormat=f.pop_int();
GLsizei width=f.pop_int();
GLenum format=f.pop_int();
GLenum type=f.pop_int();
void* data=(void*)f.pop_ptr();
f.push_int(gluBuild1DMipmaps(target,internalFormat,width,format,type,data));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluBuild2DMipmaps)
GLenum target=f.pop_int();
GLint internalFormat=f.pop_int();
GLsizei width=f.pop_int();
GLsizei height=f.pop_int();
GLenum format=f.pop_int();
GLenum type=f.pop_int();
void* data=(void*)f.pop_ptr();
f.push_int(gluBuild2DMipmaps(target,internalFormat,width,height,format,type,data));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluCylinder)
GLUquadric* quad=(GLUquadric*)f.pop_ptr();
GLdouble base=f.pop_double();
GLdouble top=f.pop_double();
GLdouble height=f.pop_double();
GLint slices=f.pop_int();
GLint stacks=f.pop_int();
(gluCylinder(quad,base,top,height,slices,stacks));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluDeleteNurbsRenderer)
GLUnurbs* nurb=(GLUnurbs*)f.pop_ptr();
(gluDeleteNurbsRenderer(nurb));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluDeleteQuadric)
GLUquadric* quad=(GLUquadric*)f.pop_ptr();
(gluDeleteQuadric(quad));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluDeleteTess)
GLUtesselator* tess=(GLUtesselator*)f.pop_ptr();
(gluDeleteTess(tess));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluDisk)
GLUquadric* quad=(GLUquadric*)f.pop_ptr();
GLdouble inner=f.pop_double();
GLdouble outer=f.pop_double();
GLint slices=f.pop_int();
GLint loops=f.pop_int();
(gluDisk(quad,inner,outer,slices,loops));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluEndCurve)
GLUnurbs* nurb=(GLUnurbs*)f.pop_ptr();
(gluEndCurve(nurb));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluEndPolygon)
GLUtesselator* tess=(GLUtesselator*)f.pop_ptr();
(gluEndPolygon(tess));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluEndSurface)
GLUnurbs* nurb=(GLUnurbs*)f.pop_ptr();
(gluEndSurface(nurb));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluEndTrim)
GLUnurbs* nurb=(GLUnurbs*)f.pop_ptr();
(gluEndTrim(nurb));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluErrorString)
GLenum error=f.pop_int();
f.push_ptr((void*)gluErrorString(error));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluGetNurbsProperty)
GLUnurbs* nurb=(GLUnurbs*)f.pop_ptr();
GLenum property=f.pop_int();
GLfloat* data=(GLfloat*)f.pop_ptr();
(gluGetNurbsProperty(nurb,property,data));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluGetString)
GLenum name=f.pop_int();
f.push_ptr((void*)gluGetString(name));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluGetTessProperty)
GLUtesselator* tess=(GLUtesselator*)f.pop_ptr();
GLenum which=f.pop_int();
GLdouble* data=(GLdouble*)f.pop_ptr();
(gluGetTessProperty(tess,which,data));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluLoadSamplingMatrices)
GLUnurbs* nurb=(GLUnurbs*)f.pop_ptr();
const GLfloat* model=(const GLfloat*)f.pop_ptr();
const GLfloat* perspective=(const GLfloat*)f.pop_ptr();
const GLint* view=(const GLint*)f.pop_ptr();
(gluLoadSamplingMatrices(nurb,model,perspective,view));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluLookAt)
GLdouble eyeX=f.pop_double();
GLdouble eyeY=f.pop_double();
GLdouble eyeZ=f.pop_double();
GLdouble centerX=f.pop_double();
GLdouble centerY=f.pop_double();
GLdouble centerZ=f.pop_double();
GLdouble upX=f.pop_double();
GLdouble upY=f.pop_double();
GLdouble upZ=f.pop_double();
(gluLookAt(eyeX,eyeY,eyeZ,centerX,centerY,centerZ,upX,upY,upZ));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluNewNurbsRenderer)
f.push_ptr((void*)gluNewNurbsRenderer());
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluNewQuadric)
f.push_ptr((void*)gluNewQuadric());
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluNewTess)
f.push_ptr((void*)gluNewTess());
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluNextContour)
GLUtesselator* tess=(GLUtesselator*)f.pop_ptr();
GLenum type=f.pop_int();
(gluNextContour(tess,type));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluNurbsCurve)
GLUnurbs* nurb=(GLUnurbs*)f.pop_ptr();
GLint knotCount=f.pop_int();
GLfloat* knots=(GLfloat*)f.pop_ptr();
GLint stride=f.pop_int();
GLfloat* control=(GLfloat*)f.pop_ptr();
GLint order=f.pop_int();
GLenum type=f.pop_int();
(gluNurbsCurve(nurb,knotCount,knots,stride,control,order,type));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluNurbsProperty)
GLUnurbs* nurb=(GLUnurbs*)f.pop_ptr();
GLenum property=f.pop_int();
GLfloat value=f.pop_float();
(gluNurbsProperty(nurb,property,value));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluNurbsSurface)
GLUnurbs* nurb=(GLUnurbs*)f.pop_ptr();
GLint sKnotCount=f.pop_int();
GLfloat* sKnots=(GLfloat*)f.pop_ptr();
GLint tKnotCount=f.pop_int();
GLfloat* tKnots=(GLfloat*)f.pop_ptr();
GLint sStride=f.pop_int();
GLint tStride=f.pop_int();
GLfloat* control=(GLfloat*)f.pop_ptr();
GLint sOrder=f.pop_int();
GLint tOrder=f.pop_int();
GLenum type=f.pop_int();
(gluNurbsSurface(nurb,sKnotCount,sKnots,tKnotCount,tKnots,sStride,tStride,control,sOrder,tOrder,type));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluOrtho2D)
GLdouble left=f.pop_double();
GLdouble right=f.pop_double();
GLdouble bottom=f.pop_double();
GLdouble top=f.pop_double();
(gluOrtho2D(left,right,bottom,top));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluPartialDisk)
GLUquadric* quad=(GLUquadric*)f.pop_ptr();
GLdouble inner=f.pop_double();
GLdouble outer=f.pop_double();
GLint slices=f.pop_int();
GLint loops=f.pop_int();
GLdouble start=f.pop_double();
GLdouble sweep=f.pop_double();
(gluPartialDisk(quad,inner,outer,slices,loops,start,sweep));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluPerspective)
GLdouble fovy=f.pop_double();
GLdouble aspect=f.pop_double();
GLdouble zNear=f.pop_double();
GLdouble zFar=f.pop_double();
(gluPerspective(fovy,aspect,zNear,zFar));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluPickMatrix)
GLdouble x=f.pop_double();
GLdouble y=f.pop_double();
GLdouble delX=f.pop_double();
GLdouble delY=f.pop_double();
GLint* viewport=(GLint*)f.pop_ptr();
(gluPickMatrix(x,y,delX,delY,viewport));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluProject)
GLdouble objX=f.pop_double();
GLdouble objY=f.pop_double();
GLdouble objZ=f.pop_double();
const GLdouble* model=(const GLdouble*)f.pop_ptr();
const GLdouble* proj=(const GLdouble*)f.pop_ptr();
const GLint* view=(const GLint*)f.pop_ptr();
GLdouble* winX=(GLdouble*)f.pop_ptr();
GLdouble* winY=(GLdouble*)f.pop_ptr();
GLdouble* winZ=(GLdouble*)f.pop_ptr();
f.push_int(gluProject(objX,objY,objZ,model,proj,view,winX,winY,winZ));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluPwlCurve)
GLUnurbs* nurb=(GLUnurbs*)f.pop_ptr();
GLint count=f.pop_int();
GLfloat* data=(GLfloat*)f.pop_ptr();
GLint stride=f.pop_int();
GLenum type=f.pop_int();
(gluPwlCurve(nurb,count,data,stride,type));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluQuadricDrawStyle)
GLUquadric* quad=(GLUquadric*)f.pop_ptr();
GLenum draw=f.pop_int();
(gluQuadricDrawStyle(quad,draw));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluQuadricNormals)
GLUquadric* quad=(GLUquadric*)f.pop_ptr();
GLenum normal=f.pop_int();
(gluQuadricNormals(quad,normal));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluQuadricOrientation)
GLUquadric* quad=(GLUquadric*)f.pop_ptr();
GLenum orientation=f.pop_int();
(gluQuadricOrientation(quad,orientation));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluQuadricTexture)
GLUquadric* quad=(GLUquadric*)f.pop_ptr();
GLboolean texture=f.pop_byte();
(gluQuadricTexture(quad,texture));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluScaleImage)
GLenum format=f.pop_int();
GLsizei wIn=f.pop_int();
GLsizei hIn=f.pop_int();
GLenum typeIn=f.pop_int();
void* dataIn=(void*)f.pop_ptr();
GLsizei wOut=f.pop_int();
GLsizei hOut=f.pop_int();
GLenum typeOut=f.pop_int();
GLvoid* dataOut=(GLvoid*)f.pop_ptr();
f.push_int(gluScaleImage(format,wIn,hIn,typeIn,dataIn,wOut,hOut,typeOut,dataOut));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluSphere)
GLUquadric* quad=(GLUquadric*)f.pop_ptr();
GLdouble radius=f.pop_double();
GLint slices=f.pop_int();
GLint stacks=f.pop_int();
(gluSphere(quad,radius,slices,stacks));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluTessBeginContour)
GLUtesselator* tess=(GLUtesselator*)f.pop_ptr();
(gluTessBeginContour(tess));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluTessBeginPolygon)
GLUtesselator* tess=(GLUtesselator*)f.pop_ptr();
GLvoid* data=(GLvoid*)f.pop_ptr();
(gluTessBeginPolygon(tess,data));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluTessEndContour)
GLUtesselator* tess=(GLUtesselator*)f.pop_ptr();
(gluTessEndContour(tess));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluTessEndPolygon)
GLUtesselator* tess=(GLUtesselator*)f.pop_ptr();
(gluTessEndPolygon(tess));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluTessNormal)
GLUtesselator* tess=(GLUtesselator*)f.pop_ptr();
GLdouble valueX=f.pop_double();
GLdouble valueY=f.pop_double();
GLdouble valueZ=f.pop_double();
(gluTessNormal(tess,valueX,valueY,valueZ));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluTessProperty)
GLUtesselator* tess=(GLUtesselator*)f.pop_ptr();
GLenum which=f.pop_int();
GLdouble data=f.pop_double();
(gluTessProperty(tess,which,data));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluTessVertex)
GLUtesselator* tess=(GLUtesselator*)f.pop_ptr();
GLdouble* location=(GLdouble*)f.pop_ptr();
GLvoid* data=(GLvoid*)f.pop_ptr();
(gluTessVertex(tess,location,data));
END_STDLIB_FUNCTION
OPENGL_FUNCTION(glue_gluUnProject)
GLdouble winX=f.pop_double();
GLdouble winY=f.pop_double();
GLdouble winZ=f.pop_double();
const GLdouble* model=(const GLdouble*)f.pop_ptr();
const GLdouble* proj=(const GLdouble*)f.pop_ptr();
const GLint* view=(const GLint*)f.pop_ptr();
GLdouble* objX=(GLdouble*)f.pop_ptr();
GLdouble* objY=(GLdouble*)f.pop_ptr();
GLdouble* objZ=(GLdouble*)f.pop_ptr();
f.push_int(gluUnProject(winX,winY,winZ,model,proj,view,objX,objY,objZ));
END_STDLIB_FUNCTION


void init_openglu( machine_t& m ) {
class_t* rootclss = m.get_root_class();
register_native_function(
	m, rootclss,
	"gluBeginCurve;GLUnurbs::*",
	glue_gluBeginCurve );
register_native_function(
	m, rootclss,
	"gluBeginPolygon;GLUtesselator::*",
	glue_gluBeginPolygon );
register_native_function(
	m, rootclss,
	"gluBeginSurface;GLUnurbs::*",
	glue_gluBeginSurface );
register_native_function(
	m, rootclss,
	"gluBeginTrim;GLUnurbs::*",
	glue_gluBeginTrim );
register_native_function(
	m, rootclss,
	"gluBuild1DMipmaps;GLenum::;GLint::;GLsizei::;GLenum::;GLenum::;v*",
	glue_gluBuild1DMipmaps );
register_native_function(
	m, rootclss,
	"gluBuild2DMipmaps;GLenum::;GLint::;GLsizei::;GLsizei::;GLenum::;GLenum::;v*",
	glue_gluBuild2DMipmaps );
register_native_function(
	m, rootclss,
	"gluCylinder;GLUquadric::*;GLdouble::;GLdouble::;GLdouble::;GLint::;GLint::",
	glue_gluCylinder );
register_native_function(
	m, rootclss,
	"gluDeleteNurbsRenderer;GLUnurbs::*",
	glue_gluDeleteNurbsRenderer );
register_native_function(
	m, rootclss,
	"gluDeleteQuadric;GLUquadric::*",
	glue_gluDeleteQuadric );
register_native_function(
	m, rootclss,
	"gluDeleteTess;GLUtesselator::*",
	glue_gluDeleteTess );
register_native_function(
	m, rootclss,
	"gluDisk;GLUquadric::*;GLdouble::;GLdouble::;GLint::;GLint::",
	glue_gluDisk );
register_native_function(
	m, rootclss,
	"gluEndCurve;GLUnurbs::*",
	glue_gluEndCurve );
register_native_function(
	m, rootclss,
	"gluEndPolygon;GLUtesselator::*",
	glue_gluEndPolygon );
register_native_function(
	m, rootclss,
	"gluEndSurface;GLUnurbs::*",
	glue_gluEndSurface );
register_native_function(
	m, rootclss,
	"gluEndTrim;GLUnurbs::*",
	glue_gluEndTrim );
register_native_function(
	m, rootclss,
	"gluErrorString;GLenum::",
	glue_gluErrorString );
register_native_function(
	m, rootclss,
	"gluGetNurbsProperty;GLUnurbs::*;GLenum::;GLfloat::*",
	glue_gluGetNurbsProperty );
register_native_function(
	m, rootclss,
	"gluGetString;GLenum::",
	glue_gluGetString );
register_native_function(
	m, rootclss,
	"gluGetTessProperty;GLUtesselator::*;GLenum::;GLdouble::*",
	glue_gluGetTessProperty );
register_native_function(
	m, rootclss,
	"gluLoadSamplingMatrices;GLUnurbs::*;.GLfloat::*;.GLfloat::*;.GLint::*",
	glue_gluLoadSamplingMatrices );
register_native_function(
	m, rootclss,
	"gluLookAt;GLdouble::;GLdouble::;GLdouble::;GLdouble::;GLdouble::;GLdouble::;GLdouble::;GLdouble::;GLdouble::",
	glue_gluLookAt );
register_native_function(
	m, rootclss,
	"gluNewNurbsRenderer",
	glue_gluNewNurbsRenderer );
register_native_function(
	m, rootclss,
	"gluNewQuadric",
	glue_gluNewQuadric );
register_native_function(
	m, rootclss,
	"gluNewTess",
	glue_gluNewTess );
register_native_function(
	m, rootclss,
	"gluNextContour;GLUtesselator::*;GLenum::",
	glue_gluNextContour );
register_native_function(
	m, rootclss,
	"gluNurbsCurve;GLUnurbs::*;GLint::;GLfloat::*;GLint::;GLfloat::*;GLint::;GLenum::",
	glue_gluNurbsCurve );
register_native_function(
	m, rootclss,
	"gluNurbsProperty;GLUnurbs::*;GLenum::;GLfloat::",
	glue_gluNurbsProperty );
register_native_function(
	m, rootclss,
	"gluNurbsSurface;GLUnurbs::*;GLint::;GLfloat::*;GLint::;GLfloat::*;GLint::;GLint::;GLfloat::*;GLint::;GLint::;GLenum::",
	glue_gluNurbsSurface );
register_native_function(
	m, rootclss,
	"gluOrtho2D;GLdouble::;GLdouble::;GLdouble::;GLdouble::",
	glue_gluOrtho2D );
register_native_function(
	m, rootclss,
	"gluPartialDisk;GLUquadric::*;GLdouble::;GLdouble::;GLint::;GLint::;GLdouble::;GLdouble::",
	glue_gluPartialDisk );
register_native_function(
	m, rootclss,
	"gluPerspective;GLdouble::;GLdouble::;GLdouble::;GLdouble::",
	glue_gluPerspective );
register_native_function(
	m, rootclss,
	"gluPickMatrix;GLdouble::;GLdouble::;GLdouble::;GLdouble::;GLint::*",
	glue_gluPickMatrix );
register_native_function(
	m, rootclss,
	"gluProject;GLdouble::;GLdouble::;GLdouble::;.GLdouble::*;.GLdouble::*;.GLint::*;GLdouble::*;GLdouble::*;GLdouble::*",
	glue_gluProject );
register_native_function(
	m, rootclss,
	"gluPwlCurve;GLUnurbs::*;GLint::;GLfloat::*;GLint::;GLenum::",
	glue_gluPwlCurve );
register_native_function(
	m, rootclss,
	"gluQuadricDrawStyle;GLUquadric::*;GLenum::",
	glue_gluQuadricDrawStyle );
register_native_function(
	m, rootclss,
	"gluQuadricNormals;GLUquadric::*;GLenum::",
	glue_gluQuadricNormals );
register_native_function(
	m, rootclss,
	"gluQuadricOrientation;GLUquadric::*;GLenum::",
	glue_gluQuadricOrientation );
register_native_function(
	m, rootclss,
	"gluQuadricTexture;GLUquadric::*;GLboolean::",
	glue_gluQuadricTexture );
register_native_function(
	m, rootclss,
	"gluScaleImage;GLenum::;GLsizei::;GLsizei::;GLenum::;v*;GLsizei::;GLsizei::;GLenum::;GLvoid::*",
	glue_gluScaleImage );
register_native_function(
	m, rootclss,
	"gluSphere;GLUquadric::*;GLdouble::;GLint::;GLint::",
	glue_gluSphere );
register_native_function(
	m, rootclss,
	"gluTessBeginContour;GLUtesselator::*",
	glue_gluTessBeginContour );
register_native_function(
	m, rootclss,
	"gluTessBeginPolygon;GLUtesselator::*;GLvoid::*",
	glue_gluTessBeginPolygon );
register_native_function(
	m, rootclss,
	"gluTessEndContour;GLUtesselator::*",
	glue_gluTessEndContour );
register_native_function(
	m, rootclss,
	"gluTessEndPolygon;GLUtesselator::*",
	glue_gluTessEndPolygon );
register_native_function(
	m, rootclss,
	"gluTessNormal;GLUtesselator::*;GLdouble::;GLdouble::;GLdouble::",
	glue_gluTessNormal );
register_native_function(
	m, rootclss,
	"gluTessProperty;GLUtesselator::*;GLenum::;GLdouble::",
	glue_gluTessProperty );
register_native_function(
	m, rootclss,
	"gluTessVertex;GLUtesselator::*;GLdouble::*;GLvoid::*",
	glue_gluTessVertex );
register_native_function(
	m, rootclss,
	"gluUnProject;GLdouble::;GLdouble::;GLdouble::;.GLdouble::*;.GLdouble::*;.GLint::*;GLdouble::*;GLdouble::*;GLdouble::*",
	glue_gluUnProject );
}
