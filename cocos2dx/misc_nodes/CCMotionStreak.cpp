/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011 ForzeField Studios S.L.

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
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN false EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#include "CCMotionStreak.h"
#include "textures/CCTextureCache.h"
#include "shaders/ccGLStateCache.h"
#include "shaders/CCGLProgram.h"
#include "shaders/CCShaderCache.h"
#include "ccMacros.h"

#include "support/CCVertex.h"

NS_CC_BEGIN

MotionStreak::MotionStreak()
: _fastMode(false)
, _startingPositionInitialized(false)
, _texture(NULL)
, _blendFunc(BlendFunc::ALPHA_NON_PREMULTIPLIED)
, _positionR(Point::ZERO)
, _stroke(0.0f)
, _fadeDelta(0.0f)
, _minSeg(0.0f)
, _maxPoints(0)
, _nuPoints(0)
, _previousNuPoints(0)
, _pointVertexes(NULL)
, _pointState(NULL)
, _vertices(NULL)
, _colorPointer(NULL)
, _texCoords(NULL)
{
}

MotionStreak::~MotionStreak()
{
    CC_SAFE_RELEASE(_texture);
    CC_SAFE_FREE(_pointState);
    CC_SAFE_FREE(_pointVertexes);
    CC_SAFE_FREE(_vertices);
    CC_SAFE_FREE(_colorPointer);
    CC_SAFE_FREE(_texCoords);
}

