//
//  ZX Graphics library, part of Another Graphics library
//
//  Created by error on 14.01.18.
//  Copyright © 2018 errorsoft. All rights reserved.
//

#include "ZXGraphics.h"
#include "ZXGraphicsBitBlit.h"
#include <stdlib.h>
#include <assert.h>
#include <memory.h>

#define false 0
#define true 1

#define PixelOn ZXFastPixelOn
#define PixelOff ZXFastPixelOff
#define PixelChange ZXFastPixelChange
#define SetPixel ZXFastSetPixel
#define GetPixel ZXFastGetPixel

#define CalcScanSize ZXCalcScanSizeM
#define CalcFullBytes ZXCalcFullBytesM
#define PixelPtrData ZXPixelPtrDataM
#define BitmapScanSize ZXBitmapScanSizeM
#define BitmapIsOffset ZXBitmapIsOffsetM
#define BitmapIsClipRect ZXBitmapIsClipRectM
#define BitmapIsDrawLastPixel ZXBitmapIsDrawLastPixelM
#define BitmapPixelPtr ZXBitmapPixelPtrM
#define BitmapBeginPtr ZXBitmapBeginPtrM
#define BitmapEndPtr ZXBitmapEndPtrM
#define BitmapDataSize ZXBitmapDataSizeM
#define ClipSpan ClipSpanM
#define ClipValue ClipValueM
#define IsDrawRectIntersectRect IsDrawRectIntersectRectM
#define IsDrawRectIntersectSize IsDrawRectIntersectSizeM

// ====================================================================
// Macros
// ====================================================================

#define OffsetPoint(px, py, offset) \
    do { \
        (px) += (offset).x; \
        (py) += (offset).y; \
    } while(0)

#define IsPointClippedForBitmapSize(bitmap, x, y) \
    (((x) < 0) || ((y) < 0) || (x >= (bitmap)->width) || (y >= (bitmap)->height))

#define IsPointClippedForRect(rect, x, y) \
    ((x < (rect).left) || (y < (rect).top) || (x >= (rect).right) || (y >= (rect).bottom))

#ifdef USE_CLIPRECT
    #define IsPointClippedForBitmap(bitmap, x, y) \
        ( ZXBitmapIsClipRect((bitmap)) ? \
        IsPointClippedForRect((bitmap)->clipRect, (x), (y)) : \
        IsPointClippedForBitmapSize((bitmap), (x), (y)) )
#else
    #define IsPointClippedForBitmap(bitmap, x, y) \
        IsPointClippedForBitmapSize((bitmap), (x), (y)) )
#endif

// horz

#define IsYEntryToBitmap(bitmap, y) \
    ( ((y) >= 0) && ((y) <= ((bitmap)->height - 1)) )

#define IsYEntryToBitmapClip(bitmap, y) \
    ( ((y) >= (bitmap)->clipRect.top) && ((y) < ((bitmap)->clipRect.bottom)) )

#define IsHorzEntryToBitmap(bitmap, x1, x2) \
    (Max((x1), 0) <= Min((x2), (bitmap)->width - 1))

#define IsHorzEntryToBitmapClip(bitmap, x1, x2) \
    (Max((x1), (bitmap)->clipRect.left) <= Min((x2), (bitmap)->clipRect.right - 1))

// vert

#define IsXEntryToBitmap(bitmap, x) \
    ( ((x) >= 0) && ((x) <= ((bitmap)->width - 1)) )

#define IsXEntryToBitmapClip(bitmap, x) \
    ( ((x) >= (bitmap)->clipRect.left) && ((x) < ((bitmap)->clipRect.right)) )

#define IsVertEntryToBitmap(bitmap, y1, y2) \
    (Max((y1), 0) <= Min((y2), (bitmap)->height - 1))

#define IsVertEntryToBitmapClip(bitmap, y1, y2) \
    (Max((y1), (bitmap)->clipRect.top) <= Min((y2), (bitmap)->clipRect.bottom - 1))

// ====================================================================
// ZXBitmap object
// ====================================================================

ZXData ZXCreateBitmapData(int width, int heignt) {
    ZXData data;
    
    data = (ZXData)malloc(sizeof(unsigned char) * CalcScanSize(width) * heignt);
    
    return data;
}

ZXBitmapRef ZXLinkBitmapCreate(int width, int height, ZXData data) {
    ZXBitmapRef obj;
    
    obj = (ZXBitmapRef)malloc(sizeof(ZXBitmap));
    if(obj == NULL)
        return NULL;
    
    obj->width = width;
    obj->height = height;
    
    obj->isClipRect = false;
    obj->isOffset = false;
    obj->isDrawLastPixel = false;
    #ifdef USE_PRECALC_SCANSIZE
    obj->scanSize = CalcScanSize(obj->width);
    #endif
    obj->data = data;
    
    return obj;
}

ZXBitmapRef ZXBitmapCreate(int width, int height) {
    ZXData data;
    ZXBitmapRef obj;
    
    data = ZXCreateBitmapData(width, height);
    if(data == NULL)
        return NULL;
    
    obj = ZXLinkBitmapCreate(width, height, data);
    if(obj == NULL)
        free(data);
    
    return obj;
}

ZXBitmapRef ZXBitmapFree(ZXBitmapRef bitmap) {
    free(bitmap->data);
    free(bitmap);
    return NULL;
}

ZXBitmapRef ZXLinkBitmapFree(ZXBitmapRef bitmap) {
    free(bitmap);
    return NULL;
}

int ZXBitmapResize(ZXBitmapRef bitmap, int newWidth, int newHeight) {
    free(bitmap->data);
    
    bitmap->data = ZXCreateBitmapData(newWidth, newHeight);
    
    if(bitmap->data == NULL)
        return 0;
    
    #ifdef USE_PRECALC_SCANSIZE
    bitmap->scanSize = CalcScanSize(bitmap->width);
    #endif
    
    return 1;
}

int ZXBitmapCopy(ZXBitmapRef dest, ZXBitmapRef src) {
    ZXData pEndDest, pDest, pSrc;
    
    if((dest->width != src->width) || (dest->height != src->height))
        return 0;
    
    pDest = dest->data;
    pSrc = src->data;
    pEndDest = pDest + BitmapDataSize(dest);
    for(; pDest < pEndDest; pDest++) {
        *pDest = *pSrc;
        pSrc++;
    }
    
    return 1;
}

void ZXBitmapCopyInfo(ZXBitmapRef dest, ZXBitmapRef src) {
    memcpy(dest, src, sizeof(ZXBitmap));
}

void ZXBitmapCopyData(ZXBitmapRef bitmap, ZXData data) {
    ZXData pEndDest, p;

    pEndDest = bitmap->data + BitmapDataSize(bitmap);
    for(p = bitmap->data; p < pEndDest; p++) {
        *p = *data;
        data++;
    }
}

ZXBitmapRef ZXBitmapClone(ZXBitmapRef bitmap) {
    ZXBitmapRef obj;
    
    obj = ZXBitmapCreate(bitmap->width, bitmap->height);
    
    if(obj == NULL)
        return NULL;
    
    obj->offset = bitmap->offset;
    obj->clipRect = bitmap->clipRect;
    obj->isClipRect = bitmap->isClipRect;
    obj->isOffset = bitmap->isOffset;
    obj->isDrawLastPixel = bitmap->isDrawLastPixel;
    
    ZXBitmapCopy(obj, bitmap);
    
    return obj;
}

void ZXBitmapInvert(ZXBitmapRef bitmap) {
    ZXData end, p;
    
    end = bitmap->data + BitmapDataSize(bitmap);
    for(p = bitmap->data; p < end ;p++)
        *p = ~(*p);
}

//TODO: test
void ZXBitmapFlipHorz(ZXBitmapRef bitmap) {
    unsigned char t;
    int x, y, compX, w, h;
    
    w = bitmap->width / 2;
    h = bitmap->height;
    
    compX = bitmap->width - 1;
    for(x = 0; x < w; x++) {
        for(y = 0; y < h; y++) {
            t = GetPixel(bitmap, x, y);
            SetPixel(bitmap, x, y, GetPixel(bitmap, compX, y));
            SetPixel(bitmap, compX, y, t);
        }
        compX--;
    }
}

//TODO: optimize me please
void ZXBitmapFlipVert(ZXBitmapRef bitmap) {
    unsigned char t;
    int x, y, compY, w, h;
    
    w = bitmap->width;
    h = bitmap->height / 2;
    
    compY = bitmap->height - 1;
    for(y = 0; y < h; y++) {
        for(x = 0; x < w; x++) {
            t = GetPixel(bitmap, x, y);
            SetPixel(bitmap, x, y, GetPixel(bitmap, x, compY));
            SetPixel(bitmap, x, compY, t);
        }
        compY--;
    }
}

