/* SPDX - License - Identifier: GPL - 3.0 - or -later
 *
 * A tool to help cross dev for Apple II GS.
 *
 * Copyright(C) 2023 - 2026 Renaud Malaval <renaud.malaval@free.fr>.
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

#include "midisynth.h"

// _CRT_SECURE_NO_WARNINGS
#pragma warning( disable : 4996)

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

 // **********************************************************************************
 //  https://manytools.org/hacker-tools/ascii-banner/
 // 
 //                                                  ######                                     #######         #####  
 //   ####   ####  #    # #    # ###### #####  ##### #     #   ##    ####   ####    ##   #         #     ####  #     # 
 //  #    # #    # ##   # #    # #      #    #   #   #     #  #  #  #      #    #  #  #  #         #    #    # #       
 //  #      #    # # #  # #    # #####  #    #   #   ######  #    #  ####  #      #    # #         #    #    # #       
 //  #      #    # #  # # #    # #      #####    #   #       ######      # #      ###### #         #    #    # #       
 //  #    # #    # #   ##  #  #  #      #   #    #   #       #    # #    # #    # #    # #         #    #    # #     # 
 //   ####   ####  #    #   ##   ###### #    #   #   #       #    #  ####   ####  #    # ######    #     ####   #####  
 // 
 // **********************************************************************************

 /**
 * @fn static void convertPascalToC( char *pPacalString, char *pCString)
 * @brief Convert a Pascal string to a C string. The first byte of the Pascal string contains the length of the string, followed by the characters. 
 * The C string is null-terminated.
 *
 * @param[in]        pPacalString
 * @param[out]       pCString
 *
 * @return 0 if success, other values in case of error
 */
static void convertPascalToC( char *pPacalString, char *pCString)
{
    int     loop;

    for ( loop = 1; loop <= pPacalString[0]; loop++)
    {
        pCString[loop - 1] = pPacalString[loop];
    }
    pCString[loop - 1] = '\0';
}

// **********************************************************************************
//  https://manytools.org/hacker-tools/ascii-banner/
// 
//  #     #                       #####                             
//  #  #  #   ##   #    # ###### #     # ##### #    # ###### ###### 
//  #  #  #  #  #  #    # #      #         #   #    # #      #      
//  #  #  # #    # #    # #####   #####    #   #    # #####  #####  
//  #  #  # ###### #    # #            #   #   #    # #      #      
//  #  #  # #    #  #  #  #      #     #   #   #    # #      #      
//   ## ##  #    #   ##   ######  #####    #    ####  #      #
// 
// **********************************************************************************

/**
* @fn static void dumpWaveDefinedRecord( pWaveDefinedRecord pWaveDefRecord)
* @brief display the content of a wave defined record
*
* @param[in]        pWaveDefRecord
* @param[in]        iNumer
*/
static void dumpWaveDefinedRecord( pWaveDefinedRecord pWaveDefRecord, int iNumer)
{
    char                        tString[0x11] = { '\0' };

    convertPascalToC( (char *)pWaveDefRecord->sWaveName, (char *)tString);
    (void )printf( "sWaveName             = %s\n", (char *)tString);
    (void )printf( "uiDocAddress          = 0x%04X  %u\n", pWaveDefRecord->uiDocAddress, pWaveDefRecord->uiDocAddress);
    (void )printf( "uSetToZero            = %u\n", pWaveDefRecord->uSetToZero);
    (void )printf( "uSize                 = %u\n", pWaveDefRecord->uSize);
    (void )printf( "uVolume               = %u\n", pWaveDefRecord->uVolume);
    (void )printf( "uOctaveTuning         = %u\n", pWaveDefRecord->uOctaveTuning);
    (void )printf( "uSemiToneTuning       = %u\n", pWaveDefRecord->uSemiToneTuning);
    (void )printf( "uFineTuning           = %u\n", pWaveDefRecord->uFineTuning);
    DbgDumpBuffer_8bits( "Free", pWaveDefRecord->uFree, sizeof( pWaveDefRecord->uFree));
}

