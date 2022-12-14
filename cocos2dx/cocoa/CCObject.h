/****************************************************************************
Copyright (c) 2010 cocos2d-x.org

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

#ifndef __CCOBJECT_H__
#define __CCOBJECT_H__

#include "cocoa/CCDataVisitor.h"

#ifdef __EMSCRIPTEN__
#include <GLES2/gl2.h>
#endif // __EMSCRIPTEN__

NS_CC_BEGIN

/**
 * @addtogroup base_nodes
 * @{
 */

class Object;
class Node;
class Event;

/** Interface that defines how to clone an object */
class CC_DLL Clonable
{
public:
	/** returns a copy of the object */
    virtual Clonable* clone() const = 0;
	virtual ~Clonable() {};

    /** returns a copy of the object.
     @deprecated Use clone() instead
     */
    CC_DEPRECATED_ATTRIBUTE Object* copy() const;
};

class CC_DLL Object
{
public:
    // object id, ScriptSupport need public _ID
    unsigned int        _ID;
    // Lua reference id
    int                 _luaID;
protected:
    // count of references
    unsigned int        _reference;
    // count of autorelease
    unsigned int        _autoReleaseCount;
public:
    Object(void);
    virtual ~Object(void);
    
    void release(void);
    void retain(void);
    Object* autorelease(void);
    bool isSingleReference(void) const;
    unsigned int retainCount(void) const;
    virtual bool isEqual(const Object* pObject);

    virtual void acceptVisitor(DataVisitor &visitor);

    virtual void update(float dt) {CC_UNUSED_PARAM(dt);};
    
    friend class AutoreleasePool;
};


typedef void (Object::*SEL_SCHEDULE)(float);
typedef void (Object::*SEL_CallFunc)();
typedef void (Object::*SEL_CallFuncN)(Node*);
typedef void (Object::*SEL_CallFuncND)(Node*, void*);
typedef void (Object::*SEL_CallFuncO)(Object*);
typedef void (Object::*SEL_MenuHandler)(Object*);
typedef void (Object::*SEL_EventHandler)(Event*);
typedef int (Object::*SEL_Compare)(Object*);

#if EMULATE_FUNCTION_POINTER_CASTS==1
#define schedule_selector(_SELECTOR) (cocos2d::SEL_SCHEDULE)(&_SELECTOR)
#define callfunc_selector(_SELECTOR) (cocos2d::SEL_CallFunc)(&_SELECTOR)
#define callfuncN_selector(_SELECTOR) (cocos2d::SEL_CallFuncN)(&_SELECTOR)
#define callfuncND_selector(_SELECTOR) (cocos2d::SEL_CallFuncND)(&_SELECTOR)
#define callfuncO_selector(_SELECTOR) (cocos2d::SEL_CallFuncO)(&_SELECTOR)
#define menu_selector(_SELECTOR) (cocos2d::SEL_MenuHandler)(&_SELECTOR)
#define event_selector(_SELECTOR) (cocos2d::SEL_EventHandler)(&_SELECTOR)
#define compare_selector(_SELECTOR) (cocos2d::SEL_Compare)(&_SELECTOR)
#else
#define schedule_selector(_SELECTOR) static_cast<cocos2d::SEL_SCHEDULE>(&_SELECTOR)
#define callfunc_selector(_SELECTOR) static_cast<cocos2d::SEL_CallFunc>(&_SELECTOR)
#define callfuncN_selector(_SELECTOR) static_cast<cocos2d::SEL_CallFuncN>(&_SELECTOR)
#define callfuncND_selector(_SELECTOR) static_cast<cocos2d::SEL_CallFuncND>(&_SELECTOR)
#define callfuncO_selector(_SELECTOR) static_cast<cocos2d::SEL_CallFuncO>(&_SELECTOR)
#define event_selector(_SELECTOR) static_cast<cocos2d::SEL_EventHandler>(&_SELECTOR)
#define compare_selector(_SELECTOR) static_cast<cocos2d::SEL_Compare>(&_SELECTOR)

#define menu_selector(_SELECTOR) menu_selector_impl(&_SELECTOR)
template <typename R, typename O, typename ... Types>
cocos2d::SEL_MenuHandler menu_selector_impl(R(O::*f)(Types ...)) {
    static_assert(std::is_same<std::integral_constant<int, sizeof ...(Types)>, std::integral_constant<int, 1>>::value,
                  "menu_selector should accept one argument - CCObject*");
    return (cocos2d::SEL_MenuHandler) f;
}
#endif

#define add_schedule_selector(func) void schedule_selector_##func(float) { func(); }
#define add_menu_selector(func) void menu_selector_##func(Object*) { func(); }

#define schedule_selector_(_SELECTOR) static_cast<cocos2d::SEL_SCHEDULE>(&_SELECTOR##_schedule_selector)
#define menu_selector_(_SELECTOR) menu_selector_impl(&_SELECTOR##_menu_selector)
#define add_schedule_selector_(func) void func##_schedule_selector(float) { func(); }
#define add_menu_selector_(func) void func##_menu_selector(Object*) { func(); }

// new callbacks based on C++11
#define CC_CALLBACK_0(__selector__,__target__, ...) std::bind(&__selector__,__target__, ##__VA_ARGS__)
#define CC_CALLBACK_1(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, ##__VA_ARGS__)
#define CC_CALLBACK_2(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)

// end of base_nodes group
/// @}

NS_CC_END

#endif // __CCOBJECT_H__
