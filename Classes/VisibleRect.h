#ifndef __VISIBLERECT_H__
#define __VISIBLERECT_H__

#include "cocos2d.h"
USING_NS_CC;

class VisibleRect
{
public:
    static Rect getVisibleRect();
    
    static Point left();
    static Point right();
    static Point top();
    static Point bottom();
    static Point center();
    static Point leftTop();
    static Point rightTop();
    static Point leftBottom();
    static Point rightBottom();
private:
    static void lazyInit();
    static Rect s_visibleRect;
};

float scene_fix_coordinate(float cur, float origin, float fsize, float msize);
float scene_fix_scale(float scale, float fsize, float msize);

#endif /* __VISIBLERECT_H__ */