//TODO: test
//TODO: rewrite me please
int ZXBitmapRotate90(ZXBitmapRef bitmap, int angle) {
    ZXBitmapRef r;
    int x, y;
    unsigned char t;
    
    if((angle < 1) || (angle > 3))
        return 0;
    
    if((angle == 1) || (angle == 3))
        r = ZXBitmapCreate(bitmap->height, bitmap->width);
    else
        r = ZXBitmapCreate(bitmap->width, bitmap->height);
    
    if(r == NULL)
        return 0;
    
    switch(angle) {
        case 1:
            for(y = 0; y < bitmap->height; y++) {
                for(x = 0; x < bitmap->width; x++) {
                    t = GetPixel(bitmap, x, bitmap->height - y - 1);
                    SetPixel(r, y, x, t);
                }
            }
            break;
        case 2:
            for(y = 0; y < bitmap->height; y++) {
                for(x = 0; x < bitmap->width; x++) {
                    t = GetPixel(bitmap, bitmap->width - x - 1, bitmap->height - y - 1);
                    SetPixel(r, x, y, t);
                }
            }
            break;
        case 3:
            for(y = 0; y < bitmap->height; y++) {
                for(x = 0; x < bitmap->width; x++) {
                    t = GetPixel(bitmap, bitmap->width - x - 1, y);
                    SetPixel(r, y, x, t);
                }
            }
            break;
    }
    
    // it`s magic
    bitmap->height = r->height;
    bitmap->width = r->width;
#ifdef USE_PRECALC_SCANSIZE
    bitmap->scanSize = r->scanSize;
#endif
    free(bitmap->data);
    bitmap->data = r->data;
    
    ZXLinkBitmapFree(r);
    
    return 1;
}

void ZXBitmapSetClipRect(ZXBitmapRef bitmap, int x, int y, int width, int height) {
    
    if(width < 0 || height < 0)
        return;
    
    bitmap->isClipRect = (width != 0 && height != 0);
    
    bitmap->clipRect.left = ClipValueM(0, bitmap->width, x);
    bitmap->clipRect.top = ClipValueM(0, bitmap->height, y);
    bitmap->clipRect.right = Min(bitmap->clipRect.left + width, bitmap->width);
    bitmap->clipRect.bottom = Min(bitmap->clipRect.right + height, bitmap->height);
}

// ====================================================================
// Bitmap helpers
// ====================================================================

int ZXCalcScanSize(int width) {
    return ZXCalcScanSizeM(width);
}

int ZXCalcFullBytes(int x, int width) {
    return ZXCalcFullBytesM(x, width);
}

ZXData ZXPixelPtrData(ZXData data, int scanSize, int x, int y) {
    return ZXPixelPtrDataM(data, scanSize, x, y);
}

int ZXBitmapScanSize(ZXBitmapRef bitmap) {
    return ZXBitmapScanSizeM(bitmap);
}

int ZXBitmapIsOffset(ZXBitmapRef bitmap) {
    return ZXBitmapIsOffsetM(bitmap);
}

int ZXBitmapIsClipRect(ZXBitmapRef bitmap) {
    return ZXBitmapIsClipRectM(bitmap);
}

ZXData ZXBitmapPixelPtr(ZXBitmapRef bitmap, int x, int y) {
    return ZXBitmapPixelPtrM(bitmap, x, y);
}

ZXData ZXBitmapBeginPtr(ZXBitmapRef bitmap, int x, int y) {
    return ZXBitmapBeginPtrM(bitmap, x, y);
}

ZXData ZXBitmapEndPtr(ZXBitmapRef bitmap, int x, int y) {
    return ZXBitmapEndPtrM(bitmap, x, y);
}

int ZXBitmapDataSize(ZXBitmapRef bitmap) {
    return ZXBitmapDataSizeM(bitmap);
}


// ====================================================================
// Raw
// ====================================================================

void ZXRawSetPixel(ZXBitmapRef bitmap, int x, int y, int color) {
    ZXData p;
    
    p = BitmapPixelPtr(bitmap, x, y);
    
    switch(color) {
        case 0:
            *p &= ~(0x80 >> (x%8));
            break;
        case 1:
            *p |= (0x80 >> (x%8));
            break;
        case 2:
            *p ^= (0x80 >> (x%8));
            break;
    }
}

// --------------------------------------------------------------------

void ZXRawPixelOn(ZXBitmapRef bitmap, int x, int y) {
    ZXFastPixelOn(bitmap, x, y);
}

// --------------------------------------------------------------------

void ZXRawPixelOff(ZXBitmapRef bitmap, int x, int y) {
    ZXFastPixelOff(bitmap, x, y);
}

// --------------------------------------------------------------------

void ZXRawPixelChange(ZXBitmapRef bitmap, int x, int y) {
    ZXFastPixelChange(bitmap, x, y);
}

// --------------------------------------------------------------------

int ZXRawGetPixel(ZXBitmapRef bitmap, int x, int y) {
    ZXData p;
    
    p = bitmap->data + BitmapScanSize(bitmap) * y + x / 8;
    
    return (*p >> (7 - x % 8)) & 0x01;
}

// --------------------------------------------------------------------

void ZXRawDrawLine(ZXBitmapRef bitmap, int x1, int y1, int x2, int y2, int color, int isDrawLastPixel) {
    int t, dist, x_error, y_error, dx, dy, x_inc, y_inc;
    
    dx = x2 - x1;
    dy = y2 - y1;
    if(dx > 0)x_inc = 1; else x_inc = -1;
    if(dx == 0)x_inc = 0;
    if(dy > 0)y_inc = 1; else y_inc = -1;
    if(dy == 0)y_inc = 0;
    dx = Abs(dx);
    dy = Abs(dy);
    dist = Max(dx, dy);
    
    x_error = dist / 2;
    y_error = dist / 2;
    
    for(t = 0; t < dist; t++)
    {
        SetPixel(bitmap, x1, y1, color);
        
        x_error = x_error + dx;
        y_error = y_error + dy;
        if(x_error >= dist)
        {
            x_error = x_error - dist;
            x1 = x1 + x_inc;
        }
        if(y_error >= dist)
        {
            y_error = y_error - dist;
            y1 = y1 + y_inc;
        }
    }
    
    if(isDrawLastPixel)
        SetPixel(bitmap, x1, y1, color);
}

// --------------------------------------------------------------------

static const unsigned char horzRightByte[] =
{
    0x80,// 9
    0xC0,// 10
    0xE0,// 11
    0xF0,// 12
    0xF8,// 13
    0xFC,// 14
    0xFE,// 15
    0xFF,
};

static const unsigned char horzLeftByte[] =
{
    0xFF,// 0
    0x7F,// 1
    0x3F,// 2
    0x1F,// 3
    0x0F,// 4
    0x07,// 5
    0x03,// 6
    0x01,// 7
};

// DONE
void ZXRawDrawHorz(ZXBitmapRef bitmap, int y, int x1, int x2, int color) {
    int t;
    ZXData p, begin, end;
    unsigned char byte;
    
    if(x1 > x2) {
        t = x1; x1 = x2; x2 = t;
    }
    
    // less an 8 pixels
    if(x2 - x1 < 7) {
        for(t = x1; t <= x2; t++)
            SetPixel(bitmap, t, y, color);
        
        return;
    }
    
    // left pixels
    if(x1 % 8) {
        p = BitmapPixelPtr(bitmap, x1, y);
        if(color == 0)
            *p &= ~horzLeftByte[x1 % 8];
        else if(color == 1)
            *p |= horzLeftByte[x1 % 8];
        else
            *p ^= horzLeftByte[x1 % 8];
    }
    
    // full bytes
    byte = color * 0xFF;
    begin = BitmapBeginPtr(bitmap, x1, y);
    end = BitmapEndPtr(bitmap, x2, y);
    
    if(color != 2)
        for(p = begin; p < end; p++)
            *p = byte;
    else
        for(p = begin; p < end; p++)
            *p ^= byte;
    
    // right pixels
    if((x2 + 1) % 8) {
        p = BitmapPixelPtr(bitmap, x2, y);
        if(color == 0)
            *p &= ~horzRightByte[x2 % 8];
        else if(color == 1)
            *p |= horzRightByte[x2 % 8];
        else
            *p ^= horzRightByte[x2 % 8];
    }
}

// --------------------------------------------------------------------

void ZXRawDrawClipHorz(ZXBitmapRef bitmap, int y, int x1, int x2, int color) {

    // cliprect
#ifdef USE_CLIPRECT
    if(BitmapIsClipRect(bitmap)) {
        if(!IsYEntryToBitmapClip(bitmap, y))
            return;
        
        if(!IsHorzEntryToBitmapClip(bitmap, x1, x2))
            return;
        
        ClipSpan(bitmap->clipRect.left, bitmap->clipRect.right - 1, x1, x2);
    } else
#endif
    {
        if(!IsYEntryToBitmap(bitmap, y))
            return;
        
        if(!IsHorzEntryToBitmap(bitmap, x1, x2))
            return;
        
        ClipSpan(0, bitmap->width - 1, x1, x2);
    }
    
    ZXRawDrawHorz(bitmap, y, x1, x2, color);
}

