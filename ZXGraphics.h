//
//  ZX Graphics library, part of Another Graphics library
//
//  Created by error on 14.01.18.
//  Copyright © 2018 errorsoft. All rights reserved.
//

#ifndef zxGraphics_h
#define zxGraphics_h

#include "CXTypes.h"

#define USE_CLIPRECT
#define USE_OFFSET
#define USE_PRECALC_SCANSIZE

// standart colors
#define ZXBlack 0
#define ZXWhite 1
#define ZXInvert 2

// alias to unsigned char*, uses for bitmap->data
typedef unsigned char* ZXData;
typedef const unsigned char* ZXConstData;

// ====================================================================
// Point type
// ====================================================================
//typedef struct {
//    int x, y;
//} ZXPoint, *ZXPointRef;

// ====================================================================
// Rectangle type
// ====================================================================
//typedef struct {
//    int left, top, right, bottom;
//} ZXRect, *ZXRectRef;



typedef enum {
    RopDstNot = 1,
    RopSrcNot = 2,
    RopCopy = 0,
    RopAnd = 4,
    RopOr = 8,
    RopXor = 16,
} ZXRop;

// ====================================================================
// ZXBitmap class
// ====================================================================
typedef struct {
    // ----------------
    // Base members
    // DON`T MOVE !!!
    // ----------------
    // Size
    int width;
    int height;
    // Pixels
    ZXData data;
    // Flags
    char isClipRect;
    char isOffset;
    char isDrawLastPixel;
    // Additional params
    CXRect clipRect;
    CXPoint offset;
    #ifdef USE_PRECALC_SCANSIZE
    // ScanSize
    int scanSize;
    #endif
    // ----------------
    // Extended members
    // ----------------
} ZXBitmap, *ZXBitmapRef;

// ====================================================================
// Constructors & destructors
// ====================================================================
// *** Standart bitmap ***
/// Создает битмап, возвращает ссылку на него.
ZXBitmapRef ZXBitmapCreate(int width, int height);
/// Уничтожает битмап, возвращает NULL.
ZXBitmapRef ZXBitmapFree(ZXBitmapRef bitmap);
// *** Link bitmap ***
/// Создает ссылочный битмап, возвращает ссылку на него.
ZXBitmapRef ZXLinkBitmapCreate(int width, int height, ZXData data);
/// Уничтожает ссылочный битмап, возвращает NULL.
ZXBitmapRef ZXLinkBitmapFree(ZXBitmapRef bitmap);
/// Изменяет размер изображения.
int ZXBitmapResize(ZXBitmapRef bitmap, int newWidth, int newHeight);
/// Копирует src в dest
/// Возвращает 0 в случае ошибки
int ZXBitmapCopy(ZXBitmapRef dest, ZXBitmapRef src);
/// Копирует параметры изображения
void ZXBitmapCopyInfo(ZXBitmapRef dest, ZXBitmapRef src);
/// Копирует данные из data
void ZXBitmapCopyData(ZXBitmapRef bitmap, ZXData data);
/// Возвращает копию bitmap
ZXBitmapRef ZXBitmapClone(ZXBitmapRef bitmap);
/// инвертирует bitmap
void ZXBitmapInvert(ZXBitmapRef bitmap);
/// переворачивает bitmap по вертикали
void ZXBitmapFlipVert(ZXBitmapRef bitmap);
/// переворачивает bitmap по горизонтали
void ZXBitmapFlipHorz(ZXBitmapRef bitmap);
/// поворачивает bitmap на угол кратный 90 градусам (0=0, 1 = 90, 2 = 180, 3 = 270)
/// Внимание - вызывает перераспределение памяти!
int ZXBitmapRotate90(ZXBitmapRef bitmap, int angle);