static void dumpHeaderOfWavFile( tConvmArguments *pContextArg, tContextApp *pContextApp, char *pWavFilename)
{
    pWaveFileHeader         pWavFileData = NULL; // (pSequenceFileHeader)pContextApp->pInputFileData;
    char                    tString[0x11] = { '\0' };
    char                   *pRunning = NULL;
    pWaveDefinedRecord      pWaveDefRecord = NULL;
    unsigned int            uIndex;
    unsigned int            uCounter = 0;

    // replace the previous file name pBnkFilename by the new one pWavFilename
    if (pContextArg->pFullFilename)
    {
        free( pContextArg->pFullFilename);
        pContextArg->pFullFilename = NULL;
    }
    if (pContextApp->pInputFileData)
    {
        free( pContextApp->pInputFileData);
        pContextApp->pInputFileData = NULL;
    }

    pContextArg->pFullFilename = pWavFilename;

    pContextApp->uInputFileSize = getMyFileSize( pContextArg->pFullFilename);
    pContextApp->pInputFileData = readFileToMemory( pContextArg->pFullFilename);   // input file is in memory
    if (pContextApp->pInputFileData)
    {
        pWavFileData = (pWaveFileHeader )pContextApp->pInputFileData;
        (void )printf( "\nDisplay content of file %s size = %u bytes\n\n", getFileName( pContextArg->pFullFilename), pContextApp->uInputFileSize);

        (void )printf( "ulType                  = %c%c%c%c\n", (char )(pWavFileData->ulType & 0x000000FF), (char )((pWavFileData->ulType & 0x0000FF00) >> 8), (char )((pWavFileData->ulType & 0x00FF0000) >> 16), (char )((pWavFileData->ulType & 0xFF000000) >> 24));
        (void )printf( "uiVersion               = 0x%04X\n", pWavFileData->uiVersion);
        (void )printf( "uiOffestToWaveData      = 0x%04X  %u\n", pWavFileData->uiOffestToWaveData, pWavFileData->uiOffestToWaveData);
        (void )printf( "sOwnerName              = %s\n", pWavFileData->sOwnerName);
        (void )printf( "uiNumberOfValidWaveDefs = %u\n", pWavFileData->uiNumberOfValidWaveDefs);

        DbgDumpBuffer_8bits( "sFree", pWavFileData->sFree, sizeof( pWavFileData->sFree));

        pWaveDefRecord = (pWaveDefinedRecord )((char *)pContextApp->pInputFileData + sizeof( aWaveFileHeader));
        for (uIndex = 0; uIndex < pWavFileData->uiNumberOfValidWaveDefs; uIndex++)
        {
            (void )printf( " #%u\n", uIndex);
            dumpWaveDefinedRecord( pWaveDefRecord, uIndex);
            pWaveDefRecord++;
        }

        pRunning = pContextApp->pInputFileData + 0x0900;
        for (uIndex = 0; uIndex < (pContextApp->uInputFileSize - 0x0900); uIndex++)
        {
            if (*pRunning == '\0')
                uCounter++;
            pRunning++;
        }
        if (uCounter)
            (void )printf( "ERROR: %u values are set to 0x00 in samples\n", uCounter);

        (void )printf( "\n");
    }
}

// **********************************************************************************
//  https://manytools.org/hacker-tools/ascii-banner/
// 
//  ###                                                               #####                             
//   #  #    #  ####  ##### #####  #    # #    # ###### #    # ##### #     # ##### #    # ###### ###### 
//   #  ##   # #        #   #    # #    # ##  ## #      ##   #   #   #         #   #    # #      #      
//   #  # #  #  ####    #   #    # #    # # ## # #####  # #  #   #    #####    #   #    # #####  #####  
//   #  #  # #      #   #   #####  #    # #    # #      #  # #   #         #   #   #    # #      #      
//   #  #   ## #    #   #   #   #  #    # #    # #      #   ##   #   #     #   #   #    # #      #      
//  ### #    #  ####    #   #    #  ####  #    # ###### #    #   #    #####    #    ####  #      #
// 
// **********************************************************************************

