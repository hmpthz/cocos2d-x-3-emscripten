/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2009      On-Core
 
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
#ifndef __EFFECTS_CCGRID_H__
#define __EFFECTS_CCGRID_H__

#include "cocoa/CCObject.h"
#include "base_nodes/CCNode.h"
#include "CCCamera.h"
#include "ccTypes.h"
#include "textures/CCTexture2D.h"
#include "CCDirector.h"
#include "kazmath/mat4.h"
#ifdef __EMSCRIPTEN__
#include "base_nodes/CCGLBufferedNode.h"
#endif // __EMSCRIPTEN__

NS_CC_BEGIN

class Texture2D;
class Grabber;
class GLProgram;

/**
 * @addtogroup effects
 * @{
 */

/** Base class for other
*/
class CC_DLL GridBase : public Object
{
public:
    /** create one Grid */
    static GridBase* create(const Size& gridSize, Texture2D *texture, bool flipped);
    /** create one Grid */
    static GridBase* create(const Size& gridSize);

    virtual ~GridBase(void);

    bool initWithSize(const Size& gridSize, Texture2D *texture, bool bFlipped);
    bool initWithSize(const Size& gridSize);

    /** whether or not the grid is active */
    inline bool isActive(void) { return _active; }
    void setActive(bool bActive);

    /** number of times that the grid will be reused */
    inline int getReuseGrid(void) { return _reuseGrid; }
    inline void setReuseGrid(int nReuseGrid) { _reuseGrid = nReuseGrid; }

    /** size of the grid */
    inline const Size& getGridSize(void) { return _gridSize; }
    inline void setGridSize(const Size& gridSize) { _gridSize = gridSize; }

    /** pixels between the grids */
    inline const Point& getStep(void) { return _step; }
    inline void setStep(const Point& step) { _step = step; }

    /** is texture flipped */
    inline bool isTextureFlipped(void) { return _isTextureFlipped; }
    void setTextureFlipped(bool bFlipped);

    void beforeDraw(void);
    void afterDraw(Node *target);
    virtual void blit(void);
    virtual void reuse(void);
    virtual void calculateVertexPoints(void);

    void set2DProjection(void);

protected:
    bool _active;
    int  _reuseGrid;
    Size _gridSize;
    Texture2D *_texture;
    Point _step;
    Grabber *_grabber;
    bool _isTextureFlipped;
    GLProgram* _shaderProgram;
    Director::Projection _directorProjection;
};

/**
 Grid3D is a 3D grid implementation. Each vertex has 3 dimensions: x,y,z
 */
class CC_DLL Grid3D : public GridBase
#ifdef __EMSCRIPTEN__
, public GLBufferedNode
#endif // __EMSCRIPTEN__
{
public:
    /** create one Grid */
    static Grid3D* create(const Size& gridSize, Texture2D *texture, bool bFlipped);
    /** create one Grid */
    static Grid3D* create(const Size& gridSize);

    Grid3D();
    ~Grid3D(void);

    /** returns the vertex at a given position */
    Vertex3F getVertex(const Point& pos) const;
    /** @deprecated Use getVertex() instead */
    CC_DEPRECATED_ATTRIBUTE Vertex3F vertex(const Point& pos) const { return getVertex(pos); }
    /** returns the original (non-transformed) vertex at a given position */
    Vertex3F getOriginalVertex(const Point& pos) const;
    /** @deprecated Use getOriginalVertex() instead */
    CC_DEPRECATED_ATTRIBUTE Vertex3F originalVertex(const Point& pos) const { return getOriginalVertex(pos); }

    /** sets a new vertex at a given position */
    void setVertex(const Point& pos, const Vertex3F& vertex);

    // Overrides
    virtual void blit() override;
    virtual void reuse() override;
    virtual void calculateVertexPoints() override;

protected:
    GLvoid *_texCoordinates;
    GLvoid *_vertices;
    GLvoid *_originalVertices;
    GLushort *_indices;
};

/**
 TiledGrid3D is a 3D grid implementation. It differs from Grid3D in that
 the tiles can be separated from the grid.
*/
class CC_DLL TiledGrid3D : public GridBase
#ifdef __EMSCRIPTEN__
, public GLBufferedNode
#endif // __EMSCRIPTEN__
{
public:
    /** create one Grid */
    static TiledGrid3D* create(const Size& gridSize, Texture2D *texture, bool bFlipped);
    /** create one Grid */
    static TiledGrid3D* create(const Size& gridSize);

    TiledGrid3D();
    ~TiledGrid3D();

    /** returns the tile at the given position */
    Quad3 getTile(const Point& pos) const;
    /** returns the tile at the given position */
    CC_DEPRECATED_ATTRIBUTE Quad3 tile(const Point& pos) const { return getTile(pos); }
    /** returns the original tile (untransformed) at the given position */
    Quad3 getOriginalTile(const Point& pos) const;
    /** returns the original tile (untransformed) at the given position */
    CC_DEPRECATED_ATTRIBUTE Quad3 originalTile(const Point& pos) const { return getOriginalTile(pos); }

    /** sets a new tile */
    void setTile(const Point& pos, const Quad3& coords);

    // Overrides
    virtual void blit() override;
    virtual void reuse() override;
    virtual void calculateVertexPoints() override;

protected:
    GLvoid *_texCoordinates;
    GLvoid *_vertices;
    GLvoid *_originalVertices;
    GLushort *_indices;
};

// end of effects group
/// @}

NS_CC_END

#endif // __EFFECTS_CCGRID_H__