void ZXBitmapSetClipRect(ZXBitmapRef bitmap, int x, int y, int width, int height);
// ====================================================================
// Raw
// ====================================================================
// *** Pixels access ***
/// Функция утанавливает пиксель.
/// Не производит сдвиг и отсечение.
/// Рекомендуеться использовать макрос ZXFastSetPixel.
void ZXRawSetPixel(ZXBitmapRef bitmap, int x, int y, int color);
/// Функция включает пиксель.
/// Не производит сдвиг и отсечение.
/// Рекомендуеться использовать макрос ZXFastPixelOn.
void ZXRawPixelOn(ZXBitmapRef bitmap, int x, int y);
/// Функция выключает пиксель.
/// Не производит сдвиг и отсечение.
/// Рекомендуеться использовать макрос ZXFastPixelOff.
void ZXRawPixelOff(ZXBitmapRef bitmap, int x, int y);
/// Функция инвертирует пиксель.
/// Не производит сдвиг и отсечение.
/// Рекомендуеться использовать макрос ZXFastPixelChange.
void ZXRawPixelChange(ZXBitmapRef bitmap, int x, int y);
/// Функция возвращает цвет пикселя.
/// Не производит сдвиг и отсечение.
/// Рекомендуеться использовать макрос ZXFastGetPixel.
int ZXRawGetPixel(ZXBitmapRef bitmap, int x, int y);

// *** Lines drawing ***
/// Функция рисует линию.
/// Не производит сдвиг и отсечение.
/// isDrawLastPixel - рисовать ли последний пиксель.
void ZXRawDrawLine(ZXBitmapRef bitmap, int x1, int y1, int x2, int y2, int color, int isDrawLastPixel);
/// Функция рисует горизонтальную линию.
/// Не производит сдвиг и отсечение.
/// Всегда рисует последний писель.
void ZXRawDrawHorz(ZXBitmapRef bitmap, int y, int x1, int x2, int color);
/// Функция горизонтальную рисует линию.
/// Производит отсечение.
/// Всегда рисует последний писель.
void ZXRawDrawClipHorz(ZXBitmapRef bitmap, int y, int x1, int x2, int color);
/// Функция рисует вертикальную линию.
/// Не производит сдвиг и отсечение.
/// Всегда рисует последний писель.
void ZXRawDrawVert(ZXBitmapRef bitmap, int x, int y1, int y2, int color);
/// Функция рисует вертикальную линию.
/// Производит отсечение.
/// Всегда рисует последний писель.
void ZXRawDrawClipVert(ZXBitmapRef bitmap, int x, int y1, int y2, int color);

// *** Rects drawing ***
/// Функция рисует прямоугольник.
/// Производит отсечение.
/// Всегда рисует последний писель.
void ZXRawDrawRect(ZXBitmapRef bitmap, int x1, int y1, int x2, int y2, int color);
/// Функция рисует стороны прямоугольника.
/// Производит отсечение.
/// Всегда рисует последний писель.
void ZXRawDrawSides(ZXBitmapRef bitmap, int x1, int y1, int x2, int y2, int sides, int color);
/// Функция рисует заполненный прямоугольник.
/// Не производит сдвиг и отсечение.
/// Всегда рисует последний писель.
void ZXRawFillRect(ZXBitmapRef bitmap, int x1, int y1, int x2, int y2, int color);

// *** BitBlit functions ***
/// Функция производит блиттинг прямоугольной области из src в bitmap (Copy).
/// Не производит сдвиг и отсечение.
/// Rop: src
void ZXRawBitBlitCopy(ZXBitmapRef bitmap, int x, int y,
                  ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight);
/// Функция производит блиттинг прямоугольной области из src в bitmap
/// Не производит сдвиг и отсечение.
/// Rop: bitmap Or src
void ZXRawBitBlitOr(ZXBitmapRef bitmap, int x, int y,
                    ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight);
/// Функция производит блиттинг прямоугольной области из src в bitmap
/// Не производит сдвиг и отсечение.
/// Rop: bitmap And src
void ZXRawBitBlitAnd(ZXBitmapRef bitmap, int x, int y,
                     ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight);
/// Функция производит блиттинг прямоугольной области из src в bitmap
/// Не производит сдвиг и отсечение.
/// Rop: bitmap Xor src
void ZXRawBitBlitXor(ZXBitmapRef bitmap, int x, int y,
                     ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight);