static char *dumpHeaderOfBnkFile( tConvmArguments *pContextArg, tContextApp *pContextApp, char *pBnkFilename)
{
    pInstrumentFileHeader   pBnkfileData = NULL; // (pSequenceFileHeader)pContextApp->pInputFileData;
    char                    tString[0x11] = { '\0' };
    char                   *pWaveFilename = NULL;
    char                   *pInstrumentName = NULL;
    unsigned int            uIndex;

    // replace the previous file name $1 by the new one pBnkFilename
    if (pContextArg->pFullFilename)
    {
        free( pContextArg->pFullFilename);
        pContextArg->pFullFilename = NULL;
    }
    if (pContextApp->pInputFileData)
    {
        free( pContextApp->pInputFileData);
        pContextApp->pInputFileData = NULL;
    }

    pContextArg->pFullFilename = pBnkFilename;

    pContextApp->uInputFileSize = getMyFileSize( pContextArg->pFullFilename);
    pContextApp->pInputFileData = readFileToMemory( pContextArg->pFullFilename);   // input file is in memory
    if (pContextApp->pInputFileData)
    {
        pBnkfileData = (pInstrumentFileHeader )pContextApp->pInputFileData;
        (void )printf( "\nDisplay content of file %s size = %u bytes\n\n", getFileName( pContextArg->pFullFilename), pContextApp->uInputFileSize);

        (void )printf( "ulType                = %c%c%c%c\n", (char )(pBnkfileData->ulType & 0x000000FF), (char )((pBnkfileData->ulType & 0x0000FF00) >> 8), (char )((pBnkfileData->ulType & 0x00FF0000) >> 16), (char )((pBnkfileData->ulType & 0xFF000000) >> 24));
        (void )printf( "uiVersion             = 0x%04X\n", pBnkfileData->uiVersion);
        (void )printf( "uiHeaderSize          = 0x%04X  %u\n", pBnkfileData->uiHeaderSize, pBnkfileData->uiHeaderSize);
        (void )printf( "sOwnerName            = %s\n", pBnkfileData->sOwnerName);

        convertPascalToC( (char *)pBnkfileData->sWaveFileName, (char *)tString);
        pWaveFilename = getBasePathname( pContextArg->pFullFilename, 0x10);
        pWaveFilename = strcat(pWaveFilename, (char *)tString);
        if ( (tString[0] != '\0') && (pathFileExists( pWaveFilename) ) )
        {
            (void )printf( "sWaveName             = %s\n", (char *)tString);
        }
        else
        {
            if (tString[0] == '\0')
                (void )printf( "ERROR: sWaveName      = no file name\n");
            else
                (void )printf( "ERROR: sWaveName      = %s does not exist\n", (char *)tString);
            pWaveFilename = NULL;
        }

        (void )printf( "uMasterSemiToneTuning = %u\n", pBnkfileData->uMasterSemiToneTuning);
        (void )printf( "uMasterFineTuning     = %u\n", pBnkfileData->uMasterFineTuning);
        (void )printf( "uReserved             = %u\n", pBnkfileData->uReserved);
        (void )printf( "uMasterVolume         = %u\n", pBnkfileData->uMasterVolume);
        (void )printf( "uNumberOfInstruments  = %u\n", pBnkfileData->uNumberOfInstruments);
        (void )printf( "uiReserved            = %u\n", pBnkfileData->uiReserved);

        DbgDumpBuffer_8bits( "uWaveRefBlock", pBnkfileData->uWaveRefBlock, sizeof( pBnkfileData->uWaveRefBlock));
        DbgDumpBuffer_8bits( "uFree", pBnkfileData->uFree, sizeof( pBnkfileData->uFree));

        pInstrumentName = (char *)pBnkfileData->sInstrument_0_Name;
        for (uIndex = 0; uIndex < 16; uIndex++)
        {
            if (pInstrumentName[0] != '\0')
            {
                convertPascalToC(pInstrumentName, (char *)tString);
                (void )printf( "InstrumentName %02u     = %s\n", uIndex, (char *)tString);
            }
            else
            {
                (void )printf( "InstrumentName %02u     = <empty>\n", uIndex);
            }
            pInstrumentName += 16;
        }
        (void )printf( "\n");
    }

    return pWaveFilename;
}

// **********************************************************************************
//  https://manytools.org/hacker-tools/ascii-banner/
// 
//   #####                                                       #####                             
//  #     # ######  ####  #    # ###### #    #  ####  ######    #     # ##### #    # ###### ###### 
//  #       #      #    # #    # #      ##   # #    # #         #         #   #    # #      #      
//   #####  #####  #    # #    # #####  # #  # #      #####      #####    #   #    # #####  #####  
//        # #      #  # # #    # #      #  # # #      #               #   #   #    # #      #      
//  #     # #      #   #  #    # #      #   ## #    # #         #     #   #   #    # #      #      
//   #####  ######  ### #  ####  ###### #    #  ####  ######     #####    #    ####  #      # 
// 
// **********************************************************************************

