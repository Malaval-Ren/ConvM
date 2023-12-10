
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <direct.h>

#include <windows.h>

#include "main.h"
#include "conv.h"
#include "rle.h"
#include "bmp.h"

#include "doAction.h"


// _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)

/*
 * where to found the wnidows error code :
 * https://docs.microsoft.com/fr-fr/windows/win32/debug/system-error-codes--0-499-
 */

 /*
  * coding rules:
  *
  * BOOL             b<Name>
  * int              i<Name>
  * enum             e<Name>
  * unsigned int     u<Name>
  * unsigned char    u<Name>
  * unsigned short   u<Name>
  * unsigned long    u<Name>
  * pointer         *p<Name>
  * handle         **h<Name>
  */


/**
 * @fn static int mySystemCmd( const char *pCommand)
 * @brief parse the arguments and check it
 *
 * @param[in]        pCommand
 *
 * @return 0 is not error, other values in case of error
 */
static int mySystemCmd( const char* pCommand)
{
    char               *pTmpCommand = NULL;
    char               *pCcmdExePath = NULL;
    size_t              cmd_exe_path_len;
    size_t              tmpCommandLen;
    int                 iError = 6;

    if (pCommand)
    {
        pCcmdExePath = getenv( "COMSPEC");
        if (pCcmdExePath)
        {
            cmd_exe_path_len = strlen(pCcmdExePath);

            tmpCommandLen = strlen( pCommand);
            pTmpCommand = (char *)calloc( 1, cmd_exe_path_len + tmpCommandLen + 5);
            if (pTmpCommand)
            {
                (void )memcpy( pTmpCommand, pCcmdExePath, cmd_exe_path_len + 1);
                (void )strcat( pTmpCommand + cmd_exe_path_len, " /c ");   // cmd
                (void )memcpy( pTmpCommand + cmd_exe_path_len + 4, pCommand, tmpCommandLen + 1);

                (void )fflush( NULL);

                iError = system( pTmpCommand);
                if (iError)
                {
                    printf( "CONVM: \"%s\") failed error = %d\n", pTmpCommand, iError);
                }

                free( pTmpCommand);
            }
        }
        else
        {
            iError = 1;
        }
    }

    return iError;
}


/**
 * @fn static char *createOutputPathname( char *pFullFilename, char *pOutPathname, enum eCommandNumber eCommand)
 * @brief alloc and change the file extension
 *
 * @param[in]        pFullFilename
 * @param[in]        pOutPathname
 * @param[in]        eCommand
 *
 * @return a new pointer to the new pathname
 */
static char *createOutputPathname( char *pFullFilename, char *pOutPathname, enum eCommandNumber eCommand)
{
    char            *pOutputPathname = NULL;
    const char      *pEndString = NULL;
    size_t           uOutputPathnameLen = 0;

    if ((pFullFilename) && (eCommand != eNONE))
    {
        if (pOutPathname)
        {
            pEndString = getFileName( pFullFilename);
            uOutputPathnameLen = strlen( (const char *)pOutPathname) + strlen( (const char *)pEndString) + (size_t )16;
        }
        else
        {
            uOutputPathnameLen = strlen( (const char *)pFullFilename) + (size_t )16;
        }

        pOutputPathname = (char *)calloc( 1, uOutputPathnameLen);
        if (pOutputPathname)
        {
            if (pOutPathname)
            {
                (void )strncpy_s( pOutputPathname, uOutputPathnameLen, pOutPathname, strlen( (const char *)pOutPathname));
                pOutputPathname = strcat( pOutputPathname, pEndString);
                pEndString = NULL;
            }
            else
            {
                (void )strncpy_s( pOutputPathname, uOutputPathnameLen, pFullFilename, strlen( (const char *)pFullFilename));
                //(void )strncpy( pOutputPathname, pfullFilename, uOutputPathnameLen);
            }

            // Change the extention of the output file
            if (pOutputPathname[strlen( pOutputPathname) - 4] == '.')
            {
                pEndString = &pOutputPathname[strlen( pOutputPathname) - 3];
            }

            switch( eCommand)
            {
                case eRLE_COMP:
                {
                    if (pEndString)
                    {
                        pOutputPathname[strlen( pOutputPathname) - 3] = 'p';
                        pOutputPathname[strlen( pOutputPathname) - 2] = 'n';
                        pOutputPathname[strlen( pOutputPathname) - 1] = 't';
                    }
                    else
                    {
                        pOutputPathname = strcat( pOutputPathname, ".pnt");
                    }
                }
                break;
                case eRLE_DECO:
                    // Fall Through
                case eTO_PIC:
                {
                    if (pEndString)
                    {
                        pOutputPathname[strlen( pOutputPathname) - 3] = 'p';
                        pOutputPathname[strlen( pOutputPathname) - 2] = 'i';
                        pOutputPathname[strlen( pOutputPathname) - 1] = 'c';
                    }
                    else
                    {
                        pOutputPathname = strcat( pOutputPathname, ".pic");
                    }
                }
                break;
                case eTO_BMP:
                {
                    if (pEndString)
                    {
                        pOutputPathname[strlen( pOutputPathname) - 3] = 'b';
                        pOutputPathname[strlen( pOutputPathname) - 2] = 'm';
                        pOutputPathname[strlen( pOutputPathname) - 1] = 'p';
                    }
                    else
                    {
                        pOutputPathname = strcat( pOutputPathname, ".bmp");
                    }
                }
                break;
                case eSWAP2COLOR:
                {
                    if (pEndString)
                    {
                        pOutputPathname[strlen( pOutputPathname) - 4] = '\0';
                    }
                    pOutputPathname = strcat( pOutputPathname, "-s.bmp");
                }
                break;
                case eEXTSPRITE:
                {
                    if (pEndString)
                    {
                        pOutputPathname[strlen( pOutputPathname) - 4] = '\0';
                    }
                    pOutputPathname = strcat( pOutputPathname, ".aii");
                }
                break;
                default:
                    // Not possible to pass here
                break;
            }
        }
    }

    return pOutputPathname;
}



