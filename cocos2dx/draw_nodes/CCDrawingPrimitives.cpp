/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2011      Zynga Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

/*
 *
 * IMPORTANT       IMPORTANT        IMPORTANT        IMPORTANT
 *
 *
 * LEGACY FUNCTIONS
 *
 * USE DrawNode instead
 *
 */

#include "CCDrawingPrimitives.h"
#include "ccTypes.h"
#include "ccMacros.h"
#include "CCGL.h"
#include "CCDirector.h"
#include "shaders/ccGLStateCache.h"
#include "shaders/CCShaderCache.h"
#include "shaders/CCGLProgram.h"
#include "actions/CCActionCatmullRom.h"
#include <string.h>
#include <cmath>

NS_CC_BEGIN
#ifndef M_PI
    #define M_PI       3.14159265358979323846
#endif

namespace DrawPrimitives {

static bool s_initialized = false;
static GLProgram* s_shader = NULL;
static int s_colorLocation = -1;
static Color4F s_color(1.0f,1.0f,1.0f,1.0f);
static int s_pointSizeLocation = -1;
static GLfloat s_pointSize = 1.0f;

#ifdef __EMSCRIPTEN__
static GLuint s_bufferObject = 0;
static GLuint s_bufferSize = 0;

static void setGLBufferData(void *buf, GLuint bufSize)
{
    if(s_bufferSize < bufSize)
    {
        if(s_bufferObject)
        {
            glDeleteBuffers(1, &s_bufferObject);
        }
        glGenBuffers(1, &s_bufferObject);
        s_bufferSize = bufSize;

        glBindBuffer(GL_ARRAY_BUFFER, s_bufferObject);
        glBufferData(GL_ARRAY_BUFFER, bufSize, buf, GL_DYNAMIC_DRAW);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, s_bufferObject);
        glBufferSubData(GL_ARRAY_BUFFER, 0, bufSize, buf);
    }
}

#endif // __EMSCRIPTEN__

static void lazy_init( void )
{
    if( ! s_initialized ) {

        //
        // Position and 1 color passed as a uniform (to simulate glColor4ub )
        //
        s_shader = ShaderCache::getInstance()->programForKey(GLProgram::SHADER_NAME_POSITION_U_COLOR);
        s_shader->retain();
        
        s_colorLocation = glGetUniformLocation( s_shader->getProgram(), "u_color");
    CHECK_GL_ERROR_DEBUG();
        s_pointSizeLocation = glGetUniformLocation( s_shader->getProgram(), "u_pointSize");
    CHECK_GL_ERROR_DEBUG();

        s_initialized = true;
    }
}

// When switching from backround to foreground on android, we want the params to be initialized again
void init()
{
    lazy_init();
}

void free()
{
	CC_SAFE_RELEASE_NULL(s_shader);
	s_initialized = false;
}

void drawPoint( const Point& point )
{
    lazy_init();

    Vertex2F p;
    p.x = point.x;
    p.y = point.y;

    GL::enableVertexAttribs( GL::VERTEX_ATTRIB_FLAG_POSITION );
    s_shader->use();
    s_shader->setUniformsForBuiltins();

    s_shader->setUniformLocationWith4fv(s_colorLocation, (GLfloat*) &s_color.r, 1);
    s_shader->setUniformLocationWith1f(s_pointSizeLocation, s_pointSize);

#ifdef __EMSCRIPTEN__
    setGLBufferData(&p, 8);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, &p);
#endif // __EMSCRIPTEN__

    glDrawArrays(GL_POINTS, 0, 1);

    CC_INCREMENT_GL_DRAWS(1);
}

void drawPoints( const Point *points, unsigned int numberOfPoints )
{
    lazy_init();

    GL::enableVertexAttribs( GL::VERTEX_ATTRIB_FLAG_POSITION );
    s_shader->use();
    s_shader->setUniformsForBuiltins();
    s_shader->setUniformLocationWith4fv(s_colorLocation, (GLfloat*) &s_color.r, 1);
    s_shader->setUniformLocationWith1f(s_pointSizeLocation, s_pointSize);

    // XXX: Mac OpenGL error. arrays can't go out of scope before draw is executed
    Vertex2F* newPoints = new Vertex2F[numberOfPoints];

    // iPhone and 32-bit machines optimization
    if( sizeof(Point) == sizeof(Vertex2F) )
    {
#ifdef __EMSCRIPTEN__
        setGLBufferData((void*) points, numberOfPoints * sizeof(Point));
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, points);
#endif // __EMSCRIPTEN__
    }
    else
    {
        // Mac on 64-bit
        for( unsigned int i=0; i<numberOfPoints;i++) {
            newPoints[i].x = points[i].x;
            newPoints[i].y = points[i].y;
        }
#ifdef __EMSCRIPTEN__
        // Suspect Emscripten won't be emitting 64-bit code for a while yet,
        // but want to make sure this continues to work even if they do.
        setGLBufferData(newPoints, numberOfPoints * sizeof(Vertex2F));
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, newPoints);
#endif // __EMSCRIPTEN__
    }

    glDrawArrays(GL_POINTS, 0, (GLsizei) numberOfPoints);

    CC_SAFE_DELETE_ARRAY(newPoints);

    CC_INCREMENT_GL_DRAWS(1);
}


