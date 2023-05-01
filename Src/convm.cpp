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
 * unsigned long    u<Name>
 * pointer         *p<Name>
 */

/*
 * version management is :
 *  4 numbers separate by a '.'
 *  major.minor.maintenance.build (example: 1.1.3.99)
 */

/*
 * complement ligne de iCommand pour debug :
 * 
 * -dblf ../../textes.cc
 *
 * -rled ../../AUBERGIS.shr
 * 
 * -dump ../../../Iron_Lord/BootImages/iron_lord_logo_gimpA.pic
 * -dump ../../../Iron_Lord/BootImages/ironlord_g9.pic
 * -dump ../../../Iron_Lord/BootImages/ironlord_g9_ex.bmp
 * -dump ../../../Iron_Lord/dessin.bmp/enall2.bmp
 * 
 * -2pic +lower ../../../Iron_Lord/dessin.bmp/medite.ch.bmp
 * -2pic +lower ../../../Iron_Lord/dessin.bmp/enall2.bmp
 * -2pic +lower ../../../Iron_Lord/Olivier--Ren--Fred/enemyallc.bmp
 * -2pic +lower ../../../Iron_Lord/Olivier--Ren--Fred/enemyall_gimp.bmp
 * 
 * -extxt 3 ../../../Iron_Lord/Olivier--Renaud/iron_lord_game_20221112_104906.AMm
 * -extxt 3 ../../../Iron_Lord/Olivier--Renaud/Iron_Lord_fr_sorcier.uss
 * 
 * -2bmp ../../../Iron_Lord/BootImages/ironlord_g9.pic
 * 
 * -apbm ../../../Iron_Lord/dessin.bmp/medite.ch.bmp
 * -apbm ../../../Iron_Lord/dessin.bmp/brasdefer4.bmp
 * -apbm ../../../Iron_Lord/dessin.bmp/brasdefer6.bmp
 *
 * in the palette 0
 * -swap 2 11 ../../../Iron_Lord/dessin.bmp/sorcie.ch.bmp
 * -swap 3 12 ../../../Iron_Lord/dessin.bmp/sorcie.ch.bmp
 * in the palette 1
 * -swap 17 26 ../../../Iron_Lord/dessin.bmp/medite.ch.bmp
 * -swap 18 29 ../../../Iron_Lord/dessin.bmp/medite.ch.bmp
 * -swap 19 25 ../../../Iron_Lord/dessin.bmp/medite.ch.bmp
 * -swap 20 24 ../../../Iron_Lord/dessin.bmp/medite.ch.bmp
 * -swap 21 23 ../../../Iron_Lord/dessin.bmp/medite.ch.bmp
 *
 * -cmpl ../../../Iron_Lord/dessin.bmp/brafer.ch.bmp ../../../Iron_Lord/dessin.bmp/brasdefer2.bmp
 * 
 * -cppl ../../../Iron_Lord/dessin.bmp/brafer.ch.bmp ../../../Iron_Lord/dessin.bmp/brasdefer22.bmp
 *
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
    printf("   -crlf            - convert CR to LF\n");
    printf("   -lfcr            - convert LF to CR\n");
    printf("   -dblf            - replace 2 first $0A (LF) by one in a serie\n");
    printf("   -dbcr            - replace 2 first $0D (CR) by one in a serie\n");
    printf("   -detab <col>     - convert tabs to spaces (tab stop every COL columns)\n");
    printf("   -dump            - dump content of a supported file format\n");
    printf("   -extxt <minlen>  - extract string of minlen from a binary file\n");
    printf("   -rlec            - not implemented\n");
    printf("   -rled            - decompress with rle algorithms file\n");
    printf("   -2bmp            - convert .scr, .shr, .pnt, .pic to .bmp\n");
    printf("   -2pic            - convert .bmp to .pic\n");
    printf("   -apbm            - add a palette to .bmp 16 to 256 colors\n");
    printf("   -swap <col><col> - swap 2 colors in palette and bitmap\n");
    printf("   -cmpl            - compare palette of bmp 4 bits per pixel\n");
    printf("   -cppl            - copy palette of bmp 4 bits per pixel\n");

    printf("\n  <option> is one of the following:\n");
    printf("   +lower           - the output file name is in lower case\n");

    printf("\n  <filespec> file extension could be:\n");
    printf("   -crlf to -detab  : any type of text\n");
    printf("   -dump            : any\n");
    printf("   -rlec -rled      : .scr, .shr, .pnt, .pic\n");
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
    const char  *pCmdtext[] = { "none", "-crlf", "-lfcr", "-dblf", "-dbcr", "-detab", "-dump", "-extxt", "-rlec", "-rled", "-2bmp", "-2pic", "-apbm", "-swap", "-cmpl", "-cppl"};

    if ((iCommand == eCRLF) || (iCommand == eLFCR) || (iCommand == eDOUBLE_0A) || (iCommand == eDOUBLE_0D) || (iCommand == eDETAB) || (iCommand == eEXT_TXT) || (iCommand == eADDPALBMP) || (iCommand == eSWAP2COLOR) || (iCommand == eCOMPPALET) || (iCommand == eCOPYPALET))
    {
        iError = 0;
    }
    else
    {
        pEndString = getFileName( pPathFilename);
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
                    if (iCommand == eDUMP)
                    {
                        if ((strcmp((const char*)pLastPointChar, "scr") != 0) && (strcmp((const char*)pLastPointChar, "pic") != 0) &&
                            ((strcmp((const char*)pLastPointChar, "shr") != 0) && (strcmp((const char*)pLastPointChar, "pnt") != 0)) &&
                             (strcmp((const char*)pLastPointChar, "bmp") != 0))
                        {
                            bErrorExt = TRUE;
                        }
                    }
                    else if (iCommand == eRLE_DECO)
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
                    else if (iCommand == eRLE_COMP)
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
                    else if (iCommand == eTO_BMP)
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
                    else if (iCommand == eTO_PIC)
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
    char        *pRunning;
    const char  *pEndString;

    pEndString = getFileName( *pPathname);
    if (pEndString)
    {
        pRunning = (char *)pEndString;

        while (*pRunning != '\0')
        {
            if (isupper( *pRunning))
            {
                *pRunning = tolower( *pRunning);
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
    size_t          uLen;
    char           *pConvmParam = NULL;
    char           *pOptionParam = NULL;
    char           *pDuplicateString = NULL;
    const char     *pEndString = NULL;
    unsigned int    uIndex;
    unsigned int    uCounter;
    int             iCommand = eNONE;
    BOOL            bLowerCase = FALSE;

    if (argv[1][0] == '-')  // lower case the convmspec arguments
    {
        uLen = strlen((char *)(argv[1]));
        pConvmParam = (char *)calloc( 1, uLen + 1);
        if (pConvmParam)
        {
            for (uIndex = 0; uIndex <= uLen; uIndex++)
            {
                pConvmParam[uIndex] = tolower(argv[1][uIndex]);
            }
        }
        else
        {
            exitOnError((char *)"out of memory", NULL, NULL, 1);
        }
    }

    if (argv[2][0] == '+')  // lower case the option arguments
    {
        uLen = strlen((char *)(argv[2]));
        pOptionParam = (char *)calloc( 1, uLen + 1);
        if (pOptionParam)
        {
            for (uIndex = 0; uIndex <= uLen; uIndex++)
            {
                pOptionParam[uIndex] = tolower(argv[2][uIndex]);
            }
        }
        else
        {
            exitOnError((char *)"out of memory", NULL, NULL, 1);
        }
    }

    /*
     * Read through command-line arguments for convmspecs and options.
     */
    uIndex = 1;
    while (uIndex < (unsigned int)argc)
    {
        if ((pConvmParam) && (*pConvmParam == '-') && (argv[uIndex][0] == '-'))     // convmspec
        {
            if (!strcmp((const char*)pConvmParam, "-crlf"))
            {
                iCommand = eCRLF;
            }
            else if (!strcmp((const char*)pConvmParam, "-lfcr"))
            {
                iCommand = eLFCR;
            }
            else if (!strcmp((const char*)pConvmParam, "-dblf"))
            {
                iCommand = eDOUBLE_0A;
            }
            else if (!strcmp((const char*)pConvmParam, "-dbcr"))
            {
                iCommand = eDOUBLE_0D;
            }
            else if (!strcmp((const char*)pConvmParam, "-detab"))
            {
                iCommand = eDETAB;
                if (sscanf((const char*)argv[++uIndex], "%d", (int*)&pContext->uTabColumns) == 1)
                {
                    if (pContext->uTabColumns < 2)
                        pContext->uTabColumns = 4;
                }
                else
                {
                    pContext->uTabColumns = 4;
                }
            }
            else if (!strcmp((const char*)pConvmParam, "-dump"))
            {
                iCommand = eDUMP;
            }
            else if (!strcmp((const char*)pConvmParam, "-extxt"))
            {
                iCommand = eEXT_TXT;
                if (sscanf((const char*)argv[++uIndex], "%d", (int*)&pContext->uMinSentenseLen) == 1)
                {
                    if (pContext->uMinSentenseLen < 1)
                        pContext->uMinSentenseLen = 1;
                }
                else
                {
                    pContext->uMinSentenseLen = 1;
                }
            }
            else if (!strcmp((const char*)pConvmParam, "-rlec"))
            {
                //iCommand = eRLE_COMP;
                exitOnError((char*)"not implemented <convmspec>", pConvmParam, NULL, 4);
            }
            else if (!strcmp((const char*)pConvmParam, "-rled"))
            {
                iCommand = eRLE_DECO;
            }
            else if (!strcmp((const char*)pConvmParam, "-2bmp"))
            {
                iCommand = eTO_BMP;
            }
            else if (!strcmp((const char*)pConvmParam, "-2pic"))
            {
                iCommand = eTO_PIC;
            }
            else if (!strcmp((const char*)pConvmParam, "-apbm"))
            {
                iCommand = eADDPALBMP;
            }
            else if (!strcmp((const char*)pConvmParam, "-swap"))
            {
                iCommand = eSWAP2COLOR;
                uIndex++;
                pContext->uSwapColumnA = (unsigned int)atoi((const char*)argv[uIndex]);
                uIndex++;
                pContext->uSwapColumnB = (unsigned int)atoi((const char*)argv[uIndex]);
                if ((pContext->uSwapColumnA > 255) || (pContext->uSwapColumnB > 255))
                {
                    exitOnError((char*)"illegal convmspec parameter", pConvmParam, NULL, 2);
                }

                if (pContext->uSwapColumnA > pContext->uSwapColumnB)
                {
                    uCounter = pContext->uSwapColumnA;
                    pContext->uSwapColumnA = pContext->uSwapColumnB;
                    pContext->uSwapColumnB = uCounter;
                }

                if ((pContext->uSwapColumnB - pContext->uSwapColumnA) > 15)
                {
                    exitOnError((char*)"Swap only color in the same IIGS palette", pConvmParam, NULL, 2);
                }
            }
            else if (!strcmp((const char*)pConvmParam, "-cmpl"))
            {
                iCommand = eCOMPPALET;
            }
            else if (!strcmp((const char*)pConvmParam, "-cppl"))
            {
                iCommand = eCOPYPALET;
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

                        if (!checkFileExtension( pContext->pFullFilename, iCommand))
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
                                free( pContext->pFullFilename);
                                pContext->pFullFilename = pDuplicateString;
                            }
                            exitOnError((char *)"input file does not exist", pContext->pFullFilename, NULL, 2);
                        }
                    }
                }
                else
                {
                    exitOnError((char *)"input file does not exist", pContext->pFullFilename, NULL, 2);
                }

            }
            else if (!pContext->pOutputPathname)
            {
                pContext->pOutputPathname = (char *)calloc( 1, _MAX_PATH + _MAX_PATH);
                if (!pContext->pOutputPathname)
                {
                    if (pContext->pFullFilename)
                    {
                        free(pContext->pFullFilename);
                        pContext->pFullFilename = NULL;
                    }
                    exitOnError((char *)"out of memory", NULL, NULL, 1);
                }
                else
                {
                    if (((argv[uIndex][0] == '.') && (argv[uIndex][1] == '.')) ||
                        ((argv[uIndex][2] != ':') && ((argv[uIndex][3] != '/') && (argv[uIndex][3] != '\\'))) ||
                        (((argv[uIndex][0] != '/') && (argv[uIndex][2] != '/')) || ((argv[uIndex][0] != '\\') && (argv[uIndex][2] != '\\'))))
                    {
                        if (_fullpath(pContext->pOutputPathname, argv[uIndex], _MAX_PATH) != NULL)
                        {
                            if ((iCommand != eCOMPPALET) && (iCommand != eCOPYPALET))
                            {
                                if (!directoryExists((const char *)pContext->pOutputPathname))
                                {
                                    if (pathFileExists((const char *)pContext->pOutputPathname))
                                    {
                                        exitOnError((char *)"output parameter must be a folder", NULL, argv[uIndex], 2);
                                    }
                                    else
                                    {
                                        exitOnError((char *)"output folder does not exist", NULL, argv[uIndex], 2);
                                    }
                                }
                            }
                            else
                            {
                                if (pathFileExists((const char *)pContext->pOutputPathname))
                                {
                                    if (getMyFileSize(pContext->pOutputPathname) == 0)
                                    {
                                        exitOnError((char *)"size equal to 0 for file", NULL, pContext->pOutputPathname, 3);
                                    }
                                }
                                else
                                {
                                    exitOnError((char *)"second file does not exist", pContext->pOutputPathname, NULL, 2);
                                }
                            }
                        }
                        else
                        {
                            exitOnError((char *)"invalid output parameter", NULL, argv[uIndex], 2);
                        }
                    }
                    else
                    {
                        strncpy(pContext->pOutputPathname, (const char *)argv[uIndex], strlen((const char *)argv[uIndex]) + (size_t)1);
                    }
                }

                pContext->pOutputPathname = parseAntiSlashChar(&pContext->pOutputPathname);
                if (pContext->pOutputPathname)
                {
                    if ((iCommand != eCOMPPALET) && (iCommand != eCOPYPALET))
                    {
                        if (!directoryExists((const char *)pContext->pOutputPathname))
                        {
                            // Try to create it
                            (void )CreateDirectoryA((const char *)pContext->pOutputPathname, NULL);
                        }

                        if (directoryExists((const char *)pContext->pOutputPathname))
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
                            exitOnError((char *)"output folder does not exist", pContext->pOutputPathname, NULL, 2);
                            // TODO: try create the output folder
                        }
                    }
                }
                else
                {
                    exitOnError((char *)"out of memory", NULL, NULL, 1);
                }
            }
            else
            {
                exitOnError((char *)"too many parameters", NULL, argv[uIndex], 1);
            }
        }

        uIndex++;
    }

    if (pConvmParam)
    {
        free( pConvmParam);
        pConvmParam = NULL;
    }

    if (pOptionParam)
    {
        free( pOptionParam);
        pConvmParam = NULL;
    }

    if (iCommand == eNONE)
    {
        if (pContext->pFullFilename)
        {
            free( pContext->pFullFilename);
            pContext->pFullFilename = NULL;
        }
        if (pContext->pOutputPathname)
        {
            free( pContext->pOutputPathname);
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
            cmd_exe_path_len = strlen( pCcmdExePath);

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
                free( pDuplicateString);
                free( pShortPathname);
            }
            else
            {
                iError = mySystemCmd((char*)pCheckEmulatorPresent);
                if (!iError)    // Check if iix is installed to use it
                {
                    if (iCommand == eRLE_COMP)
                    {
                        (void )strncpy_s( pDuplicateString, _MAX_PATH + _MAX_PATH, pCmdEmulatorPnt, (int )strlen(pCmdEmulatorPnt));
                        pDuplicateString = strcat( pDuplicateString, pEndString);
                        pDuplicateString = strcat( pDuplicateString, pShortPathname);
                        pDuplicateString = strcat( pDuplicateString, pEndString);

                        iError = mySystemCmd( pDuplicateString);
                        if (iError)
                        {
                            printf("CONVM: Failed to change filetype of '%s' with iix, error = %d\n", pShortPathname, errno);
                        }
                    }
                    else    // eRLE_DECO or eTO_PIC
                    {
                        (void)strncpy_s(pDuplicateString, _MAX_PATH + _MAX_PATH, pCmdEmulatorPic, (int )strlen(pCmdEmulatorPic));
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
                    if (iCommand == eRLE_COMP)   // PNT
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

    if ((pFullFilename) && (iCommand != eNONE))
    {
        if (pOutPathname)
        {
            pEndString = getFileName(pFullFilename);
            uOutputPathnameLen = strlen((const char*)pOutPathname) + strlen((const char*)pEndString) + (size_t)16;
        }
        else
        {
            uOutputPathnameLen = strlen((const char*)pFullFilename) + (size_t)16;
        }
        pOutputPathname = (char *)calloc(1, uOutputPathnameLen);

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

            if (iCommand == eRLE_COMP)
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
            else if ((iCommand == eRLE_DECO) || (iCommand == eTO_PIC))
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
            else if (iCommand == eTO_BMP)
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
            else if (iCommand == eSWAP2COLOR)
            {
                if (pEndString)
                {
                    pOutputPathname[strlen(pOutputPathname) - 4] = '\0';
                }
                pOutputPathname = strcat(pOutputPathname, "-s.bmp");
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
    ConvmArguments  context = {};
    char           *pfullOutputFilename = NULL;
    char           *pInputFileData = NULL;
    char           *pOutputFileData = NULL;
    char           *pTempFileData = NULL;
    const char     *pEndString = NULL;
    int             iCommand = eNONE;
    unsigned int    uInputFileSize = 0;
    unsigned int    uOutputFileSize = 0;
    unsigned int    uDataSize = 0;

    /* Message Information */
    pEndString = getFileName(argv[0]);
    if (!pEndString)
    {
        pEndString = argv[0];
    }
    // TODO : Get the verion from the file conv.rc

    printf("\n%s v1.9.4.71, (c) R. Malaval & F. Mure 2022-23.\n", pEndString);
    pEndString = NULL;

    if (argc < 3)
    {
        if ((argc == 2) && (argv[1]))
        {
            if ( (!strcmp((const char *)argv[1], "-help")) || (!strcmp((const char *)argv[1], "-h")) || (!strcmp((const char *)argv[1], "-?")) )
            {
                usage();
                exitOnError( NULL, NULL, NULL, 1);
            }
        }
        usage();
        printf("\n");
        exitOnError((char *)"not enough parameters", NULL, NULL, 1);
    }

    iCommand = parseArguments( argc, argv, &context);

    // doTest();

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

    uInputFileSize = getMyFileSize( context.pFullFilename);

    printf("\n");
    pInputFileData = readFileToMemory( context.pFullFilename);   // input file is in memory
    if (pInputFileData)
    {
        if (iCommand <= eDETAB)
        {
            pOutputFileData = doConvertJob( pInputFileData, uInputFileSize, (unsigned int)iCommand, (unsigned int)context.uTabColumns, (unsigned int *)&uDataSize);
            if (pOutputFileData)
            {
                pfullOutputFilename = getBasePathname( context.pFullFilename, (unsigned int)strlen( TEMP_FILE_NAME));
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
                    exitOnError( (char *)"failed to write output file", NULL, pfullOutputFilename, 4);
                }

                free(pfullOutputFilename);
                free(pOutputFileData);
            }
            free(pInputFileData);
        }
        else if (iCommand == eDUMP)
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
                if (context.pFullFilename)
                {
                    pEndString = (const char *)strrchr( context.pFullFilename, '.');
                    if (strcmp((const char *)pEndString, ".bmp") == 0)
                    {
                        doDumpBmp( context.pFullFilename, pInputFileData, uInputFileSize);
                    }
                    else
                    {
                        doDumpPic(context.pFullFilename, pInputFileData, uInputFileSize);
                    }
                    pEndString = NULL;
                }
                //printf("CONVM: nothing is done. Dump is only for .shr or .pnt\n");
            }
        }
        else if (iCommand == eEXT_TXT)
        {
            if (context.pFullFilename)
            {
                pOutputFileData = doExtractTextFromBinay(pInputFileData, uInputFileSize, context.uMinSentenseLen, (unsigned int*)&uDataSize);
                if ((pOutputFileData) && (uDataSize > 0))
                {
                    context.pOutputPathname = (char *)calloc(1, strlen(context.pFullFilename) + 5);
                    if (context.pOutputPathname)
                    {
                        (void )strncpy_s(context.pOutputPathname, strlen(context.pFullFilename) + 4, context.pFullFilename, strlen((const char*)context.pFullFilename));
                        context.pOutputPathname = strcat(context.pOutputPathname, (const char *)".txt");
                        if (writeFileFromMemory(context.pOutputPathname, pOutputFileData, uDataSize))
                        {
                            exitOnError((char *)"failed to write output file", NULL, context.pOutputPathname, 4);
                        }
                    }
                }
                else
                {
                    exitOnError((char *)"file too big", NULL, context.pFullFilename, 5);
                }
                free(pOutputFileData);
            }
        }
        else if ((iCommand == eRLE_COMP) || (iCommand == eRLE_DECO))
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
        else if (iCommand == eTO_BMP)
        {
            if (CheckShrFileFormat( pInputFileData, uInputFileSize))
            {
                // decompress file before convert to BMP
                pTempFileData = DoRleJob(pInputFileData, uInputFileSize, eRLE_DECO, &uDataSize);
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
        else if (iCommand == eTO_PIC)
        {
            if (CheckBmpFileFormat( pInputFileData, uInputFileSize))
            {
                pOutputFileData = DoBmpJob(pInputFileData, uInputFileSize, iCommand, &uDataSize);
                if ((pOutputFileData) && (context.pFullFilename) && (uDataSize > 0))
                {
                    pfullOutputFilename = createOutputPathname( context.pFullFilename, context.pOutputPathname, iCommand);
                    if (pfullOutputFilename)
                    {
                        if (!writeFileFromMemory( pfullOutputFilename, pOutputFileData, uDataSize))
                        {
                            updateFileType(&context, iCommand, pfullOutputFilename);
                        }
                        else
                        {
                            exitOnError((char*)"failed to write output file", NULL, pfullOutputFilename, 4);
                        }
                        free( pfullOutputFilename);
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
        else if (iCommand == eADDPALBMP)
        {
            if (context.pFullFilename)
            {
                pEndString = (const char*)strrchr( context.pFullFilename, '.');
                if (strcmp((const char*)pEndString, ".bmp") == 0)
                {
                    pOutputFileData = DoAddPaletteToBmp( pInputFileData, uInputFileSize, iCommand, &uDataSize);
                    if ((pOutputFileData) && (context.pFullFilename) && (uDataSize > 0))
                    {
                        pfullOutputFilename = createOutputPathname( context.pFullFilename, context.pOutputPathname, iCommand);
                        if (pfullOutputFilename)
                        {
                            (void)strncpy_s(pfullOutputFilename, strlen( pfullOutputFilename), context.pFullFilename, strlen( (const char *)context.pFullFilename) - 4);
                            pfullOutputFilename = strcat(pfullOutputFilename, (const char *)"2.bmp");

                            if (writeFileFromMemory( pfullOutputFilename, pOutputFileData, uDataSize))
                            {
                                exitOnError( (char *)"failed to write output file", NULL, pfullOutputFilename, 4);
                            }
                            free( pfullOutputFilename);
                        }
                        free( pOutputFileData);
                    }
                }
                else
                {
                    exitOnError((char*)"file not compatible", NULL, context.pFullFilename, 5);
                }
            }
        }
        else if (iCommand == eSWAP2COLOR)
        {
            if (context.pFullFilename)
            {
                pEndString = (const char*)strrchr(context.pFullFilename, '.');
                if (strcmp((const char*)pEndString, ".bmp") == 0)
                {
                    if (CheckBmpFileFormat(pInputFileData, uInputFileSize))
                    {
                        pOutputFileData = DoSwapColor(pInputFileData, uInputFileSize, context.uSwapColumnA, context.uSwapColumnB, &uDataSize);
                        if ((pOutputFileData) && (context.pFullFilename) && (uDataSize > 0))
                        {
                            pfullOutputFilename = createOutputPathname(context.pFullFilename, context.pOutputPathname, iCommand);
                            if (pfullOutputFilename)
                            {
                                if (writeFileFromMemory(pfullOutputFilename, pOutputFileData, uDataSize))
                                {
                                    exitOnError((char *)"failed to write output file", NULL, pfullOutputFilename, 4);
                                }
                                free( pfullOutputFilename);
                            }
                            free( pOutputFileData);
                        }
                    }
                    else
                    {
                        exitOnError((char *)"file not compatible", NULL, context.pFullFilename, 5);
                    }
                }
                else
                {
                    exitOnError((char *)"file not compatible", NULL, context.pFullFilename, 5);
                }
            }
        }
        else if (iCommand == eCOMPPALET)
        {
            if ((context.pFullFilename) && (context.pOutputPathname))
            {
                uOutputFileSize = getMyFileSize(context.pOutputPathname);

                pOutputFileData = readFileToMemory(context.pOutputPathname);   // second input file is in memory
                if (pOutputFileData)
                {
                    if (CheckBmpFileFormat( pInputFileData, uInputFileSize) && CheckBmpFileFormat( pOutputFileData, uOutputFileSize))
                    {
                        FormatBMP *pBmpIn16ColorsImageOne = (FormatBMP *)pInputFileData;
                        FormatBMP *pBmpIn16ColorsImageTwo = (FormatBMP *)pOutputFileData;

                        if ((pBmpIn16ColorsImageOne->Nbr_Bit_Par_Pixel == 4) && (pBmpIn16ColorsImageTwo->Nbr_Bit_Par_Pixel == 4))
                        {
                            DoComparePalette( pInputFileData, uInputFileSize, pOutputFileData, uOutputFileSize);
                        }
                        else
                        {
                            exitOnError((char *)"file not compatible", NULL, context.pFullFilename, 5);
                        }
                    }
                    else
                    {
                        exitOnError((char *)"file not compatible", NULL, context.pFullFilename, 5);
                    }
                    free( pOutputFileData);
                }
            }
        }
        else if (iCommand == eCOPYPALET)
        {
            if ((context.pFullFilename) && (context.pOutputPathname))
            {
                uOutputFileSize = getMyFileSize( context.pOutputPathname);

                pOutputFileData = readFileToMemory( context.pOutputPathname);   // second input file is in memory
                if (pOutputFileData)
                {
                    if (CheckBmpFileFormat( pInputFileData, uInputFileSize) && CheckBmpFileFormat( pOutputFileData, uOutputFileSize))
                    {
                        FormatBMP *pBmpIn16ColorsImageOne = (FormatBMP *)pInputFileData;
                        FormatBMP *pBmpIn16ColorsImageTwo = (FormatBMP *)pOutputFileData;

                        if ((pBmpIn16ColorsImageOne->Nbr_Bit_Par_Pixel == 4) && (pBmpIn16ColorsImageTwo->Nbr_Bit_Par_Pixel == 4))
                        {
                            DoCopyPalette( pInputFileData, uInputFileSize, pOutputFileData, uOutputFileSize);
                            if (writeFileFromMemory( context.pOutputPathname, pOutputFileData, uOutputFileSize))
                            {
                                exitOnError((char *)"failed to write output file", NULL, pfullOutputFilename, 4);
                            }
                        }
                        else
                        {
                            exitOnError((char *)"file not compatible", NULL, context.pFullFilename, 5);
                        }
                    }
                    else
                    {
                        exitOnError((char *)"file not compatible", NULL, context.pFullFilename, 5);
                    }
                    free( pOutputFileData);
                }
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
        free( context.pFullFilename);
    }
    if (context.pOutputPathname)
    {
        free( context.pOutputPathname);
    }

    printf(" Work is done with success\n");
    return 0;
}
