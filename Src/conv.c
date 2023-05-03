
#include <io.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <direct.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <windows.h>

#include "main.h"

#include "conv.h"

// _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)


/**
* @fn void exitOnError( char *pExplain, char **pDetail, char *pInfo, unsigned int uError)
* @brief Center for exit when an error occur
*
* @param[in]        pExplain
* @param[in,out]    pDetail     // this pointer if free
* @param[in]        pInfo
* @param[in]        uError
*
*/
void exitOnError( char *pExplain, char *pDetail, char *pInfo, unsigned int uError)
{
    const char  *pEndString = NULL;
    const char  *pSpaceString = NULL;
    char        *pMessage = NULL;
    size_t       uLen;

    if (pExplain)
    {
        pEndString = "\"";
        pSpaceString = " ";

        uLen = strlen( (const char *)pExplain);
        if (pDetail)
        {
            uLen += strlen( (const char *)pDetail);
        }
        if (pInfo)
        {
            uLen += strlen( (const char *)pInfo);
        }
        uLen += (size_t )32;    // a marge for char ' '; '"'

        pMessage = (char *)calloc( 1, uLen);
        if (pMessage)
        {
            (void)strncpy_s( pMessage, uLen, (const char *)pExplain, strlen( (const char *)pExplain));

            if (pDetail)
            {
                pMessage = strcat( pMessage, (const char *)pSpaceString);
                pMessage = strcat( pMessage, (const char *)pSpaceString);
                pMessage = strcat( pMessage, (const char *)pEndString);
                pMessage = strcat( pMessage, (const char *)pDetail);
                pMessage = strcat( pMessage, (const char *)pEndString);
            }

            if (pInfo)
            {
                pMessage = strcat( pMessage, (const char *)pSpaceString);
                pMessage = strcat( pMessage, (const char *)pSpaceString);
                pMessage = strcat( pMessage, (const char *)pEndString);
                pMessage = strcat( pMessage, (const char *)pInfo);
                pMessage = strcat( pMessage, (const char *)pEndString);
            }

            printf( "CONVM : %s\n", pMessage);

            free( pMessage);
        }
    }

    exit( uError);
}

/**
* @fn char *parseAntiSlashChar( char **pPathname)
* @brief Replace character anti slash '\\' by a slash '/'
*
* @param[in,out]   pPathname
*
* @@return A new pointer pResultFilename, this is pPathname content corrected and pPathname is free or NULL
*/
char *parseAntiSlashChar( char **pPathname)
{
    char    *pResultFilename = NULL;
    char    *pInRunner;
    char    *pOutRunner;
    //int      iModif = 0;

    pResultFilename = (char *)calloc( 1, _MAX_PATH + _MAX_PATH);
    if (pResultFilename)
    {
        pInRunner = *pPathname;
        pOutRunner = pResultFilename;
        while (*pInRunner != '\0')
        {
            if (*pInRunner == '\\')
            {
                *pOutRunner = '/';
                pOutRunner++;
                //iModif++;
                if (*pInRunner == '\\')
                {
                    pInRunner++;
                }
            }

            *pOutRunner = *pInRunner;
            pInRunner++;
            pOutRunner++;
        }
        *pOutRunner = '\0';

        //if (iModif != 0)
        //{
        //  // printf( "Full in  : %s\n", *pPathname);
        //  printf( "Working file : %s\n\n", pResultFilename);
        //  printf( "Anti Slash corrected is = %d\n", iModif);
        //}

        free( *pPathname);
        *pPathname = NULL;
    }

    return pResultFilename;
}