// --------------------------------------------------------------------

// DONE
void ZXRawDrawVert(ZXBitmapRef bitmap, int x, int y1, int y2, int color) {
    unsigned char byte;
    ZXData p;
    int t, y, scanline;
    
    if(y1 > y2)
        Swap(y1, y2, t);
    
    byte = 0x80 >> (x % 8);
    p = BitmapPixelPtr(bitmap, x, y1);
    scanline = BitmapScanSize(bitmap);
    
    if(color == 0)
        for(y = y1; y <= y2; y++) {
            *p &= ~byte;
            p += scanline;
        }
    else if(color == 1)
        for(y = y1; y <= y2; y++) {
            *p |= byte;
            p += scanline;
        }
    else
        for(y = y1; y <= y2; y++) {
            *p ^= byte;
            p += scanline;
        }
}

// --------------------------------------------------------------------

void ZXRawDrawClipVert(ZXBitmapRef bitmap, int x, int y1, int y2, int color) {

    // cliprect
#ifdef USE_CLIPRECT
    if(BitmapIsClipRect(bitmap)) {
        if(!IsXEntryToBitmapClip(bitmap, x))
            return;
        
        if(!IsVertEntryToBitmapClip(bitmap, y1, y2))
            return;
        
        ClipSpan(bitmap->clipRect.top, bitmap->clipRect.bottom - 1, y1, y2);
    } else
#endif
    {
        if(!IsXEntryToBitmap(bitmap, x))
            return;
        
        if(!IsVertEntryToBitmap(bitmap, y1, y2))
            return;
        
        ClipSpan(0, bitmap->height - 1, y1, y2);
    }
    
    ZXRawDrawVert(bitmap, x, y1, y2, color);
}

// --------------------------------------------------------------------

// ====1====
// |       |
// 3       4
// |       |
// ====2====
void ZXRawDrawRect(ZXBitmapRef bitmap, int x1, int y1, int x2, int y2, int color) {
    
    // 1
    ZXRawDrawClipHorz(bitmap, y1, x1, x2, color);
    // 2
    if(y1 != y2)
        ZXRawDrawClipHorz(bitmap, y2, x1, x2, color);
    // 3, 4
    if(y2 - y1 > 1) {
        // 3
        ZXRawDrawClipVert(bitmap, x1, y1 + 1, y2 - 1, color);
        // 4
        if(x1 != x2)
            ZXRawDrawClipVert(bitmap, x2, y1 + 1, y2 - 1, color);
    }
}

// --------------------------------------------------------------------

void ZXRawDrawSides(ZXBitmapRef bitmap, int x1, int y1, int x2, int y2, int sides, int color) {
    
    // 1
    if(sides & SideTop)
        ZXRawDrawClipHorz(bitmap, y1, x1, x2, color);
    // 2
    if(y1 != y2)
        if(sides & SideBottom)
            ZXRawDrawClipHorz(bitmap, y2, x1, x2, color);
    // 3, 4
    if(y2 - y1 > 1) {
        // 3
        if(sides & SideLeft)
            ZXRawDrawClipVert(bitmap, x1, y1 + BoolTo01(sides & SideTop), y2 - BoolTo01(sides & SideBottom), color);
        // 4
        if(x1 != x2)
            if(sides & SideRight)
                ZXRawDrawClipVert(bitmap, x2, y1 + BoolTo01(sides & SideTop), y2 - BoolTo01(sides & SideBottom), color);
    }
}

// --------------------------------------------------------------------

void ZXRawFillRect(ZXBitmapRef bitmap, int x1, int y1, int x2, int y2, int color) {
    int y;
    
    for(y = y1; y <= y2; y++)
        ZXRawDrawHorz(bitmap, y, x1, x2, color);
}

// --------------------------------------------------------------------

// 0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20  21  22  23  24
// _________________________________________________________________________________________________
// H   |   |   |   |   |   |   |   H   |   |   |   |   |   |   |   H   |   |   |   |   |   |   |   H
//         '                       '                               '               '
//         '                       '                               '               '
//         '       left byte       '          full bytes           '  end byte     '
//         ^_______________________^_______________________________^_______________^________________________
// offset->H   |   |   |   |   |   |   |   H   |   |   |   |   |   |   |   H   |   |   |   |   |   |   |   H
//     -----------------------------------------------------------------------------------------------------
//     | X   X   X   X ' X   X   X   X | X   X   X   X ' X   X   X   X | X   X   X   X ' X   X   X   X |
//     -------------------------------------------------------------------------------------------------
//     <-- srcOffset

static const unsigned char rByte[] =
{
    0x00,
    0x80,// 9
    0xC0,// 10
    0xE0,// 11
    0xF0,// 12
    0xF8,// 13
    0xFC,// 14
    0xFE,// 15
};

static const unsigned char lByte[] =
{
    0xFF,// 0
    0x7F,// 1
    0x3F,// 2
    0x1F,// 3
    0x0F,// 4
    0x07,// 5
    0x03,// 6
    0x01,// 7
};

// WARNING!!!
// GOLD VERSION.
// If you do not want to go to hell, then do not go into this code.
// This function is working of 100%, but is very hard for mind.
// Any changes of this fuction or any BitBlit functions should be accompanied
// running test functions in zxGraphicsTest.h.
// BEGIN $RawBitBlit(WriteByte1, WriteByte2, WriteByte3, WriteByte4)$. Do not change this line!
void ZXRawBitBlitCopy(ZXBitmapRef bitmap, int x, int y,
                  ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight)
{
    /* dest and src */
    ZXData pScan, pSrcScan, pScanEnd;
    ZXData p, pSrc;
    unsigned char b;
    int scan, srcScan;
    /* offsets, flags, etc */
    int offset, srcOffset, endPixels;
    int fullBytes, endLine;
    int compRealOffset, realOffset;
    int isRegular;
    
    /* calculate the sizes of scan lines */
    scan = BitmapScanSize(bitmap);
    srcScan = BitmapScanSize(src);
    
    /* get pointers to a byte corresponding to a pixel */
    pScan = BitmapPixelPtr(bitmap, x, y);
    pSrcScan = BitmapPixelPtr(src, srcX, srcY);
    /* calculating end vertical line */
    endLine = y + srcHeight;
    
    /* bitmap offset */
    offset = x % 8;
    /* src offset */
    srcOffset = srcX % 8;
    
    /* get information about how much pixels should be shifted, */
    /* important: when (offset < srcOffset) one byte is completely absorbed when outputting the left byte */
    if(offset >= srcOffset)
        realOffset = offset - srcOffset;
    else
        realOffset = 8 - (srcOffset - offset);
    compRealOffset = 8 - realOffset;
    
    /* count of full-size bytes */
    fullBytes = CalcFullBytes(x, srcWidth);
    /* offset end pixel */
    endPixels = (x + srcWidth) % 8;
    /* regular draw or not */
    /* a) snap to right colum */
    /* b) snap to left colum */
    isRegular = (offset + srcWidth >= 8) || (offset == 0);
    
    /* main loop y */
    for(; y < endLine; y++) {
        p = pScan;
        pSrc = pSrcScan;
        
        /* take care of left byte */
        /* xxxxxxxx XXXXXXXX xxxxxxxx */
        /* ^ we here */
        if(offset != 0) {
            if (offset >= srcOffset) {
                b = *pSrc >> realOffset;
                
                /* need next byte if we haven't real offset */
                if(realOffset == 0)
                    pSrc++;
            }
            else {
                b = (*pSrc << compRealOffset) |
                (*(pSrc + 1) >> realOffset);
                /* we completely used a byte, take a new */
                pSrc++;
            }
            
            /* combine dest and src */
            if(isRegular)
                *p = (*p & rByte[offset]) | (b & lByte[offset]); /* $WriteByte1$ */
            else
                *p = (*p & ~(rByte[srcWidth] >> offset)) | (b & rByte[srcWidth] >> offset); /* $WriteByte2$ */
            
            p++;
        }
        
        /* main loop x */
        /* xxxxxxxx XXXXXXXX xxxxxxxx */
        /*          ^ we here */
        pScanEnd = p + fullBytes;
        if(realOffset != 0) {
            for(; p < pScanEnd; p++) {
                b = (*pSrc << compRealOffset) |
                (*(pSrc + 1) >> realOffset);
                
                /* combine dest and src */
                *p = b; /* $WriteByte3$ */
                
                pSrc++;
            }
        }
        else {
            for(; p < pScanEnd; p++) {
                b = *pSrc;
                
                /* combine dest and src */
                *p = b; /* $WriteByte3$ */
                
                pSrc++;
            }
        }
        
        /* take care of right byte */
        /* xxxxxxxx XXXXXXXX xxxxxxxx */
        /*                   ^ we here */
        if(isRegular && endPixels) {
            if(realOffset != 0) {
                b = *pSrc << compRealOffset;
                /* .. end pixel is double byte src */
                if(endPixels > realOffset)
                    b |= *(pSrc + 1) >> realOffset;
            }
            else
                b = *pSrc;
            
            /* combine dest and src */
            *p = (*p & lByte[endPixels]) | (b & rByte[endPixels]);  /* $WriteByte4$ */
        }
        pScan += scan;
        pSrcScan += srcScan;
    }
}
// END $RawBitBlit$. Do not change this line!