void drawLine( const Point& origin, const Point& destination )
{
    lazy_init();

    Vertex2F vertices[2] = {
        Vertex2F(origin.x, origin.y),
        Vertex2F(destination.x, destination.y)
    };

    s_shader->use();
    s_shader->setUniformsForBuiltins();
    s_shader->setUniformLocationWith4fv(s_colorLocation, (GLfloat*) &s_color.r, 1);

    GL::enableVertexAttribs( GL::VERTEX_ATTRIB_FLAG_POSITION );
#ifdef __EMSCRIPTEN__
    setGLBufferData(vertices, 16);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, vertices);
#endif // __EMSCRIPTEN__
    glDrawArrays(GL_LINES, 0, 2);

    CC_INCREMENT_GL_DRAWS(1);
}

void drawRect( Point origin, Point destination )
{
    drawLine(Point(origin.x, origin.y), Point(destination.x, origin.y));
    drawLine(Point(destination.x, origin.y), Point(destination.x, destination.y));
    drawLine(Point(destination.x, destination.y), Point(origin.x, destination.y));
    drawLine(Point(origin.x, destination.y), Point(origin.x, origin.y));
}

void drawSolidRect( Point origin, Point destination, Color4F color )
{
    Point vertices[] = {
        origin,
        Point(destination.x, origin.y),
        destination,
        Point(origin.x, destination.y)
    };

    drawSolidPoly(vertices, 4, color );
}

void drawPoly( const Point *poli, unsigned int numberOfPoints, bool closePolygon )
{
    lazy_init();

    s_shader->use();
    s_shader->setUniformsForBuiltins();
    s_shader->setUniformLocationWith4fv(s_colorLocation, (GLfloat*) &s_color.r, 1);

    GL::enableVertexAttribs( GL::VERTEX_ATTRIB_FLAG_POSITION );

    // iPhone and 32-bit machines optimization
    if( sizeof(Point) == sizeof(Vertex2F) )
    {
#ifdef __EMSCRIPTEN__
        setGLBufferData((void*) poli, numberOfPoints * sizeof(Point));
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, poli);
#endif // __EMSCRIPTEN__

        if( closePolygon )
            glDrawArrays(GL_LINE_LOOP, 0, (GLsizei) numberOfPoints);
        else
            glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) numberOfPoints);
    }
    else
    {
        // Mac on 64-bit
        // XXX: Mac OpenGL error. arrays can't go out of scope before draw is executed
        Vertex2F* newPoli = new Vertex2F[numberOfPoints];
        for( unsigned int i=0; i<numberOfPoints;i++) {
            newPoli[i].x = poli[i].x;
            newPoli[i].y = poli[i].y;
        }
#ifdef __EMSCRIPTEN__
        setGLBufferData(newPoli, numberOfPoints * sizeof(Vertex2F));
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, newPoli);
#endif // __EMSCRIPTEN__

        if( closePolygon )
            glDrawArrays(GL_LINE_LOOP, 0, (GLsizei) numberOfPoints);
        else
            glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) numberOfPoints);

        CC_SAFE_DELETE_ARRAY(newPoli);
    }

    CC_INCREMENT_GL_DRAWS(1);
}

void drawSolidPoly( const Point *poli, unsigned int numberOfPoints, Color4F color )
{
    lazy_init();

    s_shader->use();
    s_shader->setUniformsForBuiltins();
    s_shader->setUniformLocationWith4fv(s_colorLocation, (GLfloat*) &color.r, 1);

    GL::enableVertexAttribs( GL::VERTEX_ATTRIB_FLAG_POSITION );

    // XXX: Mac OpenGL error. arrays can't go out of scope before draw is executed
    Vertex2F* newPoli = new Vertex2F[numberOfPoints];

    // iPhone and 32-bit machines optimization
    if( sizeof(Point) == sizeof(Vertex2F) )
    {
#ifdef __EMSCRIPTEN__
        setGLBufferData((void*) poli, numberOfPoints * sizeof(Point));
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, poli);
#endif // __EMSCRIPTEN__
    }
    else
    {
        // Mac on 64-bit
        for( unsigned int i=0; i<numberOfPoints;i++)
        {
            newPoli[i] = Vertex2F( poli[i].x, poli[i].y );
        }
#ifdef __EMSCRIPTEN__
        setGLBufferData(newPoli, numberOfPoints * sizeof(Vertex2F));
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, newPoli);
#endif // __EMSCRIPTEN__
    }    

    glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei) numberOfPoints);

    CC_SAFE_DELETE_ARRAY(newPoli);
    CC_INCREMENT_GL_DRAWS(1);
}

