/* FROM : http://www.rosettacode.org/wiki/Run-iLength_encoding/C */
/* This page was last modified on 27 December 2012, at 06:48. */
/* Content is available under GNU Free Documentation License 1.2 unless otherwise noted. */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <conio.h>
#include <malloc.h>
#include <direct.h>

#include <windows.h>

#include "main.h"


// _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)

/**
* @fn static unsigned int rle_encode( char *pOutputFileData, const char *pInputFileData, unsigned int uLen)
* @brief encode in pInputFileData in out pOutputFileData
*
* @param[in,out]    pOutputFileData
* @param[in]        pInputFileData
* @param[in]        uLen
*
* @@return return size of encoded data
*/
static unsigned int rle_encode( char *pOutputFileData, const char *pInputFileData, unsigned int uLen)
{
    unsigned int    uTotalEncodedData = 0;

    if ((pOutputFileData) && (pInputFileData))
    {
    }

    return uTotalEncodedData;
}

/**
* @fn static unsigned int rle_decode( char *pOutputFileData, unsigned int uOutputFileSize, char *pInputFileData, unsigned int uLen)
* @brief decode in pointer in out pointer
*
* @param[in,out]    pOutputFileData
* @param[in]        pInputFileData
* @param[in]        uLen
*
* @@return return size of decoded data
*/
static unsigned int rle_decode( char *pOutputFileData, unsigned int uOutputFileSize, char *pInputFileData, unsigned int uLen)
{
    char           *pRunning;
    unsigned int    uTotalDecodedData = 0;
    unsigned char   type;
    unsigned char   count;
    unsigned char   index;

    if ((pOutputFileData) && (pInputFileData))
    {
        while (uLen > 0)
        {
            type = ((unsigned char)*pInputFileData & 0xc0) >> 6;
            count = ((unsigned char)*pInputFileData & 0x3f) + 1;
            pInputFileData++;
            uLen--;
            //printf("pOutputFileData        : %p\tsize = %d\twrite = %d\tuLen = %d\n", (void*)pOutputFileData, uOutputFileSize, nbrWrite, uLen);

            switch (type)
            {
                case 0: // AllDifferent
                {
                    uTotalDecodedData += count;
                    for (index = 0; index < count; index++)
                    {
                        *pOutputFileData = (unsigned char)*pInputFileData;
                        pOutputFileData++;
                        pInputFileData++;
                        uLen--;
                    }
                }
                break;
                case 1: // RepeatNextByte
                {
                    uLen--;
                    uTotalDecodedData += count;
                    for (index = 0; index < count; index++)
                    {
                        *pOutputFileData = (unsigned char)*pInputFileData;
                        pOutputFileData++;
                    }
                    pInputFileData++;
                }
                break;
                case 2: // RepeatNext4Bytes
                {
                    uLen -= 4;
                    pRunning = pInputFileData;
                    uTotalDecodedData += count;
                    for (index = 0; index < count; index++)
                    {
                        *pOutputFileData = (unsigned char)*pRunning;
                        pOutputFileData++;
                        pRunning++;
                        *pOutputFileData = (unsigned char)*pRunning;
                        pOutputFileData++;
                        pRunning++;
                        *pOutputFileData = (unsigned char)*pRunning;
                        pOutputFileData++;
                        pRunning++;
                        *pOutputFileData = (unsigned char)*pRunning;
                        pOutputFileData++;

                        pRunning = pInputFileData;
                    }
                    pInputFileData++;
                    pInputFileData++;
                    pInputFileData++;
                    pInputFileData++;
                }
                break;
                case 3: // Repeat4ofNextByte
                {
                    uLen--;
                    count *= 4;
                    uTotalDecodedData += count;
                    for (index = 0; index < count; index++)
                    {
                        *pOutputFileData = (unsigned char)*pInputFileData;
                        pOutputFileData++;
                    }
                    pInputFileData++;
                }
                break;
                default:
                {
                    return 0;
                }
                break;
            }
        }
    }

    return uTotalDecodedData;
}