// --------------------------------------------------------------------

// BEGIN $RawMaskBlit(WriteByte1, WriteByte2, WriteByte3, WriteByte4)$. Do not change this line!
void ZXRawMaskBlitCopy(ZXBitmapRef bitmap, int x, int y,
                   ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXData maskData)
{
    /* dest and src */
    ZXData pScan, pSrcScan, pMaskScan, pScanEnd;
    ZXData p, pSrc, pMask;
    unsigned char b, m;
    int scan, srcScan;
    /* offsets, flags, etc */
    int offset, srcOffset, endPixels;
    int fullBytes, endLine;
    int compRealOffset, realOffset;
    int isRegular;
    
    /* calculate the sizes of scan lines */
    scan = BitmapScanSize(bitmap);
    srcScan = BitmapScanSize(src);
    
    /* get pointers to a byte corresponding to a pixel */
    pScan = BitmapPixelPtr(bitmap, x, y);
    pSrcScan = BitmapPixelPtr(src, srcX, srcY);
    pMaskScan = PixelPtrData(maskData, srcScan, srcX, srcY);
    /* calculating end vertical line */
    endLine = y + srcHeight;
    
    /* bitmap offset */
    offset = x % 8;
    /* src offset */
    srcOffset = srcX % 8;
    
    /* get information about how much pixels should be shifted, */
    /* important: when (offset < srcOffset) one byte is completely absorbed when outputting the left byte */
    if(offset >= srcOffset)
        realOffset = offset - srcOffset;
    else
        realOffset = 8 - (srcOffset - offset);
    compRealOffset = 8 - realOffset;
    
    /* count of full-size bytes */
    fullBytes = CalcFullBytes(x, srcWidth);
    /* offset end pixel */
    endPixels = (x + srcWidth) % 8;
    /* regular draw or not */
    /* a) snap to right colum */
    /* b) snap to left colum */
    isRegular = (offset + srcWidth >= 8) || (offset == 0);
    
    /* main loop y */
    for(; y < endLine; y++) {
        p = pScan;
        pSrc = pSrcScan;
        pMask = pMaskScan;
        
        /* take care of left byte */
        /* xxxxxxxx XXXXXXXX xxxxxxxx */
        /* ^ we here */
        if(offset != 0) {
            if (offset >= srcOffset) {
                b = *pSrc >> realOffset;
                m = *pMask >> realOffset;
                
                /* need next byte if we haven't real offset */
                if(realOffset == 0) {
                    pSrc++;
                    pMask++;
                }
            }
            else {
                b = (*pSrc << compRealOffset) |
                (*(pSrc + 1) >> realOffset);
                m = (*pMask << compRealOffset) |
                (*(pMask + 1) >> realOffset);
                /* we completely used a byte, take a new */
                pSrc++;
                pMask++;
            }
            
            /* combine dest and src */
            if(isRegular) {
                b = (*p & rByte[offset]) | (b & lByte[offset]);/* $WriteByte1$ */
                *p = (*p & ~m) | (b & m);
            }
            else {
                b = (*p & ~(rByte[srcWidth] >> offset)) | (b & (rByte[srcWidth] >> offset));/* $WriteByte2$ */
                *p = (*p & ~m) | (b & m);
            }
            
            p++;
        }
        
        /* main loop x */
        /* xxxxxxxx XXXXXXXX xxxxxxxx */
        /*          ^ we here */
        pScanEnd = p + fullBytes;
        if(realOffset != 0) {
            for(; p < pScanEnd; p++) {
                b = (*pSrc << compRealOffset) |
                (*(pSrc + 1) >> realOffset);
                m = (*pMask << compRealOffset) |
                (*(pMask + 1) >> realOffset);
                
                /* combine dest and src */
                b = b; /* $WriteByte3$ */
                *p = (*p & (~m)) | (b & m);
                
                pSrc++;
                pMask++;
            }
        }
        else {
            for(; p < pScanEnd; p++) {
                b = *pSrc;
                m = *pMask;
                
                /* combine dest and src */
                b = b; /* $WriteByte3$ */
                *p = (*p & (~m)) | (b & m);
                
                pSrc++;
                pMask++;
            }
        }
        
        /* take care of right byte */
        /* xxxxxxxx XXXXXXXX xxxxxxxx */
        /*                   ^ we here */
        if(isRegular && endPixels) {
            if(realOffset != 0) {
                b = *pSrc << compRealOffset;
                m = *pMask << compRealOffset;
                /* .. end pixel is double byte src */
                if(endPixels > realOffset) {
                    b |= *(pSrc + 1) >> realOffset;
                    m |= *(pMask + 1) >> realOffset;
                }
            }
            else {
                b = *pSrc;
                m = *pMask;
            }
            
            /* combine dest and src */
            b = (*p & lByte[endPixels]) | (b & rByte[endPixels]);  /* $WriteByte4$ */
            *p = (*p & (~m)) | (b & m);
        }
        pScan += scan;
        pSrcScan += srcScan;
        pMaskScan += srcScan;
    }
}
// END $RawMaskBlit$. Do not change this line!

// --------------------------------------------------------------------

void ZXRawBitBlitOr(ZXBitmapRef bitmap, int x, int y,
                    ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight) {
    
    RawBitBlit(*p |= (b & lByte[offset]),
               *p |= (b & (rByte[srcWidth] >> offset)),
               *p |= b,
               *p |= (b & rByte[endPixels]));
}

// --------------------------------------------------------------------

void ZXRawBitBlitAnd(ZXBitmapRef bitmap, int x, int y,
                     ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight) {
    
    RawBitBlit(*p &= (b | ~lByte[offset]),
               *p &= (b | ~(rByte[srcWidth] >> offset)),
               *p &= b,
               *p &= (b | ~rByte[endPixels]));
}

// --------------------------------------------------------------------

void ZXRawBitBlitXor(ZXBitmapRef bitmap, int x, int y,
                     ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight) {
    
    RawBitBlit(*p ^= (b & lByte[offset]),
               *p ^= (b & (rByte[srcWidth] >> offset)),
               *p ^= b,
               *p ^= (b & rByte[endPixels]));
}

// --------------------------------------------------------------------

void ZXRawBitBlitNotCopy(ZXBitmapRef bitmap, int x, int y,
                     ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight) {
    
    RawBitBlit(*p = (*p & rByte[offset]) | ((~b) & lByte[offset]),
               *p = (*p & ~(rByte[srcWidth] >> offset)) | ((~b) & (rByte[srcWidth] >> offset)),
               *p = ~b,
               *p = (*p & lByte[endPixels]) | ((~b) & rByte[endPixels]));
}

// --------------------------------------------------------------------

void ZXRawBitBlitNotOr(ZXBitmapRef bitmap, int x, int y,
                       ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight) {
    
    RawBitBlit(*p |= ((~b) & lByte[offset]),
               *p |= ((~b) & (rByte[srcWidth] >> offset)),
               *p |= (~b),
               *p |= ((~b) & rByte[endPixels]));
}

// --------------------------------------------------------------------

void ZXRawBitBlitNotAnd(ZXBitmapRef bitmap, int x, int y,
                        ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight) {
    
    RawBitBlit(*p &= ((~b) | ~lByte[offset]),
               *p &= ((~b) | ~(rByte[srcWidth] >> offset)),
               *p &= (~b),
               *p &= ((~b) | ~rByte[endPixels]));
}

// --------------------------------------------------------------------

void ZXRawBitBlitNotXor(ZXBitmapRef bitmap, int x, int y,
                        ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight) {
    
    RawBitBlit(*p ^= ((~b) & lByte[offset]),
               *p ^= ((~b) & (rByte[srcWidth] >> offset)),
               *p ^= (~b),
               *p ^= ((~b) & rByte[endPixels]));
}

// --------------------------------------------------------------------

void ZXRawMaskBlitOr(ZXBitmapRef bitmap, int x, int y,
                     ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXData maskData) {
    
    RawMaskBlit(b = *p | (b & lByte[offset]),
                b = *p | (b & (rByte[srcWidth] >> offset)),
                b = *p | b,
                b = *p | (b & rByte[endPixels]));
}

// --------------------------------------------------------------------

void ZXRawMaskBlitAnd(ZXBitmapRef bitmap, int x, int y,
                      ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXData maskData) {
    
    RawMaskBlit(b = *p & (b | ~lByte[offset]),
                b = *p & (b | ~(rByte[srcWidth] >> offset)),
                b = *p & b,
                b = *p & (b | ~rByte[endPixels]));
}

// --------------------------------------------------------------------

