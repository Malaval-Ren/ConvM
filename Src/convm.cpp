/* SPDX - License - Identifier: GPL - 3.0 - or -later
 *
 * A tool to help cross dev for Apple II GS.
 *
 * Copyright(C) 2023 - 2024 Renaud Malaval <renaud.malaval@free.fr>.
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
 * unsigned int     u<Name>
 * unsigned char    u<Name>
 * unsigned short   u<Name>
 * unsigned long    u<Name>
 * pointer         *p<Name>
 * handle         **h<Name>
 */

/*
 * version management is :
 *  4 numbers separate by a '.'
 *  major.minor.maintenance.build (example: 1.1.3.99)
 */

/*
 * complement ligne de eCommand pour debug :
 * 
 * -dblf ../../textes.cc
 *
 * -rled ../../AUBERGIS.shr
 * 
 * -dump ../../../Iron_Lord/BootImages/iron_lord_logo_gimpA.pic
 * -dump ../../../Iron_Lord/BootImages/ironlord_g9.pic
 * -dump ../../../Iron_Lord/BootImages/ironlord_g9_ex.bmp
 * -dump ../../../Iron_Lord/dessin.bmp/enall2.bmp
 * -dump ../../../Iron_Lord/dessin.bmp/medite.ch.bmp 
 * -dump ../../../Iron_Lord/picks/iron.lord.256.bmp
 * -dump G:\DEV\Python\scbeditorII\ironlord_test2.bmp
 * 
 * -2pic +lower ../../../Iron_Lord/dessin.bmp/medite.ch.bmp
 * -2pic +lower ../../../Iron_Lord/dessin.bmp/enall2.bmp
 * -2pic +lower ../../../Iron_Lord/Olivier--Ren--Fred/enemyallc.bmp
 * -2pic +lower ../../../Iron_Lord/Olivier--Ren--Fred/enemyall_gimp.bmp
 * -2pic +lower ../../../Iron_Lord/dessin.bmp/tasetarc1.pic
 * -2pic +lower ../../../Iron_Lord/picks/Iron_Lord_2c.bmp
 * -2pic +lower ../../../Iron_Lord/dessin.bmp/carte.bmp ../../../Iron_Lord/dessin/
 * 
 * -ncpl ../../../Iron_Lord/dessin/moine.ch.pic
 * 
 * -extxt 3 ../../../Iron_Lord/Olivier--Renaud/iron_lord_game_20221112_104906.AMm
 * -extxt 3 ../../../Iron_Lord/Olivier--Renaud/Iron_Lord_fr_sorcier.uss
 * 
 * -2bmp ../../../Iron_Lord/BootImages/ironlord_g9.pic
 * 
 * -apbm4 ../../../Iron_Lord/dessin.bmp/medite.ch.bmp
 * -apbm4 ../../../Iron_Lord/dessin.bmp/brasdefer4.bmp
 * -apbm4 ../../../Iron_Lord/dessin.bmp/brasdefer6.bmp
 * 
 * -ipbm8 1 1 ../../../Iron_Lord/dessin.bmp/brasdefer4.bmp ../../../Iron_Lord/dessin.bmp/brafer.ch.bmp
 * -ipbm8 0 1 ../../../Iron_Lord/dessin.bmp/tacadre.bmp ../../../Iron_Lord/dessin.bmp/tasetarc111.bmp
 * -ipbm8 0 2 ../../../Iron_Lord/dessin.bmp/tacadre.bmp ../../../Iron_Lord/dessin.bmp/tasetarc111.bmp
 * -ipbm8 0 1 ../../../Iron_Lord/dessin.bmp/meunie.ch.src.bmp  ../../../Iron_Lord/dessin.bmp/meunie.ch.bmp
 *
 * in the palette 0
 * -swap 2 11 ../../../Iron_Lord/dessin.bmp/sorcie.ch.bmp
 * -swap 3 12 ../../../Iron_Lord/dessin.bmp/sorcie.ch.bmp
 * -swap 2 11 ../../../Iron_Lord/dessin.bmp/brafer.ch.dbg.bmp
 * -swap 2 11 ../../../Iron_Lord/dessin.bmp/brafer.ch.dbg.bmp
 * in the palette 1
 * -swap 17 26 ../../../Iron_Lord/dessin.bmp/medite.ch.bmp
 * -swap 18 29 ../../../Iron_Lord/dessin.bmp/medite.ch.bmp
 * -swap 19 25 ../../../Iron_Lord/dessin.bmp/medite.ch.bmp
 * -swap 21 31 ../../../Iron_Lord/dessin.bmp/tacadre.bmp
 * -swap 37 45 ../../../Iron_Lord/dessin.bmp/tasetarc1.bmp
 * 
 * -cmpl ../../../Iron_Lord/dessin.bmp/brafer.ch.bmp ../../../Iron_Lord/dessin.bmp/brasdefer2.bmp
 * 
 * -cppl ../../../Iron_Lord/dessin.bmp/brafer.ch.bmp ../../../Iron_Lord/dessin.bmp/brasdefer22.bmp
 * -cppl ../../../Iron_Lord/dessin.bmp/brafer.ch.OK.bmp ../../../Iron_Lord/dessin.bmp/brasdefer4.bmp
 * -cppl ../../../Iron_Lord/dessin.bmp/brafer.ch.OK.bmp ../../../Iron_Lord/dessin.bmp/brasdefer6.bmp
 *
 * -xtrspr 284 82 80 16 15 ../../../Iron_Lord/dessin.bmp/chevaux.bmp
 * 
 * -ncpl ../../../Iron_Lord/dessin.bmp/medite.ch.bmp
 * -ncpl ../../../Iron_Lord/Flyer/ironlord_test2.bmp
 * -ncpl ../../../Iron_Lord/dessin.bmp/wTerrain5.bmp
 * -ncpl ../../../../../../DEV/Python/scbeditorII/wlooser_dead3.pic
 * -ncpl ../../../../../../DEV/Python/scbeditorII/Chevalier_g2_256c.bmp
 */