/// Функция производит блиттинг прямоугольной области из src в bitmap
/// Не производит сдвиг и отсечение.
/// Rop: Not src
void ZXRawBitBlitNotCopy(ZXBitmapRef bitmap, int x, int y,
                     ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight);
/// Функция производит блиттинг прямоугольной области из src в bitmap
/// Не производит сдвиг и отсечение.
/// Rop: bitmap Or (Not src)
void ZXRawBitBlitNotOr(ZXBitmapRef bitmap, int x, int y,
                       ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight);
/// Функция производит блиттинг прямоугольной области из src в bitmap
/// Не производит сдвиг и отсечение.
/// Rop: bitmap And (Not src)
void ZXRawBitBlitNotAnd(ZXBitmapRef bitmap, int x, int y,
                        ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight);
/// Функция производит блиттинг прямоугольной области из src в bitmap
/// Не производит сдвиг и отсечение.
/// Rop: bitmap Xor (Not src)
void ZXRawBitBlitNotXor(ZXBitmapRef bitmap, int x, int y,
                        ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight);
/// Функция производит блиттинг прямоугольной области из src в bitmap по маске mask
/// Не производит сдвиг и отсечение.
/// Rop: src(masked)
void ZXRawMaskBlitCopy(ZXBitmapRef bitmap, int x, int y,
                   ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXData maskData);
/// Функция производит блиттинг прямоугольной области из src в bitmap по маске mask
/// Не производит сдвиг и отсечение.
/// Rop: bitmap Or src(masked)
void ZXRawMaskBlitOr(ZXBitmapRef bitmap, int x, int y,
                     ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXData maskData);
/// Функция производит блиттинг прямоугольной области из src в bitmap по маске mask
/// Не производит сдвиг и отсечение.
/// Rop: bitmap And src(masked)
void ZXRawMaskBlitAnd(ZXBitmapRef bitmap, int x, int y,
                      ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXData maskData);
/// Функция производит блиттинг прямоугольной области из src в bitmap по маске mask
/// Не производит сдвиг и отсечение.
/// Rop: bitmap Xor src(masked)
void ZXRawMaskBlitXor(ZXBitmapRef bitmap, int x, int y,
                      ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXData maskData);
/// Функция производит блиттинг прямоугольной области из src в bitmap по маске mask
/// Не производит сдвиг и отсечение.
/// Rop: (Not src)(masked)
void ZXRawMaskBlitNotCopy(ZXBitmapRef bitmap, int x, int y,
                      ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXData maskData);
/// Функция производит блиттинг прямоугольной области из src в bitmap по маске mask
/// Не производит сдвиг и отсечение.
/// Rop: bitmap Or (Not src)(masked)
void ZXRawMaskBlitNotOr(ZXBitmapRef bitmap, int x, int y,
                        ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXData maskData);
/// Функция производит блиттинг прямоугольной области из src в bitmap по маске mask
/// Не производит сдвиг и отсечение.
/// Rop: bitmap And (Not src)(masked)
void ZXRawMaskBlitNotAnd(ZXBitmapRef bitmap, int x, int y,
                         ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXData maskData);
/// Функция производит блиттинг прямоугольной области из src в bitmap по маске mask
/// Не производит сдвиг и отсечение.
/// Rop: bitmap Xor (Not src)(masked)
void ZXRawMaskBlitNotXor(ZXBitmapRef bitmap, int x, int y,
                         ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXData maskData);

/// Функция производит блиттинг прямоугольной области из src в bitmap
/// Не производит сдвиг и отсечение.
/// Rop: по параметру
void ZXRawBitBlit(ZXBitmapRef bitmap, int x, int y,
                  ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, int rop);
/// Функция производит блиттинг прямоугольной области из src в bitmap по маске mask
/// Не производит сдвиг и отсечение.
/// Rop: по параметру
void ZXRawMaskBlit(ZXBitmapRef bitmap, int x, int y,
                   ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXData maskData, int rop);