void ZXRawMaskBlitXor(ZXBitmapRef bitmap, int x, int y,
                      ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXData maskData) {
    
    RawMaskBlit(b = *p ^ (b & lByte[offset]),
                b = *p ^ (b & (rByte[srcWidth] >> offset)),
                b = *p ^ b,
                b = *p ^ (b & rByte[endPixels]));
}

// --------------------------------------------------------------------

void ZXRawMaskBlitNotCopy(ZXBitmapRef bitmap, int x, int y,
                      ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXData maskData) {
    
    RawMaskBlit(b = (*p & rByte[offset]) | ((~b) & lByte[offset]),
                b = (*p & ~(rByte[srcWidth] >> offset)) | ((~b) & (rByte[srcWidth] >> offset)),
                b = ~b,
                b = (*p & lByte[endPixels]) | ((~b) & rByte[endPixels]));
}

// --------------------------------------------------------------------

void ZXRawMaskBlitNotOr(ZXBitmapRef bitmap, int x, int y,
                        ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXData maskData) {
    
    RawMaskBlit(b = *p | ((~b) & lByte[offset]),
                b = *p | ((~b) & (rByte[srcWidth] >> offset)),
                b = *p | (~b),
                b = *p | ((~b) & rByte[endPixels]));
}

// --------------------------------------------------------------------

void ZXRawMaskBlitNotAnd(ZXBitmapRef bitmap, int x, int y,
                         ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXData maskData) {
    
    RawMaskBlit(b = *p & ((~b) | ~lByte[offset]),
                b = *p & ((~b) | ~(rByte[srcWidth] >> offset)),
                b = *p & (~b),
                b = *p & ((~b) | ~rByte[endPixels]));
}

// --------------------------------------------------------------------

void ZXRawMaskBlitNotXor(ZXBitmapRef bitmap, int x, int y,
                         ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXData maskData) {
    
    RawMaskBlit(b = *p ^ ((~b) & lByte[offset]),
                b = *p ^ ((~b) & (rByte[srcWidth] >> offset)),
                b = *p ^ (~b),
                b = *p ^ ((~b) & rByte[endPixels]));
}


// --------------------------------------------------------------------

void ZXRawBitBlit(ZXBitmapRef bitmap, int x, int y,
                        ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, int rop) {
    
    // DstNot
    if(rop & RopDstNot)
        ZXRawFillRect(bitmap, x, y, x + srcWidth - 1, y + srcHeight - 1, ZXInvert);
    
    // Copy
    if((rop & (RopOr | RopAnd | RopXor)) == 0) {
        if(rop & RopSrcNot)
            ZXRawBitBlitNotCopy(bitmap, x, y, src, srcX, srcY, srcWidth, srcHeight);
        else
            ZXRawBitBlitCopy(bitmap, x, y, src, srcX, srcY, srcWidth, srcHeight);
    }
    // Or
    else if(rop & RopOr) {
        if(rop & RopSrcNot)
            ZXRawBitBlitNotOr(bitmap, x, y, src, srcX, srcY, srcWidth, srcHeight);
        else
            ZXRawBitBlitOr(bitmap, x, y, src, srcX, srcY, srcWidth, srcHeight);
    }
    // And
    else if(rop & RopAnd) {
        if(rop & RopSrcNot)
            ZXRawBitBlitNotAnd(bitmap, x, y, src, srcX, srcY, srcWidth, srcHeight);
        else
            ZXRawBitBlitAnd(bitmap, x, y, src, srcX, srcY, srcWidth, srcHeight);
    }
    // Xor
    else if (rop & RopXor) {
        if(rop & RopSrcNot)
            ZXRawBitBlitNotXor(bitmap, x, y, src, srcX, srcY, srcWidth, srcHeight);
        else
            ZXRawBitBlitXor(bitmap, x, y, src, srcX, srcY, srcWidth, srcHeight);
    }
}

// --------------------------------------------------------------------

void ZXRawMaskBlit(ZXBitmapRef bitmap, int x, int y,
                         ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXData maskData, int rop) {
    ZXData temp;
    
    // DstNot
    if(rop & RopDstNot) {
        temp = src->data;
        src->data = maskData;
        
        ZXRawBitBlitXor(bitmap, x, y, src, srcX, srcY, srcWidth, srcHeight);
        
        src->data = temp;
    }
    
    // Copy
    if((rop & (RopOr | RopAnd | RopXor)) == 0) {
        if(rop & RopSrcNot)
            ZXRawMaskBlitNotCopy(bitmap, x, y, src, srcX, srcY, srcWidth, srcHeight, maskData);
        else
            ZXRawMaskBlitCopy(bitmap, x, y, src, srcX, srcY, srcWidth, srcHeight, maskData);
    }
    // Or
    else if(rop & RopOr) {
        if(rop & RopSrcNot)
            ZXRawMaskBlitNotOr(bitmap, x, y, src, srcX, srcY, srcWidth, srcHeight, maskData);
        else
            ZXRawMaskBlitOr(bitmap, x, y, src, srcX, srcY, srcWidth, srcHeight, maskData);
    }
    // And
    else if(rop & RopAnd) {
        if(rop & RopSrcNot)
            ZXRawMaskBlitNotAnd(bitmap, x, y, src, srcX, srcY, srcWidth, srcHeight, maskData);
        else
            ZXRawMaskBlitAnd(bitmap, x, y, src, srcX, srcY, srcWidth, srcHeight, maskData);
    }
    // Xor
    else if (rop & RopXor) {
        if(rop & RopSrcNot)
            ZXRawMaskBlitNotXor(bitmap, x, y, src, srcX, srcY, srcWidth, srcHeight, maskData);
        else
            ZXRawMaskBlitXor(bitmap, x, y, src, srcX, srcY, srcWidth, srcHeight, maskData);
    }
}

// ====================================================================
// Standart
// ====================================================================

// DONE
void ZXSetPixel(ZXBitmapRef bitmap, int x, int y, int color) {
    
    if(IsPointClippedForBitmapSize(bitmap, x, y))
        return;
    
    SetPixel(bitmap, x, y, color);
}

// --------------------------------------------------------------------

// DONE
void ZXSetPixelRop(ZXBitmapRef bitmap, int x, int y, int color, int rop) {
    
    unsigned char b;
    
    if(IsPointClippedForBitmapSize(bitmap, x, y))
        return;
    
    if(rop & RopSrcNot)
        color = !color;
    
    b = GetPixel(bitmap, x, y);
    if(rop & RopDstNot)
        b = !b;
    
    switch (rop & (RopOr | RopAnd | RopXor | RopCopy)) {
        case RopOr:
            color = b | color;
            break;
            
        case RopAnd:
            color = b & color;
            break;
            
        case RopXor:
            color = b ^ color;
            break;
            
        default:
            break;
    }
    
    SetPixel(bitmap, x, y, color);
}

// --------------------------------------------------------------------

// DONE
void ZXPixelOn(ZXBitmapRef bitmap, int x, int y) {
    
    if(IsPointClippedForBitmapSize(bitmap, x, y))
        return;
    
    PixelOn(bitmap, x, y);
}

// --------------------------------------------------------------------

// DONE
void ZXPixelOff(ZXBitmapRef bitmap, int x, int y) {
    
    if(IsPointClippedForBitmapSize(bitmap, x, y))
        return;
    
    PixelOff(bitmap, x, y);
}

// --------------------------------------------------------------------

// DONE
void ZXPixelChange(ZXBitmapRef bitmap, int x, int y) {
    
    if(IsPointClippedForBitmapSize(bitmap, x, y))
        return;
    
    PixelChange(bitmap, x, y);
}

// --------------------------------------------------------------------

// DONE
int ZXGetPixel(ZXBitmapRef bitmap, int x, int y) {
    
    // -1 - error, out of bounds
    if(IsPointClippedForBitmapSize(bitmap, x, y))
        return -1;
    
    return GetPixel(bitmap, x, y);
}

// --------------------------------------------------------------------

// DONE
void ZXSetPoint(ZXBitmapRef bitmap, int x, int y, int color) {
    
#ifdef USE_OFFSET
    // offset
    if(BitmapIsOffset(bitmap))
        OffsetPoint(x, y, bitmap->offset);
#endif
    
    // cliprect
#ifdef USE_CLIPRECT
    if(IsPointClippedForBitmap(bitmap, x, y))
        return;
#else
    if(IsPointClippedForBitmapSize(bitmap, x, y))
        return;
#endif
    
    SetPixel(bitmap, x, y, color);
}

// --------------------------------------------------------------------

// DONE
void ZXSetPointRop(ZXBitmapRef bitmap, int x, int y, int color, int rop) {
    
    unsigned char b;
    
#ifdef USE_OFFSET
    // offset
    if(BitmapIsOffset(bitmap))
        OffsetPoint(x, y, bitmap->offset);
#endif
    
    // cliprect
#ifdef USE_CLIPRECT
    if(IsPointClippedForBitmap(bitmap, x, y))
        return;
#else
    if(IsPointClippedForBitmapSize(bitmap, x, y))
        return;
#endif

    if(rop & RopSrcNot)
        color = !color;
    
    b = GetPixel(bitmap, x, y);
    if(rop & RopDstNot)
        b = !b;

    switch (rop & (RopOr | RopAnd | RopXor | RopCopy)) {
        case RopOr:
            color = b | color;
            break;
            
        case RopAnd:
            color = b & color;
            break;
            
        case RopXor:
            color = b ^ color;
            break;
            
        default:
            break;
    }
    
    SetPixel(bitmap, x, y, color);
}