void drawCircle( const Point& center, float radius, float angle, unsigned int segments, bool drawLineToCenter, float scaleX, float scaleY)
{
    lazy_init();

    int additionalSegment = 1;
    if (drawLineToCenter)
        additionalSegment++;

    const float coef = 2.0f * (float)M_PI/segments;

    GLfloat *vertices = (GLfloat*)calloc( sizeof(GLfloat)*2*(segments+2), 1);
    if( ! vertices )
        return;

    for(unsigned int i = 0;i <= segments; i++) {
        float rads = i*coef;
        GLfloat j = radius * cosf(rads + angle) * scaleX + center.x;
        GLfloat k = radius * sinf(rads + angle) * scaleY + center.y;

        vertices[i*2] = j;
        vertices[i*2+1] = k;
    }
    vertices[(segments+1)*2] = center.x;
    vertices[(segments+1)*2+1] = center.y;

    s_shader->use();
    s_shader->setUniformsForBuiltins();
    s_shader->setUniformLocationWith4fv(s_colorLocation, (GLfloat*) &s_color.r, 1);

    GL::enableVertexAttribs( GL::VERTEX_ATTRIB_FLAG_POSITION );

#ifdef __EMSCRIPTEN__
    setGLBufferData(vertices, sizeof(GLfloat)*2*(segments+2));
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, vertices);
#endif // __EMSCRIPTEN__
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) segments+additionalSegment);

    ::free( vertices );

    CC_INCREMENT_GL_DRAWS(1);
}

void drawCircle( const Point& center, float radius, float angle, unsigned int segments, bool drawLineToCenter)
{
    drawCircle(center, radius, angle, segments, drawLineToCenter, 1.0f, 1.0f);
}

void drawSolidCircle( const Point& center, float radius, float angle, unsigned int segments, float scaleX, float scaleY)
{
    lazy_init();
    
    const float coef = 2.0f * (float)M_PI/segments;
    
    GLfloat *vertices = (GLfloat*)calloc( sizeof(GLfloat)*2*(segments+2), 1);
    if( ! vertices )
        return;
    
    for(unsigned int i = 0;i <= segments; i++) {
        float rads = i*coef;
        GLfloat j = radius * cosf(rads + angle) * scaleX + center.x;
        GLfloat k = radius * sinf(rads + angle) * scaleY + center.y;
        
        vertices[i*2] = j;
        vertices[i*2+1] = k;
    }
    vertices[(segments+1)*2] = center.x;
    vertices[(segments+1)*2+1] = center.y;
    
    s_shader->use();
    s_shader->setUniformsForBuiltins();
    s_shader->setUniformLocationWith4fv(s_colorLocation, (GLfloat*) &s_color.r, 1);
    
    GL::enableVertexAttribs( GL::VERTEX_ATTRIB_FLAG_POSITION );
    
#ifdef __EMSCRIPTEN__
    setGLBufferData(vertices, sizeof(GLfloat)*2*(segments+2));
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, vertices);
#endif // __EMSCRIPTEN__
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei) segments+1);
    
    ::free( vertices );
    
    CC_INCREMENT_GL_DRAWS(1);
}

void drawSolidCircle( const Point& center, float radius, float angle, unsigned int segments)
{
    drawSolidCircle(center, radius, angle, segments, 1.0f, 1.0f);
}