/**
* @fn static void dumpSequenceInitialization( pSequenceInitialization pSeqInitData)
* @brief display the content of a sequence initialization structure
*
* @param[in]        pSeqInitData
*/
static void dumpSequenceInitialization( pSequenceInitialization pSeqInitData)
{
    unsigned int    uIndex;

    for (uIndex = 0; uIndex < 16; uIndex++)
    {
        if (uIndex == 0)
        {
            (void )printf( "uChannelVolumes  = %u", pSeqInitData->uChannelVolumes[uIndex]);
        }
        else
        {
            (void )printf( ", %u", pSeqInitData->uChannelVolumes[uIndex]);
        }
    }
    (void )printf( "\n");

    for (uIndex = 0; uIndex < 16; uIndex++)
    {
        if (uIndex == 0)
        {
            (void )printf( "uTrackChannel    = %u", pSeqInitData->uTrackChannel[uIndex]);
        }
        else
        {
            (void )printf( ", %u", pSeqInitData->uTrackChannel[uIndex]);
        }
    }
    (void )printf( "\n");

    for (uIndex = 0; uIndex < 16; uIndex++)
    {
        if (uIndex == 0)
        {
            (void )printf( "uTrackPlay       = %u", pSeqInitData->uTrackPlay[uIndex]);
        }
        else
        {
            (void )printf( ", %u", pSeqInitData->uTrackPlay[uIndex]);
        }
    }
    (void )printf( "\n");

    (void )printf( "uRecordTrack     = %u\n", pSeqInitData->uRecordTrack);
    (void )printf( "uTempo           = %u\n", pSeqInitData->uTempo);
    (void )printf( "uBeatsPerMeasure = %u\n", pSeqInitData->uBeatsPerMeasure);
    (void )printf( "uBeatValue       = %u\n", pSeqInitData->uBeatValue);
    (void )printf( "uTrickPerBeat    = %u\n", pSeqInitData->uTrickPerBeat);

    for (uIndex = 0; uIndex < 16; uIndex++)
    {
        if (uIndex == 0)
        {
            (void )printf( "uTrackOutputMap  = %u", pSeqInitData->uTrackOutputMap[uIndex]);
        }
        else
        {
            (void )printf( ", %u", pSeqInitData->uTrackOutputMap[uIndex]);
        }
    }
    (void )printf( "\n");
}

