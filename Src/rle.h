/* FROM : http://www.rosettacode.org/wiki/Run-length_encoding/C */
/* This page was last modified on 27 December 2012, at 06:48. */
/* Content is available under GNU Free Documentation License 1.2 unless otherwise noted. */

#ifndef MY_RLE_H
#define MY_RLE_H

#ifdef __cplusplus
extern "C" {
#endif
    
    unsigned int    rle4_decode( char *pOutputFileData, unsigned int uOutputFileSize, char *pInputFileData, unsigned int uLen);
    BOOL            CheckShrFileFormat( char *pInputFileData, unsigned int inputFileSize);
    char           *DoRleJob( char *pInputFileData, unsigned int inputFileSize, unsigned int command, unsigned int *pDataSize);
    void            doDumpPic( char *pFilePathname, char *pInputFileData, unsigned int inputFileSize);
    char           *doExtractTextFromBinay( char *pInputFileData, unsigned int uInputFileSize, unsigned int minSentenseLen, unsigned int *pDataSize);

#ifdef __cplusplus
}
#endif

#endif