// ====================================================================
// Standart
// ====================================================================
// *** Pixels access ***
/// Функция утанавливает пиксель.
/// Не производит сдвиг, но отсекает по границам битмапа.
void ZXSetPixel(ZXBitmapRef bitmap, int x, int y, int color);
/// Функция утанавливает пиксель, используя растровую операцию rop.
/// Не производит сдвиг, но отсекает по границам битмапа.
void ZXSetPixelRop(ZXBitmapRef bitmap, int x, int y, int color, int rop);
/// Функция закрашивает пиксель.
/// Не производит сдвиг, но отсекает по границам битмапа.
/// Работает быстрее функции ZXSetPixel.
void ZXPixelOn(ZXBitmapRef bitmap, int x, int y);
/// Функция стирает пиксель.
/// Не производит сдвиг, но отсекает по границам битмапа.
/// Работает быстрее функции ZXSetPixel.
void ZXPixelOff(ZXBitmapRef bitmap, int x, int y);
/// Функция инвертирует пиксель.
/// Не производит сдвиг, но отсекает по границам битмапа.
/// Работает быстрее функции ZXSetPixel.
void ZXPixelChange(ZXBitmapRef bitmap, int x, int y);

int ZXGetPixel(ZXBitmapRef bitmap, int x, int y);

// *** Point drawing ***
/// Функция устанавливает точку.
/// Производит сдвиг, отсекает по границам битмапа или прямоугольника отсечения.
/// Работает быстрее функции ZXSetPixel.
void ZXSetPoint(ZXBitmapRef bitmap, int x, int y, int color);
/// Функция устанавливает точку, используя растровую операцию rop.
/// Производит сдвиг, отсекает по границам битмапа или прямоугольника отсечения.
/// Работает быстрее функции ZXSetPixel.
void ZXSetPointRop(ZXBitmapRef bitmap, int x, int y, int color, int rop);
#define ZXPointOn(bitmap, x, y, color) ZXSetPoint((bitmap), (x), (y), (ZXWhite))
#define ZXPointOff(bitmap, x, y, color) ZXSetPoint((bitmap), (x), (y), (ZXBlack))
#define ZXPointChange(bitmap, x, y, color) ZXSetPoint((bitmap), (x), (y), (ZXInvert))
/// Функция получает цвет точки.
/// Производит сдвиг, отсекает по границам битмапа или прямоугольника отсечения.
/// Работает быстрее функции ZXGetPixel.
int ZXGetPoint(ZXBitmapRef bitmap, int x, int y);

// *** Lines drawing ***
/// Функция рисует линию.
/// Производит сдвиг и отсечение.
void ZXDrawLine(ZXBitmapRef bitmap, int x1, int y1, int x2, int y2, int color);
/// Функция рисует горизонтальную линию.
/// Производит сдвиг и отсечение.
/// Всегда рисует последний писель.
void ZXDrawHorz(ZXBitmapRef bitmap, int y, int x1, int x2, int color);
/// Функция рисует вертикальную линию.
/// Производит сдвиг и отсечение.
/// Всегда рисует последний писель.
void ZXDrawVert(ZXBitmapRef bitmap, int x, int y1, int y2, int color);

// *** Rects drawing ***
/// Функция рисует прямоугольник.
/// Производит сдвиг и отсечение.
void ZXDrawRect(ZXBitmapRef bitmap, int x1, int y1, int x2, int y2, int color);
/// Функция рисует стороны прямоугольника.
/// Производит сдвиг и отсечение.
void ZXDrawSides(ZXBitmapRef bitmap, int x1, int y1, int x2, int y2, int sides, int color);
/// Функция рисует заполненный прямоугольник.
/// Производит сдвиг и отсечение.
void ZXFillRect(ZXBitmapRef bitmap, int x1, int y1, int x2, int y2, int color);

