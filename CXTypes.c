//
//  CoreX library
//
//  Created by error on 21.01.18.
//  Copyright © 2018 errorsoft. All rights reserved.
//

#include "CXTypes.h"

// --------------------------------------------------------------------
// CXPoint support
// --------------------------------------------------------------------

CXPoint CXMakePoint(int x, int y) {
    CXPoint point;
    point.x = x;
    point.y = y;
    return point;
}

void CXSetPoint(CXPointRef point, int x, int y) {
    CXSetPointM(*point, x, y);
}

void CXOffsetPoint(CXPointRef point, int dx, int dy) {
    CXOffsetPointM(*point, dx, dy);
}

CXBool CXIsPointInRect(CXPointRef point, CXRectRef rect) {
    return CXIsPointInRectM(*point, *rect);
}

// --------------------------------------------------------------------
// CXRect support
// --------------------------------------------------------------------

// Make
CXRect CXMakeRect(int left, int top, int right, int bottom) {
    CXRect rect;
    rect.left = left;
    rect.top = top;
    rect.right = right;
    rect.bottom = bottom;
    return rect;
}

CXRect CXMakeRectEx(int left, int top, int width, int height) {
    CXRect rect;
    rect.left = left;
    rect.top = top;
    rect.right = left + width;
    rect.bottom = top + height;
    return rect;
}

void CXSetRect(CXRectRef rect, int left, int top, int right, int bottom) {
    CXSetRectM(*rect, left, top, right, bottom);
}

void CXSetRectEx(CXRectRef rect, int left, int top, int width, int height) {
    CXSetRectExM(*rect, left, top, width, height);
}

void CXOffsetRect(CXRectRef rect, int dx, int dy) {
    CXOffsetRectM(*rect, dx, dy);
}

void CXNormalizeRect(CXRectRef rect) {
    int t;
    CXNormalizeRectM(*rect, t);
}

int CXRectClipLine(CXRect rect, int *x1, int *y1, int *x2, int *y2) {
    int p1, p2;
    int *t;
    int result = 0;
    
    p1 = CXCalcClipCodeM(rect, *x1, *y1);
    p2 = CXCalcClipCodeM(rect, *x2, *y2);
    
    result += p1 + p2 * 2;
    
    // if need clipping
    while(p1 | p2) {
        
        // p1 and p2 locate on some side, all clipped
        if(p1 & p2)
            return -1;
        
        if (!p1) {
            t = y1; y1 = y2; y2 = t;
            t = x1; x1 = x2; x2 = t;
            
            p1 = p2;
            p2 = 0;
        }
        
        if(p1 & CCLeft) {
            *y1 = InterM(*y1, *y2, *x1, *x2, rect.left);
            *x1 = rect.left;
        } else if(p1 & CCTop) {
            *x1 = InterM(*x1, *x2, *y1, *y2, rect.top);
            *y1 = rect.top;
        } else if(p1 & CCRight) {
            *y1 = InterM(*y1, *y2, *x1, *x2, rect.right - 1);
            *x1 = rect.right - 1;
        } else if(p1 & CCBottom) {
            *x1 = InterM(*x1, *x2, *y1, *y2, rect.bottom - 1);
            *y1 = rect.bottom - 1;
        }
        
        p1 = CXCalcClipCodeM(rect, *x1, *y1);
    }
    
    return result;
}

int CXCalcClipCode(CXRect rect, int x, int y) {
    return CalcClipCode(rect.left, rect.top, rect.right, rect.bottom, x, y);
}

// --------------------------------------------------------------------
// Utils
// --------------------------------------------------------------------

int CalcClipCode(int left, int top, int right, int bottom, int x, int y) {
    return CalcClipCodeM(left, top, right, bottom, x, y);
}

int IsIntersectBars(int a1, int a2, int b1, int b2) {
    int t;
    
    if(a1 > a2) {
        Swap(a1, a2, t);
    }
    
    if(b1 > b2) {
        Swap(b1, b2, t);
    }
    
    return Max(a1, b1) <= Min(a2, b2);
}

int ClipLineForSize(int width, int height, int *x1, int *y1, int *x2, int *y2) {
    int p1, p2;
    int *t;
    int result = 0;
    
    p1 = CalcClipCodeM(0, 0, width, height, *x1, *y1);
    p2 = CalcClipCodeM(0, 0, width, height, *x2, *y2);
    
    result += p1 + p2 * 2;
    
    // if need clipping
    while(p1 | p2) {
        
        // p1 and p2 locate on some side, all clipped
        if(p1 & p2)
            return -1;
        
        if (!p1) {
            t = y1; y1 = y2; y2 = t;
            t = x1; x1 = x2; x2 = t;
            
            p1 = p2;
            p2 = 0;
        }
        
        if(p1 & CCLeft) {
            *y1 = InterM(*y1, *y2, *x1, *x2, 0);
            *x1 = 0;
        } else if(p1 & CCTop) {
            *x1 = InterM(*x1, *x2, *y1, *y2, 0);
            *y1 = 0;
        } else if(p1 & CCRight) {
            *y1 = InterM(*y1, *y2, *x1, *x2, width - 1);
            *x1 = width - 1;
        } else if(p1 & CCBottom) {
            *x1 = InterM(*x1, *x2, *y1, *y2, height - 1);
            *y1 = height - 1;
        }
        
        p1 = CalcClipCodeM(0, 0, width, height, *x1, *y1);
    }
    
    return result;
}

int IsPointInRect(int x, int y, int left, int top, int right, int bottom) {
    return IsPointInRectM(x, y, left, top, right, bottom);
}

int IsPointInSize(int x, int y, int width, int height) {
    return IsPointInSizeM(x, y, width, height);
}

int IsDrawRectIntersectRect(int x, int y, int width, int height, CXRect rect) {
    return IsDrawRectIntersectRectM(x, y, width, height, rect);
}

int IsDrawRectIntersectSize(int drawX, int drawY, int drawWidth, int drawHeight, int width, int height) {
    return IsDrawRectIntersectSizeM(drawX, drawY, drawWidth, drawHeight, width, height);
}

void NormalizeRect(int *x1, int *y1, int *x2, int *y2) {
    int t;
    NormalizeRectM(*x1, *y1, *x2, *y2, t);
}

void ClipSpan(int clip1, int clip2, int *val1, int *val2) {
    ClipSpanM(clip1, clip2, *val1, *val2);
}

int ClipValue(int min, int max, int val) {
    return ClipValueM(min, max, val);
}

int Inter(int y1, int y2, int x1, int x2, int x) {
    return InterM(y1, y2, x1, x2, x);
}

float InterF(float y1, float y2, float x1, float x2, float x) {
    return InterM(y1, y2, x1, x2, x);
}