/**
* @fn static void DumpShrHeader(char* pInputFileData, unsigned int inputFileSize)
* @brief Dump the content of a shr file
*
* @param[in,out]    pInputFileData
* @param[in]        inputFileSize
*
*/
static void DumpShrHeader(char* pInputFileData, unsigned int inputFileSize)
{
    HeaderSHR              *pShrStruct;
    DataSHR                *pShrDataStruct;
    char                   *pBlockName = NULL;
    unsigned short int     *pColors = NULL;
    DirEntry               *pDirEntry = NULL;
    char                    pString[32] = "";
    unsigned int            index;
    unsigned int            loop;
    unsigned int            totalOfBytesToUnpack = 0;

    if (pInputFileData)
    {
        pShrStruct = (HeaderSHR *)pInputFileData;

        //printf("long int size    : %d\n", (int)sizeof(pShrStruct->Longueur_Fichier));
        //printf("short int size   : %d\n", (int)sizeof(pShrStruct->Mode_Maitre));
        //printf("insigned int size: %d\n\n", (int)sizeof(pShrStruct->Couleur_Palette_0[0]));

        printf("HEADER\n");
        printf(" - File size         : %lu\n", pShrStruct->Longueur_Fichier);
        if (pShrStruct->Signature[0] != 0)
        {
            pBlockName = (char*)&pString;
            (void)strncpy_s( pBlockName, 32, (char *)(&pShrStruct->Signature[1]), pShrStruct->Signature[0]);
            //(void )strncpy( pfullOutputFilename, pfullFilename, strlen( pfullFilename));
            printf(" - Block Name        : %s\n", pBlockName);
        }

        if (pBlockName)
        {
            if (!strcmp((const char *)pString, "MAIN"))
            {
                printf(" - Master mode high  : %d\n", (pShrStruct->Mode_Maitre && 0xFF00));
                printf(" - Master mode low   : %d\n", (pShrStruct->Mode_Maitre && 0x00FF));
                printf(" - PixelsPerScanLine : %d\n", pShrStruct->Nbr_Pixel_Par_Ligne);
                printf(" - NumColorTables    : %d\n", pShrStruct->Nbr_Table_Couleur);
                if (pShrStruct->Nbr_Table_Couleur > 0)
                {
                    printf(" - COLORS Tables\n");
                    pColors = (unsigned short int*)&pShrStruct->Couleur_Palette_0[0];
                    for (index = 0; index < pShrStruct->Nbr_Table_Couleur; index++)
                    {
                        for (loop = 0; loop < 16; loop++)
                        {
                            if (loop == 0)
                                printf("   0x%04X", *pColors);
                            else
                                printf(" 0x%04X", *pColors);
                            pColors++;
                        }
                    }
                    printf("\n");
                }

                printf("\n + offset            : %d = 0x%04X\n\n", (int )((char *)pColors - pInputFileData), (int)((char*)pColors - pInputFileData));

                if (pColors)
                {
                    pShrDataStruct = (DataSHR *)pColors;
                }
                else
                {
                    pShrDataStruct = (DataSHR *)&pShrStruct->Couleur_Palette_0;
                }
                printf(" - NumScanLines      : %d\n", pShrDataStruct->Nbr_Scan_Ligne);

                if (pShrDataStruct->Nbr_Scan_Ligne > 0)
                {
                    printf(" - SCAN LINE\n");
                    pDirEntry = (DirEntry *)&pShrDataStruct->ScanLineDirectory;
                    loop = 0;
                    for (index = 0; index < pShrDataStruct->Nbr_Scan_Ligne; index++)
                    {
                        if (loop == 0)
                            printf("   %03d %d %d", pDirEntry->NumberOfBytes, (pDirEntry->mode && 0xFF00), (pDirEntry->mode && 0x00FF));
                        else
                            printf("  %03d %d %d", pDirEntry->NumberOfBytes, (pDirEntry->mode && 0xFF00), (pDirEntry->mode && 0x00FF));
                        totalOfBytesToUnpack += pDirEntry->NumberOfBytes;
                        pDirEntry++;
                        loop++;
                        if (loop == 16)
                        {
                            loop = 0;
                            printf("\n");
                        }
                    }
                    printf("\n");
                }

                printf("\n + BytesToUnpack     : %d = 0x%04X\n", totalOfBytesToUnpack, totalOfBytesToUnpack);
                printf(" + offset            : %d = 0x%04X\n\n", (int)((char*)pDirEntry - pInputFileData), (int)((char*)pDirEntry - pInputFileData));

            }
            else
            {
                // TODO add support of PATS, SCIB, PALETTES and MASK
            }
        }
    }

}