/**
* @fn static char *dumpHeaderOfSeqFile( tConvmArguments *pContextArg, tContextApp *pContextApp)
* @brief display the content of a sequence file header and return the name of the instrument file if it exists
*
* @param[in]        pContextArg
* @param[out]       pContextApp
*
* @return name of instrument file .bnk if it exists, else return NULL
*/
static char *dumpHeaderOfSeqFile( tConvmArguments *pContextArg, tContextApp *pContextApp)
{
    pSequenceFileHeader         pSeqfileData = (pSequenceFileHeader )pContextApp->pInputFileData;
    char                       *pTrackName = NULL;
    unsigned int                uIndex;
    char                        tString[0x11] = { '\0' };
    char                       *pInstrumentFilename = getBasePathname( pContextArg->pFullFilename, 0x10);
    pSequenceInitialization     pSeqInitData = (pSequenceInitialization )(pContextApp->pInputFileData + pSeqfileData->uiOffestToSeqData);

    (void )printf( "\nDisplay content of file %s size = %u bytes\n\n", getFileName ( pContextArg->pFullFilename), pContextApp->uInputFileSize );

    (void )printf( "ulType                 = %c%c%c%c\n", (char )(pSeqfileData->ulType & 0x000000FF), (char )((pSeqfileData->ulType & 0x0000FF00) >> 8), (char )((pSeqfileData->ulType & 0x00FF0000) >> 16), (char )((pSeqfileData->ulType & 0xFF000000) >> 24));
    (void )printf( "uiVersion              = 0x%04X\n", pSeqfileData->uiVersion);
    (void )printf( "uiOffestToSeqData      = 0x%04X  %u\n", pSeqfileData->uiOffestToSeqData, pSeqfileData->uiOffestToSeqData);
    (void )printf( "sOwnerName             = %s\n", pSeqfileData->sOwnerName);

    convertPascalToC( (char *)pSeqfileData->sInstrumentName, (char *)tString);
    pInstrumentFilename = strcat( pInstrumentFilename, (char *)tString);
    if ( (tString[0] != '\0') && ( pathFileExists( pInstrumentFilename) ) )
    {
        (void )printf( "sInstrumentName        = %s\n", (char *)tString);
    }
    else
    {
        if (tString[0] == '\0')
            (void )printf( "ERROR: sInstrumentName = no file name\n");
        else
            (void )printf( "ERROR: sInstrumentName = %s does not exist\n", (char *)tString);
        pInstrumentFilename = NULL;
    }

    pTrackName = (char *)pSeqfileData->sTrack_0_Name;
    for (uIndex = 0; uIndex < 16; uIndex++)
    {
        if (pTrackName[0] != '\0')
        {
            convertPascalToC( pTrackName, (char *)tString);
            (void )printf( "TrackName %02u          = %s\n", uIndex, (char *)tString);
        }
        else
        {
            (void )printf( "TrackName %02u          = <empty>\n", uIndex);
        }
        pTrackName += 16;
    }
    (void )printf( "\n");

    for (uIndex = 0; uIndex < 16; uIndex++)
    {
        (void )printf( " #%u\n", uIndex);
        dumpSequenceInitialization( pSeqInitData);
        pSeqInitData = (pSequenceInitialization )((char * )pSeqInitData + sizeof( aSequenceInitialization));
    }

    // (void )printf( "\n");
    // (void )printf( "pTrackName = 0x%p\n", pTrackName);
    // (void )printf( "begin      = 0x%p\n", pContextApp->pInputFileData);
    // (void )printf( "end        = 0x%p\n", pContextApp->pInputFileData + pContextApp->uInputFileSize);
    // (void )printf( "before     = 0x%p\n", pSeqInitData);
    pSeqInitData = (pSequenceInitialization )((char * )pSeqInitData + sizeof( aSequenceInitialization));
    // (void )printf( "after      = 0x%p\n", pSeqInitData);

    if (*(unsigned short*)(pContextApp->pInputFileData + pContextApp->uInputFileSize - 2) != 0xFFFF)
        (void )printf( "ERROR: the end of is not equal to 0xFFFF\n");

    (void)printf("\n");

    return pInstrumentFilename;
}

// **********************************************************************************
//  https://manytools.org/hacker-tools/ascii-banner/
// 
//                 ######                       #     #             #####                            
//   #####   ####  #     # #    # #    # #####  ##   ## # #####  # #     # #   # #    # ##### #    # 
//   #    # #    # #     # #    # ##  ## #    # # # # # # #    # # #        # #  ##   #   #   #    # 
//   #    # #    # #     # #    # # ## # #    # #  #  # # #    # #  #####    #   # #  #   #   ###### 
//   #    # #    # #     # #    # #    # #####  #     # # #    # #       #   #   #  # #   #   #    # 
//   #    # #    # #     # #    # #    # #      #     # # #    # # #     #   #   #   ##   #   #    # 
//   #####   ####  ######   ####  #    # #      #     # # #####  #  #####    #   #    #   #   #    # 
// 
// **********************************************************************************

/**
 * @fn int doDumpMidiSynth( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
 * @brief Dump the content of the seq file
 *
 * @param[in]        pContextArg
 * @param[in,out]    pContextApp
 * @param[in]        eCommand
 *
 * @return if success, other values in case of error
 */
int doDumpMidiSynth( tConvmArguments *pContextArg, tContextApp *pContextApp, enum eCommandNumber eCommand)
{
    char    *pBnkFilename = NULL;
    char    *pWavFilename = NULL;

    if ( (pContextArg) && (pContextApp) )
    {
        if (pContextApp->pInputFileData)
        {
            if ( pContextApp->uInputFileSize > 131072 )
                (void )printf( "ERROR: this file size too big to be compatible with IIGS\n");

            pBnkFilename = dumpHeaderOfSeqFile( pContextArg, pContextApp);                         // sequence file .seq
            if (pBnkFilename)
            {
                pWavFilename = dumpHeaderOfBnkFile( pContextArg, pContextApp, pBnkFilename);       // Instrument file .bnk
                if (pWavFilename)
                {
                    dumpHeaderOfWavFile( pContextArg, pContextApp, pWavFilename);   // Wave file .wav
                }
            }
        }
    }

    return 0;
}