/**
* @fn static void updateFileType( ConvmArguments *pContext, enum eCommandNumber eCommand, char *pfullOutputFilename)
* @brief if iix emulator is present set a filetype and aux filetype to tthe output file
*
* @param[in]        pContext
* @param[in]        eCommand
* @param[in]        pfullOutputFilename
*
*/
static void updateFileType( tConvmArguments * pContext, enum eCommandNumber eCommand, char *pfullOutputFilename)
{
    const char  *pCheckEmulatorPresent = "where iix > nul\0";
    const char  *pCmdEmulatorPnt = "iix chtyp -t PNT -a 2 ";
    const char  *pCmdEmulatorPic = "iix chtyp -t PIC -a 0 ";
    const char  *pEndString = "\"";
    char        *pDuplicateString = NULL;
    char        *pShortPathname = NULL;
    char        *ptempFilename = NULL;
    int          iError;

    if ((pContext) && (pfullOutputFilename))
    {
        pShortPathname = (char *)calloc( 1, strlen( pfullOutputFilename) + 1);
        pDuplicateString = (char *)calloc( 1, _MAX_PATH + _MAX_PATH);
        if ((!pDuplicateString) || (!pShortPathname))
        {
            exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"out of memory", NULL, NULL, 1);
        }
        else
        {
            printf( "CONVM: from = %s\n", pfullOutputFilename);
            ptempFilename = getBasePathname( pfullOutputFilename, 16);
            if (ptempFilename)
            {
                /* Change Windows pathname to Dos pathname (8.3) */
                iError = GetShortPathNameA( (LPCSTR)ptempFilename, (LPSTR )pShortPathname, (DWORD )strlen(ptempFilename) + 1);
                if (!iError)
                {
                    printf( "CONVM: GetShortPathName() failed error = %d\n", (int )GetLastError());
                    pShortPathname = strdup( pfullOutputFilename);
                }
                else
                {
                    if (!strlen( pShortPathname))
                    {
                        pShortPathname = strdup( pfullOutputFilename);
                    }
                    else
                    {
                        pShortPathname = strcat( pShortPathname, getFileName(pfullOutputFilename));
                    }
                }
                printf( "CONVM: to   = %s\n", pShortPathname);
                free( ptempFilename);
            }
            else
            {
                pShortPathname = strdup( pfullOutputFilename);
            }

            /* little cyber security protection */
            if ((*(unsigned long*)pCheckEmulatorPresent != (unsigned long)1919248503) ||
                ((*(unsigned long*)pCmdEmulatorPnt != (unsigned long)544762217) ||
                    (*(unsigned long*)pCmdEmulatorPic != ((unsigned long)544762217))))
            {
                free( pDuplicateString);
                free( pShortPathname);
            }
            else
            {
                iError = mySystemCmd( (char *)pCheckEmulatorPresent);
                if (!iError)    // Check if iix is installed to use it
                {
                    if (eCommand == eRLE_COMP)
                    {
                        (void )strncpy_s( pDuplicateString, _MAX_PATH + _MAX_PATH, pCmdEmulatorPnt, (int )strlen( pCmdEmulatorPnt));
                        pDuplicateString = strcat( pDuplicateString, pEndString);
                        pDuplicateString = strcat( pDuplicateString, pShortPathname);
                        pDuplicateString = strcat( pDuplicateString, pEndString);

                        iError = mySystemCmd(pDuplicateString);
                        if (iError)
                        {
                            printf( "CONVM: Failed to change filetype of '%s' with iix, error = %d\n", pShortPathname, errno);
                        }
                    }
                    else    // eRLE_DECO or eTO_PIC
                    {
                        (void )strncpy_s( pDuplicateString, _MAX_PATH + _MAX_PATH, pCmdEmulatorPic, (int )strlen( pCmdEmulatorPic));
                        pDuplicateString = strcat( pDuplicateString, pEndString);
                        pDuplicateString = strcat( pDuplicateString, pShortPathname);
                        pDuplicateString = strcat( pDuplicateString, pEndString);

                        iError = mySystemCmd( pDuplicateString);
                        if (iError)
                        {
                            printf( "CONVM: Failed to change filetype of '%s' with iix, error = %d\n", pShortPathname, errno);
                        }
                    }
                }
                else        // CiderPress File Attribute Preservation Tags
                {
                    (void )strncpy_s( pDuplicateString, _MAX_PATH + _MAX_PATH, pShortPathname, (int )strlen( pShortPathname));
                    if (eCommand == eRLE_COMP)   // PNT
                    {
                        pEndString = "#c00002\0";
                    }
                    else    // RLE_DECO or TO_PIC
                    {
                        pEndString = "#c10000\0"; // PIC
                    }
                    pDuplicateString = strcat( pDuplicateString, pEndString);
                    (void )rename( (const char *)pShortPathname, (const char *)pDuplicateString);
                }

                free( pDuplicateString);
                free( pShortPathname);
            }
        }
    }
}