// --------------------------------------------------------------------

// DONE
int ZXGetPoint(ZXBitmapRef bitmap, int x, int y) {
    
#ifdef USE_OFFSET
    // offset
    if(BitmapIsOffset(bitmap))
        OffsetPoint(x, y, bitmap->offset);
#endif
    
    // cliprect
#ifdef USE_CLIPRECT
    if(IsPointClippedForBitmap(bitmap, x, y))
        return -1;
#else
    if(IsPointClippedForBitmapSize(bitmap, x, y))
        return -1;
#endif
    
    return GetPixel(bitmap, x, y);
}

// --------------------------------------------------------------------

// DONE
void ZXDrawLine(ZXBitmapRef bitmap, int x1, int y1, int x2, int y2, int color) {
    int clip;
    
#ifdef USE_OFFSET
    // offset
    if(BitmapIsOffset(bitmap)) {
        OffsetPoint(x1, y1, bitmap->offset);
        OffsetPoint(x2, y2, bitmap->offset);
    }
#endif
    
    // cliprect
#ifdef USE_CLIPRECT
    if(BitmapIsClipRect(bitmap))
        clip = CXRectClipLine(bitmap->clipRect, &x1, &y1, &x2, &y2);
    else
#endif
        clip = ClipLineForSize(bitmap->width, bitmap->height, &x1, &y1, &x2, &y2);
    
    if(clip != -1) {
        if(clip <= 1)
            ZXRawDrawLine(bitmap, x1, y1, x2, y2, color, BitmapIsDrawLastPixel(bitmap));
        else
            ZXRawDrawLine(bitmap, x1, y1, x2, y2, color, true);
    }
}

// --------------------------------------------------------------------

// DONE
//FIXME: Very dirt code! (macros hell)
void ZXDrawHorz(ZXBitmapRef bitmap, int y, int x1, int x2, int color) {
    int t;
    
    // offset
#ifdef USE_OFFSET
    if(BitmapIsOffset(bitmap)) {
        y += bitmap->offset.y;
        x1 += bitmap->offset.x;
        x2 += bitmap->offset.x;
    }
#endif
    
    if(x1 > x2)
        Swap(x1, x2, t);
    
    // cliprect
#ifdef USE_CLIPRECT
    if(BitmapIsClipRect(bitmap)) {
        if(!IsYEntryToBitmapClip(bitmap, y))
           return;
        
        if(!IsHorzEntryToBitmapClip(bitmap, x1, x2))
            return;
        
        ClipSpan(bitmap->clipRect.left, bitmap->clipRect.right - 1, x1, x2);
    } else
#endif
    {
        if(!IsYEntryToBitmap(bitmap, y))
            return;
        
        if(!IsHorzEntryToBitmap(bitmap, x1, x2))
            return;
        
        ClipSpan(0, bitmap->width - 1, x1, x2);
    }
    
    ZXRawDrawHorz(bitmap, y, x1, x2, color);
}

// --------------------------------------------------------------------

// DONE
//FIXME: Very dirt code! (macros hell)
void ZXDrawVert(ZXBitmapRef bitmap, int x, int y1, int y2, int color) {
    int t;
    
    // offset
#ifdef USE_OFFSET
    if(BitmapIsOffset(bitmap)) {
        x += bitmap->offset.x;
        y1 += bitmap->offset.y;
        y2 += bitmap->offset.y;
    }
#endif
    
    if(y1 > y2)
        Swap(y1, y2, t);
    
    // cliprect
#ifdef USE_CLIPRECT
    if(BitmapIsClipRect(bitmap)) {
        if(!IsXEntryToBitmapClip(bitmap, x))
            return;
        
        if(!IsVertEntryToBitmapClip(bitmap, y1, y2))
            return;
        
        ClipSpan(bitmap->clipRect.top, bitmap->clipRect.bottom - 1, y1, y2);
    } else
#endif
    {
        if(!IsXEntryToBitmap(bitmap, x))
            return;
        
        if(!IsVertEntryToBitmap(bitmap, y1, y2))
            return;
        
        ClipSpan(0, bitmap->height - 1, y1, y2);
    }
    
    ZXRawDrawVert(bitmap, x, y1, y2, color);
}

// --------------------------------------------------------------------

// DONE
// ====1====
// |       |
// 3       4
// |       |
// ====2====
void ZXDrawRect(ZXBitmapRef bitmap, int x1, int y1, int x2, int y2, int color) {
    int t;
    
    if(!BitmapIsDrawLastPixel(bitmap)) {
        if((x1 == x2) || (y1 == y2))
            return;
        
        if(x1 < x2)x2--; else x2++;
        if(y1 < y2)y2--; else y2++;
    }
    
    NormalizeRectM(x1, y1, x2, y2, t);
    
    // 1
    ZXDrawHorz(bitmap, y1, x1, x2, color);
    // 2
    if(y1 != y2)
        ZXDrawHorz(bitmap, y2, x1, x2, color);
    // 3, 4
    if(y2 - y1 > 1) {
        // 3
        ZXDrawVert(bitmap, x1, y1 + 1, y2 - 1, color);
        // 4
        if(x1 != x2)
            ZXDrawVert(bitmap, x2, y1 + 1, y2 - 1, color);
    }
}

// --------------------------------------------------------------------

void ZXDrawSides(ZXBitmapRef bitmap, int x1, int y1, int x2, int y2, int sides, int color) {
    int t;
    
    if(!BitmapIsDrawLastPixel(bitmap)) {
        if((x1 == x2) || (y1 == y2))
            return;
        
        if(x1 < x2)x2--; else x2++;
        if(y1 < y2)y2--; else y2++;
    }
    
    NormalizeRectM(x1, y1, x2, y2, t);
    
    // 1
    if(sides & SideTop)
        ZXDrawHorz(bitmap, y1, x1, x2, color);
    // 2
    if(y1 != y2)
        if(sides & SideBottom)
            ZXDrawHorz(bitmap, y2, x1, x2, color);
    // 3, 4
    if(y2 - y1 > 1) {
        // 3
        if(sides & SideLeft)
            ZXDrawVert(bitmap, x1, y1 + BoolTo01(sides & SideTop), y2 - BoolTo01(sides & SideBottom), color);
        // 4
        if(x1 != x2)
            if(sides & SideRight)
                ZXDrawVert(bitmap, x2, y1 + BoolTo01(sides & SideTop), y2 - BoolTo01(sides & SideBottom), color);
    }
}

// --------------------------------------------------------------------

// DONE
//FIXME: too slow, rewrite for ZXRawDrawHorz !
void ZXFillRect(ZXBitmapRef bitmap, int x1, int y1, int x2, int y2, int color) {
    int t, y;
    
    if(!BitmapIsDrawLastPixel(bitmap)) {
        if((x1 == x2) || (y1 == y2))
            return;
        
        if(x1 < x2)x2--; else x2++;
        if(y1 < y2)y2--; else y2++;
    }
    
    NormalizeRectM(x1, y1, x2, y2, t);
    
    for(y = y1; y <= y2; y++)
        ZXDrawHorz(bitmap, y, x1, x2, color);
}

// --------------------------------------------------------------------

