/****************************************************************************
Copyright (c) 2012 cocos2d-x.org

http://www.cocos2d-x.org

Created by Jung Sang-Taik on 12. 3. 16..
Copyright (c) 2012 Neofect. All rights reserved.

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

#ifndef __CCScale9Sprite_H__
#define __CCScale9Sprite_H__

#include "cocos2d.h"
#include "../../ExtensionMacros.h"

NS_CC_EXT_BEGIN

/**
 * @addtogroup GUI
 * @{
 * @addtogroup control_extension
 * @{
 */

/**
 * A 9-slice sprite for cocos2d.
 *
 * 9-slice scaling allows you to specify how scaling is applied
 * to specific areas of a sprite. With 9-slice scaling (3x3 grid),
 * you can ensure that the sprite does not become distorted when
 * scaled.
 *
 * @see http://yannickloriot.com/library/ios/cccontrolextension/Classes/CCScale9Sprite.html
 */
class Scale9Sprite : public NodeRGBA
{
public:
    Scale9Sprite();
    virtual ~Scale9Sprite();

public:
    static Scale9Sprite* create();

    /**
     * Creates a 9-slice sprite with a texture file, a delimitation zone and
     * with the specified cap insets.
     *
     * @see initWithFile:rect:centerRegion:
     */
    static Scale9Sprite* create(const char* file, Rect rect,  Rect capInsets);

    /**
     * Creates a 9-slice sprite with a texture file. The whole texture will be
     * broken down into a 3??3 grid of equal blocks.
     *
     * @see initWithFile:capInsets:
     */
    static Scale9Sprite* create(Rect capInsets, const char* file);

    /**
     * Creates a 9-slice sprite with a texture file and a delimitation zone. The
     * texture will be broken down into a 3??3 grid of equal blocks.
     *
     * @see initWithFile:rect:
     */
    static Scale9Sprite* create(const char* file, Rect rect);

    /**
     * Creates a 9-slice sprite with a texture file. The whole texture will be
     * broken down into a 3??3 grid of equal blocks.
     *
     * @see initWithFile:
     */
    static Scale9Sprite* create(const char* file);

    /**
     * Creates a 9-slice sprite with an sprite frame.
     * Once the sprite is created, you can then call its "setContentSize:" method
     * to resize the sprite will all it's 9-slice goodness intract.
     * It respects the anchorPoint too.
     *
     * @see initWithSpriteFrame:
     */
    static Scale9Sprite* createWithSpriteFrame(SpriteFrame* spriteFrame);

    /**
     * Creates a 9-slice sprite with an sprite frame and the centre of its zone.
     * Once the sprite is created, you can then call its "setContentSize:" method
     * to resize the sprite will all it's 9-slice goodness intract.
     * It respects the anchorPoint too.
     *
     * @see initWithSpriteFrame:centerRegion:
     */
    static Scale9Sprite* createWithSpriteFrame(SpriteFrame* spriteFrame, Rect capInsets);

    /**
     * Creates a 9-slice sprite with an sprite frame name.
     * Once the sprite is created, you can then call its "setContentSize:" method
     * to resize the sprite will all it's 9-slice goodness intract.
     * It respects the anchorPoint too.
     *
     * @see initWithSpriteFrameName:
     */
    static Scale9Sprite* createWithSpriteFrameName(const char*spriteFrameName);

    /**
     * Creates a 9-slice sprite with an sprite frame name and the centre of its
     * zone.
     * Once the sprite is created, you can then call its "setContentSize:" method
     * to resize the sprite will all it's 9-slice goodness intract.
     * It respects the anchorPoint too.
     *
     * @see initWithSpriteFrameName:centerRegion:
     */
    static Scale9Sprite* createWithSpriteFrameName(const char*spriteFrameName, Rect capInsets);

    /**
     * Initializes a 9-slice sprite with a texture file, a delimitation zone and
     * with the specified cap insets.
     * Once the sprite is created, you can then call its "setContentSize:" method
     * to resize the sprite will all it's 9-slice goodness intract.
     * It respects the anchorPoint too.
     *
     * @param file The name of the texture file.
     * @param rect The rectangle that describes the sub-part of the texture that
     * is the whole image. If the shape is the whole texture, set this to the 
     * texture's full rect.
     * @param capInsets The values to use for the cap insets.
     */
    virtual bool initWithFile(const char* file, Rect rect,  Rect capInsets);
    
    /**
     * Initializes a 9-slice sprite with a texture file and a delimitation zone. The
     * texture will be broken down into a 3??3 grid of equal blocks.
     * Once the sprite is created, you can then call its "setContentSize:" method
     * to resize the sprite will all it's 9-slice goodness intract.
     * It respects the anchorPoint too.
     *
     * @param file The name of the texture file.
     * @param rect The rectangle that describes the sub-part of the texture that
     * is the whole image. If the shape is the whole texture, set this to the 
     * texture's full rect.
     */
    virtual bool initWithFile(const char* file, Rect rect);
    
    /**
     * Initializes a 9-slice sprite with a texture file and with the specified cap
     * insets.
     * Once the sprite is created, you can then call its "setContentSize:" method
     * to resize the sprite will all it's 9-slice goodness intract.
     * It respects the anchorPoint too.
     *
     * @param file The name of the texture file.
     * @param capInsets The values to use for the cap insets.
     */
    virtual bool initWithFile(Rect capInsets, const char* file);
    