/**
* @fn int doDetab( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
* @brief if iix emulator is present set a filetype and aux filetype to tthe output file
*
* @param[in]        pContext
* @param[in]        eCommand
* @param[in]        pfullOutputFilename
 *
 * @return 0 no error or exit program
*/
int doDetab( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
{
    char            *pfullOutputFilename = NULL;
    unsigned int     uDataSize = 0;

    if ((pContextArg) && (pContextApp))
    {
        pContextApp->pOutputFileData = doConvertJob( pContextApp->pInputFileData, pContextApp->uInputFileSize, eCommand, (unsigned int )pContextArg->uTabColumns, &uDataSize);
        if (pContextApp->pOutputFileData)
        {
            pfullOutputFilename = getBasePathname( pContextArg->pFullFilename, (unsigned int )strlen( TEMP_FILE_NAME));
            pfullOutputFilename = strcat( pfullOutputFilename, TEMP_FILE_NAME);

            if (!writeFileFromMemory( pfullOutputFilename, pContextApp->pOutputFileData, uDataSize))
            {
                if (pContextArg->pFullFilename)
                {
                    (void )remove( (const char *)pContextArg->pFullFilename);
                    (void )rename( (const char *)pfullOutputFilename, (const char *)pContextArg->pFullFilename);
                }
            }
            else
            {
                exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"failed to write output file", NULL, pfullOutputFilename, 4);
            }

            free( pfullOutputFilename);
            free( pContextApp->pOutputFileData);
        }
        free( pContextApp->pInputFileData);
    }

    return 0;
}