/*
 * @fn void usage()
 * @brief Show how to use soft
 *
 */
void usage()
{
    (void )printf( "Usage: convm <convmspec> <option> \"<filespec>\" \"<output folder or file>\"\n");
    (void )printf( "\n  <convmspec> is one of the following:\n");
    (void )printf( "   -crlf                        - convert CR to LF\n");
    (void )printf( "   -lfcr                        - convert LF to CR\n");
    (void )printf( "   -dblf                        - replace 2 first $0A (LF) by one in a serie\n");
    (void )printf( "   -dbcr                        - replace 2 first $0D (CR) by one in a serie\n");
    (void )printf( "   -detab <col>                 - convert tabs to spaces (tab stop every COL columns)\n");
    (void )printf( "   -dump                        - dump content of a supported file format\n");
    (void )printf( "   -extxt <minlen>              - extract string of minlen from a binary file\n");
    (void )printf( "   -rlec                        - not implemented\n");
    (void )printf( "   -rled                        - decompress with rle algorithms file\n");
    (void )printf( "   -2bmp                        - convert .scr, .shr, .pnt, .pic to .bmp\n");
    (void )printf( "   -2pic                        - convert .bmp to .pic\n");
    (void )printf( "   -brcc                        - reduce color chart from 0..256 in R G B (bmp) to 0..15 in R G B (pic)\n");
    (void )printf( "   -ncpl                        - display tle line number with color index not used in .pic and .bmp\n");
    (void )printf( "   -apbm4                       - add palette to .bmp 16 to 256 colors\n");
    (void )printf( "   -ipbm8 <lin> <lin>           - insert palette 16 colors to .bmp 256 colors at lines\n");
    (void )printf( "   -swap <col> <col>            - swap 2 colors in palette and bitmap\n");
    (void )printf( "   -cmpl                        - compare palette of bmp 4 bits per pixel\n");
    (void )printf( "   -cppl                        - copy the 16 colors of bmp 4 bits per pixel and fist 16 colors of bmp 8 bits per pixel\n");
    (void )printf( "   -xtrspr <col> <lin> <lin> <col> <lin> - extract sprite form BMP (4 bits per pixel) to text to .aii\n");

    (void )printf( "\n  <option> is one of the following:\n");
    (void )printf( "   +lower             - the output file name is in lower case\n");

    (void )printf( "\n  <filespec> file extension could be:\n");
    (void )printf( "   -crlf to -detab    : any type of text\n");
    (void )printf( "   -dump              : any\n");
    (void )printf( "   -rlec -rled        : .scr, .shr, .pnt, .pic\n");
    (void )printf( "   -ncpl              : .pic, .bmp\n");
    (void )printf( "   -apbm4 -ipbm8\n   -swap -cmpl -cppl\n   -brcc  : .bmp\n");
}