/**
* @fn char *parseSpaceChar( char **pPathname)
* @brief parse the pPathname if a space if found add an anti slash before
*
* @param[in,out]   pPathname
*
* @return A new pointer pResultFilename, this is pPathname content corrected and pPathname is free or NULL
*/
char *parseSpaceChar( char **pPathname)
{
    char    *pResultFilename = NULL;
    char    *pInRunner;
    char    *pOutRunner;
    //int      iModif = 0;

    if ((pPathname) && (*pPathname))
    {
        pResultFilename = (char *)calloc(1, _MAX_PATH + _MAX_PATH);
        if (pResultFilename)
        {
            pInRunner = *pPathname;
            pOutRunner = pResultFilename;
            while (*pInRunner != '\0')
            {
                if (*pInRunner == ' ')
                {
                    *pOutRunner = '\\';
                    pOutRunner++;
                    //iModif++;
                }
                else if (*pInRunner == '\\')
                {
                    *pOutRunner = '\\';
                    pOutRunner++;
                }

                *pOutRunner = *pInRunner;
                pInRunner++;
                pOutRunner++;
            }
            *pOutRunner = '\0';

            //if (iModif != 0)
            //{
            //  //printf( "Full in  : %s\n", *pPathname);
            //  printf( "Working file : %s\n\n", pResultFilename);
            //  printf( "Space char corrected is = %d\n", iModif);
            //}

            free( *pPathname);
            *pPathname = NULL;
        }
    }

    return pResultFilename;
}


/**
* @fn char *getBasePathname( const char *pPathname, unsigned int uExtension)
* @brief Parse the pPathname to get folder(s) only
*
* @param[in]    pPathname
* @param[in]    uExtension
*
* @return A new pointer pResultFilename to the base of pPathname (remove at the end the filename) or NULL
*/
char *getBasePathname( const char *pPathname, unsigned int uExtension)
{
    char       *pBackRunning;
    char       *pResultFilename = NULL;
    size_t      uLenResultFilename;
    //char        dirSeparator = '\\';      // set dirSeparator according to platform
    char        dirSeparator = '/';     // set dirSeparator according to platform

    if (pPathname)
    {
        uLenResultFilename = strlen( pPathname) + (size_t )uExtension;
        pResultFilename = (char *)calloc(1,((size_t )( uLenResultFilename) + (size_t )( 1)));
        if (pResultFilename)
        {
            (void)strncpy_s(pResultFilename, uLenResultFilename, pPathname, (int )strlen( pPathname));
            //(void)strncpy( pResultFilename, pPathname, uLenResultFilename);

            pBackRunning = (char *)&(pResultFilename[(int )strlen( pResultFilename) - 1]);
            while (*pBackRunning != dirSeparator)
            {
                pBackRunning--;
            }

            pBackRunning++;
            *pBackRunning = '\0';
        }
    }

    return pResultFilename;
}


/**
* @fn char *getFileName( char *pPathname)
* @brief Parse the pPathname to get filename
*
* @param[in]    pPathname
*
* @return the filename from the pPathname
*/
char *getFileName( char *pPathname)
{
    char    *pReturnName = NULL;
    char    *pRunning;

    if (pPathname)
    {
        for (pRunning = pPathname; *pRunning; pRunning++)
        {
            if ((*pRunning == '/') || (*pRunning == '\\') || (*pRunning == ':'))
            {
                pReturnName = pRunning;
            }
        }

        if (pReturnName)
        {
            if ((*pReturnName == '/') || (*pReturnName == '\\') || (*pReturnName == ':'))
            {
                pReturnName++;
            }
        }
    }
    return pReturnName;
}


/**
* @fn unsigned int getMyFileSize( char *pPathname)
* @brief get the size of file pPathname
*
* @param[in]    pPathname
*
* @return Size of the file pPathname or 0
*/
unsigned int getMyFileSize( char *pPathname)
{
    errno_t         iError;
    unsigned int    uSize = 0;
    FILE           *pFile = NULL;

    iError = fopen_s( &pFile, pPathname, "rb");
    if (iError == 0)
    {
        if (pFile)
        {
            fseek( pFile, 0L, SEEK_END);
            uSize = ftell( pFile);
            fclose( pFile);
        }
    }

    return uSize;
}


