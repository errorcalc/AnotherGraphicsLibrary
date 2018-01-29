//
//  CoreX library
//
//  Created by error on 21.01.18.
//  Copyright © 2018 errorsoft. All rights reserved.
//

#ifndef cxTypes_h
#define cxTypes_h

#define CXTYPES_PURE_CODE

#define FakeProc(body) \
do { \
    body \
} while(0)

#define Max(a, b) \
    (((a) >= (b)) ? (a) : (b))

#define Min(a, b) \
    (((a) <= (b)) ? (a) : (b))

#define Abs(value) \
    (((value) >= 0) ? (value) : -(value))

#define Swap(a, b, t) \
FakeProc( \
    (t) = (a); \
    (a) = (b); \
    (b) = (t); \
)

#define Sign(x) \
    (((x) == 0) ? 0 : (((x) > 0) ? 1 : -1))

#define BoolTo01(b) \
    ((b) ? 1 : 0)

// ====================================================================
// Types
// ====================================================================

typedef unsigned char CXBool;

typedef char CXChar;

typedef CXChar *CXString;

typedef enum {
    SideLeft = 1,
    SideTop = 2,
    SideRight = 4,
    SideBottom = 8
} CXSide;

// ====================================================================
// CXPoint type
// ====================================================================
typedef struct {
    int x, y;
} CXPoint, *CXPointRef;

// ====================================================================
// CXRect type
// ====================================================================
typedef struct {
    int left, top, right, bottom;
} CXRect, *CXRectRef;

// --------------------------------------------------------------------
// CXPoint support
// --------------------------------------------------------------------

// Make
CXPoint CXMakePoint(int x, int y);

// Set
#define CXSetPointM(point, x, y) \
FakeProc( \
    (point).x = (x); \
    (point).y = (y); \
)
void CXSetPoint(CXPointRef point, int x, int y);

// Offset
#define CXOffsetPointM(point, dx, dy) \
FakeProc( \
    (point).x += (dx); \
    (point).y += (dy); \
)
void CXOffsetPoint(CXPointRef point, int dx, int dy);

// Is point in rect
#define CXIsPointInRectM(point, rect) \
    (((point).x >= (rect).left) && \
     ((point).y >= (rect).top) && \
     ((point).x < (rect).right) && \
     ((point).y < (rect).bottom))
CXBool CXIsPointInRect(CXPointRef point, CXRectRef rect);

// --------------------------------------------------------------------
// CXRect support
// --------------------------------------------------------------------

// Make
CXRect CXMakeRect(int left, int top, int right, int bottom);
CXRect CXMakeRectEx(int left, int top, int width, int height);

// Set
#define CXSetRectM(rect, left, top, right, bottom) \
FakeProc( \
    (rect).left = (left); \
    (rect).top = (top); \
    (rect).right = (right); \
    (rect).bottom = (bottom); \
)
void CXSetRect(CXRectRef rect, int left, int top, int right, int bottom);
#define CXSetRectExM(rect, left, top, width, height) \
FakeProc( \
    (rect).left = (left); \
    (rect).top = (top); \
    (rect).right = (left) + (width); \
    (rect).bottom = (top) + (height); \
)
void CXSetRectEx(CXRectRef rect, int left, int top, int width, int height);

// Offset
#define CXOffsetRectM(rect, dx, dy) \
FakeProc( \
    (rect).left += (dx); \
    (rect).right += (dx); \
    (rect).top += (dy); \
    (rect).bottom += (dy); \
)
void CXOffsetRect(CXRectRef rect, int dx, int dy);

// Normalize rect
#define CXNormalizeRectM(rect, t) \
FakeProc( \
    if((rect).left < (rect).right) \
        Swap((rect).left, (rect).right, (t)); \
    if((rect).top < (rect).bottom) \
        Swap((rect).top, (rect).bottom, (t)); \
)
void CXNormalizeRect(CXRectRef rect);

/// Производит обрезание линии по произвольному прямоугольнику
/// return:
/// -1: all cliped
/// 0: no cliped
/// 1: cliped point one
/// 2: cliped point two
/// 3: cliped all points
int CXRectClipLine(CXRect rect, int *x1, int *y1, int *x2, int *y2);

