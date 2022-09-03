// convm.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>

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
 * unsigned int     u<Name>
 * unsigned char    u<Name>
 * unsigned short   u<Name>
 * unsigned #       u<Name>
 * pointer         *p<Name>
 */

/*
 * version management is :
 *  4 numbers separate by a '.'
 *  major.minor.maintenance.build (example: 1.1.3.99)
 */

/*
 * complement ligne de iCommand pour debug :
 * -dblf ../../textes.cc
 *
 * -rled ../../AUBERGIS.shr
 */


/*
 * @fn void usage()
 * @brief Show how to use soft
 *
 */
void usage()
{
    printf("Usage: convm <convmspec> <option> \"<filespec>\" \"<output folder>\"\n");
    printf("\n  <convmspec> is one of the following:\n");
    printf("   -crlf        - convert CR to LF\n");
    printf("   -lfcr        - convert LF to CR\n");
    printf("   -dblf        - replace 2 first $0A (LF) by one in a serie\n");
    printf("   -dbcr        - replace 2 first $0D (CR) by one in a serie\n");
    printf("   -detab <col> - convert tabs to spaces (tab stop every COL columns)\n");
    printf("   -dump        - dump content of a supported file format\n");
    printf("   -rlec        - not implemented\n");
    printf("   -rled        - decompress with rle algorithms file\n");
    printf("   -2bmp        - convert .scr, .shr, .pnt, .pic to .bmp\n");
    printf("   -2pic        - convert .bmp to .pic\n");

    printf("\n  <option> is one of the following:\n");
    printf("   +lower       - the output file name is in lower case\n");

    printf("\n  <filespec> file extension could be:\n");
    printf("   -crlf to -detab : any type of text\n");
    printf("   -dump           : any\n");
    printf("   -rlec -rled     : .scr, .shr, .pnt, .pic\n");
}

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

        uLen = strlen((const char*)pExplain);
        if (pDetail)
        {
            uLen += strlen((const char*)pDetail);
        }
        if (pInfo)
        {
            uLen += strlen((const char*)pInfo);
        }
        uLen += (size_t )32;    // a marge for char ' '; '"'

        pMessage = (char*)calloc( 1, uLen);
        if (pMessage)
        {
            (void)strncpy_s( pMessage, uLen, (const char*)pExplain, strlen((const char *)pExplain));

            if (pDetail)
            {
                pMessage = strcat(pMessage, (const char*)pSpaceString);
                pMessage = strcat(pMessage, (const char*)pSpaceString);
                pMessage = strcat(pMessage, (const char*)pEndString);
                pMessage = strcat(pMessage, (const char*)pDetail);
                pMessage = strcat(pMessage, (const char*)pEndString);
            }

            if (pInfo)
            {
                pMessage = strcat(pMessage, (const char*)pSpaceString);
                pMessage = strcat(pMessage, (const char*)pSpaceString);
                pMessage = strcat(pMessage, (const char*)pEndString);
                pMessage = strcat(pMessage, (const char*)pInfo);
                pMessage = strcat(pMessage, (const char*)pEndString);
            }

            printf("CONVM : %s\n", pMessage);

            free( pMessage);
        }
    }

    exit(uError);
}