/**
* @fn int doDump( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
* @brief Display the hearder of a BMP or PIC image file 
*
* @param[in]        pContextArg
* @param[in]        pContextApp
* @param[in]        eCommand
 *
 * @return 0 no error or exit program
*/
int doDump( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
{
    const char      *pEndString;

    if ((pContextArg) && (pContextApp))
    {
        if (CheckShrFileFormat( pContextApp->pInputFileData, pContextApp->uInputFileSize))
        {
            pContextApp->pOutputFileData = DoRleJob( pContextApp->pInputFileData, pContextApp->uInputFileSize, eCommand, NULL);
            if (pContextApp->pOutputFileData)
            {
                free( pContextApp->pOutputFileData);
            }
        }
        else
        {
            if (pContextArg->pFullFilename)
            {
                pEndString = (const char *)strrchr( pContextArg->pFullFilename, '.');
                if (strcmp( (const char *)pEndString, ".bmp") == 0)
                {
                    doDumpBmp( pContextArg->pFullFilename, pContextApp->pInputFileData, pContextApp->uInputFileSize);
                }
                else
                {
                    doDumpPic( pContextArg->pFullFilename, pContextApp->pInputFileData, pContextApp->uInputFileSize);
                }
                pEndString = NULL;
            }
            //printf("CONVM: nothing is done. Dump is only for .shr or .pnt\n");
        }
    }

    return 0;
}

/**
* @fn int doExt2Txt( tConvmArguments *pContextArg, tContextApp *pContextApp)
* @brief 
*
* @param[in]        pContextArg
* @param[in]        pContextApp
 *
 * @return 0 no error or exit program
*/
int doExt2Txt( tConvmArguments *pContextArg, tContextApp *pContextApp)
{
    unsigned int     uDataSize = 0;

    if ((pContextArg) && (pContextApp))
    {
        if (pContextArg->pFullFilename)
        {
            pContextApp->pOutputFileData = doExtractTextFromBinay( pContextApp->pInputFileData, pContextApp->uInputFileSize, pContextArg->uMinSentenseLen, (unsigned int *)&uDataSize);
            if ((pContextApp->pOutputFileData) && (uDataSize > 0))
            {
                pContextArg->pOutputPathname = (char *)calloc( 1, strlen( pContextArg->pFullFilename) + 5);
                if (pContextArg->pOutputPathname)
                {
                    (void )strncpy_s( pContextArg->pOutputPathname, strlen( pContextArg->pFullFilename) + 4, pContextArg->pFullFilename, strlen( (const char *)pContextArg->pFullFilename));
                    pContextArg->pOutputPathname = strcat(pContextArg->pOutputPathname, (const char*)".txt");
                    if (writeFileFromMemory( pContextArg->pOutputPathname, pContextApp->pOutputFileData, uDataSize))
                    {
                        exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"failed to write output file", NULL, pContextArg->pOutputPathname, 4);
                    }
                }
            }
            else
            {
                exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"file too big", NULL, pContextArg->pFullFilename, 5);
            }
            free( pContextApp->pOutputFileData);
        }

    }
    return 0;
}

/**
* @fn int doRleComDecom( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
* @brief
*
* @param[in]        pContextArg
* @param[in]        pContextApp
* @param[in]        eCommand
 *
 * @return 0 no error or exit program
*/
int doRleComDecom( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
{
    char            *pfullOutputFilename = NULL;
    unsigned int     uDataSize = 0;

    if ((pContextArg) && (pContextApp))
    {
        pContextApp->pOutputFileData = DoRleJob(pContextApp->pInputFileData, pContextApp->uInputFileSize, eCommand, &uDataSize);
        if ((pContextApp->pOutputFileData) && (pContextArg->pFullFilename) && (uDataSize > 0))
        {
            pfullOutputFilename = createOutputPathname( pContextArg->pFullFilename, pContextArg->pOutputPathname, eCommand);
            if (pfullOutputFilename)
            {
                if (!writeFileFromMemory( pfullOutputFilename, pContextApp->pOutputFileData, uDataSize))
                {
                    updateFileType( pContextArg, eCommand, pfullOutputFilename);
                }
                else
                {
                    exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"failed to write output file", NULL, pfullOutputFilename, 4);
                }
                free( pfullOutputFilename);
            }
            else
            {
                exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"file not compatible", NULL, pContextArg->pFullFilename, 5);
            }

            free( pContextApp->pOutputFileData);
        }
        else
        {
            exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"file not compatible", NULL, pContextArg->pFullFilename, 5);
        }
        free( pContextApp->pInputFileData);
    }

    return 0;
}