/**
* @fn int checkFileExtension( char *pPathFilename, int eCommand)
* @brief Check the 4 last char of file name to be .scr, .shr, .pnt, .pic
*
* @param[in]        pPathFilename
* @param[in]        eCommand
*
* @return 0 if ok, or 3 on error but never return the software exit
*/
int checkFileExtension( char *pPathFilename, int eCommand)
{
    const char  *pEndString = NULL;
    char        *pRunning = NULL;
    char        *pLastPointChar = NULL;
    int          iError = 0;
    BOOL         bErrorNoExt = FALSE;
    BOOL         bError = FALSE;
    BOOL         bErrorCmd = FALSE;
    BOOL         bErrorExt = FALSE;
    const char  *pCmdtext[] = { "none", "-crlf", "-lfcr", "-dblf", "-dbcr", "-detab", "-dump", "-extxt", "-rlec", "-rled", "-2bmp", "-2pic", "-ncpl", "-apbm4", "-ipbm8", "-swap", "-cmpl", "-cppl", "-xtrspr"};

    if ((eCommand == eCRLF) || (eCommand == eLFCR) || (eCommand == eDOUBLE_0A) || (eCommand == eDOUBLE_0D) || (eCommand == eDETAB) || (eCommand == eEXT_TXT) || (eCommand == eADDPALBMP4) || (eCommand == eINSERTPALBMP8) || (eCommand == eSWAP2COLOR) || (eCommand == eCOMPPALET) || (eCommand == eCOPYPALET) || (eCommand == eEXTSPRITE))
    {
        iError = 0;
    }
    else
    {
        pEndString = getFileName( pPathFilename);
        if (pEndString)
        {
            pRunning = (char *)pEndString;

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
                    if (eCommand == eDUMP)
                    {
                        if (  (strcmp( (const char *)pLastPointChar, "scr") != 0) && ( strcmp( (const char *)pLastPointChar, "pic") != 0) &&
                            ( (strcmp( (const char *)pLastPointChar, "shr") != 0) && ( strcmp( (const char *)pLastPointChar, "pnt") != 0) ) &&
                              (strcmp( (const char *)pLastPointChar, "bmp") != 0) )
                        {
                            bErrorExt = TRUE;
                        }
                    }
                    else if (eCommand == eRLE_DECO)
                    {
                        if ( (strcmp( (const char *)pLastPointChar, "shr") != 0) && (strcmp( (const char *)pLastPointChar, "pnt") != 0))
                        {
                            if ( (strcmp( (const char *)pLastPointChar, "scr") == 0) || (strcmp( (const char *)pLastPointChar, "pic") == 0))
                            {
                                bErrorCmd = TRUE;
                            }
                            else if (strcmp((const char *)pLastPointChar, "bmp") == 0)
                            {
                                bErrorExt = TRUE;
                            }
                            else
                            {
                                bError = TRUE;
                            }
                        }
                    }
                    else if (eCommand == eRLE_COMP)
                    {
                        if ( (strcmp( (const char *)pLastPointChar, "scr") != 0) && (strcmp( (const char *)pLastPointChar, "pic") != 0) )
                        {
                            if ( (strcmp( (const char *)pLastPointChar, "shr") == 0) || (strcmp( (const char *)pLastPointChar, "pnt") == 0) )
                            {
                                bErrorCmd = TRUE;
                            }
                            else if (strcmp((const char *)pLastPointChar, "bmp") == 0)
                            {
                                bErrorExt = TRUE;
                            }
                            else
                            {
                                bError = TRUE;
                            }
                        }
                    }
                    else if (eCommand == eTO_BMP)
                    {
                        // strcmp == 0 -> OK 
                        if ( (strcmp( (const char *)pLastPointChar, "scr") != 0) && (strcmp( (const char *)pLastPointChar, "pic") != 0) &&
                            ( (strcmp( (const char *)pLastPointChar, "shr") != 0) && (strcmp( (const char *)pLastPointChar, "pnt") != 0) ) )
                        {
                            if (strcmp( (const char *)pLastPointChar, "bmp") == 0)
                            {
                                bErrorCmd = TRUE;
                            }
                            else
                            {
                                bErrorExt = TRUE;
                            }
                        }
                    }
                    else if ( (eCommand == eTO_PIC) || (eCommand == eREDUCECOLORCHART) )
                    {
                        if (strcmp( (const char *)pLastPointChar, "bmp") != 0)
                        {
                            bError = TRUE;
                        }
                    }
                    else if (eCommand == eNUMCOLORPERLINE)
                    {
                        if ( (strcmp( (const char *)pLastPointChar, "pic") != 0) && (strcmp( (const char *)pLastPointChar, "bmp") != 0) )
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
                    exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"file extension not supported", (char *)pCmdtext[eCommand], (char *)pEndString, 3);
                    iError = 1; // fake value program has ended
                }

                if (bErrorCmd)
                {
                    exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"command already done on this file", (char *)pCmdtext[eCommand], (char *)pEndString, 3);
                    iError = 1; // fake value program has ended
                }

                if (bErrorExt)
                {
                    exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"file not compatible with command", (char *)pCmdtext[eCommand], (char *)pEndString, 5);
                    iError = 1; // fake value program has ended
                }
            }
            else
            {
                bErrorNoExt = TRUE;
            }

            if (bErrorNoExt)
            {
                exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"file without extension", NULL, pPathFilename, 3);
                iError = 1; // fake value program has ended
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
*/
void pathanmeToLowerCase( char **pPathname)
{
    char        *pRunning;
    const char  *pEndString;

    if (pPathname)
    {
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
}

const char *pGRen   = "renaud.malaval@free.fr";
const char *pGSpace = " et ";
const char *pGFred  = "frederic.mure@free.fr";

/**
* @fn static enum eCommandNumber parseArguments( int argc, char *argv[], ConvmArguments *pContext)
* @brief parse the arguments and check it
*
* @param[in]        argc
* @param[in]        argv[]
* @param[in,out]    pContext
*
* @return the convmspec value and filled pContext structure, on error the software exit
*/
static enum eCommandNumber parseArguments( int argc, char *argv[], tConvmArguments *pContext)
{
    char                    *pConvmParam = NULL;
    char                    *pOptionParam = NULL;
    char                    *pDuplicateString = NULL;
    const char              *pEndString = NULL;
    unsigned int             uIndex;
    unsigned int             uCounter;
    size_t                   uLen;
    enum eCommandNumber      eCommand = eNONE;
    BOOL                     bLowerCase = FALSE;

    if (argv[1][0] == '-')  // lower case the convmspec arguments
    {
        uLen = strlen( (char *)( argv[1]));
        pConvmParam = (char *)calloc( 1, uLen + 1);
        if (pConvmParam)
        {
            for (uIndex = 0; uIndex <= uLen; uIndex++)
            {
                pConvmParam[uIndex] = tolower( argv[1][uIndex]);
            }
        }
        else
        {
            exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"out of memory", NULL, NULL, 1);
        }
    }

    if (argv[2][0] == '+')  // lower case the option arguments
    {
        uLen = strlen( (char *)( argv[2]));
        pOptionParam = (char *)calloc( 1, uLen + 1);
        if (pOptionParam)
        {
            for (uIndex = 0; uIndex <= uLen; uIndex++)
            {
                pOptionParam[uIndex] = tolower( argv[2][uIndex]);
            }
        }
        else
        {
            exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"out of memory", NULL, NULL, 1);
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
            if (!strcmp( (const char *)pConvmParam, "-crlf"))
            {
                eCommand = eCRLF;
            }
            else if (!strcmp( (const char *)pConvmParam, "-lfcr"))
            {
                eCommand = eLFCR;
            }
            else if (!strcmp( (const char *)pConvmParam, "-dblf"))
            {
                eCommand = eDOUBLE_0A;
            }
            else if (!strcmp( (const char *)pConvmParam, "-dbcr"))
            {
                eCommand = eDOUBLE_0D;
            }
            else if (!strcmp( (const char *)pConvmParam, "-detab"))
            {
                eCommand = eDETAB;
                if (sscanf( (const char *)argv[++uIndex], "%d", (int*)&pContext->uTabColumns) == 1)
                {
                    if (pContext->uTabColumns < 2)
                    {
                        pContext->uTabColumns = 4;
                    }
                }
                else
                {
                    pContext->uTabColumns = 4;
                }
            }
            else if (!strcmp( (const char *)pConvmParam, "-dump"))
            {
                eCommand = eDUMP;
            }
            else if (!strcmp( (const char *)pConvmParam, "-extxt"))
            {
                eCommand = eEXT_TXT;
                if (sscanf( (const char *)argv[++uIndex], "%d", (int*)&pContext->uMinSentenseLen) == 1)
                {
                    if (pContext->uMinSentenseLen < 1)
                    {
                        pContext->uMinSentenseLen = 1;
                    }
                }
                else
                {
                    pContext->uMinSentenseLen = 1;
                }
            }
            else if (!strcmp( (const char *)pConvmParam, "-rlec"))
            {
                // eCommand = eRLE_COMP;
                exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"not implemented <convmspec>", pConvmParam, NULL, 4);
            }
            else if (!strcmp( (const char *)pConvmParam, "-rled"))
            {
                eCommand = eRLE_DECO;
            }
            else if (!strcmp( (const char *)pConvmParam, "-2bmp"))
            {
                eCommand = eTO_BMP;
            }
            else if (!strcmp( (const char *)pConvmParam, "-2pic"))
            {
                eCommand = eTO_PIC;
            }
            else if (!strcmp( (const char *)pConvmParam, "-brcc"))
            {
                eCommand = eREDUCECOLORCHART;
            }
            else if (!strcmp((const char*)pConvmParam, "-ncpl"))
            {
                eCommand = eNUMCOLORPERLINE;
            }
            else if (!strcmp( (const char *)pConvmParam, "-apbm4"))
            {
                eCommand = eADDPALBMP4;
            }
            else if (!strcmp( (const char *)pConvmParam, "-ipbm8"))
            {
                eCommand = eINSERTPALBMP8;
                uIndex++;
                pContext->uSwapColumnA = (unsigned int)atoi( (const char *)argv[uIndex]);
                uIndex++;
                pContext->uSwapColumnB = (unsigned int)atoi( (const char *)argv[uIndex]);
                if ((pContext->uSwapColumnA > 15) || (pContext->uSwapColumnB > 15))
                {
                    exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"illegal convmspec parameter", pConvmParam, NULL, 2);
                }
            }
            else if (!strcmp( (const char *)pConvmParam, "-swap"))
            {
                eCommand = eSWAP2COLOR;
                uIndex++;
                pContext->uSwapColumnA = (unsigned int)atoi( (const char *)argv[uIndex]);
                uIndex++;
                pContext->uSwapColumnB = (unsigned int)atoi( (const char *)argv[uIndex]);
                if ((pContext->uSwapColumnA > 255) || (pContext->uSwapColumnB > 255))
                {
                    exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"illegal convmspec parameter", pConvmParam, NULL, 2);
                }

                if (pContext->uSwapColumnA > pContext->uSwapColumnB)
                {
                    uCounter = pContext->uSwapColumnA;
                    pContext->uSwapColumnA = pContext->uSwapColumnB;
                    pContext->uSwapColumnB = uCounter;
                }

                if ((pContext->uSwapColumnB - pContext->uSwapColumnA) > 15)
                {
                    exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"Swap only color in the same IIGS palette", pConvmParam, NULL, 2);
                }
            }
            else if (!strcmp( (const char *)pConvmParam, "-cmpl"))
            {
                eCommand = eCOMPPALET;
            }
            else if (!strcmp( (const char *)pConvmParam, "-cppl"))
            {
                eCommand = eCOPYPALET;
            }
            else if (!strcmp( (const char *)pConvmParam, "-xtrspr"))
            {
                eCommand = eEXTSPRITE;
                uIndex++;
                pContext->uSwapColumnA = (unsigned int )atoi( (const char *)argv[uIndex]);   // From X
                if (pContext->uSwapColumnA & 0x0001)
                {
                    exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"illegal convmspec parameter impair value", pConvmParam, NULL, 2);
                }
                uIndex++;
                pContext->uSwapColumnB = (unsigned int )atoi( (const char *)argv[uIndex]);   // from Y
                uIndex++;
                pContext->uNumerOfLine = (unsigned int )atoi( (const char *)argv[uIndex]);   // uNumerOfLine
                uIndex++;
                pContext->uLargeur = (unsigned int )atoi( (const char *)argv[uIndex]);       // uLargeur
                if (pContext->uLargeur & 0x0001)
                {
                    exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"illegal convmspec parameter impair value", pConvmParam, NULL, 2);
                }
                uIndex++;
                pContext->uHauteur = (unsigned int )atoi( (const char *)argv[uIndex]);       // uHauteur
            }
            else
            {
                exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"illegal convmspec parameter", pConvmParam, NULL, 2);
            }
        }
        else if ((pOptionParam) && (*pOptionParam == '+') && (argv[uIndex][0] == '+'))  // option
        {
            if (!strcmp( (const char *)pOptionParam, "+lower"))
            {
                bLowerCase = TRUE;
            }
            else
            {
                if (pConvmParam)
                {
                    free(pConvmParam);
                }

                exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"illegal option parameter", pOptionParam, NULL, 2);
            }
        }
        else
        {
            if (!pContext->pFullFilename)
            {
                pContext->pFullFilename = (char *)calloc( 1, _MAX_PATH + _MAX_PATH);
                if (!pContext->pFullFilename)
                {
                    exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"out of memory", NULL, NULL, 1);
                }
                else
                {
                    if ( ((argv[uIndex][0] == '.') && (argv[uIndex][1] == '.')) ||
                        ((argv[uIndex][2] != ':') && ( (argv[uIndex][3] != '/') && (argv[uIndex][3] != '\\')) ) ||
                        (((argv[uIndex][0] != '/') && (argv[uIndex][2] != '/')) || ((argv[uIndex][0] != '\\') && (argv[uIndex][2] != '\\'))) )
                    {
                        if (_fullpath(pContext->pFullFilename, argv[uIndex], _MAX_PATH) != NULL)
                        {
                            if (!pathFileExists( (const char *)pContext->pFullFilename))
                            {
                                exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"input file does not exist", NULL, argv[uIndex], 2);
                            }
                        }
                        else
                        {
                            exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"invalid input parameter", NULL, argv[uIndex], 2);
                        }
                    }
                    else
                    {
                        (void )strncpy(pContext->pFullFilename, (const char *)argv[uIndex], strlen( (const char *)argv[uIndex]) + (size_t)1);
                    }
                }

                pContext->pFullFilename = parseAntiSlashChar( &pContext->pFullFilename);
                if (pathFileExists( (const char *)pContext->pFullFilename))
                {
                    if (getMyFileSize( pContext->pFullFilename) == 0)
                    {
                        exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"size equal to 0 for file", NULL, pContext->pFullFilename, 3);
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

                        if (!checkFileExtension( pContext->pFullFilename, eCommand))
                        {
                            if (pDuplicateString)
                            {
                                free( pContext->pFullFilename);
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
                            exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"input file does not exist", pContext->pFullFilename, NULL, 2);
                        }
                    }
                }
                else
                {
                    exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"input file does not exist", pContext->pFullFilename, NULL, 2);
                }
            }
            else if (!pContext->pOutputPathname)
            {
                pContext->pOutputPathname = (char *)calloc( 1, _MAX_PATH + _MAX_PATH);
                if (!pContext->pOutputPathname)
                {
                    if (pContext->pFullFilename)
                    {
                        free( pContext->pFullFilename);
                        pContext->pFullFilename = NULL;
                    }
                    exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"out of memory", NULL, NULL, 1);
                }
                else
                {
                    if (((argv[uIndex][0] == '.') && (argv[uIndex][1] == '.')) ||
                        ((argv[uIndex][2] != ':') && ((argv[uIndex][3] != '/') && (argv[uIndex][3] != '\\'))) ||
                        (((argv[uIndex][0] != '/') && (argv[uIndex][2] != '/')) || ((argv[uIndex][0] != '\\') && (argv[uIndex][2] != '\\'))))
                    {
                        if (_fullpath( pContext->pOutputPathname, argv[uIndex], _MAX_PATH) != NULL)
                        {
                            if ((eCommand != eCOMPPALET) && (eCommand != eCOPYPALET) && (eCommand != eINSERTPALBMP8))
                            {
                                if (!directoryExists( (const char *)pContext->pOutputPathname))
                                {
                                    if (pathFileExists( (const char *)pContext->pOutputPathname))
                                    {
                                        exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"output parameter must be a folder", NULL, argv[uIndex], 2);
                                    }
                                    else
                                    {
                                        exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"output folder does not exist", NULL, argv[uIndex], 2);
                                    }
                                }
                            }
                            else
                            {
                                if (pathFileExists( (const char *)pContext->pOutputPathname))
                                {
                                    if (getMyFileSize( pContext->pOutputPathname) == 0)
                                    {
                                        exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"size equal to 0 for file", NULL, pContext->pOutputPathname, 3);
                                    }
                                }
                                else
                                {
                                    exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"second file does not exist", pContext->pOutputPathname, NULL, 2);
                                }
                            }
                        }
                        else
                        {
                            exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"invalid output parameter", NULL, argv[uIndex], 2);
                        }
                    }
                    else
                    {
                        (void )strncpy( pContext->pOutputPathname, (const char *)argv[uIndex], strlen( (const char *)argv[uIndex]) + (size_t)1);
                    }
                }

                pContext->pOutputPathname = parseAntiSlashChar( &pContext->pOutputPathname);
                if (pContext->pOutputPathname)
                {
                    if ((eCommand != eCOMPPALET) && (eCommand != eCOPYPALET) && (eCommand != eINSERTPALBMP8))
                    {
                        if (!directoryExists( (const char *)pContext->pOutputPathname))
                        {
                            // Try to create it
                            (void )CreateDirectoryA( (const char *)pContext->pOutputPathname, NULL);
                        }

                        if (directoryExists( (const char *)pContext->pOutputPathname))
                        {
                            (void )printf( "\n");
                            (void )printf( "param output path is : %s\n", pContext->pOutputPathname);
                        }
                        else
                        {
                            if (pContext->pOutputPathname)
                            {
                                free( pContext->pOutputPathname);
                                pContext->pOutputPathname = NULL;
                            }
                            if (pContext->pFullFilename)
                            {
                                free( pContext->pFullFilename);
                                pContext->pFullFilename = NULL;
                            }
                            exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"output folder does not exist", pContext->pOutputPathname, NULL, 2);
                            // TODO: try create the output folder
                        }
                    }
                }
                else
                {
                    exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"out of memory", NULL, NULL, 1);
                }
            }
            else
            {
                exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"too many parameters", NULL, argv[uIndex], 1);
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

    if ((eCommand == eNONE) || (eCommand >= eLast))
    {
        eCommand = eNONE;
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
        exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"parameters not understood", NULL, NULL, 1);
    }

    return eCommand;
}