/**
* @fn int checkFileExtension( char *pPathFilename, int iCommand)
* @brief Check the 4 last char of file name to be .scr, .shr, .pnt, .pic
*
* @param[in]        pPathFilename
* @param[in]        iCommand
*
* @return 0 if ok, or 3 on error but never return the software exit
*/
int checkFileExtension( char *pPathFilename, int iCommand)
{
    const char  *pEndString = NULL;
    char        *pRunning = NULL;
    char        *pLastPointChar = NULL;
    int          iError = 0;
    BOOL         bErrorNoExt = FALSE;
    BOOL         bError = FALSE;
    BOOL         bErrorCmd = FALSE;
    BOOL         bErrorExt = FALSE;
    const char  *pCmdtext[] = { "none", "-crlf", "-lfcr", "-dblf", "-dbcr", "-detab", "-dump", "-rlec", "-rled", "-2bmp", "-2pic" };

    if ((iCommand == CRLF) || (iCommand == LFCR) || (iCommand == DOUBLE_0A) || (iCommand == DOUBLE_0D) || (iCommand == DETAB) || (iCommand == DUMP))
    {
        iError = 0;
    }
    else
    {
        pEndString = getFileName(pPathFilename);
        if (pEndString)
        {
            pRunning = (char*)pEndString;

            while (*pRunning != '\0')
            {
                if (*pRunning == '.')
                {
                    pLastPointChar = pRunning;
                }
                pRunning++;
            }

            if (pLastPointChar)
            {
                pLastPointChar++;
                if (*pLastPointChar == '\0')
                {
                    bErrorNoExt = TRUE;
                }
                else if (strlen(pLastPointChar) == 3)
                {
                    if (iCommand == RLE_DECO)
                    {
                        if ((strcmp((const char*)pLastPointChar, "shr") != 0) && (strcmp((const char*)pLastPointChar, "pnt") != 0))
                        {
                            if ((strcmp((const char*)pLastPointChar, "scr") == 0) || (strcmp((const char*)pLastPointChar, "pic") == 0))
                            {
                                bErrorCmd = TRUE;
                            }
                            else if (strcmp((const char*)pLastPointChar, "bmp") == 0)
                            {
                                bErrorExt = TRUE;
                            }
                            else
                            {
                                bError = TRUE;
                            }
                        }
                    }
                    else if (iCommand == RLE_COMP)
                    {
                        if ((strcmp((const char*)pLastPointChar, "scr") != 0) && (strcmp((const char*)pLastPointChar, "pic") != 0))
                        {
                            if ((strcmp((const char*)pLastPointChar, "shr") == 0) || (strcmp((const char*)pLastPointChar, "pnt") == 0))
                            {
                                bErrorCmd = TRUE;
                            }
                            else if (strcmp((const char*)pLastPointChar, "bmp") == 0)
                            {
                                bErrorExt = TRUE;
                            }
                            else
                            {
                                bError = TRUE;
                            }
                        }
                    }
                    else if (iCommand == TO_BMP)
                    {
                        // strcmp == 0 -> OK 
                        if ((strcmp((const char*)pLastPointChar, "scr") != 0) && (strcmp((const char*)pLastPointChar, "pic") != 0) &&
                            ((strcmp((const char*)pLastPointChar, "shr") != 0) && (strcmp((const char*)pLastPointChar, "pnt") != 0)))
                        {
                            if (strcmp((const char*)pLastPointChar, "bmp") == 0)
                            {
                                bErrorCmd = TRUE;
                            }
                            else
                            {
                                bErrorExt = TRUE;
                            }
                        }
                    }
                    else if (iCommand == TO_PIC)
                    {
                        if (strcmp((const char*)pLastPointChar, "bmp") != 0)
                        {
                            bError = TRUE;
                        }
                    }
                    else
                    {
                        bError = TRUE;
                    }
                }
                else
                {
                    bError = TRUE;
                }

                if (bError)
                {
                    exitOnError((char*)"file extension not supported", (char*)pCmdtext[iCommand], (char *)pEndString, 3);
                    iError = 1;
                }

                if (bErrorCmd)
                {
                    exitOnError((char*)"command already done on this file", (char *)pCmdtext[iCommand], (char *)pEndString, 3);
                    iError = 1;
                }

                if (bErrorExt)
                {
                    exitOnError((char*)"file not compatible with command", (char *)pCmdtext[iCommand], (char *)pEndString, 5);
                    iError = 1;
                }
            }
            else
            {
                bErrorNoExt = TRUE;
            }

            if (bErrorNoExt)
            {
                exitOnError((char*)"file without extension", NULL, pPathFilename, 3);
                iError = 1;
            }
        }
    }

    return iError;
}

/**
* @fn void pathanmeToLowerCase( char **pPathname)
* @brief parse the pathname and set only the file name in lower case
*
* @param[in]        pPathname
*
*/
void pathanmeToLowerCase( char **pPathname)
{
    char        *pRunning = NULL;
    const char  *pEndString = NULL;
    char        *pBasePathename = NULL;

    //printf("before input path is : %s\n", context.pFullFilename);
    pEndString = getFileName( *pPathname);
    if (pEndString)
    {
        char* pRunning = (char*)pEndString;

        while (*pRunning != '\0')
        {
            if (isupper(*pRunning))
            {
                *pRunning = tolower(*pRunning);
            }
            pRunning++;
        }
        *pRunning = '\0';
    }
}