/**
* @fn int doToBmp( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
* @brief
*
* @param[in]        pContextArg
* @param[in]        pContextApp
* @param[in]        eCommand
 *
 * @return 0 no error or exit program
*/
int doToBmp( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
{
    char            *pfullOutputFilename = NULL;
    char            *pTempFileData = NULL;
    unsigned int     uDataSize = 0;

    if ((pContextArg) && (pContextApp))
    {
        if (CheckShrFileFormat( pContextApp->pInputFileData, pContextApp->uInputFileSize))
        {
            // decompress file before convert to BMP
            pTempFileData = DoRleJob( pContextApp->pInputFileData, pContextApp->uInputFileSize, eRLE_DECO, &uDataSize);
            if ((pTempFileData) && (uDataSize > 0))
            {
                pContextApp->pOutputFileData = DoBmpJob( pTempFileData, uDataSize, eCommand, &uDataSize, NULL);
                free( pTempFileData);
                pTempFileData = NULL;
            }
        }
        else
        {
            pContextApp->pOutputFileData = DoBmpJob( pContextApp->pInputFileData, pContextApp->uInputFileSize, eCommand, &uDataSize, NULL);
        }

        if ((pContextApp->pOutputFileData) && (pContextArg->pFullFilename) && (uDataSize > 0))
        {
            pfullOutputFilename = createOutputPathname( pContextArg->pFullFilename, pContextArg->pOutputPathname, eCommand);
            if (pfullOutputFilename)
            {
                if (writeFileFromMemory( pfullOutputFilename, pContextApp->pOutputFileData, uDataSize))
                {
                    exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"failed to write output file", NULL, pfullOutputFilename, 4);
                }
                free(pfullOutputFilename);
            }
            else
            {
                exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"file not compatible", NULL, pContextArg->pFullFilename, 5);
            }
            free( pContextApp->pOutputFileData);
        }
        else
        {
            exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"file not compatible", NULL, pContextArg->pFullFilename, 5);
        }
        free( pContextApp->pInputFileData);
    }

    return 0;
}

/**
* @fn int doToPic( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand )
* @brief
*
* @param[in]        pContextArg
* @param[in]        pContextApp
* @param[in]        eCommand
 *
 * @return 0 no error or exit program
*/
int doToPic( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand )
{
    char            *pfullOutputFilename = NULL;
    char            *pScbFileData = NULL;
    unsigned int     uDataSize = 0;

    if ((pContextArg) && (pContextApp))
    {
        if (CheckBmpFileFormat( pContextApp->pInputFileData, pContextApp->uInputFileSize))
        {
            if (pContextArg->pFullFilename != NULL)
            {
                pfullOutputFilename = (char *)calloc(1, strlen(pContextArg->pFullFilename) + 1);
                if (pfullOutputFilename)
                {
                    (void )strncpy_s( pfullOutputFilename, strlen(pContextArg->pFullFilename) + 1, pContextArg->pFullFilename, strlen( (const char*)pContextArg->pFullFilename));

                    pfullOutputFilename[ strlen( pfullOutputFilename) - 3] = 's';
                    pfullOutputFilename[ strlen( pfullOutputFilename) - 2] = 'c';
                    pfullOutputFilename[ strlen( pfullOutputFilename) - 1] = 'b';

                    if (pathFileExists( (const char *)pfullOutputFilename))
                    {
                        pScbFileData = readFileToMemory( pfullOutputFilename);
                    }

                    free( pfullOutputFilename);
                    pfullOutputFilename = NULL;
                }
            }

            pContextApp->pOutputFileData = DoBmpJob( pContextApp->pInputFileData, pContextApp->uInputFileSize, eCommand, &uDataSize, pScbFileData);
            if ((pContextApp->pOutputFileData) && (pContextArg->pFullFilename) && (uDataSize > 0))
            {
                pfullOutputFilename = createOutputPathname( pContextArg->pFullFilename, pContextArg->pOutputPathname, eCommand);
                if (pfullOutputFilename)
                {
                    if (!writeFileFromMemory( pfullOutputFilename, pContextApp->pOutputFileData, uDataSize))
                    {
                        updateFileType( pContextArg, eCommand, pfullOutputFilename);
                    }
                    else
                    {
                        exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"failed to write output file", NULL, pfullOutputFilename, 4);
                    }
                    free( pfullOutputFilename);
                }
                else
                {
                    exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"file not compatible", NULL, pContextArg->pFullFilename, 5);
                }
                free( pContextApp->pOutputFileData);
            }

            if (pScbFileData)
            {
                free( pScbFileData);
                pScbFileData = NULL;
            }
        }
        else
        {
            exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"file not compatible", NULL, pContextArg->pFullFilename, 5);
        }
    }

    return 0;
}

