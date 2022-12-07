
#ifndef MY_BMP_H
#define MY_BMP_H

#ifdef __cplusplus
extern "C" {
#endif

    void    doDumpBmp( char *pFilePathname, char  *pInputFileData, unsigned int inputFileSize);
    char   *DoBmpJob( char *pInputFileData, unsigned int inputFileSize, unsigned int command, unsigned int *pDataSize);
    BOOL    CheckBmpFileFormat( char *pInputFileData, unsigned int inputFileSize);

#ifdef __cplusplus
}
#endif

#endif