/**
* @fn int parseArguments(int argc, char *argv[], ConvmArguments *pContext)
* @brief parse the arguments and check it
*
* @param[in]        argc
* @param[in]        argv[]
* @param[in,out]    pContext
*
* @return the convmspec value and filled pContext structure, on error the software exit
*/
int parseArguments(int argc, char *argv[], ConvmArguments *pContext)
{
    char           *pConvmParam = NULL;
    char           *pOptionParam = NULL;
    char           *pDuplicateString = NULL;
    const char     *pEndString = NULL;
    unsigned int    uIndex = 0;
    unsigned int    uCounter = 0;
    int             iCommand = NONE;
    BOOL            bLowerCase = FALSE;

    if (argv[1][0] == '-')  // lower case the convmspec arguments
    {
        uIndex = 0;
        pConvmParam = (char*)calloc( 1, strlen((char*)(argv[1])) + 2);
        if (pConvmParam)
        {
            while (pConvmParam[uIndex] = tolower(argv[1][uIndex]))
            {
                uIndex++;
            }
        }
        else
        {
            exitOnError((char*)"out of memory", NULL, NULL, 1);
        }
    }

    if (argv[2][0] == '+')  // lower case the option arguments
    {
        uIndex = 0;
        pOptionParam = (char*)calloc( 1, strlen((char*)(argv[2])) + 2);
        if (pOptionParam)
        {
            while (pOptionParam[uIndex] = tolower(argv[2][uIndex]))
            {
                uIndex++;
            }
        }
        else
        {
            exitOnError((char*)"out of memory", NULL, NULL, 1);
        }
    }

    /*
     * Read through command-line arguments for convmspecs and options.
     */
    for (uIndex = 1; uIndex < (unsigned int)argc; uIndex++)
    {
        if ((pConvmParam) && (*pConvmParam == '-') && (argv[uIndex][0] == '-'))     // convmspec
        {
            if (!strcmp((const char*)pConvmParam, "-crlf"))
            {
                iCommand = CRLF;
            }
            else if (!strcmp((const char*)pConvmParam, "-lfcr"))
            {
                iCommand = LFCR;
            }
            else if (!strcmp((const char*)pConvmParam, "-dblf"))
            {
                iCommand = DOUBLE_0A;
            }
            else if (!strcmp((const char*)pConvmParam, "-dbcr"))
            {
                iCommand = DOUBLE_0D;
            }
            else if (!strcmp((const char*)pConvmParam, "-detab"))
            {
                iCommand = DETAB;
                (void)sscanf((const char*)argv[++uIndex], "%d", (int*)&pContext->iTabColumns);
            }
            else if (!strcmp((const char*)pConvmParam, "-dump"))
            {
                iCommand = DUMP;
            }
            else if (!strcmp((const char*)pConvmParam, "-rlec"))
            {
                //iCommand = RLE_COMP;
                exitOnError((char*)"not implemented <convmspec>", pConvmParam, NULL, 4);
            }
            else if (!strcmp((const char*)pConvmParam, "-rled"))
            {
                iCommand = RLE_DECO;
            }
            else if (!strcmp((const char*)pConvmParam, "-2bmp"))
            {
                iCommand = TO_BMP;
            }
            else if (!strcmp((const char*)pConvmParam, "-2pic"))
            {
                iCommand = TO_PIC;
            }
            else
            {
                exitOnError((char*)"illegal convmspec parameter", pConvmParam, NULL, 2);
            }
        }
        else if ((pOptionParam) && (*pOptionParam == '+') && (argv[uIndex][0] == '+'))  // option
        {
            if (!strcmp((const char*)pOptionParam, "+lower"))
            {
                bLowerCase = TRUE;
            }
            else
            {
                if (pConvmParam)
                {
                    free(pConvmParam);
                }

                exitOnError((char*)"illegal option parameter", pOptionParam, NULL, 2);
            }
        }
        else
        {
            if (!pContext->pFullFilename)
            {
                pContext->pFullFilename = (char *)calloc( 1, _MAX_PATH + _MAX_PATH);
                if (!pContext->pFullFilename)
                {
                    exitOnError((char*)"out of memory", NULL, NULL, 1);
                }
                else
                {
                    if ( ((argv[uIndex][0] == '.') && (argv[uIndex][1] == '.')) ||
                        ((argv[uIndex][2] != ':') && ( (argv[uIndex][3] != '/') && (argv[uIndex][3] != '\\')) ) ||
                        (((argv[uIndex][0] != '/') && (argv[uIndex][2] != '/')) || ((argv[uIndex][0] != '\\') && (argv[uIndex][2] != '\\'))) )
                    {
                        if (_fullpath(pContext->pFullFilename, argv[uIndex], _MAX_PATH) != NULL)
                        {
                            if (!pathFileExists((const char*)pContext->pFullFilename))
                            {
                                exitOnError((char*)"input file does not exist", NULL, argv[uIndex], 2);
                            }
                        }
                        else
                        {
                            exitOnError((char*)"invalid input parameter", NULL, argv[uIndex], 2);
                        }
                    }
                    else
                    {
                        strncpy(pContext->pFullFilename, (const char*)argv[uIndex], strlen((const char*)argv[uIndex]) + (size_t)1);
                    }
                }

                pContext->pFullFilename = parseAntiSlashChar(&pContext->pFullFilename);
                if (pathFileExists((const char*)pContext->pFullFilename))
                {
                    if (getMyFileSize(pContext->pFullFilename) == 0)
                    {
                        exitOnError((char*)"size equal to 0 for file", NULL, pContext->pFullFilename, 3);
                    }
                    else
                    {
                        if (bLowerCase)
                        {
                            pathanmeToLowerCase( &pContext->pFullFilename);
                        }
                        else
                        {
                            pDuplicateString = strdup( pContext->pFullFilename);
                            if (pDuplicateString)
                            {
                                pathanmeToLowerCase( &pContext->pFullFilename);
                            }
                        }

                        if (!checkFileExtension(pContext->pFullFilename, iCommand))
                        {
                            if (pDuplicateString)
                            {
                                free(pContext->pFullFilename);
                                pContext->pFullFilename = pDuplicateString;
                            }
                            pEndString = NULL;
                        }
                        else
                        {
                            if (pDuplicateString)
                            {
                                free(pContext->pFullFilename);
                                pContext->pFullFilename = pDuplicateString;
                            }
                            exitOnError((char*)"input file does not exist", pContext->pFullFilename, NULL, 2);
                        }
                    }
                }
                else
                {
                    exitOnError((char*)"input file does not exist", pContext->pFullFilename, NULL, 2);
                }

            }
            else if (!pContext->pOutputPathname)
            {
                pContext->pOutputPathname = (char*)calloc( 1, _MAX_PATH + _MAX_PATH);
                if (!pContext->pOutputPathname)
                {
                    if (pContext->pFullFilename)
                    {
                        free(pContext->pFullFilename);
                        pContext->pFullFilename = NULL;
                    }
                    exitOnError((char*)"out of memory", NULL, NULL, 1);
                }
                else
                {
                    if (((argv[uIndex][0] == '.') && (argv[uIndex][1] == '.')) ||
                        ((argv[uIndex][2] != ':') && ((argv[uIndex][3] != '/') && (argv[uIndex][3] != '\\'))) ||
                        (((argv[uIndex][0] != '/') && (argv[uIndex][2] != '/')) || ((argv[uIndex][0] != '\\') && (argv[uIndex][2] != '\\'))))
                    {
                        if (_fullpath(pContext->pOutputPathname, argv[uIndex], _MAX_PATH) != NULL)
                        {
                            if (!directoryExists((const char*)pContext->pOutputPathname))
                            {
                                exitOnError((char*)"output file does not exist", NULL, argv[uIndex], 2);
                            }
                        }
                        else
                        {
                            exitOnError((char*)"invalid output parameter", NULL, argv[uIndex], 2);
                        }
                    }
                    else
                    {
                        strncpy(pContext->pOutputPathname, (const char*)argv[uIndex], strlen((const char*)argv[uIndex]) + (size_t)1);
                    }
                }

                pContext->pOutputPathname = parseAntiSlashChar(&pContext->pOutputPathname);
                if (pContext->pOutputPathname)
                {
                    if (!directoryExists((const char*)pContext->pOutputPathname))
                    {
                        // Try to create it
                        (void)CreateDirectoryA((const char*)pContext->pOutputPathname, NULL);
                    }

                    if (directoryExists((const char*)pContext->pOutputPathname))
                    {
                        printf("\n");
                        printf("param output path is : %s\n", pContext->pOutputPathname);
                    }
                    else
                    {
                        if (pContext->pOutputPathname)
                        {
                            free(pContext->pOutputPathname);
                            pContext->pOutputPathname = NULL;
                        }
                        if (pContext->pFullFilename)
                        {
                            free(pContext->pFullFilename);
                            pContext->pFullFilename = NULL;
                        }
                        exitOnError((char*)"output folder does not exist", pContext->pOutputPathname, NULL, 2);
                        // TODO: try create the output folder
                    }
                }
                else
                {
                    exitOnError((char*)"out of memory", NULL, NULL, 1);
                }
            }
            else
            {
                exitOnError((char*)"too many parameters", NULL, argv[uIndex], 1);
            }
        }
    }

    if (pConvmParam)
    {
        free(pConvmParam);
        pConvmParam = NULL;
    }

    if (pOptionParam)
    {
        free(pOptionParam);
        pConvmParam = NULL;
    }

    if (iCommand == NONE)
    {
        if (pContext->pFullFilename)
        {
            free(pContext->pFullFilename);
            pContext->pFullFilename = NULL;
        }
        if (pContext->pOutputPathname)
        {
            free(pContext->pOutputPathname);
            pContext->pOutputPathname = NULL;
        }
        exitOnError((char *)"parameters not understood", NULL, NULL, 1);
    }

    return iCommand;
}

