/* WARING: DO NOT EDIT THIS CODE, THIS CODE IS AUTO GENERATED,
   SEE zxGraphicsGen.py

   Copyright © 2018 errorsoft. All rights reserved. */

#define RawBitBlit(WriteByte1, WriteByte2, WriteByte3, WriteByte4) \
do { \
    /* dest and src */ \
    ZXData pScan, pSrcScan, pScanEnd; \
    ZXData p, pSrc; \
    unsigned char b; \
    int scan, srcScan; \
    /* offsets, flags, etc */ \
    int offset, srcOffset, endPixels; \
    int fullBytes, endLine; \
    int compRealOffset, realOffset; \
    int isRegular; \
 \
    /* calculate the sizes of scan lines */ \
    scan = BitmapScanSize(bitmap); \
    srcScan = BitmapScanSize(src); \
 \
    /* get pointers to a byte corresponding to a pixel */ \
    pScan = BitmapPixelPtr(bitmap, x, y); \
    pSrcScan = BitmapPixelPtr(src, srcX, srcY); \
    /* calculating end vertical line */ \
    endLine = y + srcHeight; \
 \
    /* bitmap offset */ \
    offset = x % 8; \
    /* src offset */ \
    srcOffset = srcX % 8; \
 \
    /* get information about how much pixels should be shifted, */ \
    /* important: when (offset < srcOffset) one byte is completely absorbed when outputting the left byte */ \
    if(offset >= srcOffset) \
        realOffset = offset - srcOffset; \
    else \
        realOffset = 8 - (srcOffset - offset); \
    compRealOffset = 8 - realOffset; \
 \
    /* count of full-size bytes */ \
    fullBytes = CalcFullBytes(x, srcWidth); \
    /* offset end pixel */ \
    endPixels = (x + srcWidth) % 8; \
    /* regular draw or not */ \
    /* a) snap to right colum */ \
    /* b) snap to left colum */ \
    isRegular = (offset + srcWidth >= 8) || (offset == 0); \
 \
    /* main loop y */ \
    for(; y < endLine; y++) { \
        p = pScan; \
        pSrc = pSrcScan; \
 \
        /* take care of left byte */ \
        /* xxxxxxxx XXXXXXXX xxxxxxxx */ \
        /* ^ we here */ \
        if(offset != 0) { \
            if (offset >= srcOffset) { \
                b = *pSrc >> realOffset; \
 \
                /* need next byte if we haven't real offset */ \
                if(realOffset == 0) \
                    pSrc++; \
            } \
            else { \
                b = (*pSrc << compRealOffset) | \
                (*(pSrc + 1) >> realOffset); \
                /* we completely used a byte, take a new */ \
                pSrc++; \
            } \
 \
            /* combine dest and src */ \
            if(isRegular) \
                WriteByte1; \
            else \
                WriteByte2; \
 \
            p++; \
        } \
 \
        /* main loop x */ \
        /* xxxxxxxx XXXXXXXX xxxxxxxx */ \
        /*          ^ we here */ \
        pScanEnd = p + fullBytes; \
        if(realOffset != 0) { \
            for(; p < pScanEnd; p++) { \
                b = (*pSrc << compRealOffset) | \
                (*(pSrc + 1) >> realOffset); \
 \
                /* combine dest and src */ \
                WriteByte3; \
 \
                pSrc++; \
            } \
        } \
        else { \
            for(; p < pScanEnd; p++) { \
                b = *pSrc; \
 \
                /* combine dest and src */ \
                WriteByte3; \
 \
                pSrc++; \
            } \
        } \
 \
        /* take care of right byte */ \
        /* xxxxxxxx XXXXXXXX xxxxxxxx */ \
        /*                   ^ we here */ \
        if(isRegular && endPixels) { \
            if(realOffset != 0) { \
                b = *pSrc << compRealOffset; \
                /* .. end pixel is double byte src */ \
                if(endPixels > realOffset) \
                    b |= *(pSrc + 1) >> realOffset; \
            } \
            else \
                b = *pSrc; \
 \
            /* combine dest and src */ \
            WriteByte4; \
        } \
        pScan += scan; \
        pSrcScan += srcScan; \
    } \
} while(0)