// *** Ellipse drawing ***
/// Функция рисует эллипс.
/// Производит сдвиг и отсечение.
void ZXDrawEllipse(ZXBitmapRef bitmap, int x1, int y1, int x2, int y2, int color);
/// Функция рисует заполненный эллипс.
/// Производит сдвиг и отсечение.
void ZXFillEllipse(ZXBitmapRef bitmap, int x1, int y1, int x2, int y2, int color);
/// Функция заполняет bitmap заданным цветом
void ZXClear(ZXBitmapRef bitmap, int color);

// ---
int ZXIsValidBitBlitSrcParams(ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight);
int ZXClipBitBlitParams(ZXBitmapRef bitmap, int *x, int *y, int *srcX, int *srcY, int *srcWidth, int *srcHeight);
// ---

// Not 0 - ok
int ZXBitBlit(ZXBitmapRef bitmap, int x, int y,
              ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, int rop);

int ZXMaskBlit(ZXBitmapRef bitmap, int x, int y,
               ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXData maskData, int rop);

void ZXDrawBitmap(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src);
void ZXDrawBitmapRop(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, int rop);
void ZXMaskBitmap(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, ZXData maskData);
void ZXMaskBitmapRop(ZXBitmapRef bitmap, int x, int y, ZXBitmapRef src, ZXData maskData, int rop);

// WARN: THIS FUNCTIONS NOT OPTIMIZED YET!
int ZXStrechBlitRop(ZXBitmapRef bitmap, int x, int y, int width, int height,
                    ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, int rop);
int ZXStrechBlit(ZXBitmapRef bitmap, int x, int y, int width, int height,
                 ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight);
int ZXStrechMaskBlitRop(ZXBitmapRef bitmap, int x, int y, int width, int height,
                    ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXData maskData, int rop);
int ZXStrechMaskBlit(ZXBitmapRef bitmap, int x, int y, int width, int height,
                 ZXBitmapRef src, int srcX, int srcY, int srcWidth, int srcHeight, ZXData maskData);

int ZXStrechDrawBitmap(ZXBitmapRef bitmap, int x, int y, int width, int height, ZXBitmapRef src);
int ZXStrechDrawBitmapRop(ZXBitmapRef bitmap, int x, int y, int width, int height, ZXBitmapRef src, int rop);
int ZXStrechMaskBitmap(ZXBitmapRef bitmap, int x, int y, int width, int height, ZXBitmapRef src, ZXData maskData);
int ZXStrechMaskBitmapRop(ZXBitmapRef bitmap, int x, int y, int width, int height, ZXBitmapRef src, ZXData maskData, int rop);
// ====================================================================
// Fast pixel acess
// ====================================================================

// int ZXFastPixelOn(ZXBitmapRef bitmap, int x, int y);
// fast pixel on
#define ZXFastPixelOn(bitmap, x, y) \
    (*ZXBitmapPixelPtrM((bitmap), (x), (y)) |= (0x80 >> (x) % 8))

// int ZXFastPixelOff(ZXBitmapRef bitmap, int x, int y);
// fast pixel off
#define ZXFastPixelOff(bitmap, x, y) \
    (*ZXBitmapPixelPtrM((bitmap), (x), (y)) &= ~(0x80 >> (x) % 8))

// int ZXFastPixelChange(ZXBitmapRef bitmap, int x, int y);
// fast pixel invert
#define ZXFastPixelChange(bitmap, x, y) \
    (*ZXBitmapPixelPtrM((bitmap), (x), (y)) ^= (0x80 >> (x) % 8))

// void ZXFastSetPixel(ZXBitmapRef bitmap, int x, int y, color);
// fast set pixel
#define ZXFastSetPixel(bitmap, x, y, color) \
do { \
    switch((color)) { \
        case 0: ZXFastPixelOff((bitmap), (x), (y)); \
                break; \
        case 1: ZXFastPixelOn((bitmap), (x), (y)); \
                break; \
        case 2: ZXFastPixelChange((bitmap), (x), (y)); \
                break; \
        default: \
                break; \
    } \
} while(0)