MotionStreak* MotionStreak::create(float fade, float minSeg, float stroke, const Color3B& color, const char* path)
{
    MotionStreak *pRet = new MotionStreak();
    if (pRet && pRet->initWithFade(fade, minSeg, stroke, color, path))
    {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return NULL;
}

MotionStreak* MotionStreak::create(float fade, float minSeg, float stroke, const Color3B& color, Texture2D* texture)
{
    MotionStreak *pRet = new MotionStreak();
    if (pRet && pRet->initWithFade(fade, minSeg, stroke, color, texture))
    {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return NULL;
}

bool MotionStreak::initWithFade(float fade, float minSeg, float stroke, const Color3B& color, const char* path)
{
    CCASSERT(path != NULL, "Invalid filename");

    Texture2D *texture = TextureCache::getInstance()->addImage(path);
    return initWithFade(fade, minSeg, stroke, color, texture);
}

bool MotionStreak::initWithFade(float fade, float minSeg, float stroke, const Color3B& color, Texture2D* texture)
{
    Node::setPosition(Point::ZERO);
    setAnchorPoint(Point::ZERO);
    ignoreAnchorPointForPosition(true);
    _startingPositionInitialized = false;

    _positionR = Point::ZERO;
    _fastMode = true;
    _minSeg = (minSeg == -1.0f) ? stroke/5.0f : minSeg;
    _minSeg *= _minSeg;

    _stroke = stroke;
    _fadeDelta = 1.0f/fade;

    _maxPoints = (int)(fade*60.0f)+2;
    _nuPoints = 0;
    _pointState = (float *)malloc(sizeof(float) * _maxPoints);
    _pointVertexes = (Point*)malloc(sizeof(Point) * _maxPoints);

    _vertices = (Vertex2F*)malloc(sizeof(Vertex2F) * _maxPoints * 2);
    _texCoords = (Tex2F*)malloc(sizeof(Tex2F) * _maxPoints * 2);
    _colorPointer =  (GLubyte*)malloc(sizeof(GLubyte) * _maxPoints * 2 * 4);

    // Set blend mode
    _blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;

    // shader program
    setShaderProgram(ShaderCache::getInstance()->programForKey(GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR));

    setTexture(texture);
    setColor(color);
    scheduleUpdate();

    return true;
}

void MotionStreak::setPosition(const Point& position)
{
    _startingPositionInitialized = true;
    _positionR = position;
}

void MotionStreak::tintWithColor(const Color3B& colors)
{
    setColor(colors);

    // Fast assignation
    for(unsigned int i = 0; i<_nuPoints*2; i++) 
    {
        *((Color3B*) (_colorPointer+i*4)) = colors;
    }
}

Texture2D* MotionStreak::getTexture(void) const
{
    return _texture;
}

void MotionStreak::setTexture(Texture2D *texture)
{
    if (_texture != texture)
    {
        CC_SAFE_RETAIN(texture);
        CC_SAFE_RELEASE(_texture);
        _texture = texture;
    }
}

void MotionStreak::setBlendFunc(const BlendFunc &blendFunc)
{
    _blendFunc = blendFunc;
}

const BlendFunc& MotionStreak::getBlendFunc(void) const
{
    return _blendFunc;
}

void MotionStreak::setOpacity(GLubyte opacity)
{
    CCASSERT(false, "Set opacity no supported");
}

GLubyte MotionStreak::getOpacity(void) const
{
    CCASSERT(false, "Opacity no supported");
    return 0;
}

void MotionStreak::setOpacityModifyRGB(bool bValue)
{
    CC_UNUSED_PARAM(bValue);
}

bool MotionStreak::isOpacityModifyRGB(void) const
{
    return false;
}

void MotionStreak::update(float delta)
{
    if (!_startingPositionInitialized)
    {
        return;
    }
    
    delta *= _fadeDelta;

    unsigned int newIdx, newIdx2, i, i2;
    unsigned int mov = 0;

    // Update current points
    for(i = 0; i<_nuPoints; i++)
    {
        _pointState[i]-=delta;

        if(_pointState[i] <= 0)
            mov++;
        else
        {
            newIdx = i-mov;

            if(mov>0)
            {
                // Move data
                _pointState[newIdx] = _pointState[i];

                // Move point
                _pointVertexes[newIdx] = _pointVertexes[i];

                // Move vertices
                i2 = i*2;
                newIdx2 = newIdx*2;
                _vertices[newIdx2] = _vertices[i2];
                _vertices[newIdx2+1] = _vertices[i2+1];

                // Move color
                i2 *= 4;
                newIdx2 *= 4;
                _colorPointer[newIdx2+0] = _colorPointer[i2+0];
                _colorPointer[newIdx2+1] = _colorPointer[i2+1];
                _colorPointer[newIdx2+2] = _colorPointer[i2+2];
                _colorPointer[newIdx2+4] = _colorPointer[i2+4];
                _colorPointer[newIdx2+5] = _colorPointer[i2+5];
                _colorPointer[newIdx2+6] = _colorPointer[i2+6];
            }else
                newIdx2 = newIdx*8;

            const GLubyte op = (GLubyte)(_pointState[newIdx] * 255.0f);
            _colorPointer[newIdx2+3] = op;
            _colorPointer[newIdx2+7] = op;
        }
    }
    _nuPoints-=mov;

    // Append new point
    bool appendNewPoint = true;
    if(_nuPoints >= _maxPoints)
    {
        appendNewPoint = false;
    }

    else if(_nuPoints>0)
    {
        bool a1 = _pointVertexes[_nuPoints-1].getDistanceSq(_positionR) < _minSeg;
        bool a2 = (_nuPoints == 1) ? false : (_pointVertexes[_nuPoints-2].getDistanceSq(_positionR)< (_minSeg * 2.0f));
        if(a1 || a2)
        {
            appendNewPoint = false;
        }
    }

    if(appendNewPoint)
    {
        _pointVertexes[_nuPoints] = _positionR;
        _pointState[_nuPoints] = 1.0f;

        // Color assignment
        const unsigned int offset = _nuPoints*8;
        *((Color3B*)(_colorPointer + offset)) = _displayedColor;
        *((Color3B*)(_colorPointer + offset+4)) = _displayedColor;

        // Opacity
        _colorPointer[offset+3] = 255;
        _colorPointer[offset+7] = 255;

        // Generate polygon
        if(_nuPoints > 0 && _fastMode )
        {
            if(_nuPoints > 1)
            {
                ccVertexLineToPolygon(_pointVertexes, _stroke, _vertices, _nuPoints, 1);
            }
            else
            {
                ccVertexLineToPolygon(_pointVertexes, _stroke, _vertices, 0, 2);
            }
        }

        _nuPoints ++;
    }

    if( ! _fastMode )
    {
        ccVertexLineToPolygon(_pointVertexes, _stroke, _vertices, 0, _nuPoints);
    }

    // Updated Tex Coords only if they are different than previous step
    if( _nuPoints  && _previousNuPoints != _nuPoints ) {
        float texDelta = 1.0f / _nuPoints;
        for( i=0; i < _nuPoints; i++ ) {
            _texCoords[i*2] = Tex2F(0, texDelta*i);
            _texCoords[i*2+1] = Tex2F(1, texDelta*i);
        }

        _previousNuPoints = _nuPoints;
    }
}

void MotionStreak::reset()
{
    _nuPoints = 0;
}

void MotionStreak::draw()
{
    if(_nuPoints <= 1)
        return;

    CC_NODE_DRAW_SETUP();

    GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX );
    GL::blendFunc( _blendFunc.src, _blendFunc.dst );

    GL::bindTexture2D( _texture->getName() );

#ifdef __EMSCRIPTEN__
    // Size calculations from ::initWithFade
    setGLBufferData(_vertices, (sizeof(Vertex2F) * _maxPoints * 2), 0);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);

    setGLBufferData(_texCoords, (sizeof(Tex2F) * _maxPoints * 2), 1);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORDS, 2, GL_FLOAT, GL_FALSE, 0, 0);

    setGLBufferData(_colorPointer, (sizeof(GLubyte) * _maxPoints * 2 * 4), 2);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
#else
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 2, GL_FLOAT, GL_FALSE, 0, _vertices);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORDS, 2, GL_FLOAT, GL_FALSE, 0, _texCoords);
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, _colorPointer);
#endif // __EMSCRIPTEN__

    glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)_nuPoints*2);

    CC_INCREMENT_GL_DRAWS(1);
}

NS_CC_END

