
#ifndef MY_BMP_H
#define MY_BMP_H

#ifdef __cplusplus
extern "C" {
#endif

    void    doDumpBmp( char *pFilePathname, char *pInputFileData, unsigned int inputFileSize);
    char   *DoBmpJob( char *pInputFileData, unsigned int inputFileSize, unsigned int command, unsigned int *pDataSize);

    char   *DoAddPaletteToBmp( char *pInputFileData, unsigned int inputFileSize, unsigned int command, unsigned int *pDataSize);

    void    DoInsertPaletteToBmp( char *pInputFileData, unsigned int uInputFileSize, char **pOutputFileData, unsigned int *puOutputFileSize, unsigned int uFrom, unsigned int uTo);

    BOOL    CheckBmpFileFormat( char *pInputFileData, unsigned int inputFileSize);

    char   *DoSwapColor( char *pInputFileData, unsigned int inputFileSize, unsigned int uSwapColumnA, unsigned int uSwapColumnB, unsigned int *pDataSize);
    void    DoComparePalette( char *pFirstFileData, unsigned int uFirstFileSize, char *pSecondFileData, unsigned int uSecondFileSize);
    void    DoCopyPalette( char *pFirstFileData, unsigned int uFirstFileSize, char *pSecondFileData, unsigned int uSecondFileSize);

#ifdef __cplusplus
}
#endif

#endif