/**
* @fn int mySystemCmd( const char *pCommand)
* @brief parse the arguments and check it
*
* @param[in]        pCommand
*
* @return 0 is not error, other values in case of error
*/
int mySystemCmd(const char* pCommand)
{
    int                 iError = 6;
    char               *pTmpCommand = NULL;
    size_t              tmpCommandLen;
    char               *pCcmdExePath = NULL;
    size_t              cmd_exe_path_len;

    if (pCommand)
    {
        pCcmdExePath = getenv("COMSPEC");
        if (pCcmdExePath)
        {
            cmd_exe_path_len = strlen(pCcmdExePath);

            tmpCommandLen = strlen(pCommand);
            pTmpCommand = (char*)calloc(1, cmd_exe_path_len + tmpCommandLen + 5);
            if (pTmpCommand)
            {
                (void)memcpy(pTmpCommand, pCcmdExePath, cmd_exe_path_len + 1);
                (void)strcat(pTmpCommand + cmd_exe_path_len, " /c ");   // cmd
                (void)memcpy(pTmpCommand + cmd_exe_path_len + 4, pCommand, tmpCommandLen + 1);

                (void)fflush(NULL);

                iError = system(pTmpCommand);
                if (iError)
                {
                    printf("CONVM: \"%s\") failed error = %d\n", pTmpCommand, iError);
                }

                free(pTmpCommand);
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
* @fn void updateFileType( ConvmArguments *pContext, int iCommand, char *pfullOutputFilename)
* @brief if iix emulator is present set a filetype and aux filetype to tthe output file
*
* @param[in]        pContext
* @param[in]        iCommand
* @param[in]        pfullOutputFilename
*
*/
void updateFileType(ConvmArguments* pContext, int iCommand, char* pfullOutputFilename)
{
    const char* pCheckEmulatorPresent = "where iix > nul\0";
    const char* pCmdEmulatorPnt = "iix chtyp -t PNT -a 2 ";
    const char* pCmdEmulatorPic = "iix chtyp -t PIC -a 0 ";
    const char* pEndString = "\"";
    char* pDuplicateString = NULL;
    char* pShortPathname = NULL;
    char* ptempFilename = NULL;
    int          iError;

    if ((pContext) && (pfullOutputFilename))
    {
        pShortPathname = (char*)calloc(1, strlen(pfullOutputFilename) + 1);
        pDuplicateString = (char*)calloc(1, _MAX_PATH + _MAX_PATH);
        if ((!pDuplicateString) || (!pShortPathname))
        {
            exitOnError((char*)"out of memory", NULL, NULL, 1);
        }
        else
        {
            printf("CONVM: from = %s\n", pfullOutputFilename);
            ptempFilename = getBasePathname(pfullOutputFilename, 16);
            if (ptempFilename)
            {
                /* Change Windows pathname to Dos pathname (8.3) */
                iError = GetShortPathNameA((LPCSTR)ptempFilename, (LPSTR)pShortPathname, (DWORD)strlen(ptempFilename) + 1);
                if (!iError)
                {
                    printf("CONVM: GetShortPathName() failed error = %d\n", (int)GetLastError());
                    pShortPathname = strdup(pfullOutputFilename);
                }
                else
                {
                    if (!strlen(pShortPathname))
                    {
                        pShortPathname = strdup(pfullOutputFilename);
                    }
                    else
                    {
                        pShortPathname = strcat(pShortPathname, getFileName(pfullOutputFilename));
                    }
                }
                printf("CONVM: to   = %s\n", pShortPathname);
                free( ptempFilename);
            }
            else
            {
                pShortPathname = strdup(pfullOutputFilename);
            }

            /* little cyber security protection */
            if ((*(unsigned long*)pCheckEmulatorPresent != (unsigned long)1919248503) ||
                ((*(unsigned long*)pCmdEmulatorPnt != (unsigned long)544762217) ||
                    (*(unsigned long*)pCmdEmulatorPic != ((unsigned long)544762217))))
            {
                free(pDuplicateString);
                free(pShortPathname);
            }
            else
            {
                iError = mySystemCmd((char*)pCheckEmulatorPresent);
                if (!iError)    // Check if iix is installed to use it
                {
                    if (iCommand == RLE_COMP)
                    {
                        (void)strncpy_s(pDuplicateString, _MAX_PATH + _MAX_PATH, pCmdEmulatorPnt, (int)strlen(pCmdEmulatorPnt));
                        pDuplicateString = strcat(pDuplicateString, pEndString);
                        pDuplicateString = strcat(pDuplicateString, pShortPathname);
                        pDuplicateString = strcat(pDuplicateString, pEndString);

                        iError = mySystemCmd(pDuplicateString);
                        if (iError)
                        {
                            printf("CONVM: Failed to change filetype of '%s' with iix, error = %d\n", pShortPathname, errno);
                        }
                    }
                    else    // RLE_DECO or TO_PIC
                    {
                        (void)strncpy_s(pDuplicateString, _MAX_PATH + _MAX_PATH, pCmdEmulatorPic, (int)strlen(pCmdEmulatorPic));
                        pDuplicateString = strcat(pDuplicateString, pEndString);
                        pDuplicateString = strcat(pDuplicateString, pShortPathname);
                        pDuplicateString = strcat(pDuplicateString, pEndString);

                        iError = mySystemCmd(pDuplicateString);
                        if (iError)
                        {
                            printf("CONVM: Failed to change filetype of '%s' with iix, error = %d\n", pShortPathname, errno);
                        }
                    }
                }
                else        // CiderPress File Attribute Preservation Tags
                {
                    (void)strncpy_s(pDuplicateString, _MAX_PATH + _MAX_PATH, pShortPathname, (int)strlen(pShortPathname));
                    if (iCommand == RLE_COMP)   // PNT
                    {
                        pEndString = "#c00002\0";
                    }
                    else    // RLE_DECO or TO_PIC
                    {
                        pEndString = "#c10000\0"; // PIC
                    }
                    pDuplicateString = strcat(pDuplicateString, pEndString);
                    (void)rename((const char*)pShortPathname, (const char*)pDuplicateString);
                }

                free(pDuplicateString);
                free(pShortPathname);
            }
        }
    }
}

/**
* @fn char *createOutputPathname( char *pFullFilename, char *pOutPathname, int iCommand)
* @brief alloc and change the file extension
*
* @param[in]        pFullFilename
* @param[in]        pOutPathname
* @param[in]        iCommand
*
* @return a new pointer to the new pathname
*/
char *createOutputPathname( char *pFullFilename, char *pOutPathname, int iCommand)
{
    char            *pOutputPathname = NULL;
    const char      *pEndString = NULL;
    size_t           uOutputPathnameLen = 0;

    if ((pFullFilename) && (iCommand != NONE))
    {
        if (pOutPathname)
        {
            pEndString = getFileName(pFullFilename);
            uOutputPathnameLen = strlen((const char*)pOutPathname) + strlen((const char*)pEndString)  + (size_t)16;
        }
        else
        {
            uOutputPathnameLen = strlen((const char*)pFullFilename) + (size_t)16;
        }
        pOutputPathname = (char*)calloc(1, uOutputPathnameLen);

        if (pOutputPathname)
        {
            if (pOutPathname)
            {
                (void)strncpy_s(pOutputPathname, uOutputPathnameLen, pOutPathname, strlen((const char*)pOutPathname));
                pOutputPathname = strcat(pOutputPathname, pEndString);
                pEndString = NULL;
            }
            else
            {
                (void)strncpy_s(pOutputPathname, uOutputPathnameLen, pFullFilename, strlen((const char*)pFullFilename));
                //(void )strncpy( pOutputPathname, pfullFilename, uOutputPathnameLen);
            }

            // Change the extention of the output file
            if (pOutputPathname[strlen(pOutputPathname) - 4] == '.')
            {
                pEndString = &pOutputPathname[strlen(pOutputPathname) - 3];
            }

            if (iCommand == RLE_COMP)
            {
                if (pEndString)
                {
                    pOutputPathname[strlen(pOutputPathname) - 3] = 'p';
                    pOutputPathname[strlen(pOutputPathname) - 2] = 'n';
                    pOutputPathname[strlen(pOutputPathname) - 1] = 't';
                }
                else
                {
                    pOutputPathname = strcat(pOutputPathname, ".pnt");
                }
            }
            else if ((iCommand == RLE_DECO) || (iCommand == TO_PIC))
            {
                if (pEndString)
                {
                    pOutputPathname[strlen(pOutputPathname) - 3] = 'p';
                    pOutputPathname[strlen(pOutputPathname) - 2] = 'i';
                    pOutputPathname[strlen(pOutputPathname) - 1] = 'c';
                }
                else
                {
                    pOutputPathname = strcat(pOutputPathname, ".pic");
                }
            }
            else if (iCommand == TO_BMP)
            {
                if (pEndString)
                {
                    pOutputPathname[strlen(pOutputPathname) - 3] = 'b';
                    pOutputPathname[strlen(pOutputPathname) - 2] = 'm';
                    pOutputPathname[strlen(pOutputPathname) - 1] = 'p';
                }
                else
                {
                    pOutputPathname = strcat(pOutputPathname, ".bmp");
                }
            }
            else
            {
                ;
            }
        }
    }

    return pOutputPathname;
}

/**
* @fn int main( int argc, char *argv[])
* @brief The entry point of the software
*
* @param[in]        argc
* @param[in]        argv[]
*
* @return 0 if success, other values in case of error
*/
int main(int argc, char* argv[])
{
    ConvmArguments  context = { NULL, NULL, 0 };
    char           *pfullOutputFilename = NULL;
    char           *pInputFileData = NULL;
    char           *pOutputFileData = NULL;
    char           *pTempFileData = NULL;
    const char     *pEndString = NULL;
    int             iCommand = NONE;
    unsigned int    uInputFileSize = 0;
    unsigned int    uOutputFileSize = 0;
    unsigned int    uDataSize = 0;
    unsigned int    uIndex = 0;
    int             iCurrentChar = 0;
    int             iFollowChar = 0;
    int             iCurcolumn = 0;

    /* Message Information */
    pEndString = getFileName(argv[0]);
    if (!pEndString)
    {
        pEndString = argv[0];
    }
    // TODO : Get the verion from the file conv.rc

    printf("\n%s v1.2.0.5, (c) R. Malaval & F. Mure 2022.\n\n", pEndString);
    pEndString = NULL;

    if (argc < 3)
    {
        if ((argc == 2) && (argv[1]))
        {
            if ( (!strcmp((const char*)argv[1], "-help")) || (!strcmp((const char*)argv[1], "-h")) || (!strcmp((const char*)argv[1], "-?")) )
            {
                usage();
                return 0;
            }
        }
        usage();
        printf("\n");
        exitOnError((char*)"not enough parameters", NULL, NULL, 1);
    }

    iCommand = parseArguments( argc, argv, &context);

    /*
    printf("\n");
#ifdef _WIN32
    printf("x86 mode 32 bits\n\n");
#else
    printf("x64 mode 64 bits\n\n");
#endif

    printf("\n");
    */
    //pfullFilename = parseSpaceChar( &pfullFilename);
    //printf("");

    uInputFileSize = getMyFileSize(context.pFullFilename);

    printf("\n");
    pInputFileData = readFileToMemory(context.pFullFilename);   // input file is in memory
    if (pInputFileData)
    {
        if (iCommand <= DETAB)
        {
            pOutputFileData = doConvertJob( pInputFileData, uInputFileSize, (unsigned int)iCommand, (unsigned int)context.iTabColumns, (unsigned int *)&uDataSize);
            if (pOutputFileData)
            {
                pfullOutputFilename = getBasePathname( context.pFullFilename, (unsigned int)strlen(TEMP_FILE_NAME));
                pfullOutputFilename = strcat( pfullOutputFilename, TEMP_FILE_NAME);

                if (!writeFileFromMemory(pfullOutputFilename, pOutputFileData, uDataSize))
                {
                    if (context.pFullFilename)
                    {
                        (void)remove( (const char *)context.pFullFilename);
                        (void)rename( (const char *)pfullOutputFilename, (const char *)context.pFullFilename);
                    }
                }
                else
                {
                    exitOnError((char*)"failed to write output file", NULL, pfullOutputFilename, 4);
                }

                free(pfullOutputFilename);
                free(pOutputFileData);
            }
            free(pInputFileData);
        }
        else if (iCommand == DUMP)
        {
            if (CheckShrFileFormat(pInputFileData, uInputFileSize))
            {
                pOutputFileData = DoRleJob(pInputFileData, uInputFileSize, iCommand, NULL);
                if (pOutputFileData)
                {
                    free(pOutputFileData);
                }
            }
            else
            {
                printf("CONVM: nothing is done. Dump is only for .shr or .pnt\n");
            }
        }
        else if ((iCommand == RLE_COMP) || (iCommand == RLE_DECO))
        {
            pOutputFileData = DoRleJob(pInputFileData, uInputFileSize, iCommand, &uDataSize);
            if ( (pOutputFileData) && (context.pFullFilename) && (uDataSize > 0))
            {
                pfullOutputFilename = createOutputPathname( context.pFullFilename, context.pOutputPathname, iCommand);
                if (pfullOutputFilename)
                {
                    if (!writeFileFromMemory(pfullOutputFilename, pOutputFileData, uDataSize))
                    {
                        updateFileType(&context, iCommand, pfullOutputFilename);
                    }
                    else
                    {
                        exitOnError((char*)"failed to write output file", NULL, pfullOutputFilename, 4);
                    }
                    free(pfullOutputFilename);
                }
                else
                {
                    exitOnError((char*)"file not compatible", NULL, context.pFullFilename, 5);
                }

                free(pOutputFileData);
            }
            else
            {
                exitOnError((char*)"file not compatible", NULL, context.pFullFilename, 5);
            }
            free(pInputFileData);
        }
        else if (iCommand == TO_BMP)
        {
            if (CheckShrFileFormat( pInputFileData, uInputFileSize))
            {
                // decompress file before convert to BMP
                pTempFileData = DoRleJob(pInputFileData, uInputFileSize, RLE_DECO, &uDataSize);
                if ((pTempFileData) && (uDataSize > 0))
                {
                    pOutputFileData = DoBmpJob(pTempFileData, uDataSize, iCommand, &uDataSize);
                    free(pTempFileData);
                    pTempFileData = NULL;
                }
            }
            else
            {
                pOutputFileData = DoBmpJob(pInputFileData, uInputFileSize, iCommand, &uDataSize);
            }

            if ((pOutputFileData) && (context.pFullFilename) && (uDataSize > 0))
            {
                pfullOutputFilename = createOutputPathname(context.pFullFilename, context.pOutputPathname, iCommand);
                if (pfullOutputFilename)
                {
                    if (writeFileFromMemory(pfullOutputFilename, pOutputFileData, uDataSize))
                    {
                        exitOnError((char*)"failed to write output file", NULL, pfullOutputFilename, 4);
                    }
                    free(pfullOutputFilename);
                }
                else
                {
                    exitOnError((char*)"file not compatible", NULL, context.pFullFilename, 5);
                }
                free(pOutputFileData);
            }
            else
            {
                exitOnError((char*)"file not compatible", NULL, context.pFullFilename, 5);
            }
            free(pInputFileData);
        }
        else if (iCommand == TO_PIC)
        {
            if (CheckBmpFileFormat(pInputFileData, uInputFileSize))
            {
                pOutputFileData = DoBmpJob(pInputFileData, uInputFileSize, iCommand, &uDataSize);
                if ((pOutputFileData) && (context.pFullFilename) && (uDataSize > 0))
                {
                    pfullOutputFilename = createOutputPathname(context.pFullFilename, context.pOutputPathname, iCommand);
                    if (pfullOutputFilename)
                    {
                        if (!writeFileFromMemory(pfullOutputFilename, pOutputFileData, uDataSize))
                        {
                            updateFileType(&context, iCommand, pfullOutputFilename);
                        }
                        else
                        {
                            exitOnError((char*)"failed to write output file", NULL, pfullOutputFilename, 4);
                        }
                        free(pfullOutputFilename);
                    }
                    else
                    {
                        exitOnError((char*)"file not compatible", NULL, context.pFullFilename, 5);
                    }
                    free(pOutputFileData);
                }
            }
            else
            {
                exitOnError((char*)"file not compatible", NULL, context.pFullFilename, 5);
            }
        }
        else
        {
            // For next command...
            ;
        }
    }
    printf("\n");

    if (context.pFullFilename)
    {
        free(context.pFullFilename);
    }

    printf(" Work is done with success\n");
    return 0;
}