// clip code
#define CXCalcClipCodeM(rect, x, y) \
    (CalcClipCode((rect).left, (rect).top, (rect).right, (rect).bottom, x, y))
int CXRectCalcClipCode(CXRect rect, int x, int y);

// --------------------------------------------------------------------
// Utils
// --------------------------------------------------------------------

typedef enum {
    CCLeft = 1,
    CCTop = 2,
    CCRight = 4,
    CCBottom = 8
} CXClipCode;

/// вычисляет код обрезания точки прямоугольником
#define CalcClipCodeM(left, top, right, bottom, x, y) \
    ((((x) < (left)) ? CCLeft : 0) |                  \
     (((x) >= (right)) ? CCRight : 0) |                \
     (((y) < (top)) ? CCTop : 0) |                    \
     (((y) >= (bottom)) ? CCBottom : 0))
int CalcClipCode(int left, int top, int right, int bottom, int x, int y);

/// пересечение 2х отрезков a и b
int IsIntersectBars(int a1, int a2, int b1, int b2);

/// Производит обрезание линии по размеру bitmap-а
/// return:
/// -1: all cliped
/// 0: no cliped
/// 1: cliped point one
/// 2: cliped point two
/// 3: cliped all points
int ClipLineForSize(int width, int height, int *x1, int *y1, int *x2, int *y2);

// IsPointInRect
#define IsPointInRectM(x, y, left, top, right, bottom) \
    (((x) >= (left)) && \
     ((y) >= (top)) && \
     ((x) < (right)) && \
     ((y) < (bottom)))
int IsPointInRect(int x, int y, int left, int top, int right, int bottom);

// IsPointInSize
#define IsPointInSizeM(x, y, width, height) \
    (((x) >= 0) && \
     ((y) >= 0) && \
     ((x) < (width)) && \
     ((y) < (height)))
int IsPointInSize(int x, int y, int width, int height);

// NormalizeRectM
#define NormalizeRectM(x1, y1, x2, y2, t) \
FakeProc( \
    if((x2) < (x1)) \
        Swap((x1), (x2), (t));  \
    if((y2) < (y1)) \
        Swap((y1), (y2), (t)); \
)
void NormalizeRect(int *x1, int *y1, int *x2, int *y2);

// IsDrawRectIntersectRectM
//         *---* <- width
//         |
// 0---1---2---3
//     |
// *---* <- width
#define IsDrawRectIntersectRectM(x, y, width, height, rect) \
   (((width) >= 1) && \
    ((height) >= 1) && \
    ((x) + (width) > (rect).left) && \
    ((y) + (height) > (rect).top) && \
    ((x) < (rect).right) && \
    ((y) < (rect).bottom))
int IsDrawRectIntersectRect(int x, int y, int width, int height, CXRect rect);

// IsDrawRectIntersectSize
#define IsDrawRectIntersectSizeM(drawX, drawY, drawWidth, drawHeight, width, height) \
   (((drawWidth) >= 1) && \
    ((drawHeight) >= 1) && \
    ((drawX) + (width) > 0) && \
    ((drawY) + (height) > 0) && \
    ((drawX) < width) && \
    ((drawY) < height))
int IsDrawRectIntersectSize(int drawX, int drawY, int drawWidth, int drawHeight, int width, int height);

// ClipSpan
#define ClipSpanM(clip1, clip2, val1, val2) \
FakeProc( \
    (val1) = Max((clip1), (val1)); \
    (val2) = Min((clip2), (val2)); \
)
void ClipSpan(int clip1, int clip2, int *val1, int *val2);

// ClipValue
#define ClipValueM(min, max, val) \
( \
    ((val) >= (min)) ? ((val) <= (max) ? (val) : (max)) : (min) \
)
int ClipValue(int min, int max, int val);

// Inter
#define InterM(y1, y2, x1, x2, x) \
    ((y1) + ((y2) - (y1)) * ((x) - (x1)) / ((x2) - (x1)))
int Inter(int y1, int y2, int x1, int x2, int x);
float InterF(float y1, float y2, float x1, float x2, float x);

#endif /* cxTypes_h */