// void ZXFastGetPixel(ZXBitmapRef bitmap, int x, int y);
// fast get pixel
//FIXME: optimize me please
#define ZXFastGetPixel(bitmap, x, y) \
    ((*ZXBitmapPixelPtrM((bitmap), (x), (y)) >> (7 - (x) % 8)) & 0x01)

// ====================================================================
// Bitmap helpers
// ====================================================================

// int ZXCalcScanSize(int width);
// calculate scanSize for width
#define ZXCalcScanSizeM(w) ((((w)+7)/8))
int ZXCalcScanSize(int width);

// int ZXCalcFullBytes(int x, int width)
#define ZXCalcFullBytesM(x, width) \
    (((x) + (width)) / 8 - ((x) + 7) / 8)
int ZXCalcFullBytes(int x, int width);

// ZXPixelPtrData(ZXData data, int scanSize, int x, int y);
// calculate pixel ptr for data, scanSize, x, y
#define ZXPixelPtrDataM(data, scanSize, x, y) \
    ((data) + (scanSize) * (y) + (x) / 8)
ZXData ZXPixelPtrData(ZXData data, int scanSize, int x, int y);

// int ZXBitmapScanSize(ZXBitmapRef bitmap);
// return scanSize for bitmap
#ifdef USE_PRECALC_SCANSIZE
    #define ZXBitmapScanSizeM(bitmap) ((bitmap)->scanSize)
#else
    #define ZXBitmapScanSizeM(bitmap) (ZXCalcScanSize((bitmap)->width))
#endif
int ZXBitmapScanSize(ZXBitmapRef bitmap);

// return not 0 if need offset coords
#ifdef USE_OFFSET
    #define ZXBitmapIsOffsetM(bitmap) ((bitmap)->isOffset)
#else
    #define ZXBitmapIsOffsetM(bitmap) (0)
#endif
int ZXBitmapIsOffset(ZXBitmapRef bitmap);

// return not 0 if need clip coords
#ifdef USE_CLIPRECT
    #define ZXBitmapIsClipRectM(bitmap) ((bitmap)->isClipRect)
#else
    #define ZXBitmapIsClipRectM(bitmap) (0)
#endif
int ZXBitmapIsClipRect(ZXBitmapRef bitmap);

// return not 0 if need draw last pixel
#define ZXBitmapIsDrawLastPixelM(bitmap) ((bitmap)->isDrawLastPixel)
int ZXBitmapIsDrawLastPixel(ZXBitmapRef bitmap);

// ZXBitmapPixelPtr(ZXBitmapRef bitmap, int x, int y);
// calculate pixel ptr for bitmap, x, y
#define ZXBitmapPixelPtrM(bitmap, x, y) \
    ((bitmap)->data + ZXBitmapScanSizeM(bitmap) * (y) + (x) / 8)
ZXData ZXBitmapPixelPtr(ZXBitmapRef bitmap, int x, int y);

// int ZXBitmapBeginPtr(ZXBitmapRef bitmap, int x, int y);
// return first full byte for coord
#define ZXBitmapBeginPtrM(bitmap, x, y) \
    ((bitmap)->data + ZXBitmapScanSizeM((bitmap)) * (y) + ((x) + 7) / 8)
ZXData ZXBitmapBeginPtr(ZXBitmapRef bitmap, int x, int y);

// int ZXBitmapEndPtr(ZXBitmapRef bitmap, int x, int y);
// return end full byte for coord
#define ZXBitmapEndPtrM(bitmap, x, y) \
    ((bitmap)->data + ZXBitmapScanSizeM((bitmap)) * (y) + ((x) + 1) / 8)
ZXData ZXBitmapEndPtr(ZXBitmapRef bitmap, int x, int y);

// int ZXBitmapDataSize(ZXBitmapRef bitmap)
#define ZXBitmapDataSizeM(bitmap) \
    (ZXBitmapScanSizeM(bitmap) * (bitmap)->height)
int ZXBitmapDataSize(ZXBitmapRef bitmap);

// ====================================================================
// Math
// ====================================================================


#endif /* zxGraphics_h */
