/* FROM : http://www.rosettacode.org/wiki/Run-length_encoding/C */
/* This page was last modified on 27 December 2012, at 06:48. */
/* Content is available under GNU Free Documentation License 1.2 unless otherwise noted. */

#ifndef MY_RLE_H
#define MY_RLE_H

#ifdef __cplusplus
extern "C" {
#endif
    
    char           *DoRleJob( char *pInputFileData, unsigned int inputFileSize, unsigned int command, unsigned int *pDataSize);
    BOOL            CheckShrFileFormat( char *pInputFileData, unsigned int inputFileSize);

#ifdef __cplusplus
}
#endif

#endif