/**
* @fn int doAddPaletteToBmp4( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
* @brief
*
* @param[in]        pContextArg
* @param[in]        pContextApp
* @param[in]        eCommand
 *
 * @return 0 no error or exit program
*/
int doAddPaletteToBmp4( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
{
    char            *pfullOutputFilename = NULL;
    const char      *pEndString = NULL;
    unsigned int     uDataSize = 0;

    if ((pContextArg) && (pContextApp))
    {
        if (pContextArg->pFullFilename)
        {
            pEndString = (const char *)strrchr( (const char *)pContextArg->pFullFilename, '.');
            if (strcmp( (const char *)pEndString, ".bmp") == 0)
            {
                pContextApp->pOutputFileData = DoAddPaletteToBmp( pContextApp->pInputFileData, pContextApp->uInputFileSize, eCommand, &uDataSize);
                if ((pContextApp->pOutputFileData) && (pContextArg->pFullFilename) && (uDataSize > 0))
                {
                    pfullOutputFilename = createOutputPathname( pContextArg->pFullFilename, pContextArg->pOutputPathname, eCommand);
                    if (pfullOutputFilename)
                    {
                        (void)strncpy_s(pfullOutputFilename, strlen( pfullOutputFilename), pContextArg->pFullFilename, strlen( (const char *)pContextArg->pFullFilename) - 4);
                        pfullOutputFilename = strcat( pfullOutputFilename, (const char*)"2.bmp");

                        if (writeFileFromMemory( pfullOutputFilename, pContextApp->pOutputFileData, uDataSize))
                        {
                            exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"failed to write output file", NULL, pfullOutputFilename, 4);
                        }
                        free( pfullOutputFilename);
                    }
                    free( pContextApp->pOutputFileData);
                }
            }
            else
            {
                exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"file not compatible", NULL, pContextArg->pFullFilename, 5);
            }
        }
    }
    return 0;
}

/**
* @fn int doInsertPaletteToBmp8( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
* @brief
*
* @param[in]        pContextArg
* @param[in]        pContextApp
* @param[in]        eCommand
 *
 * @return 0 no error or exit program
*/
int doInsertPaletteToBmp8( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
{
    char            *pfullOutputFilename = NULL;

    if ((pContextArg) && (pContextApp))
    {
        if ((pContextArg->pFullFilename) && (pContextArg->pOutputPathname))
        {
            pContextApp->uOutputFileSize = getMyFileSize(pContextArg->pOutputPathname);

            pContextApp->pOutputFileData = readFileToMemory(pContextArg->pOutputPathname);   // second input file is in memory
            if (pContextApp->pOutputFileData)
            {
                if (CheckBmpFileFormat( pContextApp->pInputFileData, pContextApp->uInputFileSize) && CheckBmpFileFormat( pContextApp->pOutputFileData, pContextApp->uOutputFileSize))
                {
                    DoInsertPaletteToBmp( pContextApp->pInputFileData, pContextApp->uInputFileSize, &pContextApp->pOutputFileData, &pContextApp->uOutputFileSize, pContextArg->uSwapColumnA, pContextArg->uSwapColumnB);
                    if ((pContextApp->pOutputFileData) && (pContextArg->pFullFilename) && (pContextApp->uOutputFileSize > 0))
                    {
                        pfullOutputFilename = createOutputPathname( pContextArg->pFullFilename, pContextArg->pOutputPathname, eCommand);
                        if (pfullOutputFilename)
                        {
                            (void )strncpy_s( pfullOutputFilename, strlen(pfullOutputFilename), pContextArg->pFullFilename, strlen((const char*)pContextArg->pFullFilename) - 4);
                            pfullOutputFilename = strcat( pfullOutputFilename, (const char*)"2.bmp");

                            if (writeFileFromMemory( pfullOutputFilename, pContextApp->pOutputFileData, pContextApp->uOutputFileSize))
                            {
                                exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"failed to write output file", NULL, pfullOutputFilename, 4);
                            }
                            free( pfullOutputFilename);
                        }
                        free( pContextApp->pOutputFileData);
                    }
                }
            }
        }
    }

    return 0;
}

