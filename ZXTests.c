//
//  ZX Graphics library
//
//  Created by error on 19.01.18.
//  Copyright © 2018 errorsoft. All rights reserved.
//

#include "ZXTests.h"
#include "ZXGraphics.h"
#include "ZXTestsSimpleBitBlit.h"
#include <assert.h>
#include <string.h>

void bbb(void) {

}

static const unsigned char test24x24_data[] =
{
    0xAA,0xAA,0xAA,0x00,0x00,0x01,0xBF,0xFF,0xFC,0x20,0x00,0x05,0xAF,0xFF,0xF4,0x20,
    0x00,0x05,0xAE,0xEE,0xE4,0x24,0x88,0x45,0xA4,0xEE,0x44,0x24,0x82,0x45,0xA4,0xEE,
    0x44,0x20,0x00,0x05,0xA0,0x00,0x04,0x27,0x77,0x75,0xA2,0x44,0x24,0x22,0x77,0x25,
    0xA2,0x41,0x24,0x22,0x77,0x25,0xA0,0x00,0x04,0x2F,0xFF,0xF5,0xA0,0x00,0x04,0x3F,
    0xFF,0xFD,0x80,0x00,0x00,0x55,0x55,0x55,
};

static const unsigned char test24x24mask_data[] =
{
    0x00,0x30,0x00,0x00,0x38,0x00,0x00,0x7C,0x00,0x00,0x7E,0x00,0x00,0xFF,0x00,0x1F,
    0xFF,0xF8,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7C,0x3E,0x1F,0x3E,0x7F,0x3F,0x1F,0xFF,
    0xFF,0x0F,0xFF,0xFE,0x07,0xFF,0xFC,0x03,0xFF,0xF0,0x03,0xC1,0xE0,0x07,0xFF,0xC0,
    0x07,0xFF,0xC0,0x07,0xFF,0xC0,0x07,0xF7,0xE0,0x0F,0xE3,0xE0,0x0F,0xC1,0xE0,0x0F,
    0x80,0xE0,0x0F,0x00,0x70,0x1E,0x00,0x30,
};

int byteCount(ZXBitmapRef bitmap) {
    return sizeof(unsigned char) * ZXCalcScanSize(bitmap->width) * bitmap->height;
}

// пиздец может произойти если x1=x2
void ZXClipLineTestForDivByZero(void) {
    int left, right, top, bottom;
    int x1, x2, y1, y2;
    int tx1, tx2, ty1, ty2;
    CXRect r;
    
    r.left = 0;
    r.top = 0;
    r.bottom = 10;
    r.right = 10;
    
    CXRectClipLine(r, &x1, &y1, &x2, &y2);
    
    
    for(x1 = -10; x1 < 10; x1 += 2)
    for(x2 = -10; x2 < 10; x2 += 2)
    for(y1 = -10; y1 < 10; y1 += 2)
    for(y2 = -10; y2 < 10; y2 += 2)
    for(left = -10; left < 10; left += 2)
    for(right = -10; right < 10; right += 2)
    for(top = -10; top < 10; top += 2)
    for(bottom = -10; bottom < 10; bottom += 2) {
        r.left = left;
        r.top = top;
        r.right = right;
        r.bottom = bottom;
        tx1 = x1;
        ty1 = y1;
        tx2 = x2;
        ty2 = y2;
        CXRectClipLine(r, &tx1, &ty1, &tx2, &ty2);
    }
    
}

void testBitBlitSpeed(int isTestVersion) {
    ZXBitmapRef disp;
    ZXBitmapRef bitmap;
    int i, x;
    
    disp = ZXBitmapCreate(100, 100);
    bitmap = ZXLinkBitmapCreate(24, 24, (unsigned char*)test24x24_data);
    
    for(i = 0; i <= 100; i++) {
        for(x = 0; x <= 24; x++) {
            // memset(disp->data, 0, byteCount(disp));
            if(isTestVersion)
                ZXRawBitBlitCopy_test(disp, x, 3, bitmap, 0, 0, bitmap->width, bitmap->height);
            else
                ZXRawBitBlitCopy(disp, x, 3, bitmap, 0, 0, bitmap->width, bitmap->height);
        }
    }

    ZXBitmapFree(disp);
    ZXLinkBitmapFree(bitmap);
}

void testBitBlit(void) {
    ZXBitmapRef disp, disp2;
    ZXBitmapRef bitmap;
    int x, y, srcX, srcWidth, bg;
    unsigned char bgData[] = {0x00, 0xFF, 0xAA, 0x55};
    
    disp = ZXBitmapCreate(100, 40);
    disp2 = ZXBitmapCreate(100, 40);
    bitmap = ZXLinkBitmapCreate(24, 24, (unsigned char*)test24x24_data);
    
    for(y = 0; y < 4; y++) {
        for(x = 0; x <= 24; x++) {
            for(srcX = 0; srcX <= bitmap->width; srcX++) {
                for(srcWidth = 0; srcWidth <= bitmap->width - srcX; srcWidth++) {
                    for(bg = 0; bg < 4; bg++) {
                        memset(disp->data, bgData[bg], byteCount(disp));
                        memset(disp2->data, bgData[bg], byteCount(disp));
                        ZXRawBitBlitCopy(disp, x, y, bitmap, srcX, 0, srcWidth, bitmap->height);
                        ZXRawBitBlitCopy_test(disp2, x, y, bitmap, srcX, 0, srcWidth, bitmap->height);
                
                        assert(memcmp(disp->data, disp2->data, byteCount(disp)) == 0);
                    }
                }
            }
        }
    }
    
    ZXBitmapFree(disp);
    ZXBitmapFree(disp2);
    ZXLinkBitmapFree(bitmap);
}


