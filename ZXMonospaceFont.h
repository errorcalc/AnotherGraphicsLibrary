//
//  ZX Graphics library, part of Another Graphics library
//
//  Created by error on 27.01.18.
//  Copyright © 2018 errorsoft. All rights reserved.
//

#ifndef ZXMonospaceFont_h
#define ZXMonospaceFont_h

#include "ZXGraphics.h"
#include "CXTypes.h"

typedef struct {
    
    int width;
    int height;
    int cols;//|||
    ZXBitmapRef bitmap;
    
} ZXMonospaceFont, *ZXMonospaceFontRef;

#define ZXFontBlack (RopAnd | RopSrcNot)
#define ZXFontWhite (RopOr)
#define ZXFontInvert (RopXor)

ZXMonospaceFontRef ZXMonospaceFontCreate(int symbolWidth, int symbolHeight, int cols, ZXConstData fontData);
void ZXMonospaceFontFree(ZXMonospaceFontRef font);

int ZXDrawText(ZXBitmapRef bitmap, ZXMonospaceFontRef font, int x, int y, const CXChar* text, unsigned char color);
int ZXTextWidth(ZXMonospaceFontRef font, const CXChar* text);
int ZXTextHeight(ZXMonospaceFontRef font);

#endif /* ZXMonospaceFont_h */