/**
* @fn int doSwapTwoColor( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
* @brief
*
* @param[in]        pContextArg
* @param[in]        pContextApp
* @param[in]        eCommand
 *
 * @return 0 no error or exit program
*/
int doSwapTwoColor( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
{
    char                *pfullOutputFilename = NULL;
    const char          *pEndString = NULL;
    unsigned int         uDataSize = 0;

    if ((pContextArg) && (pContextApp))
    {
        if (pContextArg->pFullFilename)
        {
            pEndString = (const char *)strrchr( (const char *)pContextArg->pFullFilename, '.');
            if (strcmp( (const char *)pEndString, ".bmp") == 0)
            {
                if (CheckBmpFileFormat( pContextApp->pInputFileData, pContextApp->uInputFileSize))
                {
                    pContextApp->pOutputFileData = DoSwapColor( pContextApp->pInputFileData, pContextApp->uInputFileSize, pContextArg->uSwapColumnA, pContextArg->uSwapColumnB, &uDataSize);
                    if ((pContextApp->pOutputFileData) && (pContextArg->pFullFilename) && (uDataSize > 0))
                    {
                        pfullOutputFilename = createOutputPathname( pContextArg->pFullFilename, pContextArg->pOutputPathname, eCommand);
                        if (pfullOutputFilename)
                        {
                            if (writeFileFromMemory( pfullOutputFilename, pContextApp->pOutputFileData, uDataSize))
                            {
                                exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"failed to write output file", NULL, pfullOutputFilename, 4);
                            }
                            free( pfullOutputFilename);
                        }
                        free( pContextApp->pOutputFileData);
                    }
                }
                else
                {
                    exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"file not compatible", NULL, pContextArg->pFullFilename, 5);
                }
            }
            else
            {
                exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"file not compatible", NULL, pContextArg->pFullFilename, 5);
            }
        }
    }

    return 0;
}

/**
* @fn int doComparePalette( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
* @brief
*
* @param[in]        pContextArg
* @param[in]        pContextApp
* @param[in]        eCommand
 *
 * @return 0 no error or exit program
*/
int doComparePalette( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
{
    FormatBMP           *pBmpIn16ColorsImageOne;
    FormatBMP           *pBmpIn16ColorsImageTwo;

    if ((pContextArg) && (pContextApp))
    {
        if ((pContextArg->pFullFilename) && (pContextArg->pOutputPathname))
        {
            pContextApp->uOutputFileSize = getMyFileSize( pContextArg->pOutputPathname);

            pContextApp->pOutputFileData = readFileToMemory( pContextArg->pOutputPathname);   // second input file is in memory
            if (pContextApp->pOutputFileData)
            {
                if (CheckBmpFileFormat( pContextApp->pInputFileData, pContextApp->uInputFileSize) && CheckBmpFileFormat( pContextApp->pOutputFileData, pContextApp->uOutputFileSize))
                {
                    pBmpIn16ColorsImageOne = (FormatBMP *)pContextApp->pInputFileData;
                    pBmpIn16ColorsImageTwo = (FormatBMP *)pContextApp->pOutputFileData;

                    if ((pBmpIn16ColorsImageOne->Nbr_Bit_Par_Pixel == 4) && (pBmpIn16ColorsImageTwo->Nbr_Bit_Par_Pixel == 4))
                    {
                        DoComparePalette( pContextApp->pInputFileData, pContextApp->uInputFileSize, pContextApp->pOutputFileData, pContextApp->uOutputFileSize);
                    }
                    else
                    {
                        exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"file not compatible", NULL, pContextArg->pFullFilename, 5);
                    }
                }
                else
                {
                    exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"file not compatible", NULL, pContextArg->pFullFilename, 5);
                }
                free( pContextApp->pOutputFileData);
            }
        }
    }

    return 0;
}