typedef void (*fnBitBlit)(ZXBitmapRef bitmap, int x, int y,
                          ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight);

#define fnCount 7
void testBitBlitRop(void) {
	ZXBitmapRef disp, disp2;
	ZXBitmapRef bitmap;
	int x, y, srcX, srcWidth, bg, n;
	unsigned char bgData[] = {0x00, 0xFF, 0xAA, 0x55};
    fnBitBlit fn[fnCount] = {ZXRawBitBlitOr, ZXRawBitBlitAnd, ZXRawBitBlitXor, ZXRawBitBlitNotCopy, ZXRawBitBlitNotOr, ZXRawBitBlitNotAnd, ZXRawBitBlitNotXor};
    fnBitBlit fn_test[fnCount] = {ZXRawBitBlitOr_test, ZXRawBitBlitAnd_test, ZXRawBitBlitXor_test, ZXRawBitBlitNotCopy_test, ZXRawBitBlitNotOr_test, ZXRawBitBlitNotAnd_test, ZXRawBitBlitNotXor_test};
    
    disp = ZXBitmapCreate(100, 40);
    disp2 = ZXBitmapCreate(100, 40);
    bitmap = ZXLinkBitmapCreate(24, 24, (unsigned char*)test24x24_data);
    
    for(n = 0; n < fnCount; n++) {
        for(y = 0; y < 4; y++) {
            for(x = 0; x <= 24; x++) {
                for(srcX = 0; srcX <= bitmap->width; srcX++) {
                    for(srcWidth = 0; srcWidth <= bitmap->width - srcX; srcWidth++) {
                        for(bg = 0; bg < 4; bg++) {
                            memset(disp->data, bgData[bg], byteCount(disp));
                            memset(disp2->data, bgData[bg], byteCount(disp));
                            fn[n](disp, x, y, bitmap, srcX, 0, srcWidth, bitmap->height);
                            fn_test[n](disp2, x, y, bitmap, srcX, 0, srcWidth, bitmap->height);
                            
                            assert(memcmp(disp->data, disp2->data, byteCount(disp)) == 0);
                        }
                    }
                }
            }
        }
    }
    
    ZXBitmapFree(disp);
    ZXBitmapFree(disp2);
    ZXLinkBitmapFree(bitmap);
}

typedef void (*fnMaskBlit)(ZXBitmapRef bitmap, int x, int y,
                ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXData maskData);
typedef void (*fnMaskBlitTest)(ZXBitmapRef bitmap, int x, int y,
                ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXBitmapRef mask);

#define fnMaskCount 8
void testMaskBlit(void) {
    ZXBitmapRef disp, disp2;
    ZXBitmapRef bitmap, mask;
    int x, y, srcX, srcWidth, bg, n;
    unsigned char bgData[] = {0x00, 0xFF, 0xAA, 0x55};
    fnMaskBlit fn[fnMaskCount] = {ZXRawMaskBlitCopy, ZXRawMaskBlitOr, ZXRawMaskBlitAnd, ZXRawMaskBlitXor, ZXRawMaskBlitNotCopy, ZXRawMaskBlitNotOr, ZXRawMaskBlitNotAnd, ZXRawMaskBlitNotXor};
    fnMaskBlitTest fn_test[fnMaskCount] = {ZXRawMaskBlitCopy_test, ZXRawMaskBlitOr_test, ZXRawMaskBlitAnd_test, ZXRawMaskBlitXor_test, ZXRawMaskBlitNotCopy_test, ZXRawMaskBlitNotOr_test, ZXRawMaskBlitNotAnd_test, ZXRawMaskBlitNotXor_test};
    
    disp = ZXBitmapCreate(100, 40);
    disp2 = ZXBitmapCreate(100, 40);
    bitmap = ZXLinkBitmapCreate(24, 24, (unsigned char*)test24x24_data);
    mask = ZXLinkBitmapCreate(24, 24, (unsigned char*)test24x24mask_data);
    
    for(n = 0; n < fnMaskCount; n++) {
        for(y = 0; y < 4; y++) {
            for(x = 0; x <= 24; x++) {
                for(srcX = 0; srcX <= bitmap->width; srcX++) {
                    for(srcWidth = 0; srcWidth <= bitmap->width - srcX; srcWidth++) {
                        for(bg = 0; bg < 4; bg++) {
                            memset(disp->data, bgData[bg], byteCount(disp));
                            memset(disp2->data, bgData[bg], byteCount(disp));
                            fn[n](disp, x, y, bitmap, srcX, 0, srcWidth, bitmap->height, mask->data);
                            fn_test[n](disp2, x, y, bitmap, srcX, 0, srcWidth, bitmap->height, mask);
                            
                            assert(memcmp(disp->data, disp2->data, byteCount(disp)) == 0);
                        }
                    }
                }
            }
        }
    }
    
    ZXBitmapFree(disp);
    ZXBitmapFree(disp2);
    ZXLinkBitmapFree(bitmap);
    ZXLinkBitmapFree(mask);
}