/**
* @fn void doTest( void)
* @brief a test function in case of study rapidly something
*
*/
void doTest( void)
{

}

/**
* @fn int main( int argc, char *argv[])
* @brief The entry point of this software, content for the exit and in exitOnError() too.
*
* @param[in]        argc
* @param[in]        argv[]
*
* @return 0 if success, other values in case of error
*/
int main( int argc, char *argv[])
{
    const char          *pEndString = NULL;
    static const char   *pVersionStr = "v1.14.15.145, (c) 2022..2024 R. Malaval & F. Mure";
    tConvmArguments      contextArg = { NULL, NULL, 0, 0, 0, 0, 0, 0, 0};
    tContextApp          contextApp = { NULL, 0, NULL, 0};
    enum eCommandNumber  eCommand = eNONE;
    unsigned int         uDataSize = 0;

/*
    char                 ipconf[30] = {};
    unsigned int         uLen;

    uLen = sizeof( ipconf);
*/

/*
    for (int uIndex = 0; uIndex <= 10; uIndex++)
    {
        doTest();
    }
    exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"Success !", NULL, NULL, 0);
*/
    /* Message Information */
    pEndString = getFileName(argv[0]);
    if (!pEndString)
    {
        pEndString = argv[0];
    }
    // TODO : Get the verion from the file conv.rc

    if (argc < 3)
    {
        if ((argc == 2) && (argv[1]))
        {
            if ( ( !strcmp( (const char *)argv[1], "-h")) || ( !strcmp( (const char *)argv[1], "-?")) || ( !strcmp( (const char *)argv[1], "-help") ) )
            {
                usage();
                exitOnError( (char *)__FUNCTION__, __LINE__, NULL, NULL, NULL, 0);
            }

            if ( ( !strcmp( (const char *)argv[1], "-v")) || ( !strcmp( (const char *)argv[1], "-V")) || ( !strcmp( (const char *)argv[1], "-version") ) )
            {
                (void )printf( "%s %s\n", pEndString, pVersionStr);
                exitOnError( (char *)__FUNCTION__, __LINE__, NULL, NULL, NULL, 0);
            }
        }
        usage();
        (void )printf( "\n");
        exitOnError( (char *)__FUNCTION__, __LINE__, (char *)"not enough parameters", NULL, NULL, 1);
    }
    else
    {
        (void )printf( "\n%s %s\n", pEndString, pVersionStr);
        pEndString = NULL;
    }

    eCommand = parseArguments( argc, argv, &contextArg);

    /*
    (void )printf("\n");
#ifdef _WIN32
    (void )printf("x86 mode 32 bits\n\n");
#else
    (void )printf("x64 mode 64 bits\n\n");
#endif

    (void )printf("\n");
    */
    //pfullFilename = parseSpaceChar( &pfullFilename);
    //(void )printf("");

    contextApp.uInputFileSize = getMyFileSize( contextArg.pFullFilename);

    (void )printf( "\n");
    contextApp.pInputFileData = readFileToMemory( contextArg.pFullFilename);   // input file is in memory
    if (contextApp.pInputFileData)
    {
        switch (eCommand)
        {
            case eCRLF:  /* 13 == 0x0D == CR == Carriage Return == '\r' */  /* 10 == 0x0A == LF == Line Feed == '\n' */
                // Fall Through
            case eLFCR:  /* 10 == 0x0A == LF == Line Feed == '\n' */  /* 13 == 0x0D == CR == Carriage Return == '\r' */
                // Fall Through
            case eDOUBLE_0A: /* 10 == 0x0A == LF == Line Feed == '\n' */
                // Fall Through
            case eDOUBLE_0D: /* 13 == 0x0D == CR == Carriage Return == '\r' */
                // Fall Through
            case eDETAB:
                (void )doDetab( &contextArg, &contextApp, eCommand);
            break;
            case eDUMP:
                (void )doDump( &contextArg, &contextApp, eCommand);
            break;
            case eEXT_TXT:
                (void )doExt2Txt( &contextArg, &contextApp);
            break;
            case eRLE_COMP:
                // Fall Through
            case eRLE_DECO:
                (void )doRleComDecom( &contextArg, &contextApp, eCommand);
            break;
            case eTO_BMP:
                (void )doToBmp( &contextArg, &contextApp, eCommand);
            break;
            case eTO_PIC:
                (void )doToPic( &contextArg, &contextApp, eCommand);
            break;
            case eREDUCECOLORCHART:
                (void )doBmp_reduceColorChart( &contextArg, &contextApp, eCommand);
            break;
            case eNUMCOLORPERLINE:
            {
                (void )printf( "CONVM : Take care no fully tested\n");
                if (contextArg.pFullFilename)
                {
                    pEndString = (const char *)strrchr( (const char *)contextArg.pFullFilename, (int )'.');
                    if (pEndString)
                    {
                        if (strcmp( (const char *)pEndString, ".bmp") == 0)
                        {
                            (void )doBMP_NumberOfColor_NotUsePerLine( &contextArg, &contextApp, eCommand);
                        }
                        else
                        {
                            (void )doPIC_NumberOfColor_NotUsePerLine( &contextArg, &contextApp, eCommand);
                        }
                    }
                }
            }
            break;
            case eADDPALBMP4:
                (void )doAddPaletteToBmp4( &contextArg, &contextApp, eCommand);
            break;
            case eINSERTPALBMP8:
                (void )doInsertPaletteToBmp8( &contextArg, &contextApp, eCommand);
            break;
            case eSWAP2COLOR:
                (void )doSwapTwoColor( &contextArg, &contextApp, eCommand);
            break;
            case eCOMPPALET:
                (void )doComparePalette( &contextArg, &contextApp, eCommand);
            break;
            case eCOPYPALET:
                (void )doCopyPalette( &contextArg, &contextApp, eCommand);
            break;
            case eEXTSPRITE:
                (void )printf( "CONVM : Take care no fully tested\n");
                (void )doExtSprite( &contextArg, &contextApp, eCommand);
            break;
            default:
                (void )printf( "CONVM : parameter not supported\n");
            break;
        }
    }
    (void )printf( "\n");

    if (contextArg.pFullFilename)
    {
        free( contextArg.pFullFilename);
    }
    if (contextArg.pOutputPathname)
    {
        free( contextArg.pOutputPathname);
    }

    (void )printf( " Work is done with success\n");
    return 0;
}