/**
* @fn int doCopyPalette( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
* @brief
*
* @param[in]        pContextArg
* @param[in]        pContextApp
* @param[in]        eCommand
 *
 * @return 0 no error or exit program
*/
int doCopyPalette( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
{
    if ((pContextArg) && (pContextApp))
    {
        if ((pContextArg->pFullFilename) && (pContextArg->pOutputPathname))
        {
            pContextApp->uOutputFileSize = getMyFileSize( pContextArg->pOutputPathname);

            pContextApp->pOutputFileData = readFileToMemory( pContextArg->pOutputPathname);   // second input file is in memory
            if (pContextApp->pOutputFileData)
            {
                if (CheckBmpFileFormat( pContextApp->pInputFileData, pContextApp->uInputFileSize) && CheckBmpFileFormat( pContextApp->pOutputFileData, pContextApp->uOutputFileSize))
                {
                    FormatBMP   *pBmpIn16ColorsImageOne = (FormatBMP *)pContextApp->pInputFileData;
                    FormatBMP   *pBmpIn16ColorsImageTwo = (FormatBMP *)pContextApp->pOutputFileData;

                    if ((pBmpIn16ColorsImageOne->Nbr_Bit_Par_Pixel == 4) && (pBmpIn16ColorsImageTwo->Nbr_Bit_Par_Pixel == 4))
                    {
                        DoCopyPalette( pContextApp->pInputFileData, pContextApp->uInputFileSize, pContextApp->pOutputFileData, pContextApp->uOutputFileSize, 4);
                        if (writeFileFromMemory( pContextArg->pOutputPathname, pContextApp->pOutputFileData, pContextApp->uOutputFileSize))
                        {
                            exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"failed to write output file", NULL, pContextArg->pOutputPathname, 4);
                        }
                    }
                    else
                    {
                        FormatBMP256 *pBmpIn256ColorsImageOne = (FormatBMP256 *)pContextApp->pInputFileData;
                        FormatBMP256 *pBmpIn256ColorsImageTwo = (FormatBMP256 *)pContextApp->pOutputFileData;

                        if ((pBmpIn16ColorsImageOne->Nbr_Bit_Par_Pixel == 8) && (pBmpIn16ColorsImageTwo->Nbr_Bit_Par_Pixel == 8))
                        {
                            DoCopyPalette( pContextApp->pInputFileData, pContextApp->uInputFileSize, pContextApp->pOutputFileData, pContextApp->uOutputFileSize, 8);
                            if (writeFileFromMemory( pContextArg->pOutputPathname, pContextApp->pOutputFileData, pContextApp->uOutputFileSize))
                            {
                                exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"failed to write output file", NULL, pContextArg->pOutputPathname, 4);
                            }
                        }
                        else
                        {
                            exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"file not compatible", NULL, pContextArg->pFullFilename, 5);
                        }
                    }
                }
                else
                {
                    exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"file not compatible", NULL, pContextArg->pFullFilename, 5);
                }
                free( pContextApp->pOutputFileData);
            }
        }
    }

    return 0;
}

/**
* @fn int doExtSprite( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
* @brief
*
* @param[in]        pContextArg
* @param[in]        pContextApp
* @param[in]        eCommand
 *
 * @return 0 no error or exit program
*/
int doExtSprite( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
{
    char            *pfullOutputFilename = NULL;
    const char      *pEndString = NULL;
    unsigned int     uDataSize = 0;

    if ((pContextArg) && (pContextApp))
    {
        if (pContextArg->pFullFilename)
        {
            pEndString = (const char *)strrchr( (const char *)pContextArg->pFullFilename, '.');
            if (strcmp( (const char *)pEndString, ".bmp") == 0)
            {
                if (CheckBmpFileFormat( pContextApp->pInputFileData, pContextApp->uInputFileSize))
                {
                    pContextApp->pOutputFileData = DoExtractSprite( pContextApp->pInputFileData, pContextApp->uInputFileSize, pContextArg, &uDataSize);
                    if ((pContextApp->pOutputFileData) && (pContextArg->pFullFilename) && (uDataSize > 0))
                    {
                        pfullOutputFilename = createOutputPathname( pContextArg->pFullFilename, pContextArg->pOutputPathname, eCommand);
                        if (pfullOutputFilename)
                        {
                            if (writeFileFromMemory( pfullOutputFilename, pContextApp->pOutputFileData, uDataSize))
                            {
                                exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"failed to write output file", NULL, pfullOutputFilename, 4);
                            }
                            free( pfullOutputFilename);
                        }
                        free( pContextApp->pOutputFileData);
                    }
                }
                else
                {
                    exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"file not compatible", NULL, pContextArg->pFullFilename, 5);
                }
            }
            else
            {
                exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"file not compatible", NULL, pContextArg->pFullFilename, 5);
            }
        }
    }

    return 0;
}