/**
* @fn char *readFileToMemory( char *pPathname)
* @brief read the content of file pPathname in memory
*
* @param[in]   pPathname
*
* @return Pointer to the pPathname content or NULL
*/
char *readFileToMemory( char *pPathname)
{
    FILE       *pFile = NULL;
    char       *pCharData = NULL;
    size_t      uNumberOfElementRead = 0;
    int         iFileSize = 0;
    errno_t     iError = 0;

    iFileSize = getMyFileSize( pPathname);
    if (iFileSize)
    {
        pCharData = (char *)calloc( 1, iFileSize);
        if (pCharData)
        {
            iError = fopen_s( &pFile, pPathname, "rb");
            if ((iError == 0) && (pFile != NULL))
            {
                uNumberOfElementRead = fread( pCharData, iFileSize, 1, pFile);
                (void )fclose( pFile);
                if (uNumberOfElementRead != 1)
                {
                    iError = 1;
                }
            }

            if ((iError != 0) && (pFile != NULL))
            {
                free( pCharData);
                pCharData = NULL;
            }
        }
    }
    return pCharData;
}

/**
* @fn int writeFileFromMemory( char *pPathname, char *pCharData, unsigned int uDataSize)
* @brief Write to file pPathname data from pCharData with size uDataSize
*
* @param[in]   pPathname
* @param[in]   pCharData
* @param[in]   uDataSize
*
* @return 0 if no error
*/
int writeFileFromMemory( char *pPathname, char *pCharData, unsigned int uDataSize)
{
    errno_t     iError = 0;
    FILE       *pFile = NULL;
    size_t      numberOfElementWrite = 0;

    iError = fopen_s( &pFile, (const char *)pPathname, "wb");
    if ((iError == 0) && (pFile != NULL))
    {
        numberOfElementWrite = fwrite( pCharData, uDataSize, 1, pFile);

        (void )fclose(pFile);
        if (numberOfElementWrite != 1)
        {

            iError = 1;
        }
    }

    return iError;
}

/**
* @fn BOOL pathFileExists( const char *pPathname)
* @brief Check if pPathname exist
*
* @param[in]        pPathname
*
* @return TRUE if file exist and FALSE for other case
*/
BOOL pathFileExists( const char *pPathname)
{
    int         bFileExists = FALSE;
    FILE       *pFile = NULL;

    if (fopen_s(&pFile, pPathname, "rb"))
    {
        bFileExists = FALSE;
    }
    else
    {
        bFileExists = TRUE;
        if (pFile)
        {
            fclose(pFile);
        }
    }

    return bFileExists;
}

/**
* @fn BOOL directoryExists( const char *pAbsolutePath)
* @brief Check if pAbsolutePath folder exist
*
* @param[in]        pAbsolutePath
*
* @return TRUE if folder exist and FALSE for other case
*/
BOOL directoryExists( const char *pAbsolutePath)
{
    BOOL bFolderExists = FALSE;

    if (_access(pAbsolutePath, 0) == 0)
    {
        struct stat status;

        (void)stat(pAbsolutePath, &status);
        if ((status.st_mode & S_IFDIR) != 0)
            bFolderExists = TRUE;
        else
            bFolderExists = FALSE;
    }

    return bFolderExists;
}

