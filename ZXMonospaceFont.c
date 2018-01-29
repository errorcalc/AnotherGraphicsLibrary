//
//  ZX Graphics library, part of Another Graphics library
//
//  Created by error on 27.01.18.
//  Copyright © 2018 errorsoft. All rights reserved.
//

#include "ZXMonospaceFont.h"
#include <stdlib.h>
#include <string.h>

ZXMonospaceFontRef ZXMonospaceFontCreate(int symbolWidth, int symbolHeight, int cols, ZXConstData fontData) {
    ZXMonospaceFontRef font;
    
    font = (ZXMonospaceFontRef)malloc(sizeof(ZXMonospaceFont));
    
    if(font == NULL)
        return NULL;
    
    font->bitmap = ZXLinkBitmapCreate(symbolWidth * cols, symbolHeight * (256 / cols), (ZXData)fontData);
    
    if(font->bitmap == NULL) {
        free(font);
        return NULL;
    }
    
    font->cols = cols;
    font->width = symbolWidth;
    font->height = symbolHeight;
    
    return font;
}

void ZXMonospaceFontFree(ZXMonospaceFontRef font) {
    ZXLinkBitmapFree(font->bitmap);
    free(font);
}

//FIXME: дописать вывод кол-ва напечатаных символов
int ZXDrawText(ZXBitmapRef bitmap, ZXMonospaceFontRef font, int x, int y, const CXChar* text, unsigned char color) {
    
    CXChar* p;
    int count = 0, ch;
    
    p = (CXChar*)text;
    
    while(*p != 0) {
        
        ch = (unsigned char)(*p);
        
        
        ZXBitBlit(bitmap, x, y, font->bitmap,
                 (ch % font->cols) * font->width,
                 (ch / font->cols) * font->height,
                 font->width, font->height, color);
        //ZXSetPoint(bitmap, x, y, ZXInvert);
        
        x += font->width;
        count++;
        p++;
    }
    
    return count;
}

int ZXTextWidth(ZXMonospaceFontRef font, const CXChar* text) {
    return (int)strlen(text) * font->width;
}

int ZXTextHeight(ZXMonospaceFontRef font) {
    return font->height;
}