// DONE
//FIXME: code is very old and dirt, please rewrite me!
//FIXME: need test for cliprect before drawing
void ZXDrawEllipse(ZXBitmapRef bitmap, int x1, int y1, int x2, int y2, int color) {
    int col, row, dx, dy, a, b, x, y, w, h;
    long a_square, b_square, two_a_square, two_b_square, four_a_square, four_b_square, d;
    
    int t;
    
    if(!BitmapIsDrawLastPixel(bitmap)) {
        if((x1 == x2) || (y1 == y2))
            return;
        
        if(x1 < x2)x2--; else x2++;
        if(y1 < y2)y2--; else y2++;
    }
    
    NormalizeRectM(x1, y1, x2, y2, t);
    
    w = x2 - x1;
    h = y2 - y1;
    
    if(h < 2) {
        ZXDrawHorz(bitmap, y1, x1, x2, color);
        if(h != 0)
            ZXDrawHorz(bitmap, y2, x1, x2, color);
        return;
    }
    
    if(w < 2) {
        ZXDrawVert(bitmap, x1, y1, y2, color);
        if(w != 0)
            ZXDrawVert(bitmap, x2, y1, y2, color);
        return;
    }
    
    a = w / 2;
    b = h / 2;
    x = x1 + a;
    y = y1 + b;
    
    b_square = b * b;
    a_square = a * a;
    row = b;
    col = 0;
    two_a_square = a_square * 2;
    four_a_square = a_square * 4;
    four_b_square = b_square * 4;
    two_b_square = b_square * 2;
    
    d = two_a_square * ((row - 1) * (row)) + a_square + two_b_square * (1 - a_square);
    while(a_square * (row) > b_square * (col)) {
        dx = (w % 2) ? col + 1: col;
        dy = (h % 2) ? row + 1: row;
        ZXSetPixel(bitmap, x + dx, y + dy, color);
        ZXSetPixel(bitmap, x + dx, y - row, color);
        if(col != 0 || w % 2) {
            ZXSetPixel(bitmap, x - col, y + dy, color);
            ZXSetPixel(bitmap, x - col, y - row, color);
        }
        if(d >= 0) {
            row--;
            d -= four_a_square * (row);
        }
        d += two_b_square * (3 + (col * 2));
        col++;
    }
    
    d = two_b_square * (col + 1) * col + two_a_square * (row * (row - 2) + 1) + (1 - two_a_square) * b_square;
    while(row + 1 != 0) {
        dx = (w % 2) ? col + 1: col;
        dy = (h % 2) ? row + 1: row;
        ZXSetPixel(bitmap, x + dx, y + dy, color);
        ZXSetPixel(bitmap, x - col, y + dy, color);
        if(row != 0 || h % 2) {
            ZXSetPixel(bitmap, x + dx, y - row, color);
            ZXSetPixel(bitmap, x - col, y - row, color);
        }
        if(d <= 0) {
            col++;
            d += four_b_square * col;
        }
        
        d += two_a_square * (3 - (row * 2));
        row--;
    }
}

// --------------------------------------------------------------------

// DONE
//FIXME: code is very old and dirt, please rewrite me!
//FIXME: need test for cliprect before drawing
void ZXFillEllipse(ZXBitmapRef bitmap, int x1, int y1, int x2, int y2, int color)
{
    int col, row, dx, dy, a, b, x, y, w, h;
    long a_square, b_square, two_a_square, two_b_square, four_a_square, four_b_square, d;
    
    int t;
    
    if(!BitmapIsDrawLastPixel(bitmap)) {
        if((x1 == x2) || (y1 == y2))
            return;
        
        if(x1 < x2)x2--; else x2++;
        if(y1 < y2)y2--; else y2++;
    }
    
    NormalizeRectM(x1, y1, x2, y2, t);
    
    w = x2 - x1;
    h = y2 - y1;
    
    if(h < 2) {
        ZXDrawHorz(bitmap, y1, x1, x2, color);
        if(h != 0)
            ZXDrawHorz(bitmap, y2, x1, x2, color);
        return;
    }
    
    if(w < 2) {
        ZXDrawVert(bitmap, x1, y1, y2, color);
        if(w != 0)
            ZXDrawVert(bitmap, x2, y1, y2, color);
        return;
    }
    
    a = w / 2;
    b = h / 2;
    x = x1 + a;
    y = y1 + b;
    
    b_square = b * b;
    a_square = a * a;
    row = b;
    col = 0;
    two_a_square = a_square * 2;
    four_a_square = a_square * 4;
    four_b_square = b_square * 4;
    two_b_square = b_square * 2;
    
    d = two_a_square * ((row - 1) * (row)) + a_square + two_b_square * (1 - a_square);
    while(a_square * (row) > b_square * (col)) {
        if(d >= 0) {
            dx = (w % 2) ? col + 1: col;
            dy = (h % 2) ? row + 1: row;
            ZXDrawHorz(bitmap, y - row, x - col, x + dx, color);
            ZXDrawHorz(bitmap, y + dy, x - col, x + dx, color);
            row--;
            d -= four_a_square * (row);
        }
        d += two_b_square * (3 + (col * 2));
        col++;
    }
    
    d = two_b_square * (col + 1) * col + two_a_square * (row * (row - 2) + 1) + (1 - two_a_square) * b_square;
    while (row + 1 != 0) {
        dx = (w % 2) ? col + 1: col;
        dy = (h % 2) ? row + 1: row;
        
        ZXDrawHorz(bitmap, y - row, x - col, x + dx, color);
        if(row != 0 || h % 2)
            ZXDrawHorz(bitmap, y + dy, x - col, x + dx, color);
        if(d <= 0) {
            col++;
            d += four_b_square * col;
        }
        
        d += two_a_square * (3 - (row * 2));
        row--;
    }
}

// --------------------------------------------------------------------

void ZXClear(ZXBitmapRef bitmap, int color) {
    ZXData end;
    unsigned char* p;
    
    end = bitmap->data + BitmapDataSize(bitmap);
    if(color)
        for(p = bitmap->data; p < end; p++)
            *p = 0xFF;
    else
        for(p = bitmap->data; p < end; p++)
            *p = 0x00;
}

// --------------------------------------------------------------------

int ZXClipBitBlitParams(ZXBitmapRef bitmap, int *x, int *y, int *srcX, int *srcY, int *srcWidth, int *srcHeight) {
    
    // cliprect
#ifdef USE_CLIPRECT
    if(BitmapIsClipRect(bitmap)) {
        if(!IsDrawRectIntersectRect(*x, *y, *srcWidth, *srcHeight, bitmap->clipRect))
            return false;// nothing drawing
        
        // srcX
        if(*x < bitmap->clipRect.left) {
            *srcX += bitmap->clipRect.left - *x;
            *srcWidth -= bitmap->clipRect.left - *x;
            *x = bitmap->clipRect.left;
        }
        // srcY
        if(*y < bitmap->clipRect.top) {
            *srcY += bitmap->clipRect.top - *y;
            *srcHeight -= bitmap->clipRect.top - *y;
            *y = bitmap->clipRect.top;
            
        }
        // srcWidth
        if(*srcWidth + *x > bitmap->clipRect.right) {
            *srcWidth = bitmap->clipRect.right - *x;
        }
        // srcHeight
        if(*srcHeight + *y > bitmap->clipRect.bottom) {
            *srcHeight = bitmap->clipRect.bottom - *y;
        }
    } else
#endif
    {
        if(!IsDrawRectIntersectSize(*x, *y, *srcWidth, *srcHeight, bitmap->width, bitmap->height))
            return false;// nothing drawing
        
        // x0
        if(*x < 0) {
            *srcX += -(*x);
            *srcWidth -= -(*x);
            *x = 0;
        }
        // y0
        if(*y < 0) {
            *srcY += -(*y);
            *srcHeight -= -(*y);
            *y = 0;
        }
        // width
        if(*srcWidth + *x > bitmap->width) {
            *srcWidth = bitmap->width - *x;
        }
        // height
        if(*srcHeight + *y > bitmap->height) {
            *srcHeight = bitmap->height - *y;
        }
    }
    
    return true;
}

// --------------------------------------------------------------------

#define ZXIsValidBitBlitSrcParamsM(src, srcX, srcY, srcWidth, srcHeight) \
    (((src)->width - (srcX) >= (srcWidth)) && \
     ((src)->height - (srcY) >= (srcHeight)))

int ZXIsValidBitBlitSrcParams(ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight) {
   
    return ZXIsValidBitBlitSrcParamsM(src, srcX, srcY, srcWidth, srcHeight);
}

// --------------------------------------------------------------------

//ZXBitBlit / ZXMaskBlit

// --------------------------------------------------------------------

int ZXBitBlit(ZXBitmapRef bitmap, int x, int y,
              ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, int rop) {
    
    // offset
#ifdef USE_OFFSET
    if(BitmapIsOffset(bitmap)) {
        OffsetPoint(x, y, bitmap->offset);
    }
#endif
    
    // fix up params, return if nothing drawing
    if(!ZXClipBitBlitParams(bitmap, &x, &y, &srcX, &srcY, &srcWidth, &srcHeight))
        return false;
    
    // return if not valid drawing
    if(!ZXIsValidBitBlitSrcParamsM(src, srcX, srcY, srcWidth, srcHeight))
        return false;
    
    ZXRawBitBlit(bitmap, x, y, src, srcX, srcY, srcWidth, srcHeight, rop);
    
    return true;
}

// --------------------------------------------------------------------

int ZXMaskBlit(ZXBitmapRef bitmap, int x, int y,
               ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXData maskData, int rop) {
    
    // offset
#ifdef USE_OFFSET
    if(BitmapIsOffset(bitmap)) {
        OffsetPoint(x, y, bitmap->offset);
    }
#endif
    
    // fix up params, return if nothing drawing
    if(!ZXClipBitBlitParams(bitmap, &x, &y, &srcX, &srcY, &srcWidth, &srcHeight))
        return false;
    
    // return if not valid drawing
    if(!ZXIsValidBitBlitSrcParamsM(src, srcX, srcY, srcWidth, srcHeight))
        return false;
    
    ZXRawBitBlit(bitmap, x, y, src, srcX, srcY, srcWidth, srcHeight, rop);
    
    return true;
}

// --------------------------------------------------------------------

void ZXDrawBitmap(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src) {
    ZXBitBlit(bitmap, x, y, src, 0, 0, src->width, src->height, RopCopy);
}

