//
//  ZX Graphics library, part of Another Graphics library
//
//  Created by error on 20.01.18.
//  Copyright © 2018 errorsoft. All rights reserved.
//

#include "ZXTestsSimpleBitBlit.h"
#include "ZXGraphics.h"

void ZXRawBitBlitCopy_test(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight) {
    int sx, sy;
    int c;
    
    for(sy = y; sy < y + srcHeight; sy++)
        for(sx = x; sx < x + srcWidth; sx++) {
            c = ZXGetPixel(src, sx - x + srcX, sy - y + srcY);
            ZXSetPixel(bitmap, sx, sy, c);
        }
}

void ZXRawBitBlitOr_test(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight) {
    int sx, sy;
    int c;
    
    for(sy = y; sy < y + srcHeight; sy++)
        for(sx = x; sx < x + srcWidth; sx++) {
            c = ZXGetPixel(src, sx - x + srcX, sy - y + srcY);
            if(c)
                ZXPixelOn(bitmap, sx, sy);
        }
}

void ZXRawBitBlitAnd_test(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight) {
    int sx, sy;
    int c;
    
    for(sy = y; sy < y + srcHeight; sy++)
        for(sx = x; sx < x + srcWidth; sx++) {
            c = ZXGetPixel(src, sx - x + srcX, sy - y + srcY);
            if(c == 0)
                ZXPixelOff(bitmap, sx, sy);
        }
}

void ZXRawBitBlitXor_test(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight) {
    int sx, sy;
    int c;
    
    for(sy = y; sy < y + srcHeight; sy++)
        for(sx = x; sx < x + srcWidth; sx++) {
            c = ZXGetPixel(src, sx - x + srcX, sy - y + srcY);
            if(c)
                ZXPixelChange(bitmap, sx, sy);
        }
}

void ZXRawBitBlitNotCopy_test(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight) {
    int sx, sy;
    int c;
    
    for(sy = y; sy < y + srcHeight; sy++)
        for(sx = x; sx < x + srcWidth; sx++) {
            c = 1 - ZXGetPixel(src, sx - x + srcX, sy - y + srcY);
            ZXSetPixel(bitmap, sx, sy, c);
        }
}

void ZXRawBitBlitNotOr_test(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight) {
    int sx, sy;
    int c;
    
    for(sy = y; sy < y + srcHeight; sy++)
        for(sx = x; sx < x + srcWidth; sx++) {
            c = 1 - ZXGetPixel(src, sx - x + srcX, sy - y + srcY);
            if(c)
                ZXPixelOn(bitmap, sx, sy);
        }
}

void ZXRawBitBlitNotAnd_test(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight) {
    int sx, sy;
    int c;
    
    for(sy = y; sy < y + srcHeight; sy++)
        for(sx = x; sx < x + srcWidth; sx++) {
            c = 1 - ZXGetPixel(src, sx - x + srcX, sy - y + srcY);
            if(c == 0)
                ZXPixelOff(bitmap, sx, sy);
        }
}

void ZXRawBitBlitNotXor_test(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight) {
    int sx, sy;
    int c;
    
    for(sy = y; sy < y + srcHeight; sy++)
        for(sx = x; sx < x + srcWidth; sx++) {
            c = 1 - ZXGetPixel(src, sx - x + srcX, sy - y + srcY);
            if(c)
                ZXPixelChange(bitmap, sx, sy);
        }
}

void ZXRawMaskBlitCopy_test(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXBitmapRef mask) {
    int sx, sy;
    int c, m;
    
    for(sy = y; sy < y + srcHeight; sy++)
        for(sx = x; sx < x + srcWidth; sx++) {
            c = ZXGetPixel(src, sx - x + srcX, sy - y + srcY);
            m = ZXGetPixel(mask, sx - x + srcX, sy - y + srcY);
            
            if(m)
                ZXSetPixel(bitmap, sx, sy, c);
        }
}

void ZXRawMaskBlitOr_test(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXBitmapRef mask) {
    int sx, sy;
    int c, m;
    
    for(sy = y; sy < y + srcHeight; sy++)
        for(sx = x; sx < x + srcWidth; sx++) {
            c = ZXGetPixel(src, sx - x + srcX, sy - y + srcY);
            m = ZXGetPixel(mask, sx - x + srcX, sy - y + srcY);
            
            if(m && c)
                ZXPixelOn(bitmap, sx, sy);
        }
}

void ZXRawMaskBlitAnd_test(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXBitmapRef mask) {
    int sx, sy;
    int c, m;
    
    for(sy = y; sy < y + srcHeight; sy++)
        for(sx = x; sx < x + srcWidth; sx++) {
            c = ZXGetPixel(src, sx - x + srcX, sy - y + srcY);
            m = ZXGetPixel(mask, sx - x + srcX, sy - y + srcY);
            
            if((c == 0) && m)
                ZXPixelOff(bitmap, sx, sy);
        }
}

void ZXRawMaskBlitXor_test(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXBitmapRef mask) {
    int sx, sy;
    int c, m;
    
    for(sy = y; sy < y + srcHeight; sy++)
        for(sx = x; sx < x + srcWidth; sx++) {
            c = ZXGetPixel(src, sx - x + srcX, sy - y + srcY);
            m = ZXGetPixel(mask, sx - x + srcX, sy - y + srcY);
            
            if(m && c)
                ZXPixelChange(bitmap, sx, sy);
        }
}

void ZXRawMaskBlitNotCopy_test(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXBitmapRef mask) {
    int sx, sy;
    int c, m;
    
    for(sy = y; sy < y + srcHeight; sy++)
        for(sx = x; sx < x + srcWidth; sx++) {
            c = 1 - ZXGetPixel(src, sx - x + srcX, sy - y + srcY);
            m = ZXGetPixel(mask, sx - x + srcX, sy - y + srcY);
            
            if(m)
                ZXSetPixel(bitmap, sx, sy, c);
        }
}

void ZXRawMaskBlitNotOr_test(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXBitmapRef mask) {
    int sx, sy;
    int c, m;
    
    for(sy = y; sy < y + srcHeight; sy++)
        for(sx = x; sx < x + srcWidth; sx++) {
            c = 1 - ZXGetPixel(src, sx - x + srcX, sy - y + srcY);
            m = ZXGetPixel(mask, sx - x + srcX, sy - y + srcY);
            
            if(m && c)
                ZXPixelOn(bitmap, sx, sy);
        }
}

void ZXRawMaskBlitNotAnd_test(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXBitmapRef mask) {
    int sx, sy;
    int c, m;
    
    for(sy = y; sy < y + srcHeight; sy++)
        for(sx = x; sx < x + srcWidth; sx++) {
            c = 1 - ZXGetPixel(src, sx - x + srcX, sy - y + srcY);
            m = ZXGetPixel(mask, sx - x + srcX, sy - y + srcY);
            
            if((c == 0) && m)
                ZXPixelOff(bitmap, sx, sy);
        }
}

void ZXRawMaskBlitNotXor_test(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXBitmapRef mask) {
    int sx, sy;
    int c, m;
    
    for(sy = y; sy < y + srcHeight; sy++)
        for(sx = x; sx < x + srcWidth; sx++) {
            c = 1 - ZXGetPixel(src, sx - x + srcX, sy - y + srcY);
            m = ZXGetPixel(mask, sx - x + srcX, sy - y + srcY);
            
            if(m && c)
                ZXPixelChange(bitmap, sx, sy);
        }
}