    /**
     * Initializes a 9-slice sprite with a texture file. The whole texture will be
     * broken down into a 3??3 grid of equal blocks.
     * Once the sprite is created, you can then call its "setContentSize:" method
     * to resize the sprite will all it's 9-slice goodness intract.
     * It respects the anchorPoint too.
     *
     * @param file The name of the texture file.
     */
    virtual bool initWithFile(const char* file);
    
    /**
     * Initializes a 9-slice sprite with an sprite frame and with the specified 
     * cap insets.
     * Once the sprite is created, you can then call its "setContentSize:" method
     * to resize the sprite will all it's 9-slice goodness intract.
     * It respects the anchorPoint too.
     *
     * @param spriteFrame The sprite frame object.
     * @param capInsets The values to use for the cap insets.
     */
    virtual bool initWithSpriteFrame(SpriteFrame* spriteFrame, Rect capInsets);

    /**
     * Initializes a 9-slice sprite with an sprite frame.
     * Once the sprite is created, you can then call its "setContentSize:" method
     * to resize the sprite will all it's 9-slice goodness intract.
     * It respects the anchorPoint too.
     *
     * @param spriteFrame The sprite frame object.
     */
    virtual bool initWithSpriteFrame(SpriteFrame* spriteFrame);

    /**
     * Initializes a 9-slice sprite with an sprite frame name and with the specified 
     * cap insets.
     * Once the sprite is created, you can then call its "setContentSize:" method
     * to resize the sprite will all it's 9-slice goodness intract.
     * It respects the anchorPoint too.
     *
     * @param spriteFrameName The sprite frame name.
     * @param capInsets The values to use for the cap insets.
     */
    virtual bool initWithSpriteFrameName(const char*spriteFrameName, Rect capInsets);

    /**
     * Initializes a 9-slice sprite with an sprite frame name.
     * Once the sprite is created, you can then call its "setContentSize:" method
     * to resize the sprite will all it's 9-slice goodness intract.
     * It respects the anchorPoint too.
     *
     * @param spriteFrameName The sprite frame name.
     */
    virtual bool initWithSpriteFrameName(const char*spriteFrameName);

    virtual bool init();
    virtual bool initWithBatchNode(SpriteBatchNode* batchnode, Rect rect, bool rotated, Rect capInsets);
    virtual bool initWithBatchNode(SpriteBatchNode* batchnode, Rect rect, Rect capInsets);

    /**
     * Creates and returns a new sprite object with the specified cap insets.
     * You use this method to add cap insets to a sprite or to change the existing
     * cap insets of a sprite. In both cases, you get back a new image and the 
     * original sprite remains untouched.
     *
     * @param capInsets The values to use for the cap insets.
     */
    Scale9Sprite* resizableSpriteWithCapInsets(Rect capInsets);
    
    virtual bool updateWithBatchNode(SpriteBatchNode* batchnode, Rect rect, bool rotated, Rect capInsets);
    virtual void setSpriteFrame(SpriteFrame * spriteFrame);

    // overrides
    virtual void setContentSize(const Size & size) override;
    virtual void visit() override;
    virtual void setOpacityModifyRGB(bool bValue) override;
    virtual bool isOpacityModifyRGB(void) const override;
    virtual void setOpacity(GLubyte opacity) override;
	virtual GLubyte getOpacity() const override;
    virtual void setColor(const Color3B& color) override;
	virtual const Color3B& getColor() const override;
    virtual void updateDisplayedOpacity(GLubyte parentOpacity) override;
    virtual void updateDisplayedColor(const Color3B& parentColor) override;

protected:
    void updateCapInset();
    void updatePositions();

    bool _spritesGenerated;
    Rect _spriteRect;
    bool   _spriteFrameRotated;
    Rect _capInsetsInternal;
    bool _positionsAreDirty;

    SpriteBatchNode* _scale9Image;
    Sprite* _topLeft;
    Sprite* _top;
    Sprite* _topRight;
    Sprite* _left;
    Sprite* _centre;
    Sprite* _right;
    Sprite* _bottomLeft;
    Sprite* _bottom;
    Sprite* _bottomRight;

    bool _opacityModifyRGB;

    /** Original sprite's size. */
    CC_SYNTHESIZE_READONLY(Size, _originalSize, OriginalSize);
    /** Prefered sprite's size. By default the prefered size is the original size. */

    //if the preferredSize component is given as -1, it is ignored
    CC_PROPERTY(Size, _preferredSize, PreferredSize);
    /**
     * The end-cap insets.
     * On a non-resizeable sprite, this property is set to CGRect::ZERO; the sprite
     * does not use end caps and the entire sprite is subject to stretching.
     */
    CC_PROPERTY(Rect, _capInsets, CapInsets);
    /** Sets the left side inset */
    CC_PROPERTY(float, _insetLeft, InsetLeft);
    /** Sets the top side inset */
    CC_PROPERTY(float, _insetTop, InsetTop);
    /** Sets the right side inset */
    CC_PROPERTY(float, _insetRight, InsetRight);
    /** Sets the bottom side inset */
    CC_PROPERTY(float, _insetBottom, InsetBottom);
};

// end of GUI group
/// @}
/// @}

NS_CC_EXT_END

#endif // __CCScale9Sprite_H__