// --------------------------------------------------------------------

void ZXDrawBitmapRop(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, int rop) {
    ZXBitBlit(bitmap, x, y, src, 0, 0, src->width, src->height, rop);
}

// --------------------------------------------------------------------

void ZXMaskBitmap(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, ZXData maskData) {
    ZXMaskBlit(bitmap, x, y, src, 0, 0, src->width, src->height, maskData, RopCopy);
}

// --------------------------------------------------------------------

void ZXMaskBitmapRop(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, ZXData maskData, int rop) {
    ZXMaskBlit(bitmap, x, y, src, 0, 0, src->width, src->height, maskData, rop);
}

// --------------------------------------------------------------------

#define FixInt8 int
#define FixInt8ToInt(n) ((n) >> 16)
#define IntToFixInt8(n) ((n) << 16)
#define FloatToFixInt8(n) ((int)((n)*256*256))

//TODO: All optimize
int ZXStrechBlitRop(ZXBitmapRef bitmap, int x, int y, int width, int height,
                     ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, int rop) {

    int endX, endY, ix, iy;
    int beginX;
    FixInt8 dx, dy, sx, sy;
    unsigned char b;
    
    // bad params
    if((width < 0 && srcWidth < 0) || (height < 0 && height < 0))
        return false;
    
    // end points of bitmap
    endX = x + Abs(width);
    endY = y + Abs(height);
    
    // mirror x
    if(width < 0)
        beginX = FloatToFixInt8(srcX + Abs(srcWidth));
    else
        beginX = FloatToFixInt8(srcX + (srcWidth < 0));
    
    // mirror y
    if(height < 0)
        sy = IntToFixInt8(srcY + Abs(srcHeight));
    else
        sy = IntToFixInt8(srcY + (srcHeight < 0));
    
    // calc dx/dy
    dx = FloatToFixInt8(srcWidth / (float)width);
    dy = FloatToFixInt8(srcHeight / (float)height);

    // fixup (magic)
    beginX += + dx / 2;
    sy += dy / 2;
    
    // work, too slow, need rewrite this
    for(iy = y; iy < endY; iy++) {
        
        sx = beginX;
        for(ix = x; ix < endX; ix++) {
            
            b = ZXGetPixel(src, FixInt8ToInt(sx), FixInt8ToInt(sy));
            ZXSetPointRop(bitmap, ix, iy, b, rop);
            
            sx += dx;
        }
        sy += dy;
    }
    
    return true;
}

// --------------------------------------------------------------------

int ZXStrechBlit(ZXBitmapRef bitmap, int x, int y, int width, int height,
                 ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight) {
    int endX, endY, ix, iy;
    int beginX;
    FixInt8 dx, dy, sx, sy;
    unsigned char b;
    
    // bad params
    if((width < 0 && srcWidth < 0) || (height < 0 && height < 0))
        return false;
    
    // end points of bitmap
    endX = x + Abs(width);
    endY = y + Abs(height);
    
    // mirror x
    if(width < 0)
        beginX = FloatToFixInt8(srcX + Abs(srcWidth));
    else
        beginX = FloatToFixInt8(srcX + (srcWidth < 0));
    
    // mirror y
    if(height < 0)
        sy = IntToFixInt8(srcY + Abs(srcHeight));
    else
        sy = IntToFixInt8(srcY + (srcHeight < 0));
    
    // calc dx/dy
    dx = FloatToFixInt8(srcWidth / (float)width);
    dy = FloatToFixInt8(srcHeight / (float)height);
    
    // fixup (magic)
    beginX += + dx / 2;
    sy += dy / 2;
    
    // work, too slow, need rewrite this
    for(iy = y; iy < endY; iy++) {
        
        sx = beginX;
        for(ix = x; ix < endX; ix++) {
            
            b = ZXGetPixel(src, FixInt8ToInt(sx), FixInt8ToInt(sy));
            ZXSetPoint(bitmap, ix, iy, b);
            
            sx += dx;
        }
        sy += dy;
    }
    
    return true;
}

// --------------------------------------------------------------------

int ZXStrechMaskBlitRop(ZXBitmapRef bitmap, int x, int y, int width, int height,
                 ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXData maskData, int rop) {
    int endX, endY, ix, iy;
    int beginX;
    FixInt8 dx, dy, sx, sy, rx, ry;
    unsigned char b;
    ZXBitmap mask;
    
    // bad params
    if((width < 0 && srcWidth < 0) || (height < 0 && height < 0))
        return false;
    
    ZXBitmapCopyInfo(&mask, src);
    mask.data = maskData;
    
    // end points of bitmap
    endX = x + Abs(width);
    endY = y + Abs(height);
    
    // mirror x
    if(width < 0)
        beginX = FloatToFixInt8(srcX + Abs(srcWidth));
    else
        beginX = FloatToFixInt8(srcX + (srcWidth < 0));
    
    // mirror y
    if(height < 0)
        sy = IntToFixInt8(srcY + Abs(srcHeight));
    else
        sy = IntToFixInt8(srcY + (srcHeight < 0));
    
    // calc dx/dy
    dx = FloatToFixInt8(srcWidth / (float)width);
    dy = FloatToFixInt8(srcHeight / (float)height);
    
    // fixup (magic)
    beginX += + dx / 2;
    sy += dy / 2;
    
    // work, too slow, need rewrite this
    for(iy = y; iy < endY; iy++) {
        
        sx = beginX;
        for(ix = x; ix < endX; ix++) {
            
            rx = FixInt8ToInt(sx);
            ry = FixInt8ToInt(sy);
            
            if(ZXGetPixel(&mask, rx, ry)) {
                b = ZXGetPixel(src, FixInt8ToInt(sx), FixInt8ToInt(sy));
                ZXSetPointRop(bitmap, ix, iy, b, rop);
            }
            
            sx += dx;
        }
        sy += dy;
    }
    
    return true;
}

// --------------------------------------------------------------------

int ZXStrechMaskBlit(ZXBitmapRef bitmap, int x, int y, int width, int height,
                 ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXData maskData) {
    int endX, endY, ix, iy;
    int beginX;
    FixInt8 dx, dy, sx, sy, rx, ry;
    unsigned char b;
    ZXBitmap mask;
    
    // bad params
    if((width < 0 && srcWidth < 0) || (height < 0 && height < 0))
        return false;
    
    ZXBitmapCopyInfo(&mask, src);
    mask.data = maskData;
    
    // end points of bitmap
    endX = x + Abs(width);
    endY = y + Abs(height);
    
    // mirror x
    if(width < 0)
        beginX = FloatToFixInt8(srcX + Abs(srcWidth));
    else
        beginX = FloatToFixInt8(srcX + (srcWidth < 0));
    
    // mirror y
    if(height < 0)
        sy = IntToFixInt8(srcY + Abs(srcHeight));
    else
        sy = IntToFixInt8(srcY + (srcHeight < 0));
    
    // calc dx/dy
    dx = FloatToFixInt8(srcWidth / (float)width);
    dy = FloatToFixInt8(srcHeight / (float)height);
    
    // fixup (magic)
    beginX += + dx / 2;
    sy += dy / 2;
    
    // work, too slow, need rewrite this
    for(iy = y; iy < endY; iy++) {
        
        sx = beginX;
        for(ix = x; ix < endX; ix++) {
            
            rx = FixInt8ToInt(sx);
            ry = FixInt8ToInt(sy);
            
            if(ZXGetPixel(&mask, rx, ry)) {
                b = ZXGetPixel(src, FixInt8ToInt(sx), FixInt8ToInt(sy));
                ZXSetPoint(bitmap, ix, iy, b);
            }
            
            sx += dx;
        }
        sy += dy;
    }
    
    return true;
}

// --------------------------------------------------------------------

int ZXStrechDrawBitmap(ZXBitmapRef bitmap, int x, int y, int width, int height, ZXBitmapRef src) {
    return ZXStrechBlit(bitmap, x, y, width, height, src, 0, 0, src->width, src->height);
}

// --------------------------------------------------------------------

int ZXStrechDrawBitmapRop(ZXBitmapRef bitmap, int x, int y, int width, int height, ZXBitmapRef src, int rop) {
    return ZXStrechBlitRop(bitmap, x, y, width, height, src, 0, 0, src->width, src->height, rop);
}

// --------------------------------------------------------------------

int ZXStrechMaskBitmap(ZXBitmapRef bitmap, int x, int y, int width, int height, ZXBitmapRef src, ZXData maskData) {
    return ZXStrechMaskBlit(bitmap, x, y, width, height, src, 0, 0, src->width, src->height, maskData);
}

// --------------------------------------------------------------------

int ZXStrechMaskBitmapRop(ZXBitmapRef bitmap, int x, int y, int width, int height, ZXBitmapRef src, ZXData maskData, int rop) {
    return ZXStrechMaskBlitRop(bitmap, x, y, width, height, src, 0, 0, src->width, src->height, maskData, rop);
}