/**
* @fn char *doConvertJob( char *pInputFileData, unsigned int uInputFileSize, unsigned int uCommand, unsigned int uTabColumns, unsigned int *pDataSize)
* @brief parse pInputFileData of uInputFileSize size following uCommand, could use uTabColumns and return the data size of pOutputFileData in pDataSize
*
* @param[in]        pInputFileData
* @param[in]        uInputFileSize
* @param[in]        uCommand
* @param[in]        uTabColumns
* @param[in,out]    pDataSize
*
* @return a new allocated pointer pOutputFileData
*/
char *doConvertJob( char *pInputFileData, unsigned int uInputFileSize, unsigned int uCommand, unsigned int uTabColumns, unsigned int *pDataSize)
{
    char           *pOutputFileData = NULL;
    char           *pInputRunner = NULL;
    char           *pFollowChar = NULL;
    char           *pOutputRunner = NULL;
    unsigned int    uIndex = 0;
    unsigned int    uLoop = 0;
    int             uCurcolumn = 0;
    BOOL            bMultipleSupport = FALSE;

    if (uInputFileSize <= 524288)
    {
        pOutputFileData = calloc(1,(size_t)(524288));    // allocate big place to work
        if (pOutputFileData)
        {
            pInputRunner = pInputFileData;
            pOutputRunner = pOutputFileData;
            uCurcolumn = 0;

            while (uIndex < uInputFileSize)
            {
                switch (uCommand)
                {
                    case eCRLF:  /* 13 == 0x0D == CR == Carriage Return == '\r' */  /* 10 == 0x0A == LF == Line Feed == '\n' */
                    {
                        if (*pInputRunner == '\r')
                            *pOutputRunner = 10;
                        else
                            *pOutputRunner = *pInputRunner;
                        pOutputRunner++;
                    }
                    break;

                    case eLFCR:  /* 10 == 0x0A == LF == Line Feed == '\n' */  /* 13 == 0x0D == CR == Carriage Return == '\r' */
                    {
                        if (*pInputRunner == '\n')
                            *pOutputRunner = 13;
                        else
                            *pOutputRunner = *pInputRunner;
                        pOutputRunner++;
                    }
                    break;

                    case eDOUBLE_0A: /* 10 == 0x0A == LF == Line Feed == '\n' */
                    {
                        if (uIndex < uInputFileSize)
                        {
                            pFollowChar = pInputRunner;
                            pFollowChar++;
                            if ((*pInputRunner == '\n') && (*pFollowChar == '\n') && (bMultipleSupport == FALSE))
                            {
                                *pOutputRunner = 10;
                                bMultipleSupport = TRUE;
                                pInputRunner++;
                                uIndex++;
                            }
                            else
                            {
                                *pOutputRunner = *pInputRunner;
                                bMultipleSupport = FALSE;
                            }
                        }
                        pOutputRunner++;
                    }
                    break;

                    case eDOUBLE_0D: /* 13 == 0x0D == CR == Carriage Return == '\r' */
                    {
                        if (uIndex <= uInputFileSize)
                        {
                            pFollowChar = pInputRunner;
                            pFollowChar++;
                            if ((*pInputRunner == '\r') && (*pFollowChar == '\r') && (bMultipleSupport == FALSE))
                            {
                                *pOutputRunner = 13;
                                bMultipleSupport = TRUE;
                                pInputRunner++;
                                uIndex++;
                            }
                            else
                            {
                                *pOutputRunner = *pInputRunner;
                                bMultipleSupport = FALSE;
                            }
                        }
                        pOutputRunner++;
                    }
                    break;

                    case eDETAB:
                    {
                        switch (*pInputRunner)
                        {
                            case '\t':
                            {
                                for (uLoop = (uCurcolumn % uTabColumns); uLoop < uTabColumns; uLoop++)
                                {
                                    *pOutputRunner = ' ';
                                    uCurcolumn++;
                                    pOutputRunner++;
                                }
                            }
                            break;

                            case '\n':
                            case '\r':
                            {
                                uCurcolumn = -1;
                            }

                            default:
                            {
                                uCurcolumn++;
                                *pOutputRunner = *pInputRunner;
                                pOutputRunner++;
                            }
                            break;
                        }
                    }
                    break;

                    default:
                    break;
                }
                pInputRunner++;
                uIndex++;
            }
            *pDataSize = (int)(pOutputRunner - pOutputFileData);
        }
    }

    return pOutputFileData;
}
