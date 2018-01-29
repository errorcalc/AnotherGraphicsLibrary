//
//  ZX Graphics library, part of Another Graphics library
//
//  Created by error on 20.01.18.
//  Copyright © 2018 errorsoft. All rights reserved.
//

#ifndef zxGraphicsBitBlitForTests_h
#define zxGraphicsBitBlitForTests_h

#include "ZXGraphics.h"

// tests
void ZXRawBitBlitCopy_test(ZXBitmapRef bitmap, int x, int y,
                       ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight);// for tests
void ZXRawBitBlitOr_test(ZXBitmapRef bitmap, int x, int y,
                         ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight);// for tests
void ZXRawBitBlitAnd_test(ZXBitmapRef bitmap, int x, int y,
                          ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight);// for tests
void ZXRawBitBlitXor_test(ZXBitmapRef bitmap, int x, int y,
                          ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight);// for tests
void ZXRawBitBlitNotCopy_test(ZXBitmapRef bitmap, int x, int y,
                          ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight);// for tests
void ZXRawBitBlitNotOr_test(ZXBitmapRef bitmap, int x, int y,
                            ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight);// for tests
void ZXRawBitBlitNotAnd_test(ZXBitmapRef bitmap, int x, int y,
                             ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight);// for tests
void ZXRawBitBlitNotXor_test(ZXBitmapRef bitmap, int x, int y,
                             ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight);// for tests
void ZXRawMaskBlitCopy_test(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXBitmapRef mask);
void ZXRawMaskBlitOr_test(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXBitmapRef mask);
void ZXRawMaskBlitAnd_test(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXBitmapRef mask);
void ZXRawMaskBlitXor_test(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXBitmapRef mask);
void ZXRawMaskBlitNotCopy_test(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXBitmapRef mask);
void ZXRawMaskBlitNotOr_test(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXBitmapRef mask);
void ZXRawMaskBlitNotAnd_test(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXBitmapRef mask);
void ZXRawMaskBlitNotXor_test(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXBitmapRef mask);

#endif /* zxGraphicsBitBlitForTests_h */