#define RawMaskBlit(WriteByte1, WriteByte2, WriteByte3, WriteByte4) \
do { \
    /* dest and src */ \
    ZXData pScan, pSrcScan, pMaskScan, pScanEnd; \
    ZXData p, pSrc, pMask; \
    unsigned char b, m; \
    int scan, srcScan; \
    /* offsets, flags, etc */ \
    int offset, srcOffset, endPixels; \
    int fullBytes, endLine; \
    int compRealOffset, realOffset; \
    int isRegular; \
 \
    /* calculate the sizes of scan lines */ \
    scan = BitmapScanSize(bitmap); \
    srcScan = BitmapScanSize(src); \
 \
    /* get pointers to a byte corresponding to a pixel */ \
    pScan = BitmapPixelPtr(bitmap, x, y); \
    pSrcScan = BitmapPixelPtr(src, srcX, srcY); \
    pMaskScan = PixelPtrData(maskData, srcScan, srcX, srcY); \
    /* calculating end vertical line */ \
    endLine = y + srcHeight; \
 \
    /* bitmap offset */ \
    offset = x % 8; \
    /* src offset */ \
    srcOffset = srcX % 8; \
 \
    /* get information about how much pixels should be shifted, */ \
    /* important: when (offset < srcOffset) one byte is completely absorbed when outputting the left byte */ \
    if(offset >= srcOffset) \
        realOffset = offset - srcOffset; \
    else \
        realOffset = 8 - (srcOffset - offset); \
    compRealOffset = 8 - realOffset; \
 \
    /* count of full-size bytes */ \
    fullBytes = CalcFullBytes(x, srcWidth); \
    /* offset end pixel */ \
    endPixels = (x + srcWidth) % 8; \
    /* regular draw or not */ \
    /* a) snap to right colum */ \
    /* b) snap to left colum */ \
    isRegular = (offset + srcWidth >= 8) || (offset == 0); \
 \
    /* main loop y */ \
    for(; y < endLine; y++) { \
        p = pScan; \
        pSrc = pSrcScan; \
        pMask = pMaskScan; \
 \
        /* take care of left byte */ \
        /* xxxxxxxx XXXXXXXX xxxxxxxx */ \
        /* ^ we here */ \
        if(offset != 0) { \
            if (offset >= srcOffset) { \
                b = *pSrc >> realOffset; \
                m = *pMask >> realOffset; \
 \
                /* need next byte if we haven't real offset */ \
                if(realOffset == 0) { \
                    pSrc++; \
                    pMask++; \
                } \
            } \
            else { \
                b = (*pSrc << compRealOffset) | \
                (*(pSrc + 1) >> realOffset); \
                m = (*pMask << compRealOffset) | \
                (*(pMask + 1) >> realOffset); \
                /* we completely used a byte, take a new */ \
                pSrc++; \
                pMask++; \
            } \
 \
            /* combine dest and src */ \
            if(isRegular) { \
                WriteByte1; \
                *p = (*p & ~m) | (b & m); \
            } \
            else { \
                WriteByte2; \
                *p = (*p & ~m) | (b & m); \
            } \
 \
            p++; \
        } \
 \
        /* main loop x */ \
        /* xxxxxxxx XXXXXXXX xxxxxxxx */ \
        /*          ^ we here */ \
        pScanEnd = p + fullBytes; \
        if(realOffset != 0) { \
            for(; p < pScanEnd; p++) { \
                b = (*pSrc << compRealOffset) | \
                (*(pSrc + 1) >> realOffset); \
                m = (*pMask << compRealOffset) | \
                (*(pMask + 1) >> realOffset); \
 \
                /* combine dest and src */ \
                WriteByte3; \
                *p = (*p & (~m)) | (b & m); \
 \
                pSrc++; \
                pMask++; \
            } \
        } \
        else { \
            for(; p < pScanEnd; p++) { \
                b = *pSrc; \
                m = *pMask; \
 \
                /* combine dest and src */ \
                WriteByte3; \
                *p = (*p & (~m)) | (b & m); \
 \
                pSrc++; \
                pMask++; \
            } \
        } \
 \
        /* take care of right byte */ \
        /* xxxxxxxx XXXXXXXX xxxxxxxx */ \
        /*                   ^ we here */ \
        if(isRegular && endPixels) { \
            if(realOffset != 0) { \
                b = *pSrc << compRealOffset; \
                m = *pMask << compRealOffset; \
                /* .. end pixel is double byte src */ \
                if(endPixels > realOffset) { \
                    b |= *(pSrc + 1) >> realOffset; \
                    m |= *(pMask + 1) >> realOffset; \
                } \
            } \
            else { \
                b = *pSrc; \
                m = *pMask; \
            } \
 \
            /* combine dest and src */ \
            WriteByte4; \
            *p = (*p & (~m)) | (b & m); \
        } \
        pScan += scan; \
        pSrcScan += srcScan; \
        pMaskScan += srcScan; \
    } \
} while(0)