/**
* @fn static unsigned int createPic( char *pOutputFileData, unsigned int uOutputFileSize, char *pInputFileData, unsigned int inputFileSize)
* @brief calcul where are the data to uncompress
*
* @param[in,out]    pOutputFileData
* @param[in]        uOutputFileSize
* @param[in]        pInputFileData
* @param[in]        inputFileSize
*
* @return return offset of data to decompress
*/
static unsigned int createPic(char *pOutputFileData, unsigned int uOutputFileSize, char *pInputFileData, unsigned int inputFileSize)
{
    unsigned int        iOffset = 0;
    HeaderSHR          *pShrHeaderStruct;
    DataSHR            *pShrDataStruct;
    unsigned int        iHeaderSize = 0;
    unsigned short int *pColors = NULL;
    unsigned int        colorLen;
    DirEntry           *pDirEntry = NULL;
    unsigned int        totalOfBytesToUnpack = 0;
    unsigned int        index;
    char               *pImageData;

    if (pInputFileData)
    {
        pShrHeaderStruct = (HeaderSHR *)pInputFileData;

        iHeaderSize = sizeof(HeaderSHR) - sizeof(pShrHeaderStruct->Couleur_Palette_0);

        pColors = (unsigned short int *)(pInputFileData + iHeaderSize);
        colorLen = pShrHeaderStruct->Nbr_Table_Couleur * sizeof(pShrHeaderStruct->Couleur_Palette_0);
        //printf("pColors               : %p\n", (void *)pColors);
        //printf("pColors palette size  : %d\n", colorLen);

        pShrDataStruct = (DataSHR*)(pInputFileData + iHeaderSize + colorLen);

        if (pShrDataStruct->Nbr_Scan_Ligne > 0)
        {
            pDirEntry = (DirEntry *)&pShrDataStruct->ScanLineDirectory;
            for (index = 0; index < pShrDataStruct->Nbr_Scan_Ligne; index++)
            {
                totalOfBytesToUnpack += pDirEntry->NumberOfBytes;
                pDirEntry++;
            }
        }

        pImageData = (char *)pDirEntry;
        //printf("pImageData            : %p\n", (void *)pImageData);
        //printf("pImageData size       : %d\n", totalOfBytesToUnpack);
        //printf("\npOutputFileData       : %p\n", (void*)pOutputFileData);

        if ((pColors) && (colorLen >= 32) && (pImageData))
        {
            iOffset = rle_decode(pOutputFileData, uOutputFileSize, pImageData, totalOfBytesToUnpack);
            if (iOffset)
            {
                //printf("rle_decode size       : %d\n", iOffset);
                //printf("pImageData            : %p\n", (void*)pImageData);
                //printf("pImageData size       : %d\n", totalOfBytesToUnpack);
                //printf("\npOutputFileData       : %p\n", (void*)pOutputFileData);

                char *pFun = pOutputFileData + 0x7D00 + 0xD0;
                (void )strncpy( pFun, "Renaud Malaval", sizeof("Renaud Malaval"));
                pFun = pOutputFileData + 0x7D00 + 0xE0;
                (void)strncpy( pFun, "and", sizeof("and"));
                pFun = pOutputFileData + 0x7D00 + 0xF0;
                (void)strncpy( pFun, "Fréderic Mure", sizeof("Fréderic Mure"));

                char* pPalette = pOutputFileData + 0x7E00;
                (void)memcpy(pPalette, pColors, colorLen);
                iOffset = 0x8000;
            }
        }

    }

    return iOffset;
}

/**
* @fn BOOL CheckShrFileFormat( char *pInputFileData, unsigned int inputFileSize)
* @brief Validate the format of a shr file
*
* @param[in]        pInputFileData
* @param[in]        inputFileSize
*
* @return FALSE if is ok or TRUE if problem
*/
BOOL CheckShrFileFormat(char* pInputFileData, unsigned int inputFileSize)
{
    HeaderSHR* pShrHeaderStruct = NULL;
    DataSHR* pShrDataStruct = NULL;
    unsigned int        iHeaderSize = 0;
    unsigned int        colorLen = 0;
    BOOL                bResult = FALSE;

    if (pInputFileData)
    {
        pShrHeaderStruct = (HeaderSHR*)pInputFileData;

        if (inputFileSize >= pShrHeaderStruct->Longueur_Fichier)
        {
            if ((pShrHeaderStruct->Signature[0] == 4) & (pShrHeaderStruct->Signature[1] == 'M') && (pShrHeaderStruct->Signature[2] == 'A') && (pShrHeaderStruct->Signature[3] == 'I') && (pShrHeaderStruct->Signature[4] == 'N'))
            {
                if ((pShrHeaderStruct->Nbr_Table_Couleur >= 0) && (pShrHeaderStruct->Nbr_Table_Couleur <= 200))
                {
                    iHeaderSize = sizeof(HeaderSHR) - sizeof(pShrHeaderStruct->Couleur_Palette_0);
                    colorLen = pShrHeaderStruct->Nbr_Table_Couleur * sizeof(pShrHeaderStruct->Couleur_Palette_0);
                    pShrDataStruct = (DataSHR*)(pInputFileData + iHeaderSize + colorLen);
                    if (pShrDataStruct->Nbr_Scan_Ligne > 0)
                    {
                        bResult = TRUE;
                    }
                }
            }
        }
    }

    return bResult;
}

/**
* @fn char *DoRleJob( char *pInputFileData, unsigned int inputFileSize, unsigned int command, unsigned int *pDataSize)
* @brief parse pInputFileData of inputFileSize size following command, and return the data size of pOutputFileData in pDataSize
*
* @param[in]        pInputFileData
* @param[in]        inputFileSize
* @param[in]        command
* @param[in,out]    pDataSize
*
* @return A new pointer pOutputFileData
*/
char *DoRleJob( char *pInputFileData, unsigned int inputFileSize, unsigned int command, unsigned int *pDataSize)
{
    char           *pOutputFileData = NULL;
    char           *pInputRunner = NULL;

    pOutputFileData = calloc( (size_t)( 65535), 1);
    if (pOutputFileData)
    {
        pInputRunner = pInputFileData;

        switch (command)
        {
            case DUMP:
            {
                DumpShrHeader(pInputFileData, inputFileSize);
                *pDataSize = 0;
            }
            break;

            case RLE_COMP:
                *pDataSize = rle_encode( pOutputFileData, pInputFileData, inputFileSize);
            break;

            case RLE_DECO:
                *pDataSize = createPic( pOutputFileData, 65535, pInputFileData, inputFileSize);
            break;
                
            default:
            break;
        }
    }
    return pOutputFileData;
}