void drawQuadBezier(const Point& origin, const Point& control, const Point& destination, unsigned int segments)
{
    lazy_init();

    Vertex2F* vertices = new Vertex2F[segments + 1];

    float t = 0.0f;
    for(unsigned int i = 0; i < segments; i++)
    {
        vertices[i].x = powf(1 - t, 2) * origin.x + 2.0f * (1 - t) * t * control.x + t * t * destination.x;
        vertices[i].y = powf(1 - t, 2) * origin.y + 2.0f * (1 - t) * t * control.y + t * t * destination.y;
        t += 1.0f / segments;
    }
    vertices[segments].x = destination.x;
    vertices[segments].y = destination.y;

    s_shader->use();
    s_shader->setUniformsForBuiltins();
    s_shader->setUniformLocationWith4fv(s_colorLocation, (GLfloat*) &s_color.r, 1);

    GL::enableVertexAttribs( GL::VERTEX_ATTRIB_FLAG_POSITION );

#ifdef __EMSCRIPTEN__
    setGLBufferData(vertices, (segments + 1) * sizeof(Vertex2F));
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, vertices);
#endif // __EMSCRIPTEN__
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) segments + 1);
    CC_SAFE_DELETE_ARRAY(vertices);

    CC_INCREMENT_GL_DRAWS(1);
}

void drawCatmullRom( PointArray *points, unsigned int segments )
{
    drawCardinalSpline( points, 0.5f, segments );
}

void drawCardinalSpline( PointArray *config, float tension,  unsigned int segments )
{
    lazy_init();

    Vertex2F* vertices = new Vertex2F[segments + 1];

    unsigned int p;
    float lt;
    float deltaT = 1.0f / config->count();

    for( unsigned int i=0; i < segments+1;i++) {

        float dt = (float)i / segments;

        // border
        if( dt == 1 ) {
            p = config->count() - 1;
            lt = 1;
        } else {
            p = dt / deltaT;
            lt = (dt - deltaT * (float)p) / deltaT;
        }

        // Interpolate
        Point pp0 = config->getControlPointAtIndex(p-1);
        Point pp1 = config->getControlPointAtIndex(p+0);
        Point pp2 = config->getControlPointAtIndex(p+1);
        Point pp3 = config->getControlPointAtIndex(p+2);

        Point newPos = ccCardinalSplineAt( pp0, pp1, pp2, pp3, tension, lt);
        vertices[i].x = newPos.x;
        vertices[i].y = newPos.y;
    }

    s_shader->use();
    s_shader->setUniformsForBuiltins();
    s_shader->setUniformLocationWith4fv(s_colorLocation, (GLfloat*)&s_color.r, 1);

    GL::enableVertexAttribs( GL::VERTEX_ATTRIB_FLAG_POSITION );

#ifdef __EMSCRIPTEN__
    setGLBufferData(vertices, (segments + 1) * sizeof(Vertex2F));
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, vertices);
#endif // __EMSCRIPTEN__
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) segments + 1);

    CC_SAFE_DELETE_ARRAY(vertices);
    CC_INCREMENT_GL_DRAWS(1);
}

void drawCubicBezier(const Point& origin, const Point& control1, const Point& control2, const Point& destination, unsigned int segments)
{
    lazy_init();

    Vertex2F* vertices = new Vertex2F[segments + 1];

    float t = 0;
    for(unsigned int i = 0; i < segments; i++)
    {
        vertices[i].x = powf(1 - t, 3) * origin.x + 3.0f * powf(1 - t, 2) * t * control1.x + 3.0f * (1 - t) * t * t * control2.x + t * t * t * destination.x;
        vertices[i].y = powf(1 - t, 3) * origin.y + 3.0f * powf(1 - t, 2) * t * control1.y + 3.0f * (1 - t) * t * t * control2.y + t * t * t * destination.y;
        t += 1.0f / segments;
    }
    vertices[segments].x = destination.x;
    vertices[segments].y = destination.y;

    s_shader->use();
    s_shader->setUniformsForBuiltins();
    s_shader->setUniformLocationWith4fv(s_colorLocation, (GLfloat*) &s_color.r, 1);

    GL::enableVertexAttribs( GL::VERTEX_ATTRIB_FLAG_POSITION );

#ifdef __EMSCRIPTEN__
    setGLBufferData(vertices, (segments + 1) * sizeof(Vertex2F));
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);
#else
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, vertices);
#endif // __EMSCRIPTEN__
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) segments + 1);
    CC_SAFE_DELETE_ARRAY(vertices);

    CC_INCREMENT_GL_DRAWS(1);
}

void setDrawColor4F( GLfloat r, GLfloat g, GLfloat b, GLfloat a )
{
    s_color.r = r;
    s_color.g = g;
    s_color.b = b;
    s_color.a = a;
}

void setPointSize( GLfloat pointSize )
{
    s_pointSize = pointSize * CC_CONTENT_SCALE_FACTOR();

    //TODO :glPointSize( pointSize );

}

void setDrawColor4B( GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    s_color.r = r/255.0f;
    s_color.g = g/255.0f;
    s_color.b = b/255.0f;
    s_color.a = a/255.0f;
}

} // DrawPrimitives namespace

NS_CC_END
