/* SPDX - License - Identifier: GPL - 3.0 - or -later
 *
 * A tool to help cross dev for Apple II GS.
 *
 * Copyright(C) 2023 - 2025 Renaud Malaval <renaud.malaval@free.fr>.
 *
 * This program is free software : you can redistribute it and /or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 *  GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 *  along with this program.If not, see < https://www.gnu.org/licenses/>.
 */

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
                    (void )printf( "CONVM: \"%s\") failed error = %d\n", pTmpCommand, iError);
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
                // point to the last '.' of the file name to be able to mopdify it or just change extension
                pEndString = &pOutputPathname[strlen( pOutputPathname) - 3];
            }

            switch( eCommand)
            {
                case eRLE_COMP:
                {
                    if (pEndString)
                    {
                        *(char *)pEndString = 'p';
                        pEndString++;
                        *(char *)pEndString = 'n';
                        pEndString++;
                        *(char *)pEndString = 't';
                        //pOutputPathname[strlen( pOutputPathname) - 3] = 'p';
                        //pOutputPathname[strlen( pOutputPathname) - 2] = 'n';
                        //pOutputPathname[strlen( pOutputPathname) - 1] = 't';
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
                        *(char *)pEndString = 'p';
                        pEndString++;
                        *(char *)pEndString = 'i';
                        pEndString++;
                        *(char *)pEndString = 'c';
                        //pOutputPathname[strlen( pOutputPathname) - 3] = 'p';
                        //pOutputPathname[strlen( pOutputPathname) - 2] = 'i';
                        //pOutputPathname[strlen( pOutputPathname) - 1] = 'c';
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
                        *(char *)pEndString = 'b';
                        pEndString++;
                        *(char *)pEndString = 'm';
                        pEndString++;
                        *(char *)pEndString = 'p';
                        //pOutputPathname[strlen( pOutputPathname) - 3] = 'b';
                        //pOutputPathname[strlen( pOutputPathname) - 2] = 'm';
                        //pOutputPathname[strlen( pOutputPathname) - 1] = 'p';
                    }
                    else
                    {
                        pOutputPathname = strcat( pOutputPathname, ".bmp");
                    }
                }
                break;
                case eREDUCECOLORCHART:
                {
                    if (pEndString)
                    {
                        pEndString--;
                        *(char *)pEndString = '_';
                        pEndString++;
                        *(char *)pEndString = 'r';
                        pEndString++;
                        *(char *)pEndString = 'c';
                        pEndString++;
                        *(char *)pEndString = 'c';
                        pEndString++;
                        *(char *)pEndString = '.';
                        pEndString++;
                        *(char *)pEndString = 'b';
                        pEndString++;
                        *(char *)pEndString = 'm';
                        pEndString++;
                        *(char *)pEndString = 'p';
                        pEndString++;
                        *(char *)pEndString = '\0';
                    }
                    else
                    {
                        pOutputPathname = strcat( pOutputPathname, "_rcc.bmp");
                    }
                }
                break;
                case eREMOVEDUPLICATEINDEX:
                {
                    if (pEndString)
                    {
                        pEndString--;
                        *(char *)pEndString = '_';
                        pEndString++;
                        *(char *)pEndString = 'r';
                        pEndString++;
                        *(char *)pEndString = 'd';
                        pEndString++;
                        *(char *)pEndString = 'i';
                        pEndString++;
                        *(char *)pEndString = 'c';
                        pEndString++;
                        *(char *)pEndString = '.';
                        pEndString++;
                        *(char *)pEndString = 'b';
                        pEndString++;
                        *(char *)pEndString = 'm';
                        pEndString++;
                        *(char *)pEndString = 'p';
                        pEndString++;
                        *(char *)pEndString = '\0';
                    }
                    else
                    {
                        pOutputPathname = strcat( pOutputPathname, "_rdic.bmp");
                    }
                }
                break;
                case eSWAP2COLOR:
                {
                    if (pEndString)
                    {
                        pEndString--;
                        *(char*)pEndString = '\0';
                        //pOutputPathname[strlen( pOutputPathname) - 4] = '\0';
                    }
                    pOutputPathname = strcat( pOutputPathname, "-s.bmp");
                }
                break;
                case eEXTSPRITE:
                {
                    if (pEndString)
                    {
                        pEndString--;
                        *(char*)pEndString = '\0';
                        //pOutputPathname[strlen( pOutputPathname) - 4] = '\0';
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
            (void )printf( "CONVM: from = %s\n", pfullOutputFilename);
            ptempFilename = getBasePathname( pfullOutputFilename, 16);
            if (ptempFilename)
            {
                /* Change Windows pathname to Dos pathname (8.3) */
                iError = GetShortPathNameA( (LPCSTR)ptempFilename, (LPSTR )pShortPathname, (DWORD )strlen(ptempFilename) + 1);
                if (!iError)
                {
                    (void )printf( "CONVM: GetShortPathName() failed error = %d\n", (int )GetLastError());
                    free( pShortPathname);
                    pShortPathname = strdup( pfullOutputFilename);
                }
                else
                {
                    if (!strlen( pShortPathname))
                    {
                        free( pShortPathname);
                        pShortPathname = strdup( pfullOutputFilename);
                    }
                    else
                    {
                        pShortPathname = strcat( pShortPathname, getFileName(pfullOutputFilename));
                    }
                }
                (void )printf( "CONVM: to   = %s\n", pShortPathname);
                free( ptempFilename);
                ptempFilename = NULL;
            }
            else
            {
                free(pShortPathname);
                pShortPathname = strdup( pfullOutputFilename);
            }

            /* little cyber security protection */
            if ((*(unsigned long*)pCheckEmulatorPresent != (unsigned long)1919248503) ||
                ((*(unsigned long*)pCmdEmulatorPnt != (unsigned long)544762217) ||
                    (*(unsigned long*)pCmdEmulatorPic != ((unsigned long)544762217))))
            {
                free( pDuplicateString);
                pDuplicateString = NULL;
                free( pShortPathname);
                pShortPathname = NULL;
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
                            (void )printf( "CONVM: Failed to change filetype of '%s' with iix, error = %d\n", pShortPathname, errno);
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
                            (void )printf( "CONVM: Failed to change filetype of '%s' with iix, error = %d\n", pShortPathname, errno);
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
            }
        }
        if (pDuplicateString)
        {
            free(pDuplicateString);
            pDuplicateString = NULL;
        }

        if (pShortPathname)
        {
            free(pShortPathname);
            pShortPathname = NULL;
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
            //(void )printf("CONVM: nothing is done. Dump is only for .shr or .pnt\n");
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
* @param[in]        eCommand        : used to call createOutputPathname() to create the output name
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
* @param[in]        eCommand        : used to call createOutputPathname() to create the output name
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
* @param[in]        eCommand        : used to call createOutputPathname() to create the output name
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
                pfullOutputFilename = (char *)calloc( 1, strlen(pContextArg->pFullFilename) + 1);
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
* @fn int doPIC_NumberOfColor_NotUsePerLine( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
* @brief Display the line with color index not used in PIC
*
* @param[in]        pContextArg
* @param[in]        pContextApp
* @param[in]        eCommand        : used to call createOutputPathname() to create the output name
 *
 * @return 0 no error or exit program
*/
int doPIC_NumberOfColor_NotUsePerLine( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
{
    char           *pInputRunner;
    FormatPIC      *pPicImage;
    unsigned int    uIndex;
    unsigned int    uLoop;
    COORD           tCoord;
    BOOL            bColorNotUsed;
    unsigned int    uTableNumberOfcolorIndex[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    unsigned int    uCounter = 0;
    unsigned int    uLessUsed = 0;

    if ((pContextArg) && (pContextApp) && (pContextApp->pInputFileData) && (pContextApp->uInputFileSize))
    {
        (void )printf( "\nDisplay line number with color(s) index not used :\n\n");
        (void )printf( "Cursor colors        :                       C  C        C  C\n");
        pPicImage = (FormatPIC *)pContextApp->pInputFileData;
        pInputRunner = pContextApp->pInputFileData;

        for (uIndex = 0; uIndex < 200; uIndex++)
        {
            //if (uIndex == 51)
            //{
            //    uIndex = uIndex;
            //}

            for (uLoop = 0; uLoop < 160; uLoop++)
            {
                uTableNumberOfcolorIndex[ (*pInputRunner & 0xF0) >> 4] += 1;
                uTableNumberOfcolorIndex[ *pInputRunner & 0x0F] += 1;
                pInputRunner++;
            }

            bColorNotUsed = FALSE;
            for (uLoop = 0; uLoop < 16; uLoop++)
            {
                if (uTableNumberOfcolorIndex[uLoop] == 0)
                {
                    bColorNotUsed = TRUE;
                    break;
                }
            }

            uLessUsed = uTableNumberOfcolorIndex[0];
            for (uLoop = 1; uLoop < 16; uLoop++)
            {
                if (uTableNumberOfcolorIndex[uLoop] < uLessUsed)
                {
                    uLessUsed = uTableNumberOfcolorIndex[uLoop];
                }
            }

            (void )printf( "line #%03u : not used :", uIndex);
            for (uLoop = 0; uLoop < 16; uLoop++)
            {
                if (uTableNumberOfcolorIndex[uLoop] == 0)
                {
                    (void )printf( " %02u", uLoop);
                    uCounter++;
                }
                else
                {
                    (void )printf( "   ");
                }
            }

            whereCursorXY( &tCoord);        // get cursor position in windows console
            moveCursorXY( 74, tCoord.Y);    // shift position X of the cursor in windows console

            (void )printf( " free color= %02u / 16  uLessUsed= %03u  SCB= %d  ", uCounter, uLessUsed, pPicImage->SCB[uIndex]);
            if (bColorNotUsed == FALSE)
            {
                for (uLoop = 0; uLoop < 16; uLoop++)
                {
                    (void )printf( " %03u", uTableNumberOfcolorIndex[uLoop]);
                }
            }
            (void )printf( "\n");

            (void )memset( uTableNumberOfcolorIndex, 0, sizeof( uTableNumberOfcolorIndex));
            uCounter = 0;
        }
    }
    return 0;
}


/**
* @fn int doBmp_ReduceColorChart( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
* @brief Reduce th color chart from 0..256 of R G B (bmp) to 0..15 of RGB (pic)
*
* @param[in]        pContextArg
* @param[in]        pContextApp
* @param[in]        eCommand        : used to call createOutputPathname() to create the output name
 *
 * @return 0 no error or exit program
*/
int doBmp_ReduceColorChart( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
{
    char               *pfullOutputFilename;
    BMPHeader          *pHeaderBmp;
    FormatBMP256       *pBmpImage;
    unsigned int        uIndex;

    if ( (pContextArg) && (pContextApp) && (pContextApp->pInputFileData) && (pContextApp->uInputFileSize) )
    {
        pHeaderBmp = (BMPHeader *)pContextApp->pInputFileData;

        if (pHeaderBmp->Nbr_Bit_Par_Pixel == 8)
        {
            pBmpImage = (FormatBMP256 *)pContextApp->pInputFileData;

            // convert palett of BMP keep high 8 bit only low bits a set to 0
            for (uIndex = 0; uIndex < pBmpImage->Nbr_Couleur_Image; uIndex++)
            {
                pBmpImage->Couleur_Palettes[uIndex] = pBmpImage->Couleur_Palettes[uIndex] & 0x00F0F0F0;
            }

            pfullOutputFilename = createOutputPathname( pContextArg->pFullFilename, pContextArg->pOutputPathname, eCommand);
            if (pfullOutputFilename)
            {
                if (writeFileFromMemory( pfullOutputFilename, (char *)pBmpImage, pContextApp->uInputFileSize))
                {
                    exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"failed to write output file", NULL, pfullOutputFilename, 4);
                }

                free( pfullOutputFilename);
                pfullOutputFilename = NULL;
            }
        }
        else
        {
            exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"Support only 8 bits per pxiels", NULL, NULL, 1964);
        }
    }

    return 0;
}

static void showColorPalett( FormatBMP256 *pBmpImage)
{
    unsigned int        uIndex;
    unsigned int        uCounter;
    unsigned int        uMoreUsed;

    (void )printf( "\nShow color palett :\n\n");
    uCounter = 0;
    uMoreUsed = 0;
    for (uIndex = 0; uIndex < pBmpImage->Nbr_Couleur_Image; uIndex++)
    {
        if (uMoreUsed == 0)
        {
            (void )printf( "#%03u: $%06lx", uCounter, pBmpImage->Couleur_Palettes[uIndex]);
            uCounter++;
        }
        else
        {
            (void )printf( " $%06lx", pBmpImage->Couleur_Palettes[uIndex]);
        }
        uMoreUsed++;
        if (uMoreUsed >= 16)
        {
            (void )printf( "\n");
            uMoreUsed = 0;
        }
    }

    (void )printf( "\n");
}

/**
* @fn int doBmp_RemoveDuplicateIndexToSameColor( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
* @brief -rdic : Remove duplicate index to the same color to reduce palett size
*
* @param[in]        pContextArg
* @param[in]        pContextApp
* @param[in]        eCommand        : used to call createOutputPathname() to create the output name
 *
 * @return 0 no error or exit program
*/
int doBmp_RemoveDuplicateIndexToSameColor( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
{
    char               *pfullOutputFilename;
    char               *pInputRunner;
    BMPHeader          *pHeaderBmp;
    FormatBMP256       *pBmpImage;
    unsigned int        uIndex;
    unsigned int        uLoop;
    unsigned int        uCounter;
    unsigned int        uMoreUsed;

    if ( (pContextArg) && (pContextApp) && (pContextApp->pInputFileData) && (pContextApp->uInputFileSize) )
    {
        pHeaderBmp = (BMPHeader *)pContextApp->pInputFileData;

        if (pHeaderBmp->Nbr_Bit_Par_Pixel == 8)
        {
            pBmpImage = (FormatBMP256 *)pContextApp->pInputFileData;

            for (uCounter = 0; uCounter < pBmpImage->Nbr_Couleur_Image; uCounter++)
            {
                uMoreUsed = uCounter;

                for (uIndex = uCounter + 1; uIndex < pBmpImage->Nbr_Couleur_Image; uIndex++)
                {
                    // Check if color are equal at index uCounter and uIndex
                    if (pBmpImage->Couleur_Palettes[uCounter] == pBmpImage->Couleur_Palettes[uIndex])
                    {
                        pInputRunner = (char *)pBmpImage->bitmap;
                        for (uLoop = 0; uLoop < sizeof( pBmpImage->bitmap); uLoop++)
                        {
                            if (*pInputRunner == uIndex)
                            {
                                *pInputRunner = uCounter;
                            }
                            pInputRunner++;
                        }
                        pBmpImage->Couleur_Palettes[uIndex] = 0x00000000;
                        uMoreUsed++;
                    }
                }
                if (uMoreUsed > uCounter)
                {
                    (void )printf( "\nFor color index= %d  duplicate value= %d\n", uCounter, uMoreUsed);
                    showColorPalett( pBmpImage);
                }
                if (uMoreUsed >= pBmpImage->Nbr_Couleur_Image)
                {
                    break;
                }
            }

            pfullOutputFilename = createOutputPathname( pContextArg->pFullFilename, pContextArg->pOutputPathname, eCommand);
            if (pfullOutputFilename)
            {
                if (writeFileFromMemory( pfullOutputFilename, (char *)pBmpImage, pContextApp->uInputFileSize))
                {
                    exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"failed to write output file", NULL, pfullOutputFilename, 4);
                }

                free( pfullOutputFilename);
                pfullOutputFilename = NULL;
            }
        }
        else
        {
            exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"Support only 8 bits per pxiels", NULL, NULL, 1964);
        }
    }

    return 0;
}


/**
* @fn static double rgbToGrayScale( unsigned char uRed, unsigned char uGreen, unsigned char uBlue)
* @brief Convert RGB color in gray
*
* @param[in]        uRed
* @param[in]        uGreen
* @param[in]        uBlue
* 
* @return a gray value
*/
static double rgbToGrayScale( unsigned char uRed, unsigned char uGreen, unsigned char uBlue)
{
    return (0.2126 * uRed) + (0.7152 * uGreen) + (0.0722 * uBlue);
}

/**
* @fn static BOOL isTheDarkColor( unsigned long int uARGBcolorOne, unsigned long int uARGBcolorTwo)
* @brief Compare 2 RGB color to found the darker
*
* @param[in]        uARGBcolorOne
* @param[in]        uARGBcolorTwo
*
* @return return TRUE if uARGBcolorOne is the daker color; FALSE if fGrayScaleTow is the darker color
*/
static BOOL isTheDarkColor( unsigned long int uARGBcolorOne, unsigned long int uARGBcolorTwo)
{
    unsigned char   uRed   = (unsigned char)((uARGBcolorOne & 0x00FF0000) >> 16);
    unsigned char   uGreen = (unsigned char)((uARGBcolorOne & 0x0000FF00) >> 8);
    unsigned char   uBlue  = (unsigned char)(uARGBcolorOne & 0x000000FF);
    double          fGrayScaleOne = rgbToGrayScale( uRed, uGreen, uBlue);
    double          fGrayScaleTow;

    uRed = (unsigned char)((uARGBcolorTwo & 0x00FF0000) >> 16);
    uGreen = (unsigned char)((uARGBcolorTwo & 0x0000FF00) >> 8);
    uBlue = (unsigned char)(uARGBcolorTwo & 0x000000FF);

    fGrayScaleTow = rgbToGrayScale( uRed, uGreen, uBlue);
    
    return fGrayScaleOne < fGrayScaleTow;
}

/**
* @fn int doBMP_NumberOfColor_NotUsePerLine( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
* @brief Display the line with color index not used in BMP
*        Ugly code, just a théorical test to have a PIC with 16 paletts in a BMP with more than 16 colors... 
*
* @param[in]        pContextArg
* @param[in]        pContextApp
* @param[in]        eCommand
 *
 * @return 0 no error or exit program
*/
int doBMP_NumberOfColor_NotUsePerLine( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
{
    char               *pInputRunner;
    BMPHeader          *pHeaderBmp;
    FormatBMP256       *pBmpImage;
    unsigned short      uTemp;
    int                 iVarPicX;
    unsigned int        uVarPicY;
    unsigned int        uIndex;
    unsigned int        uLoop;
    BOOL                bFound;
    BOOL                bSwapWasDone;
    COORD               tCoord;
    unsigned int        uCounter;
    unsigned int        uMoreUsed;
    unsigned short      uTableNumberOfcolor[200] = { 0 };
    unsigned int        uTableNumberOfcolorByline[256] = { 0 };
    unsigned long int   uARGBcolorCurrent;
    unsigned long int   uARGBcolorMini = 0;
    unsigned long int   uCouleur_Palett_tmp[256] = { 0 };
    unsigned long int   ulColorToRemove = 0;
    unsigned char       uDarkColorIndex = 0;
    unsigned short      uTableSCB[200] = { 0 };
    unsigned int        uFirstDiff;

    //unsigned short      uTableNumberOfcolorIndex[200][64] = { 0xFFFF };
    unsigned short( *uTableNumberOfcolorIndex)[200][64] = calloc( 1, sizeof( * uTableNumberOfcolorIndex));

    if ( (pContextArg) && (pContextApp) && (pContextApp->pInputFileData) && (pContextApp->uInputFileSize) )
    {
        (void )printf( "\nDisplay 64 1st colors used by line\n\n");

        pHeaderBmp = (BMPHeader *)pContextApp->pInputFileData;

        if (pHeaderBmp->Nbr_Bit_Par_Pixel == 8)
        {
            pBmpImage = (FormatBMP256 *)pContextApp->pInputFileData;

            // convert palett of BMP keep high 8 bit only low bits a set to 0
            for (uIndex = 0; uIndex < pBmpImage->Nbr_Couleur_Image; uIndex++)
            {
                uCouleur_Palett_tmp[uIndex] = pBmpImage->Couleur_Palettes[uIndex] & 0x00F0F0F0;
            }

//            doBmp_ReduceColorChart( pContextArg, pContextApp, eCommand);

            // build table of color index by line
            uCounter = 0;
            uMoreUsed = 0;
            for (iVarPicX = (int )pBmpImage->Longueur_Image - 1; iVarPicX >= 0; iVarPicX--)
            {
                (void )memset( &(* uTableNumberOfcolorIndex)[iVarPicX][0], 0xFFFF, 64);

                for (uVarPicY = 0; uVarPicY < (pBmpImage->Longueur_Image - 1); uVarPicY++)
                {
                    if (uCounter == 0)
                    {
                        ( * uTableNumberOfcolorIndex )[iVarPicX][uCounter] = pBmpImage->bitmap[iVarPicX][uVarPicY];
                        uCounter++;
                    }
                    else if (uCounter < 64)
                    {
                        bFound = FALSE;
                        for (uIndex = 0; uIndex < uCounter; uIndex++)   // Search if color index is already in table
                        {
                            if ( ( * uTableNumberOfcolorIndex)[iVarPicX][uIndex] == pBmpImage->bitmap[iVarPicX][uVarPicY])
                            {
                                bFound = TRUE;
                            }
                        }

                        if ( (bFound == FALSE) && (( * uTableNumberOfcolorIndex)[iVarPicX][uIndex] != pBmpImage->bitmap[iVarPicX][uVarPicY]) )
                        {
                            ( * uTableNumberOfcolorIndex)[iVarPicX][uIndex] = pBmpImage->bitmap[iVarPicX][uVarPicY];
                            uCounter++;
                        }
                    }
                    else
                    {
                        uMoreUsed++;
                    }
                }
                uCounter = 0;
                uMoreUsed = 0;
            }

            // Display original table
            (void )printf( "\nDisplay original table :\n");
            uCounter = 0;
            for (iVarPicX = (int )pBmpImage->Longueur_Image - 1; iVarPicX >= 0; iVarPicX--)
            {
                for (uLoop = 0; uLoop < 64; uLoop++)
                {
                    if (( * uTableNumberOfcolorIndex )[iVarPicX][uLoop] == 0xFFFF)
                    {
                        break;
                    }

                    if (uLoop == 0)
                    {
                        (void )printf( "#%03u: %03u", uCounter, ( * uTableNumberOfcolorIndex )[iVarPicX][uLoop]);
                    }
                    else
                    {
                        (void )printf( " %03u", ( * uTableNumberOfcolorIndex )[iVarPicX][uLoop]);
                    }
                }
                whereCursorXY( &tCoord);        // get cursor position in windows console
                moveCursorXY( 65, tCoord.Y);    // shift position X of the cursor in windows console
                (void )printf( "\n");
                uCounter++;
            }

            (void )printf( "\nShow duplicate color :\n");
            for (uCounter = 0; uCounter < pBmpImage->Nbr_Couleur_Image; uCounter++)
            {
                uMoreUsed = uCounter;

                //uCouleur_Palett_count[uCounter] = uCouleur_Palett_tmp[uCounter];
                for (uIndex = uCounter; uIndex < pBmpImage->Nbr_Couleur_Image; uIndex++)
                {
                    if (pBmpImage->Couleur_Palettes[uCounter] == pBmpImage->Couleur_Palettes[uIndex])
                    {
                        if (uMoreUsed == uCounter)
                        {
                            (void )printf( "#%03u: %03u", uCounter, uIndex);
                        }
                        else
                        {
                            (void )printf( " %03u", uIndex);
                        }

                        pInputRunner = (char *)&pBmpImage->bitmap[0][0];
                        for (uLoop = 0; uLoop < sizeof( pBmpImage->bitmap); uLoop++)
                        {
                            if (*pInputRunner == uIndex)
                            {
                                *pInputRunner = uCounter;
                            }
                            pInputRunner++;
                        }

                        uMoreUsed++;
                    }
                }
                whereCursorXY( &tCoord);        // get cursor position in windows console
                moveCursorXY( 65, tCoord.Y);    // shift position X of the cursor in windows console
                (void )printf( "\n");

                if (uMoreUsed >= 256)
                    break;
            }

            (void )printf( "\nShow poor color used in palett :\n\n");
            pInputRunner = (char*)&pBmpImage->bitmap[0][0];
            for (uLoop = 0; uLoop < sizeof( pBmpImage->bitmap); uLoop++)
            {
                uTableNumberOfcolorByline[*pInputRunner] += 1;
                pInputRunner++;
            }

            for (uLoop = 0; uLoop < 16; uLoop++)
            {
                if (uLoop == 0)
                {
                    (void )printf( "      %02u", uLoop);
                }
                else
                {
                    (void )printf( " %02u", uLoop);
                }
            }
            (void )printf( "\n\n");

            uCounter = 0;
            uMoreUsed = 0;
            uARGBcolorCurrent = 0;
            for (uLoop = 0; uLoop < 256; uLoop++)
            {
                if (uMoreUsed == 0)
                {
                    if (uTableNumberOfcolorByline[uLoop] < 9)
                    {
                        (void )printf( "#%03u: %02u", uCounter, uTableNumberOfcolorByline[uLoop]);
                    }
                    else
                    {
                        (void )printf( "#%03u:   ", uCounter);
                    }
                    uCounter++;
                }
                else
                {
                    if (uTableNumberOfcolorByline[uLoop] < 9)
                    {
                        (void )printf( " %02u", uTableNumberOfcolorByline[uLoop]);
                    }
                    else
                    {
                        (void )printf( "   ");
                    }
                }
                if (uTableNumberOfcolorByline[uLoop] > uARGBcolorCurrent)
                {
                    uARGBcolorCurrent = uTableNumberOfcolorByline[uLoop];
                }

                uMoreUsed++;
                if (uMoreUsed >= 16)
                {
                    (void )printf( "\n");
                    uMoreUsed = 0;
                }
            }
            (void )printf( "\nThe color the most used is %lu times\n", uARGBcolorCurrent);

            (void )printf( "\nShow all color used in palett :\n\n");
            uCounter = 0;
            uMoreUsed = 0;
            for (uLoop = 0; uLoop < 256; uLoop++)
            {
                if (uMoreUsed == 0)
                {
                    (void )printf( "#%03u: %05u", uCounter, uTableNumberOfcolorByline[uLoop]);
                    uCounter++;
                }
                else
                {
                    (void )printf( " %04u", uTableNumberOfcolorByline[uLoop]);
                }

                uMoreUsed++;
                if (uMoreUsed >= 16)
                {
                    (void )printf( "\n");
                    uMoreUsed = 0;
                }
            }

            (void )printf( "\nShow color palett :\n\n");
            uCounter = 0;
            uMoreUsed = 0;
            for (uIndex = 0; uIndex < pBmpImage->Nbr_Couleur_Image; uIndex++)
            {
                if (uMoreUsed == 0)
                {
                    (void )printf( "#%03u: $%06lx", uCounter, pBmpImage->Couleur_Palettes[uIndex]);
                    uCounter++;
                }
                else
                {
                    (void )printf( " $%06lx", pBmpImage->Couleur_Palettes[uIndex]);
                }
                uMoreUsed++;
                if (uMoreUsed >= 16)
                {
                    (void )printf( "\n");
                    uMoreUsed = 0;
                }
            }

            (void )printf( "\n");
            free( uTableNumberOfcolorIndex);
            return 0;

//            pInputRunner = pContextApp->pInputFileData;

            uDarkColorIndex = 0;



            // detect colors present in the palett multiple times to remove it.
            for (uIndex = 0; uIndex < pBmpImage->Nbr_Couleur_Image - 1; uIndex++)
            {
                uMoreUsed = 0;
                uCounter = 0;
                bFound = FALSE;
                ulColorToRemove = uCouleur_Palett_tmp[uIndex];
                for (uLoop = uIndex + 1; uLoop < pBmpImage->Nbr_Couleur_Image; uLoop++)
                {
                    if ( (bFound == FALSE) && (ulColorToRemove == uCouleur_Palett_tmp[uLoop]) )
                    {
                        uCounter++;
                        bFound = TRUE;
                        uMoreUsed = uIndex;
                    }
                    else if ( (bFound == TRUE) && (ulColorToRemove == uCouleur_Palett_tmp[uLoop]) )
                    {
                        uCounter++;
                    }
                }

                if ((uMoreUsed + uCounter) == (pBmpImage->Nbr_Couleur_Image - 1))
                {
                    (void )printf( "Color ulColorToRemove %08lu @ %d exist %d times\n", ulColorToRemove, uMoreUsed, uCounter);
                    break;
                }
                else if (uCounter != 0)
                {
                    (void )printf( "Color ulColorToRemove %08lu @ %d exist %d times. TODO : remove it\n", ulColorToRemove, uMoreUsed, uCounter);
                    // in this case we can remove multiple usage
                }
            }
            pBmpImage->Nbr_Couleur_Importante = uMoreUsed;
            (void )printf( "The important color are %d\n", uMoreUsed);






            // Find the dark color index
            uARGBcolorMini = pBmpImage->Couleur_Palettes[0];
            for (uIndex = 1; uIndex < pBmpImage->Nbr_Couleur_Image; uIndex++)
            {
                // 4 octets A = 00 R = xx G = xx B = xx
                uARGBcolorCurrent = pBmpImage->Couleur_Palettes[uIndex];

                if ( !isTheDarkColor( uARGBcolorMini, uARGBcolorCurrent))
                {
                    uDarkColorIndex = uIndex;
                    uARGBcolorMini = uARGBcolorCurrent;
                }
            }
            (void )printf( "The index of the darker color is %u\n", (unsigned int )uDarkColorIndex);

            // build table of color index by line
            uCounter = 0;
            uMoreUsed = 0;
            for (iVarPicX = (int)pBmpImage->Longueur_Image - 1; iVarPicX >= 0; iVarPicX--)
            {
                (void )memset(&(*uTableNumberOfcolorIndex)[iVarPicX][0], 0xFFFF, 64);

                for (uVarPicY = 0; uVarPicY < (pBmpImage->Longueur_Image - 1); uVarPicY++)
                {
                    if (uCounter == 0)
                    {
                        ( * uTableNumberOfcolorIndex )[iVarPicX][uCounter] = pBmpImage->bitmap[iVarPicX][uVarPicY];
                        uCounter++;
                    }
                    else if (uCounter < 16)
                    {
                        bFound = FALSE;
                        for (uIndex = 0; uIndex < uCounter; uIndex++)   // Search if color index is already in table
                        {
                            if ( ( * uTableNumberOfcolorIndex )[iVarPicX][uIndex] == pBmpImage->bitmap[iVarPicX][uVarPicY] )
                            {
                                bFound = TRUE;
                            }
                        }

                        if ( (bFound == FALSE) && (( * uTableNumberOfcolorIndex )[iVarPicX][uIndex] != pBmpImage->bitmap[iVarPicX][uVarPicY]) )
                        {
                            ( * uTableNumberOfcolorIndex )[iVarPicX][uIndex] = pBmpImage->bitmap[iVarPicX][uVarPicY];
                            uCounter++;
                        }
                    }
                    else
                    {
                        uMoreUsed++;
                    }
                }
                uCounter = 0;
                uMoreUsed = 0;
            }

            // Display original table
            (void )printf( "\nDisplay original table :\n");
            for (iVarPicX = (int )pBmpImage->Longueur_Image - 1; iVarPicX >= 0; iVarPicX--)
            {
                for (uLoop = 0; uLoop < 16; uLoop++)
                {
                    if (( * uTableNumberOfcolorIndex )[iVarPicX][uLoop] == 0xFFFF)
                    {
                        break;
                    }

                    if (uLoop == 0)
                    {
                        (void )printf( "#%03u: %03u", iVarPicX, ( * uTableNumberOfcolorIndex )[iVarPicX][uLoop]);
                    }
                    else
                    {
                        (void )printf( " %03u", ( * uTableNumberOfcolorIndex )[iVarPicX][uLoop]);
                    }
                }
                whereCursorXY( &tCoord);        // get cursor position in windows console
                moveCursorXY( 65, tCoord.Y);    // shift position X of the cursor in windows console
                (void )printf( "\n");
            }

            // Add dark color in all line (this could be the outline color of the cursor)
            for ( iVarPicX = (int)pBmpImage->Longueur_Image - 1; iVarPicX >= 0; iVarPicX--)
            {
                bSwapWasDone = FALSE;
                for ( uLoop = 0; uLoop < 15; uLoop++)
                {
                    if ( ( * uTableNumberOfcolorIndex )[iVarPicX][uLoop] == 0xFFFF)
                    {
                        break;
                    }
                    if ( ( * uTableNumberOfcolorIndex )[iVarPicX][uLoop] == uDarkColorIndex)
                    {
                        bSwapWasDone = TRUE;
                        break;
                    }
                }

                if ( ( !bSwapWasDone) && (uLoop < 15) )
                {
                    ( * uTableNumberOfcolorIndex )[iVarPicX][uLoop] = uDarkColorIndex;
                }
            }

            // Order the color index
            for (iVarPicX = (int )pBmpImage->Longueur_Image - 1; iVarPicX >= 0; iVarPicX--)
            {
                bSwapWasDone = FALSE;
                while (!bSwapWasDone)
                {
                    for (uLoop = 0; uLoop < 15; uLoop++)
                    {
                        if (( * uTableNumberOfcolorIndex )[iVarPicX][uLoop] == 0xFFFF)
                        {
                            uTableNumberOfcolor[iVarPicX] = uLoop;
                            bSwapWasDone = TRUE;
                            break;
                        }

                        if (( * uTableNumberOfcolorIndex )[iVarPicX][uLoop] > ( * uTableNumberOfcolorIndex )[iVarPicX][uLoop + 1])
                        {
                            uTemp = ( * uTableNumberOfcolorIndex )[iVarPicX][uLoop];
                            ( * uTableNumberOfcolorIndex )[iVarPicX][uLoop] = ( * uTableNumberOfcolorIndex )[iVarPicX][uLoop + 1];
                            ( * uTableNumberOfcolorIndex )[iVarPicX][uLoop + 1] = uTemp;
                            break;
                        }
                    }
                    if (uLoop == 15)
                    {
                        break;
                    }
                }
            }

            // Add missing color to have 12 color by line
            // (void )memset( uCouleur_Palett_tmp, 0, sizeof( uCouleur_Palett_tmp));
            (void )printf( "\n");
            iVarPicX = (int)pBmpImage->Longueur_Image - 1;
            while (iVarPicX != 0)
            {
                if ( (iVarPicX == 144) || (iVarPicX == 117) )
                {
                    iVarPicX = iVarPicX;
                }
                uTemp = iVarPicX - 1;
                if ((uTableNumberOfcolor[iVarPicX] >= 12) && (uTableNumberOfcolor[uTemp] >= 12))
                {
                    // Does nothing
                    uTemp = uTemp;
                }
                else if ( (uTableNumberOfcolor[iVarPicX] < 12) && (uTableNumberOfcolor[uTemp] >= 12) )
                {
                    // Here add color from after line
                    for (uIndex = 0; uIndex < 12; uIndex++)
                    {
                        if (( * uTableNumberOfcolorIndex )[iVarPicX][uIndex] != ( * uTableNumberOfcolorIndex )[uTemp][uIndex])
                        {
                            uTableNumberOfcolor[iVarPicX]++;
                            for (uLoop = uTableNumberOfcolor[iVarPicX]; uLoop > uIndex; uLoop--)
                            {
                                ( * uTableNumberOfcolorIndex )[iVarPicX][uLoop] = ( * uTableNumberOfcolorIndex )[iVarPicX][uLoop - 1];
                            }
                            ( * uTableNumberOfcolorIndex )[iVarPicX][uIndex] = ( * uTableNumberOfcolorIndex )[uTemp][uIndex];
                        }
                    }
                }
                else if ((uTableNumberOfcolor[iVarPicX] >= 12) && (uTableNumberOfcolor[uTemp] < 12))
                {
                    for (uIndex = 0; uIndex < 12; uIndex++)
                    {
                        if (( * uTableNumberOfcolorIndex )[iVarPicX][uIndex] != ( * uTableNumberOfcolorIndex )[uTemp][uIndex])
                        {
                            // check if color already exist
                            bSwapWasDone = FALSE;
                            for (uLoop = uIndex + 1; uLoop < 15; uLoop++)
                            {
                                if (( * uTableNumberOfcolorIndex )[uTemp][uLoop] == ( * uTableNumberOfcolorIndex )[iVarPicX][uIndex])
                                {
                                    bSwapWasDone = TRUE;
                                    break;
                                }
                            }

                            if (uTableNumberOfcolor[uTemp] >= 12)
                            {
                                break;
                            }

                            if (bSwapWasDone == FALSE)
                            {
                                if ((( * uTableNumberOfcolorIndex )[iVarPicX][uIndex] == 0xFFFF) && (( * uTableNumberOfcolorIndex )[uTemp][uIndex] != 0xFFFF))
                                {
                                    // Here add color from current line
                                    uIndex = uIndex;
                                }
                                else if ((( * uTableNumberOfcolorIndex )[iVarPicX][uIndex] != 0xFFFF) && (( * uTableNumberOfcolorIndex )[uTemp][uIndex] == 0xFFFF))
                                {
                                    // Add last color
                                    ( * uTableNumberOfcolorIndex )[uTemp][uIndex] = ( * uTableNumberOfcolorIndex )[iVarPicX][uIndex];
                                    uTableNumberOfcolor[uTemp]++;
                                }
                                else
                                {
                                    // Insert missing color
                                    uTableNumberOfcolor[uTemp]++;
                                    for (uLoop = uTableNumberOfcolor[uTemp]; uLoop > uIndex; uLoop--)
                                    {
                                        ( * uTableNumberOfcolorIndex )[uTemp][uLoop] = ( * uTableNumberOfcolorIndex )[uTemp][uLoop - 1];
                                    }
                                    ( * uTableNumberOfcolorIndex )[uTemp][uIndex] = ( * uTableNumberOfcolorIndex )[iVarPicX][uIndex];
                                }
                            }
                        }
                    }
                }

                // display result in live line by line
                /*
                for (uLoop = 0; uLoop < 16; uLoop++)
                {
                    if (uTableNumberOfcolorIndex[iVarPicX][uLoop] == 0xFFFF)
                    {
                        break;
                    }

                    if (uLoop == 0)
                    {
                        (void )printf( "#%03u: %03u", iVarPicX, uTableNumberOfcolorIndex[iVarPicX][uLoop]);
                    }
                    else
                    {
                        (void )printf( " %03u", uTableNumberOfcolorIndex[iVarPicX][uLoop]);
                    }
                }
                (void )printf( "\n");
                */
                iVarPicX--;
            }

            // RE-Order the color index
            for (iVarPicX = (int )pBmpImage->Longueur_Image - 1; iVarPicX >= 0; iVarPicX--)
            {
                bSwapWasDone = FALSE;
                while ( !bSwapWasDone)
                {
                    for (uLoop = 0; uLoop < 15; uLoop++)
                    {
                        if (( * uTableNumberOfcolorIndex )[iVarPicX][uLoop] == 0xFFFF)
                        {
                            uTableNumberOfcolor[iVarPicX] = uLoop;
                            bSwapWasDone = TRUE;
                            break;
                        }

                        if (( * uTableNumberOfcolorIndex )[iVarPicX][uLoop] > ( * uTableNumberOfcolorIndex )[iVarPicX][uLoop + 1])
                        {
                            uTemp = ( * uTableNumberOfcolorIndex )[iVarPicX][uLoop];
                            ( * uTableNumberOfcolorIndex )[iVarPicX][uLoop] = ( * uTableNumberOfcolorIndex )[iVarPicX][uLoop + 1];
                            ( * uTableNumberOfcolorIndex )[iVarPicX][uLoop + 1] = uTemp;
                            break;
                        }
                    }
                    if (uLoop == 15)
                    {
                        break;
                    }
                }
            }

            // RE-Display result in live line by line
            (void )printf( "\nDisplay result table :\n");
            for (iVarPicX = (int )pBmpImage->Longueur_Image - 1; iVarPicX >= 0; iVarPicX--)
            {
                for (uLoop = 0; uLoop < 16; uLoop++)
                {
                    if (( * uTableNumberOfcolorIndex )[iVarPicX][uLoop] == 0xFFFF)
                    {
                        break;
                    }

                    if (uLoop == 0)
                    {
                        (void )printf( "#%03u: %03u", iVarPicX, ( * uTableNumberOfcolorIndex )[iVarPicX][uLoop]);
                    }
                    else
                    {
                        (void )printf( " %03u", ( * uTableNumberOfcolorIndex )[iVarPicX][uLoop]);
                    }
                }
                whereCursorXY( &tCoord);        // get cursor position in windows console
                moveCursorXY( 65, tCoord.Y);    // shift position X of the cursor in windows console
                (void )printf( "\n");
            }
            (void )printf( "\n");

            // count number of palett equal
//              unsigned int        uTableSCB[64][2] = { 0 };
            uCounter = 199;
            uTableSCB[uCounter] = 1;
            uFirstDiff = 0xFFFF;
            int iFrom = (int )pBmpImage->Longueur_Image - 2;
            bFound = FALSE;
            while (uCounter > 0)
            {
                for (iVarPicX = iFrom; iVarPicX >= 0; iVarPicX--)
                {
                    bSwapWasDone = FALSE;
                    for (uIndex = 0; uIndex < 12; uIndex++)
                    {
                        if (( * uTableNumberOfcolorIndex )[uCounter][uIndex] != ( * uTableNumberOfcolorIndex )[iVarPicX][uIndex])
                        {
                            if (uFirstDiff == 0xFFFF)
                            {
                                uFirstDiff = iVarPicX;
                            }
                            bSwapWasDone = TRUE;
                            break;
                        }
                    }

                    if (bSwapWasDone == FALSE)
                    {
                        if ( (bSwapWasDone == FALSE) && (bFound == FALSE) )
                        {
                            uTableSCB[uCounter]++;
                            if (iVarPicX == 0)
                            {
                                bFound = TRUE;
                            }
                        }
                        else
                        {
                            uTableSCB[uCounter] = 0;
                            uFirstDiff = 0xFFFF;    // Complete end of loops this count is alredy include in another palett
                            break;
                        }
                    }
                }

                if (uFirstDiff != 0xFFFF)
                {
                    uCounter = uFirstDiff;
                    if (uCounter < 200)
                    {
                        iFrom = uFirstDiff - 1;
                        uTableSCB[uCounter] = 1;
                        uFirstDiff = 0xFFFF;
                    }
                    else
                    {
                        uFirstDiff = uFirstDiff;
                        break;
                    }
                }
                else
                {
                    uFirstDiff = uFirstDiff;    // Complete end of loops this count is alredy include in another palett
                    break;
                }
            }

            (void )printf( "\n");
            uCounter = 0;
            uMoreUsed = 0;
            for (uIndex = 0; uIndex < 200; uIndex++)
            {
                if (uTableSCB[uIndex] != 0)
                {
                    (void )printf( "Palett %03d is used %d\n", uIndex, uTableSCB[uIndex]);
                    uMoreUsed += uTableSCB[uIndex];
                    uCounter++;
                }
            }
            (void )printf( "Number of Palett to have is %d for %d lines\n", uCounter, uMoreUsed);

            (void )printf( "\n");

            /*
            // Display the result
            for (iVarPicX = (int )pBmpImage->Longueur_Image - 1; iVarPicX >= 0; iVarPicX--)
            {
                for (uLoop = 0; uLoop < 16; uLoop++)
                {
                    if (( * uTableNumberOfcolorIndex )[iVarPicX][uLoop] == 0xFFFF)
                    {
                        break;
                    }

                    if (uLoop == 0)
                    {
                        (void )printf( "#%03u: %03u", iVarPicX, ( * uTableNumberOfcolorIndex )[iVarPicX][uLoop]);
                    }
                    else
                    {
                        (void )printf( " %03u", ( * uTableNumberOfcolorIndex )[iVarPicX][uLoop]);
                    }
                }
                whereCursorXY( &tCoord);        // get cursor position in windows console
                moveCursorXY( 65, tCoord.Y);    // shift position X of the cursor in windows console
                (void )printf( "\n");
            }
            */
        }
        else
        {
            exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"Not implemented", NULL, NULL, 1964);
        }

        free( uTableNumberOfcolorIndex);
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
                        (void )strncpy_s(pfullOutputFilename, strlen( pfullOutputFilename), pContextArg->pFullFilename, strlen( (const char *)pContextArg->pFullFilename) - 4);
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

    if ( (pContextArg) && (pContextApp) )
    {
        if ( (pContextArg->pFullFilename) && (pContextArg->pOutputPathname) )
        {
            pContextApp->uOutputFileSize = getMyFileSize(pContextArg->pOutputPathname);

            pContextApp->pOutputFileData = readFileToMemory(pContextArg->pOutputPathname);   // second input file is in memory
            if ( pContextApp->pOutputFileData)
            {
                if ( CheckBmpFileFormat( pContextApp->pInputFileData, pContextApp->uInputFileSize) && CheckBmpFileFormat( pContextApp->pOutputFileData, pContextApp->uOutputFileSize))
                {
                    DoInsertPaletteToBmp( pContextApp->pInputFileData, pContextApp->uInputFileSize, &pContextApp->pOutputFileData, &pContextApp->uOutputFileSize, pContextArg->uSwapColumnA, pContextArg->uSwapColumnB);
                    if ( (pContextApp->pOutputFileData) && (pContextArg->pFullFilename) && (pContextApp->uOutputFileSize > 0) )
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

    if (( pContextArg) && (pContextApp))
    {
        if ( pContextArg->pFullFilename)
        {
            pEndString = (const char *)strrchr( (const char *)pContextArg->pFullFilename, '.');
            if ( strcmp( (const char *)pEndString, ".bmp") == 0)
            {
                if ( CheckBmpFileFormat( pContextApp->pInputFileData, pContextApp->uInputFileSize))
                {
                    pContextApp->pOutputFileData = DoSwapColor( pContextApp->pInputFileData, pContextApp->uInputFileSize, pContextArg->uSwapColumnA, pContextArg->uSwapColumnB, &uDataSize);
                    if ( (pContextApp->pOutputFileData) && (pContextArg->pFullFilename) && (uDataSize > 0) )
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

    if ( (pContextArg) && (pContextApp) )
    {
        if ( (pContextArg->pFullFilename) && (pContextArg->pOutputPathname) )
        {
            pContextApp->uOutputFileSize = getMyFileSize( pContextArg->pOutputPathname);

            pContextApp->pOutputFileData = readFileToMemory( pContextArg->pOutputPathname);   // second input file is in memory
            if ( pContextApp->pOutputFileData)
            {
                if ( CheckBmpFileFormat( pContextApp->pInputFileData, pContextApp->uInputFileSize) && CheckBmpFileFormat( pContextApp->pOutputFileData, pContextApp->uOutputFileSize))
                {
                    pBmpIn16ColorsImageOne = (FormatBMP *)pContextApp->pInputFileData;
                    pBmpIn16ColorsImageTwo = (FormatBMP *)pContextApp->pOutputFileData;

                    if ( (pBmpIn16ColorsImageOne->Nbr_Bit_Par_Pixel == 4) && (pBmpIn16ColorsImageTwo->Nbr_Bit_Par_Pixel == 4) )
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
    if ( (pContextArg) && (pContextApp) )
    {
        if ( (pContextArg->pFullFilename) && (pContextArg->pOutputPathname) )
        {
            pContextApp->uOutputFileSize = getMyFileSize( pContextArg->pOutputPathname);

            pContextApp->pOutputFileData = readFileToMemory( pContextArg->pOutputPathname);   // second input file is in memory
            if (pContextApp->pOutputFileData)
            {
                if ( CheckBmpFileFormat( pContextApp->pInputFileData, pContextApp->uInputFileSize) && CheckBmpFileFormat( pContextApp->pOutputFileData, pContextApp->uOutputFileSize))
                {
                    FormatBMP   *pBmpIn16ColorsImageOne = (FormatBMP *)pContextApp->pInputFileData;
                    FormatBMP   *pBmpIn16ColorsImageTwo = (FormatBMP *)pContextApp->pOutputFileData;

                    if ( (pBmpIn16ColorsImageOne->Nbr_Bit_Par_Pixel == 4) && (pBmpIn16ColorsImageTwo->Nbr_Bit_Par_Pixel == 4) )
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

                        if ( (pBmpIn16ColorsImageOne->Nbr_Bit_Par_Pixel == 8) && (pBmpIn16ColorsImageTwo->Nbr_Bit_Par_Pixel == 8) )
                        {
                            DoCopyPalette( pContextApp->pInputFileData, pContextApp->uInputFileSize, pContextApp->pOutputFileData, pContextApp->uOutputFileSize, 8);
                            if ( writeFileFromMemory( pContextArg->pOutputPathname, pContextApp->pOutputFileData, pContextApp->uOutputFileSize))
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

    if ( (pContextArg) && (pContextApp) )
    {
        if (pContextArg->pFullFilename)
        {
            pEndString = (const char *)strrchr( (const char *)pContextArg->pFullFilename, '.');
            if ( strcmp( (const char *)pEndString, ".bmp") == 0)
            {
                if ( CheckBmpFileFormat( pContextApp->pInputFileData, pContextApp->uInputFileSize))
                {
                    pContextApp->pOutputFileData = DoExtractSprite( pContextApp->pInputFileData, pContextApp->uInputFileSize, pContextArg, &uDataSize);
                    if ((pContextApp->pOutputFileData) && (pContextArg->pFullFilename) && (uDataSize > 0))
                    {
                        pfullOutputFilename = createOutputPathname( pContextArg->pFullFilename, pContextArg->pOutputPathname, eCommand);
                        if ( pfullOutputFilename)
                        {
                            if ( writeFileFromMemory( pfullOutputFilename, pContextApp->pOutputFileData, uDataSize))
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
