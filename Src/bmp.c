
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <conio.h>
#include <malloc.h>
#include <direct.h>

#include <windows.h>

#include "main.h"
#include "conv.h"
#include "rle.h"

// _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)

unsigned int bmp_rle4_decode( char *pOutputFileData, unsigned int uOutputFileSize, FormatBMP *pInputFileData)
{
    FormatBMP      *pBmpImage = NULL;
    char           *pInputLine;
    char           *pOutputLine;
    unsigned char   table[258] = {0};
    unsigned int    uBitmapSize = 0;
    unsigned int    uPixelsCounter = 0;
    unsigned int    uLoopLine;
    unsigned int    uInputLineIndex = 0;
    unsigned int    uNbrAddedLine;
    unsigned int    uNbrOfLine = 0;
    unsigned char   uindex;
    unsigned char   uCode = 0xFF;
    unsigned char   uValue;
    unsigned char   uNextHighValue;
    BOOLEAN         bNextHighValuePresent = FALSE;
    unsigned char   uHighValue = 0;

    if ((pOutputFileData) && (pInputFileData))
    {
        pBmpImage = (FormatBMP *)pOutputFileData;
        
        // Compute the line
        pInputLine = (char *)pInputFileData + pInputFileData->Offset_Image; // - (pInputFileData->Largeur_Image));
        pOutputLine = pOutputFileData + pInputFileData->Offset_Image;
        
        for (uLoopLine = 0; uLoopLine < pBmpImage->Taille_Map; uLoopLine++)
        {
            uCode = *pInputLine;
            pInputLine++;
            uValue = *pInputLine;
            pInputLine++;
            uInputLineIndex += 2;
            if (uCode == 0)
            {
                if (uValue == 0)        // end of line
                {
                    uNbrAddedLine = 0;
                    if (uPixelsCounter > pBmpImage->Largeur_Image)
                    {
                        float fNbrOfLine = (float )(uPixelsCounter / pBmpImage->Largeur_Image);
                        uNbrAddedLine = (unsigned int)fNbrOfLine;
                        uLoopLine += uNbrAddedLine;
                        uPixelsCounter = uPixelsCounter - (uNbrAddedLine * pBmpImage->Largeur_Image);
                        uInputLineIndex = (uNbrAddedLine * pBmpImage->Largeur_Image) + uPixelsCounter;
                    }
                    pOutputLine = pOutputLine + (pBmpImage->Largeur_Image - uPixelsCounter);
                    uNbrOfLine += uNbrAddedLine + 1;
                    printf( "[%03u : %03u] _ %04d _ %04u : end of line\t\t%04u\n\n", uCode, uValue, uInputLineIndex + pBmpImage->Offset_Image, (uNbrAddedLine * pBmpImage->Largeur_Image) + uPixelsCounter, uNbrOfLine);
                    uPixelsCounter = 0;
                    if (uLoopLine > pBmpImage->Longueur_Image)
                        break;
                }
                else if (uValue == 1)   // end of bitmap
                {
                    break;
                }
                else if (uValue == 2)   // Delta - move coordinate
                {
                    uValue = *pInputLine;
                    uInputLineIndex += uValue;
                    pInputLine++;
                    pOutputLine = pOutputLine + uValue;
                    uValue = *pInputLine;
                    pOutputLine = pOutputLine + (uValue * pBmpImage->Largeur_Image);
                    uInputLineIndex += (uValue * pBmpImage->Largeur_Image);
                    printf( "[%03u : %03u] _ %04d _ Move\n", uCode, uValue, uInputLineIndex + pBmpImage->Offset_Image);
                }
                else
                {
                    if (uCode == 0)
                    {
                        uCode = uValue;
                        uValue = *(pInputLine + 1);
                        //pInputLine++;
                        uInputLineIndex += 1;
                    }

                    if (!bNextHighValuePresent)
                    {
                        for (uindex = 0; uindex < (uCode >> 1); uindex++)
                        {
                            *pOutputLine = *pInputLine;
                            table[uindex] = *pInputLine;
                            pOutputLine++;
                            uBitmapSize++;
                            uPixelsCounter += 2;
                            uInputLineIndex += 1;
                        }

                        if (uCode & 0x01)
                        {
                            uNextHighValue = uValue & 0xF0;
                            bNextHighValuePresent = TRUE;
                        }
                        else
                        {
                            uHighValue = 0;
                            bNextHighValuePresent = FALSE;
                        }
                    }
                    else
                    {
                        uCode++;
                        for (uindex = 0; uindex < (uCode >> 1); uindex++)
                        {
                            *pOutputLine = uHighValue & ((*pInputLine & 0xF0) >> 4);
                            table[uindex] = *pOutputLine;
                            uHighValue = (*pInputLine & 0x0F);
                            pOutputLine++;
                            uBitmapSize++;
                            uPixelsCounter += 2;
                            uInputLineIndex += 1;
                        }

                        if (uCode & 0x01)
                        {
                            uHighValue = 0;
                            bNextHighValuePresent = FALSE;
                        }
                        else
                        {
                            uNextHighValue = uValue & 0xF0;
                            bNextHighValuePresent = TRUE;
                        }

                    }
                    table[uindex] = '\0';
                    printf( "[%03u : %03u] _ %04d _ %04u : ", uCode, uValue, uInputLineIndex + pBmpImage->Offset_Image, uCode >> 1);
                    for (uindex = 0; uindex < (uCode >> 1); uindex++)
                    {
                        if ((uindex) && ((uindex % 40) == 0))
                            printf( "\n                          : ");

                        printf( "%02x ", table[uindex]);
                    }
                    printf( "\n");

                    /*  CODE WORKING ONLY FOR PAIR VALUE
                    if (uCode & 0x01)     // impair
                    {
                        for (uindex = 0; uindex < ((uCode - 1) >> 1); uindex++)
                        {
                            *pOutputLine = *pInputLine;
                            table[uindex] = *pInputLine;
                            pOutputLine++;
                            pInputLine++;
                            uBitmapSize++;
                            uPixelsCounter += 2;
                            uInputLineIndex += 1;
                        }
                        uNextHighValue = uValue & 0xF0;
                        bNextHighValuePresent = TRUE;
                        table[uindex] = '\0';
                        printf( "[%03u : %03u] _ %04d _ %04u : ", uCode, uValue, uInputLineIndex + pBmpImage->Offset_Image, uCode >> 1);
                        for (uindex = 0; uindex < (uCode >> 1); uindex++)
                        {
                            if ((uindex) && ((uindex % 40) == 0))
                                printf( "\n                          : ");

                            printf( "%02x ", table[uindex]);
                        }
                        printf( "\n");
                    }
                    else    // pair
                    {
                        for (uindex = 0; uindex < (uCode >> 1); uindex++)
                        {
                            *pOutputLine = *pInputLine;
                            table[uindex] = *pInputLine;
                            pOutputLine++;
                            pInputLine++;
                            uBitmapSize++;
                            uPixelsCounter += 2;
                            uInputLineIndex += 1;
                        }

                        if (*pInputLine == 0)
                        {
                            pInputLine++;
                        }

                        bNextHighValuePresent = FALSE;
                        table[uindex] = '\0';
                        printf("[%03u : %03u] _ %04d _ %04u : ", uCode, uValue, uInputLineIndex + pBmpImage->Offset_Image, uCode >> 1);
                        for (uindex = 0; uindex < (uCode >> 1); uindex++)
                        {
                            if ((uindex) && ((uindex % 40) == 0))
                                printf("\n                          : ");

                            printf("%02x ", table[uindex]);
                        }
                        printf("\n");
                    }
                    */
                }
            }
            else
            {
                if (!bNextHighValuePresent)
                {
                    for (uindex = 0; uindex < (uCode >> 1); uindex++)
                    {
                        *pOutputLine = uValue;
                        table[uindex] = uValue;
                        pOutputLine++;
                        uBitmapSize++;
                        uPixelsCounter += 2;
                        uInputLineIndex += 1;
                    }

                    if (uCode & 0x01)
                    {
                        uNextHighValue = (uValue & 0x0F) << 4;
                        bNextHighValuePresent = TRUE;
                    }
                    else
                    {
                        uHighValue = 0;
                        bNextHighValuePresent = FALSE;
                    }
                }
                else
                {
                    for (uindex = 0; uindex < (uCode >> 1); uindex++)
                    {
                        *pOutputLine = uHighValue & ((uValue & 0xF0) >> 4);
                        table[uindex] = *pOutputLine;
                        uHighValue = (uValue & 0x0F);
                        pOutputLine++;
                        uBitmapSize++;
                        uPixelsCounter += 2;
                        uInputLineIndex += 1;
                    }

                    if (uCode & 0x01)
                    {
                        uHighValue = 0;
                        bNextHighValuePresent = FALSE;
                    }
                    else
                    {
                        uNextHighValue = (uValue & 0x0F) << 4;
                        bNextHighValuePresent = TRUE;
                    }

                }
                table[uindex] = '\0';
                printf( "[%03u : %03u] _ %04d _ %04u : ", uCode, uValue, uInputLineIndex + pBmpImage->Offset_Image, uCode >> 1);
                for (uindex = 0; uindex < (uCode >> 1); uindex++)
                {
                    if ((uindex) && ((uindex % 40) == 0))
                        printf( "\n                          : ");

                    printf( "%02x ", table[uindex]);
                }
                printf( "\n");

                /*  CODE WORKING ONLY FOR PAIR VALUE
                if (uCode & 0x01)     // impair
                {
                    break;
                    for (uindex = 0; uindex < ((uCode - 1) >> 1); uindex++)
                    {
                        *pOutputLine = uValue;
                        table[uindex] = uValue;
                        pOutputLine++;
                        uBitmapSize++;
                        uPixelsCounter += 2;
                        uInputLineIndex += 1;
                    }
                    uNextHighValue = uValue & 0xF0;
                    bNextHighValuePresent = TRUE;
                    table[uindex] = '\0';
                    printf("[%03u : %03u] _ %04d _ %04u : ", uCode, uValue, uInputLineIndex + pBmpImage->Offset_Image, uCode >> 1);
                    for (uindex = 0; uindex < ((uCode - 1) >> 1); uindex++)
                    {
                        if ((uindex) && ((uindex % 40) == 0))
                            printf("\n                          : ");

                        printf("%02x ", table[uindex]);
                    }
                    printf("\n");
                }
                else    // pair
                {
                    for (uindex = 0; uindex < (uCode >> 1); uindex++)
                    {
                        *pOutputLine = uValue;
                        table[uindex] = uValue;
                        pOutputLine++;
                        uBitmapSize++;
                        uPixelsCounter += 2;
                        uInputLineIndex += 1;
                    }
                    bNextHighValuePresent = FALSE;
                    table[uindex] = '\0';
                    printf("[%03u : %03u] _ %04d _ %04u : ", uCode, uValue, uInputLineIndex + pBmpImage->Offset_Image, uCode >> 1);
                    for (uindex = 0; uindex < (uCode >> 1); uindex++)
                    {
                        if ( (uindex) && ((uindex % 40) == 0) )
                            printf("\n                          : ");

                        printf("%02x ", table[uindex]);
                    }
                    printf("\n");
                }
                */
            }
        }
        if (uCode & 0x01)
        {
            pBmpImage->Longueur_Image = uNbrOfLine;
            pBmpImage->Taille_Image = uBitmapSize + pBmpImage->Offset_Image;
            pBmpImage->Taille_Map = uNbrOfLine * 160;
        }

        uBitmapSize += pBmpImage->Offset_Image;
    }

    return uBitmapSize;
}

/**
* @fn static unsigned int convert_to_16_pic( char *pOutputFileData, unsigned int uOutputFileSize, char *pInputFileData, unsigned int inputFileSize)
* @brief Convert a bmp with 4 bits by Pixel to a pic with 1 palette
*
* @param[in,out]    pOutputFileData
* @param[in]        uOutputFileSize
* @param[in]        pInputFileData
* @param[in]        inputFileSize
*
* @return size of data to save
*/
static unsigned int convert_to_16_pic( char *pOutputFileData, unsigned int uOutputFileSize, char *pInputFileData, unsigned int inputFileSize)
{
    FormatPIC          *pPicImage = NULL;
    FormatBMP          *pBmpImage = NULL;
    char               *pFun;
    char               *pDecompressedData = NULL;
    unsigned int        uDecompressedSize = 0;
    unsigned short int *pPicPalette;
    unsigned long int   uValueD = 0;
    unsigned int        iOffset = 0;
    unsigned int        uIndex = 0;
    unsigned int        uExtented = 0;
    unsigned int        uVarPicX = 0;
    int                 iVarBmpX;
    unsigned short int  uValueF = 0;
    unsigned char       uColorRed = 0;
    unsigned char       uColorGreen = 0;
    unsigned char       uColorBlue = 0;

    if ( (pInputFileData) && (inputFileSize) )
    {
        pBmpImage = (FormatBMP *)pInputFileData;

        if (pBmpImage->Type_Compression == 0)       // BI_RGB
        {
            pDecompressedData = pInputFileData;
            uDecompressedSize = inputFileSize;
        }
        else if (pBmpImage->Type_Compression == 2)  // BI_RLE4
        {
            pDecompressedData = calloc( 1, (size_t)( 98304));
            if (pDecompressedData)
            {
                // Copy the bmp header
                (void )memcpy( pDecompressedData, pInputFileData, pBmpImage->Offset_Image);
                ((FormatBMP *)pDecompressedData)->Type_Compression = 0;

                iOffset = bmp_rle4_decode( pDecompressedData, 98304, pBmpImage);
                if (iOffset)
                {
                    uDecompressedSize = iOffset;
                }
            }

            if (writeFileFromMemory( "encours.tmp.bmp", pDecompressedData, uDecompressedSize))
            {
                exitOnError( (char *)__FUNCTION__, __LINE__,(char*)"failed to write output file", NULL, "encours.tmp.bmp", 4);
            }

        }
        else
        {
            pDecompressedData = NULL;
            uDecompressedSize = 0;
        }
    }
    
    if ((pOutputFileData) && (uOutputFileSize > 0) && (pDecompressedData))
    {
        pPicImage = (FormatPIC *)pOutputFileData;

        pBmpImage = (FormatBMP *)pDecompressedData;

        if (pBmpImage->Longueur_Image > 199)
            uExtented = ((pBmpImage->Longueur_Image - 200) * 160);

        iVarBmpX = pBmpImage->Longueur_Image - 1;
        for (uVarPicX = 0; uVarPicX < pBmpImage->Longueur_Image; uVarPicX++)
        {
            (void )memcpy( pPicImage->MonImage[uVarPicX], pBmpImage->bitmap[iVarBmpX], 160);
            iVarBmpX--;
        }

        pFun = pOutputFileData + 0x7D00 + uExtented + 0xD0;
        (void )strncpy( pFun, "Frederic Mure", sizeof( "Frederic Mure"));
        pFun = pOutputFileData + 0x7D00 + uExtented + 0xE0;
        (void )strncpy( pFun, "and", sizeof("and"));
        pFun = pOutputFileData + 0x7D00 + uExtented + 0xF0;
        (void )strncpy( pFun, "Renaud Malaval", sizeof( "Renaud Malaval"));

        // std SCB = 200 octets and 56 octets of free space
        if (uExtented)
            // if extented the SCB = 400 and 112 octets (2 * 56) of free space
            uExtented += 56;

        pPicPalette = (unsigned short int *) ((char *)&pPicImage->Couleur_Palette_0 + uExtented);

        for (uIndex = 0; uIndex < 16; uIndex++)            // traitement des 16 couleurs de la palette
        {
            uValueD = pBmpImage->Couleur_Palette_0[uIndex];                   // 00 60 20 00 = 00206000     0=00 R=20 G=60 B=00
            uColorRed = (unsigned char)((uValueD & 0x00F00000) / 1048576);    // ne garde que le R et le décale en unitee
            uColorGreen = (unsigned char)((uValueD & 0x0000F000) / 4096);     // ne garde que le G et le décale en unitee
            uColorBlue = (unsigned char)((uValueD & 0x000000F0) / 16);        // ne garde que le B deja en unitee
            uValueF = (uColorRed * 256) + (uColorGreen * 16) + (uColorBlue);  // transformation de 00R0G0B0 en 0=0 R=2 G=6 B=0 Passage niveau couleur FF en F
            //pPicImage->Couleur_Palette_0[uIndex] = uValueF;
            *pPicPalette = uValueF;
            pPicPalette++;
        }

        iOffset = (unsigned int )sizeof( FormatPIC) + uExtented;

        pBmpImage = (FormatBMP *)pInputFileData;
        if (pBmpImage->Type_Compression == 2)
        {
            free( pDecompressedData);
            pDecompressedData = NULL;
        }
    }

    return iOffset;
}

/**
* @fn static unsigned int convert_to_256_pic( char *pOutputFileData, unsigned int uOutputFileSize, char *pInputFileData, unsigned int inputFileSize, char *pScbFileData)
* @brief Convert a bmp with 8 Bits by pixel 256 colors to a pic with 15 palettes
*
* @param[in,out]    pOutputFileData
* @param[in]        uOutputFileSize : size of pOutputFileData
* @param[in]        pInputFileData
* @param[in]        inputFileSize : size of pInputFileData
* @param[in]        pScbFileData : customize the palette go generate a pic file
*
* @return size of data to save
*/
static unsigned int convert_to_256_pic( char *pOutputFileData, unsigned int uOutputFileSize, char *pInputFileData, unsigned int inputFileSize, char *pScbFileData)
{
    FormatPIC           *pPicImage = NULL;
    FormatBMP256        *pBmpImage = NULL;
    CustomPaletteHeader *pScbHeader = NULL;
    CustomPalette       *pScbOngoin = NULL;
    unsigned short int  *pPicPalette;
    unsigned long int   *pBmpPalette;
    char                *pFun;
    unsigned int         uOffset = 0;
    int                  iVarPicX = 0;
    int                  iVarBmpX = 199;
    unsigned int         uIndex = 0;
    unsigned int         uLoop;
    unsigned long int    uValueD = 0;
    unsigned char        uColorRed = 0;
    unsigned char        uColorGreen = 0;
    unsigned char        uColorBlue = 0;
    BOOLEAN              bPallette = FALSE;

    if ((pOutputFileData) && (uOutputFileSize > 0) && (pInputFileData))
    {
        pBmpImage = (FormatBMP256 *)pInputFileData;

        pPicImage = (FormatPIC *)pOutputFileData;

        if (pScbFileData)
        {
            pScbHeader = (CustomPaletteHeader *)pScbFileData;
            pScbOngoin = (CustomPalette *)( pScbFileData + sizeof( CustomPaletteHeader));
        }

        // Do the job on the SCB to set the right palette for each line
        for (iVarPicX = 0; iVarPicX < 200; iVarPicX++)
        {
            uIndex = 0;
            pPicImage->SCB[iVarPicX] = 0;

            if (pScbOngoin)     // palette management if a file exist with extension is .scb
            {
                if ((iVarPicX >= pScbOngoin->uFromScbIndex) && (iVarPicX <= pScbOngoin->uToScbIndex))
                {
                    pPicImage->SCB[iVarPicX] = pScbOngoin->uPaletteIndex;
                }
            }

            for (uLoop = 0; uLoop < 160; uLoop++)
            {
                if ( (pScbOngoin == NULL) && (bPallette == FALSE) && ((pBmpImage->bitmap[iVarBmpX][uIndex] >> 4) > 0))
                {
                    pPicImage->SCB[iVarPicX] = pBmpImage->bitmap[iVarBmpX][uIndex] >> 4;    // x / 16
                    bPallette = TRUE;
                }

                // Copy the bit map
                // uOffet correction of the color index  from  .BMP 8 bit 0..255  to  .PIC only from 0..15
                uOffset = (pBmpImage->bitmap[iVarBmpX][uIndex] >> 4) << 4;              // (x / 16) * 16
                uColorRed = (pBmpImage->bitmap[iVarBmpX][uIndex++] - uOffset) << 4;     // 1er  pixel

                uOffset = (pBmpImage->bitmap[iVarBmpX][uIndex] >> 4) << 4;              // (x / 16) * 16
                uColorGreen = pBmpImage->bitmap[iVarBmpX][uIndex++] - uOffset;          // 2eme pixel

                pPicImage->MonImage[iVarPicX][uLoop] = (uColorRed + uColorGreen);
            }
            iVarBmpX--;
            bPallette = FALSE;
        }

        pPicImage->Libre[0] = '[';
        pFun = pPicImage->Libre + 3;
        (void)strncpy(pFun, "2023", sizeof("2023"));
        pFun = pPicImage->Libre + 8;
        (void)strncpy(pFun, "Frederic Mure", sizeof( "Frederic Mure"));
        pFun = pPicImage->Libre + 24;
        (void)strncpy(pFun, "ET", sizeof( "ET"));
        pFun = pPicImage->Libre + 40;
        (void)strncpy(pFun, "Renaud Malaval", sizeof( "Renaud Malaval"));
        pPicImage->Libre[55] = ']';

        pPicPalette = (unsigned short int *)&pPicImage->Couleur_Palette_0;
        pBmpPalette = (unsigned long int *)&pBmpImage->Couleur_Palettes;
        // creation de la palette BMP a partir des palettes IIGS
        for (uIndex = 0; uIndex < 256; uIndex++)            // traitement des 256 couleurs de la palette
        {
            uValueD = *pBmpPalette;

            uColorRed   = (unsigned char)(((uValueD & 0x00FF0000) >> 16) / 16);
            uColorGreen = (unsigned char)(((uValueD & 0x0000FF00) >> 8) / 16);
            uColorBlue  = (unsigned char)(( uValueD & 0x000000FF) / 16);
            *pPicPalette = (uColorRed << 8) + (uColorGreen << 4) + uColorBlue;

            pPicPalette++;
            pBmpPalette++;
        }

        uOffset = (unsigned int)sizeof( FormatPIC);
    }

    return uOffset;
}

/**
* @fn static unsigned int bmp_256_colors( char *pOutputFileData, unsigned int uOutputFileSize, char *pInputFileData, unsigned int inputFileSize)
* @brief convert pic 256 colors to bmp 256 colors
*
* @param[in,out]    pOutputFileData
* @param[in]        uOutputFileSize
* @param[in]        pInputFileData
* @param[in]        inputFileSize
*
* @return size of data to save
*/
static unsigned int bmp_256_colors(char *pOutputFileData, unsigned int uOutputFileSize, char *pInputFileData, unsigned int inputFileSize)
{
    FormatPIC          *pPicImage = NULL;
    FormatBMP256       *pBmpImage = NULL;
    unsigned short int *pPicPalette;
    unsigned long int  *pBmpPalette;
    int                 iVarPicX = 0;
    int                 iVarBmpX = 199;
    unsigned int        uOffset = 0;
    unsigned int        uIndex;
    unsigned int        uLoop;
    unsigned short int  uValueD;
    unsigned char       uColorRed;
    unsigned char       uColorGreen;
    unsigned char       uColorBlue;
    unsigned int        uPaletteOffset;

    if ((pOutputFileData) && (uOutputFileSize > 0) && (pInputFileData))
    {
        pPicImage = (FormatPIC *)pInputFileData;

        pBmpImage = (FormatBMP256 *)pOutputFileData;
        pBmpImage->Signature = 19778;
        pBmpImage->Taille_Image = sizeof( FormatBMP256);    // 65078
        pBmpImage->Offset_Image = sizeof( FormatBMP256) - sizeof( pBmpImage->bitmap);
        pBmpImage->Reserves = 1296452946;
        pBmpImage->Entete2 = 40;
        pBmpImage->Largeur_Image = 320;
        pBmpImage->Longueur_Image = 200;
        pBmpImage->Nbr_Plan = 1;
        pBmpImage->Nbr_Bit_Par_Pixel = 8;
        pBmpImage->Taille_Map = sizeof( pBmpImage->bitmap);
        pBmpImage->Resolution_H = 2835;
        pBmpImage->Resolution_V = 2835;

        for (iVarPicX = 0; iVarPicX < 200; iVarPicX++)
        {
            uIndex = 0;
            uPaletteOffset = pPicImage->SCB[iVarPicX] * 16;     // 16 colors in a table of int (16 bits)
            for (uLoop = 0; uLoop < 160; uLoop++)
            {
                pBmpImage->bitmap[iVarBmpX][uIndex++] = ((pPicImage->MonImage[iVarPicX][uLoop] & 0xF0) >> 4) + uPaletteOffset;
                pBmpImage->bitmap[iVarBmpX][uIndex++] = (pPicImage->MonImage[iVarPicX][uLoop] & 0x0F) + uPaletteOffset;
            }
            iVarBmpX--;
        }

        pPicPalette = (unsigned short int *)&pPicImage->Couleur_Palette_0;
        pBmpPalette = (unsigned long int *)&pBmpImage->Couleur_Palettes;
        // creation de la palette PIC and SCB à partir des couleurs de BMP
        for (uIndex = 0; uIndex < 256; uIndex++)            // traitement des 256 couleurs de la palette
        {
            uValueD = *pPicPalette;
            uColorRed = (unsigned char)((  uValueD & 0x0F00) >> 8) + (unsigned char)((uValueD & 0x0F00) >> 4);
            uColorGreen = (unsigned char)((uValueD & 0x00F0) >> 4) + (unsigned char)( uValueD & 0x00F0);
            uColorBlue = (unsigned char)(  uValueD & 0x000F)       + (unsigned char)((uValueD & 0x000F) << 4);

            *pBmpPalette = (uColorRed << 16) + (uColorGreen << 8) + uColorBlue;

            pPicPalette++;
            pBmpPalette++;
        }
        uOffset = (unsigned int)sizeof( FormatBMP256);
    }

    return uOffset;
}

/**
* @fn static unsigned int bmp_16_colors( char *pOutputFileData, unsigned int uOutputFileSize, char *pInputFileData, unsigned int inputFileSize)
* @brief convert pic 16 colors to bmp 16 colors
*
* @param[in,out]    pOutputFileData
* @param[in]        uOutputFileSize
* @param[in]        pInputFileData
* @param[in]        inputFileSize
*
* @return size of data to save
*/
static unsigned int bmp_16_colors(char *pOutputFileData, unsigned int uOutputFileSize, char *pInputFileData, unsigned int inputFileSize)
{
    FormatPIC          *pPicImage = NULL;
    FormatBMP          *pBmpImage = NULL;
    unsigned int        iOffset = 0;
    int                 iVarPicX = 0;
    int                 iVarBmpX = 199;
    unsigned int        uIndex = 0;
    unsigned short int  uValueD = 0;
    unsigned char       uColorRed = 0;
    unsigned char       uColorGreen = 0;
    unsigned char       uColorBlue = 0;

    if ((pOutputFileData) && (uOutputFileSize > 0) && (pInputFileData))
    {
        pPicImage = (FormatPIC *)pInputFileData;

        pBmpImage = (FormatBMP *)pOutputFileData;
        pBmpImage->Signature = 19778;
        pBmpImage->Taille_Image = sizeof( FormatBMP);    // 32118
        pBmpImage->Offset_Image = sizeof( FormatBMP) - sizeof(pBmpImage->bitmap);
        pBmpImage->Reserves = 1296452946;
        pBmpImage->Entete2 = 40;
        pBmpImage->Largeur_Image = 320;
        pBmpImage->Longueur_Image = 200;
        pBmpImage->Nbr_Plan = 1;
        pBmpImage->Nbr_Bit_Par_Pixel = 4;
        pBmpImage->Taille_Map = sizeof( pBmpImage->bitmap);
        pBmpImage->Resolution_H = 3780;
        pBmpImage->Resolution_V = 3780;

        for (iVarPicX = 0; iVarPicX < 200; iVarPicX++)
        {
            (void )memcpy( &pBmpImage->bitmap[iVarBmpX], &pPicImage->MonImage[iVarPicX], 160);
            iVarBmpX--;
        }

        // creation de la palette de 64 octets du PC contre 32 du IIGS
        for (uIndex = 0; uIndex < 16; uIndex++)                                 // traitement des 16 couleurs de la palette
        {
            uValueD = pPicImage->Couleur_Palette_0[uIndex];                     // 0x60 0x02 = 0x0260     0=0 R=2 G=6 B=0
            uColorRed = (unsigned char)((uValueD & 0x0F00) >> 8);               // ne garde que le R et le décale en unitee
            uColorGreen = (unsigned char)((uValueD & 0x00F0) >> 4);             // ne garde que le G et le décale en unitee
            uColorBlue = (unsigned char)(uValueD & 0x000F);                     // ne garde que le B deja en unitee

            // transformation de 0RGB en 0=00 R=20 G=60 B=00 Passage niveau couleur F en FF
            pBmpImage->Couleur_Palette_0[uIndex] = (uColorRed * 1048576) + (uColorGreen * 4096) + (uColorBlue * 16);
        }
        iOffset = (unsigned int)sizeof(FormatBMP);
    }

    return iOffset;
}

/**
* @fn static unsigned int countNumberOfScb( char *pInputFileData, unsigned int inputFileSize)
* @brief calcul where are the data to uncompress
*
* @param[in]        pInputFileData
* @param[in]        inputFileSize
*
* @return number of SCB
*/
static unsigned int countNumberOfScb( char *pInputFileData, unsigned int inputFileSize)
{
    char           *pInputRunner;
    char           *pScb;
    unsigned int    uScbNumber = 0;
    unsigned int    uPalette = 0;
    unsigned int    uIndex;

    if (pInputFileData)
    {
        pInputRunner = pInputFileData + (200 * 160);

        uPalette = *pInputRunner;
        pScb = pInputRunner;
        for (uIndex = 0; uIndex < 200; uIndex++)
        {
            if ((uPalette != *pInputRunner) || (uIndex == 199))
            {
                uScbNumber++;
                uPalette = *pInputRunner;
            }
            pInputRunner++;
        }
    }

    return uScbNumber;
}

/**
* @fn static void picColorsUsage( char *pInputFileData, unsigned int inputFileSize)
* @brief calcul where are the data to uncompress
*
* @param[in]        pInputFileData
* @param[in]        inputFileSize
*
*/
static void picColorsUsage( char *pInputFileData, unsigned int inputFileSize)
{
    FormatPIC          *pPicImage = NULL;
    int                 iVarPicX = 0;
    int                 iVarBmpX = 199;
    unsigned int        uIndex;
    unsigned int        uLoop;
    unsigned int        uPaletteOffset = 0xFFFF;
    unsigned short int  uIndexLessUsed[256] = { 0 };

    pPicImage = (FormatPIC *)pInputFileData;

    for (iVarPicX = 0; iVarPicX < 200; iVarPicX++)
    {
        uIndex = 0;
        if (uPaletteOffset != (pPicImage->SCB[iVarPicX] * 16))
        {
            uPaletteOffset = pPicImage->SCB[iVarPicX] * 16;     // 16 colors in a table of int (16 bits)
            for (uLoop = 0; uLoop < 160; uLoop++)
            {
                uIndexLessUsed[((pPicImage->MonImage[iVarPicX][uLoop] & 0xF0) >> 4) + uPaletteOffset] += 1;
                uIndexLessUsed[(pPicImage->MonImage[iVarPicX][uLoop] & 0x0F) + uPaletteOffset] += 1;
            }
        }
        iVarBmpX--;
    }

    return;
}

/**
* @fn static unsigned int convert_to_bmp( char *pOutputFileData, unsigned int uOutputFileSize, char *pInputFileData, unsigned int inputFileSize)
* @brief Convert a pic to bmp 4 or 8 bits per pixel
*
* @param[in,out]    pOutputFileData
* @param[in]        uOutputFileSize
* @param[in]        pInputFileData
* @param[in]        inputFileSize
*
* @return return offset of data to decompress
*/
static unsigned int convert_to_bmp( char *pOutputFileData, unsigned int uOutputFileSize, char *pInputFileData, unsigned int inputFileSize)
{
    unsigned int    uScbNumber = 0;
    unsigned int    iOffset = 0;

    if ((pOutputFileData) && (uOutputFileSize > 0) && (pInputFileData))
    {
        uScbNumber = countNumberOfScb( pInputFileData, inputFileSize);

        if (uScbNumber == 1)
        {
            iOffset = bmp_16_colors( pOutputFileData, uOutputFileSize, pInputFileData, inputFileSize);
        }
        else
        {
            picColorsUsage( pInputFileData, inputFileSize);
            iOffset = bmp_256_colors( pOutputFileData, uOutputFileSize, pInputFileData, inputFileSize);
        }
    }

    return iOffset;
}

/**
* @fn void doDumpBmp( char *pFilePathname, char *pInputFileData, unsigned int inputFileSize)
* @brief parse pInputFileData to display BMP header
*
* @param[in]        pFilePathname
* @param[in]        pInputFileData
* @param[in]        inputFileSize
*
*/
void doDumpBmp( char *pFilePathname, char *pInputFileData, unsigned int inputFileSize)
{
    FormatBMP       *pBmpImage;

    char            *pCompressionList[] = { "BI_RGB", "BI_RLE8", "BI_RLE4", "BI_BITFIELDS", "BI_JPEG", "BI_PNG", "BI_ALPHABITFIELDS", "BI_CMYK", "BI_CMYKRLE8", "BI_CMYKRLE4" };
    char            *pCompressionInfo;
    /*
    * from : https://en.wikipedia.org/wiki/BMP_file_format
        0	"BI_RGB", 
        1	"BI_RLE8"
        2	"BI_RLE4"
        3	"BI_BITFIELDS"
        4	"BI_JPEG"
        5	"BI_PNG"
        6	"BI_ALPHABITFIELDS"
        11	"BI_CMYK"
        12	"BI_CMYKRLE8"
        13	"BI_CMYKRLE4"
    */
    unsigned int uIndex;
    unsigned int uBorne = 0;
    unsigned int uCounter = 0;

    if ((pInputFileData) && (pFilePathname) && (inputFileSize > sizeof( FormatBMP)))
    {
        pBmpImage = (FormatBMP *)pInputFileData;
        printf( "\nDisplay content of bmp header\n\n");
        printf( " %s\n\n", pFilePathname);

        printf( "Signature          = %c%c\n", (char)(pBmpImage->Signature & 0x00FF), (char)((pBmpImage->Signature & 0xFF00) >> 8));
        printf( "Taille_Image       = %ld\n", pBmpImage->Taille_Image);
        printf( "Offset_Image       = %ld\n", pBmpImage->Offset_Image);
        if (pBmpImage->Reserves != 0)
            printf( "Reserves           = %c%c%c%c\n",
                (char)(pBmpImage->Reserves & 0x000000FF), (char)((pBmpImage->Reserves & 0x0000FF00) >> 8),
                (char)((pBmpImage->Reserves & 0x00FF0000) >> 16), (char)((pBmpImage->Reserves & 0xFF000000) >> 24));
        printf( "Entete2            = %ld\n", pBmpImage->Entete2);
        printf( "Largeur_Image      = %ld\n", pBmpImage->Largeur_Image);
        printf( "Longueur_Image     = %ld\n", pBmpImage->Longueur_Image);
        printf( "Nbr_Plan           = %ld\n", pBmpImage->Nbr_Plan);
        printf( "Nbr_Bit_Par_Pixel  = %ld\n", pBmpImage->Nbr_Bit_Par_Pixel);

        if ((pBmpImage->Type_Compression >= 0) && (pBmpImage->Type_Compression <= 7))
        {
            pCompressionInfo = pCompressionList[ (unsigned char)pBmpImage->Type_Compression];
        }
        else if ((pBmpImage->Type_Compression > 10) && (pBmpImage->Type_Compression < 14))
        {
            pCompressionInfo = pCompressionList[(unsigned char)pBmpImage->Type_Compression - 4];
        }
        else
        {
            pCompressionInfo = NULL;
        }

        if (pCompressionInfo)
            printf( "Type_Compression   = %ld\t%s\n", pBmpImage->Type_Compression, pCompressionInfo);
        else
            printf( "Type_Compression   = %ld\n", pBmpImage->Type_Compression);

        printf( "Taille_Map         = %ld\n", pBmpImage->Taille_Map);
        printf( "Resolution_H       = %ld\n", pBmpImage->Resolution_H);
        printf( "Resolution_V       = %ld\n", pBmpImage->Resolution_V);
        printf("\n");

        if (pBmpImage->Nbr_Bit_Par_Pixel == 8)
        {
            FormatBMP256 *pBmpIn256ColorsImage = (FormatBMP256 *)pInputFileData;
            printf("La palette de 256 couleurs\n");
            for (uIndex = 0; uIndex < 256; uIndex++)
            {
                if (uIndex == uBorne)
                    printf("#%02u = %02X %02X %02X %02X ", uCounter,
                        (unsigned char)(pBmpIn256ColorsImage->Couleur_Palettes[uIndex] & 0x000000FF), (unsigned char)((pBmpIn256ColorsImage->Couleur_Palettes[uIndex] & 0x0000FF00) >> 8),
                        (unsigned char)((pBmpIn256ColorsImage->Couleur_Palettes[uIndex] & 0x00FF0000) >> 16), (unsigned char)((pBmpIn256ColorsImage->Couleur_Palettes[uIndex] & 0xFF000000) >> 24));
                else
                    printf("%02X %02X %02X %02X ",
                        (unsigned char)(pBmpIn256ColorsImage->Couleur_Palettes[uIndex] & 0x000000FF), (unsigned char)((pBmpIn256ColorsImage->Couleur_Palettes[uIndex] & 0x0000FF00) >> 8),
                        (unsigned char)((pBmpIn256ColorsImage->Couleur_Palettes[uIndex] & 0x00FF0000) >> 16), (unsigned char)((pBmpIn256ColorsImage->Couleur_Palettes[uIndex] & 0xFF000000) >> 24));
                
                if (uIndex == (uBorne + 15))
                {
                    printf("\n");
                    uBorne += 16;
                    uCounter++;
                }
            }
        }
        else
        {
            printf("La palette de 16 couleurs\n");
            for (uIndex = 0; uIndex < 16; uIndex++)
            {
                if (uIndex == uBorne)
                    printf("#01 = %02X %02X %02X %02X ",
                        (unsigned char)(pBmpImage->Couleur_Palette_0[uIndex] & 0x000000FF), (unsigned char)((pBmpImage->Couleur_Palette_0[uIndex] & 0x0000FF00) >> 8),
                        (unsigned char)((pBmpImage->Couleur_Palette_0[uIndex] & 0x00FF0000) >> 16), (unsigned char)((pBmpImage->Couleur_Palette_0[uIndex] & 0xFF000000) >> 24));
                else
                    printf("%02X %02X %02X %02X ",
                        (unsigned char)(pBmpImage->Couleur_Palette_0[uIndex] & 0x000000FF), (unsigned char)((pBmpImage->Couleur_Palette_0[uIndex] & 0x0000FF00) >> 8),
                        (unsigned char)((pBmpImage->Couleur_Palette_0[uIndex] & 0x00FF0000) >> 16), (unsigned char)((pBmpImage->Couleur_Palette_0[uIndex] & 0xFF000000) >> 24));
            }
            printf("\n");
        }
        printf( "\n");
    }

    return;
}

/**
* @fn char *DoBmpJob( char *pInputFileData, unsigned int inputFileSize, unsigned int command, unsigned int *pDataSize, char *pScbFileData)
* @brief parse pInputFileData of inputFileSize size following command, and return the data size of pOutputFileData in pDataSize
*
* @param[in]        pInputFileData
* @param[in]        inputFileSize
* @param[in]        command
* @param[in,out]    pDataSize : size of data to save
* @param[in]        pScbFileData : customize the palette go generate a pic file
*
* @return A new pointer pOutputFileData
*/
char *DoBmpJob( char *pInputFileData, unsigned int inputFileSize, unsigned int command, unsigned int *pDataSize, char *pScbFileData)
{
    char        *pOutputFileData = NULL;
    char        *pInputRunner = NULL;
    FormatBMP   *pBmpImage;

    *pDataSize = 0;
    pOutputFileData = calloc( 1, (size_t )(98304));
    if ( (pOutputFileData) && (pInputFileData) )
    {
        pInputRunner = pInputFileData;

        switch (command)
        {
            case eTO_BMP:
            {
                *pDataSize = convert_to_bmp(pOutputFileData, (unsigned int)98304, pInputFileData, inputFileSize);
            }
            break;

            case eTO_PIC:
            {
                pBmpImage = (FormatBMP *)pInputFileData;
            
                if (pBmpImage->Nbr_Bit_Par_Pixel == 4)
                {
                    *pDataSize = convert_to_16_pic( pOutputFileData, (unsigned int)98304, pInputFileData, inputFileSize);
                }
                else if (pBmpImage->Nbr_Bit_Par_Pixel == 8)
                {
                    *pDataSize = convert_to_256_pic( pOutputFileData, (unsigned int)98304, pInputFileData, inputFileSize, pScbFileData);
                }
                else
                {
                    free( pOutputFileData);
                    pOutputFileData = NULL;
                }
            }
            break;

            default:
                break;
        }
    }

    return pOutputFileData;
}

/**
* @fn char *DoAddPaletteToBmp( char *pInputFileData, unsigned int inputFileSize, unsigned int command, unsigned int *pDataSize)
* @brief parse pInputFileData of inputFileSize size following command, and return the data size of pOutputFileData in pDataSize
*
* @param[in]        pInputFileData
* @param[in]        inputFileSize
* @param[in]        command
* @param[in,out]    pDataSize size of data to save
*
* @return A new pointer pOutputFileData
*/
char *DoAddPaletteToBmp( char *pInputFileData, unsigned int inputFileSize, unsigned int command, unsigned int *pDataSize)
{
    char           *pOutputFileData = NULL;
    FormatBMP      *pBmpIn16ColorsImage = NULL;
    FormatBMP256   *pBmpIn256ColorsImage = NULL;
    FormatBMP256   *pBmp8Image;
    char           *pInputRunner = NULL;
    char           *pOutputRunner = NULL;
    unsigned int    uIndex;

    *pDataSize = 0;

    pOutputFileData = calloc( 1, (size_t )(98304));
    if ((pOutputFileData) && (pInputFileData))
    {
        pBmpIn16ColorsImage = (FormatBMP *)pInputFileData;

        pBmp8Image = (FormatBMP256 *)pOutputFileData;
        pBmp8Image->Signature = 19778;
        pBmp8Image->Taille_Image = sizeof( FormatBMP256);    // 65078
        pBmp8Image->Reserves = 1296452946;
        pBmp8Image->Offset_Image = sizeof( FormatBMP256) - sizeof( pBmp8Image->bitmap);
        pBmp8Image->Nbr_Plan = 1;
        pBmp8Image->Nbr_Bit_Par_Pixel = 8;
        pBmp8Image->Taille_Map = sizeof( pBmp8Image->bitmap);
        pBmp8Image->Resolution_H = 2835;
        pBmp8Image->Resolution_V = 2835;

        if (pBmpIn16ColorsImage->Nbr_Bit_Par_Pixel == 4)
        {
            pBmp8Image->Entete2 = pBmpIn16ColorsImage->Entete2;
            pBmp8Image->Largeur_Image = pBmpIn16ColorsImage->Largeur_Image;
            pBmp8Image->Longueur_Image = pBmpIn16ColorsImage->Longueur_Image;

            // copy the palette 
            (void)memcpy( (char *)&pBmp8Image->Couleur_Palettes[16], (char *)pBmpIn16ColorsImage->Couleur_Palette_0, sizeof( pBmpIn16ColorsImage->Couleur_Palette_0));

            // copy and update the bitmap 4 bits per pixel to 8 bits per pixel to 
            pInputRunner = pInputFileData + pBmpIn16ColorsImage->Offset_Image;
            pOutputRunner = pOutputFileData + pBmp8Image->Offset_Image;
            for (uIndex = 0; uIndex < sizeof( pBmpIn16ColorsImage->bitmap); uIndex++)
            {
                *pOutputRunner++ = ((*pInputRunner & 0xF0) >> 4) + 16;
                *pOutputRunner++ = (*pInputRunner & 0x0F) + 16;

                pInputRunner++;
            }
            *pDataSize = (unsigned int)sizeof( FormatBMP256);
        }
        else
        {
            pBmpIn16ColorsImage = NULL;
            pBmpIn256ColorsImage = (FormatBMP256 *)pInputFileData;

            if (pBmpIn256ColorsImage->Nbr_Bit_Par_Pixel == 8)
            {
                pBmp8Image->Entete2 = pBmpIn256ColorsImage->Entete2;
                pBmp8Image->Largeur_Image = pBmpIn256ColorsImage->Largeur_Image;
                pBmp8Image->Longueur_Image = pBmpIn256ColorsImage->Longueur_Image;

                // copy the palette 
                (void )memcpy( (char *)&pBmp8Image->Couleur_Palettes[16], (char *)&pBmpIn256ColorsImage->Couleur_Palettes[0], sizeof(pBmpIn256ColorsImage->Couleur_Palettes) - sizeof(pBmpIn16ColorsImage->Couleur_Palette_0));

                pInputRunner = pInputFileData + pBmpIn256ColorsImage->Offset_Image;
                pOutputRunner = pOutputFileData + pBmp8Image->Offset_Image;

                // copy and update the bitmap 4 bits per pixel to 8 bits per pixel to 
                for (uIndex = 0; uIndex < sizeof(pBmpIn256ColorsImage->bitmap); uIndex++)
                {
                    *pOutputRunner = *pInputRunner + 16;

                    pOutputRunner++;
                    pInputRunner++;
                }
                *pDataSize = (unsigned int)sizeof( FormatBMP256);
            }
            else
            {
                free( pOutputFileData);
                pOutputFileData = NULL;
            }
        }
    }

    return pOutputFileData;
}

/**
* @fn void DoInsertPaletteToBmp( char *pInputFileData, unsigned int uInputFileSize, char **pOutputFileData, unsigned int uOutputFileSize, unsigned int uFrom, unsigned int uTo)
* @brief parse pInputFileData of inputFileSize size following command, and return the data size of pOutputFileData in pDataSize
*
* @param[in]        pInputFileData
* @param[in]        uInputFileSize
* @param[in,out]    pOutputFileData
* @param[in,out]    uOutputFileSize
* @param[in]        uFrom
* @param[in]        uTo
* 
*/
void DoInsertPaletteToBmp( char *pInputFileData, unsigned int uInputFileSize, char **pOutputFileData, unsigned int *puOutputFileSize, unsigned int uFrom, unsigned int uTo)
{
    FormatBMP       *pBmpIn16ColorsImage = NULL;
    FormatBMP256    *pBmpIn256ColorsImage = NULL;
    FormatBMP256    *pBmp8Image;
    char            *pInputRunner = NULL;
    char            *pOutputRunner = NULL;
    unsigned int     uIndex;

    if ((pInputFileData) && ((pOutputFileData) && (*pOutputFileData)))
    {
        pBmpIn16ColorsImage = (FormatBMP *)*pOutputFileData;
        if (pBmpIn16ColorsImage->Nbr_Bit_Par_Pixel == 4)
        {
            // convert to 8 bits per pixel bmp 256 colors
            pBmp8Image = calloc(1, (size_t)(98304));
            if (pBmp8Image)
            {
                pBmp8Image->Signature = pBmpIn16ColorsImage->Signature;
                pBmp8Image->Taille_Image = sizeof( FormatBMP256);    // 65078
                pBmp8Image->Reserves = 1296452946;
                pBmp8Image->Offset_Image = sizeof( FormatBMP256) - sizeof( pBmp8Image->bitmap);
                pBmp8Image->Entete2 = pBmpIn16ColorsImage->Entete2;
                pBmp8Image->Largeur_Image = pBmpIn16ColorsImage->Largeur_Image;
                pBmp8Image->Longueur_Image = pBmpIn16ColorsImage->Longueur_Image;
                pBmp8Image->Nbr_Plan = pBmpIn16ColorsImage->Nbr_Plan;
                pBmp8Image->Nbr_Bit_Par_Pixel = 8;
                pBmp8Image->Taille_Map = sizeof( pBmp8Image->bitmap);
                pBmp8Image->Resolution_H = 2835;
                pBmp8Image->Resolution_V = 2835;

                // copy the palette
                (void )memcpy((char *)pBmp8Image->Couleur_Palettes, (char *)pBmpIn16ColorsImage->Couleur_Palette_0, sizeof( pBmpIn16ColorsImage->Couleur_Palette_0));

                // copy the bitmap 4 bits per pixel to 8 bits per pixel
                pInputRunner = (char *)pBmpIn16ColorsImage + pBmpIn16ColorsImage->Offset_Image;
                pOutputRunner = (char *)pBmp8Image + pBmp8Image->Offset_Image;
                for (uIndex = 0; uIndex < sizeof( pBmpIn16ColorsImage->bitmap); uIndex++)
                {
                    *pOutputRunner++ = ((*pInputRunner & 0xF0) >> 4);
                    *pOutputRunner++ = (*pInputRunner & 0x0F);

                    pInputRunner++;
                }

                *puOutputFileSize = (unsigned int )sizeof( FormatBMP256);
                free( *pOutputFileData);
                *pOutputFileData = (char *)pBmp8Image;
                pBmp8Image = NULL;
                pBmpIn16ColorsImage = NULL;
            }
        }

        if ((pOutputFileData) && (*pOutputFileData))
        {
            pBmpIn16ColorsImage = (FormatBMP *)pInputFileData;
            if (pBmpIn16ColorsImage->Nbr_Bit_Par_Pixel == 4)
            {
                pInputRunner = (char *)&pBmpIn16ColorsImage->Couleur_Palette_0[0];
                pBmpIn256ColorsImage = (FormatBMP256 *)*pOutputFileData;
                pOutputRunner = (char *)&pBmpIn256ColorsImage->Couleur_Palettes[uTo * 16];
            }
            else
            {
                pBmpIn256ColorsImage = (FormatBMP256 *)pInputFileData;
                pInputRunner = (char *)&pBmpIn256ColorsImage->Couleur_Palettes[uFrom * 16];

                pBmpIn256ColorsImage = (FormatBMP256 *)*pOutputFileData;
                pOutputRunner = (char *)&pBmpIn256ColorsImage->Couleur_Palettes[uTo * 16];
            }
            (void )memcpy( pOutputRunner, pInputRunner, sizeof( unsigned long int) * 16);
        }
    }

    return;
}


/**
* @fn BOOL CheckBmpFileFormat( char *pInputFileData, unsigned int inputFileSize)
* @brief Validate the format of a bmp file
*
* @param[in]        pInputFileData
* @param[in]        inputFileSize
*
* @return FALSE if is ok or TRUE if problem
*/
BOOL CheckBmpFileFormat( char *pInputFileData, unsigned int inputFileSize)
{
    FormatBMP *pBmpStruct = NULL;
    BOOL       bResult = FALSE;

    if ((pInputFileData) && (inputFileSize > 0))
    {
#ifdef _DEBUG
        doDumpBmp( "encours", pInputFileData, inputFileSize);
#endif
        pBmpStruct = (FormatBMP *)pInputFileData;

        if (pBmpStruct->Signature == 19778)
        {
            if ( (pBmpStruct->Taille_Image == sizeof( FormatBMP)) || (pBmpStruct->Taille_Image == sizeof( FormatBMP256)) || (pBmpStruct->Taille_Image <= 0xFFFF))
            {
                if (pBmpStruct->Largeur_Image == 320)
                {
                    if (pBmpStruct->Longueur_Image <= 400)
                    {
                        if ( (pBmpStruct->Nbr_Bit_Par_Pixel == 4) || (pBmpStruct->Nbr_Bit_Par_Pixel == 8) )
                        {
                            if ( (pBmpStruct->Taille_Map == 32000) || (pBmpStruct->Taille_Map == 64000) || (pBmpStruct->Type_Compression == 2) )
                            {
                                if ( (pBmpStruct->Offset_Image == 118) || (pBmpStruct->Offset_Image == 1078) /* || (pBmpStruct->Offset_Image == 1162) */ )
                                {
                                    bResult = TRUE;
                                }
                                else
                                {
                                    (void )printf( "BMP : Offset_Image is wrong\n");
                                }
                            }
                            else
                            {
                                (void )printf( "BMP : Taille_Map is wrong\n");
                            }
                        }
                        else
                        {
                            (void )printf( "BMP : Nbr_Bit_Par_Pixel is wrong\n");
                        }
                    }
                    else
                    {
                        (void )printf( "BMP : Longueur_Image is wrong\n");
                    }
                }
                else
                {
                    (void )printf( "BMP : Largeur_Image is wrong\n");
                }
            }
            else
            {
                (void )printf( "BMP : Taille_Image is wrong\n");
            }
        }
        else
        {
            (void )printf( "BMP : Signature is wrong\n");
        }
    }

    return bResult;
}

/**
* @fn char *DoSwapColor( char *pInputFileData, unsigned int inputFileSize, unsigned int uSwapColumnA, unsigned int uSwapColumnB, unsigned int *pDataSize)
* @brief swap palette color for index uSwapColumnA and uSwapColumnB; update color index in bitmap too
*
* @param[in]        pInputFileData
* @param[in]        inputFileSize
* @param[in]        uSwapColumnA
* @param[in]        uSwapColumnB
* @param[in,out]    pDataSize size of data to save
*
* @return A new pointer pOutputFileData
*/
char *DoSwapColor( char *pInputFileData, unsigned int inputFileSize, unsigned int uSwapColumnA, unsigned int uSwapColumnB, unsigned int *pDataSize)
{
    char                *pOutputFileData = NULL;
    FormatBMP           *pBmpIn16ColorsImage = NULL;
    FormatBMP256        *pBmpIn256ColorsImage = NULL;
    unsigned char       *pInputRunner = NULL;
    unsigned char       *pOutputRunner = NULL;
    unsigned long int    uTempValue;
    unsigned char        cHighPixel;
    unsigned char        cLowPixel;
    BOOL                 bWorkDone = FALSE;

    *pDataSize = 0;

    pOutputFileData = calloc( 1, (size_t)(98304));
    if ((pOutputFileData) && (pInputFileData))
    {
        pBmpIn16ColorsImage = (FormatBMP *)pInputFileData;

        if (pBmpIn16ColorsImage->Nbr_Bit_Par_Pixel == 4)
        {
            if ((uSwapColumnA < 16) && (uSwapColumnB < 16))
            {
                (void )memcpy( pOutputFileData, pInputFileData, inputFileSize);
            }
        }
        else
        {
            pBmpIn16ColorsImage = NULL;
            pBmpIn256ColorsImage = (FormatBMP256 *)pInputFileData;

            if (pBmpIn256ColorsImage->Nbr_Bit_Par_Pixel == 8)
            {
                (void )memcpy( pOutputFileData, pInputFileData, inputFileSize);
            }
        }

        if (*pOutputFileData != 0)
        {
            if (pBmpIn16ColorsImage)
            {
                pBmpIn16ColorsImage = (FormatBMP *)pOutputFileData;
                pBmpIn16ColorsImage->Reserves = 1296452946;
                // swap color in the palette
                if ((uSwapColumnA < 16) && (uSwapColumnB < 16))     // test to avoid warning
                {
                    uTempValue = pBmpIn16ColorsImage->Couleur_Palette_0[uSwapColumnA];
                    pBmpIn16ColorsImage->Couleur_Palette_0[uSwapColumnA] = pBmpIn16ColorsImage->Couleur_Palette_0[uSwapColumnB];
                    pBmpIn16ColorsImage->Couleur_Palette_0[uSwapColumnB] = uTempValue;
                }

                // swap index color in bitmap
                pInputRunner = pOutputFileData + pBmpIn16ColorsImage->Offset_Image;
                pOutputRunner = pInputRunner + sizeof( pBmpIn16ColorsImage->bitmap);
                while (pInputRunner <= pOutputRunner)
                {
                    if ((*pInputRunner & 0xF0) == (unsigned char)(uSwapColumnA << 4))
                    {
                        cHighPixel = uSwapColumnB << 4;
                        bWorkDone = TRUE;
                    }
                    else if ((*pInputRunner & 0xF0) == (uSwapColumnB << 4))
                    {
                        cHighPixel = uSwapColumnA << 4;
                        bWorkDone = TRUE;
                    }
                    else
                    {
                        cHighPixel = (*pInputRunner & 0xF0);
                    }

                    if ((*pInputRunner & 0x0F) == uSwapColumnA)
                    {
                        cLowPixel = uSwapColumnB;
                        bWorkDone = TRUE;
                    }
                    else if ((*pInputRunner & 0x0F) == uSwapColumnB)
                    {
                        cLowPixel = uSwapColumnA;
                        bWorkDone = TRUE;
                    }
                    else
                    {
                        cLowPixel = (*pInputRunner & 0x0F);
                    }

                    *pInputRunner = cHighPixel + cLowPixel;
                    pInputRunner++;
                }
            }
            else
            {
                pBmpIn256ColorsImage = (FormatBMP256 *)pOutputFileData;
                pBmpIn256ColorsImage->Reserves = 1296452946;
                // swap color in the palette
                uTempValue = pBmpIn256ColorsImage->Couleur_Palettes[uSwapColumnA];
                pBmpIn256ColorsImage->Couleur_Palettes[uSwapColumnA] = pBmpIn256ColorsImage->Couleur_Palettes[uSwapColumnB];
                pBmpIn256ColorsImage->Couleur_Palettes[uSwapColumnB] = uTempValue;

                // swap index color in bitmap
                pInputRunner = pOutputFileData + pBmpIn256ColorsImage->Offset_Image;
                pOutputRunner = pInputRunner + sizeof( pBmpIn256ColorsImage->bitmap);
                while (pInputRunner <= pOutputRunner)
                {
                    if (*pInputRunner == uSwapColumnA)
                    {
                        *pInputRunner = uSwapColumnB;
                        bWorkDone = TRUE;
                    }
                    else if (*pInputRunner == uSwapColumnB)
                    {
                        *pInputRunner = uSwapColumnA;
                        bWorkDone = TRUE;
                    }
                    pInputRunner++;
                }
            }

            if (bWorkDone)
            {
                *pDataSize = inputFileSize;
            }
        }
        else
        {
            free( pOutputFileData);
            pOutputFileData = NULL;
        }
    }

    return pOutputFileData;
}

/**
* @fn void DoComparePalette( char *pFirstFileData, unsigned int uFirstFileSize, char *pSecondFileData, unsigned int uSecondFileSize)
* @brief Compare palette of bmp with 16 colors (4 bit per pixel) and show differences
*
* @param[in]        pFirstFileData
* @param[in]        uFirstFileSize
* @param[in]        pSecondFileData
* @param[in]        uSecondFileSize
*
*/
void DoComparePalette( char *pFirstFileData, unsigned int uFirstFileSize, char *pSecondFileData, unsigned int uSecondFileSize)
{
    FormatBMP       *pBmpIn16ColorsImageOne = NULL;
    FormatBMP       *pBmpIn16ColorsImageTwo = NULL;
    unsigned int     uIndex;

    if ((pFirstFileData) && (pSecondFileData) && (uFirstFileSize) && (uSecondFileSize))
    {
        pBmpIn16ColorsImageOne = (FormatBMP *)pFirstFileData;
        pBmpIn16ColorsImageTwo = (FormatBMP *)pSecondFileData;

        for (uIndex = 0; uIndex < 16; uIndex++)
        {
            if (pBmpIn16ColorsImageOne->Couleur_Palette_0[uIndex] != pBmpIn16ColorsImageTwo->Couleur_Palette_0[uIndex])
            {
                (void )printf( "Color at index %02d are different : R=%03d G=%03d B=%03d  !=  R=%03d G=%03d B=%03d\n", uIndex,
                    ((pBmpIn16ColorsImageOne->Couleur_Palette_0[uIndex] & 0x00FF0000) >> 16),
                    ((pBmpIn16ColorsImageOne->Couleur_Palette_0[uIndex] & 0x0000FF00) >> 8),
                     (pBmpIn16ColorsImageOne->Couleur_Palette_0[uIndex] & 0x000000FF),
                    ((pBmpIn16ColorsImageTwo->Couleur_Palette_0[uIndex] & 0x00FF0000) >> 16),
                    ((pBmpIn16ColorsImageTwo->Couleur_Palette_0[uIndex] & 0x0000FF00) >> 8),
                     (pBmpIn16ColorsImageTwo->Couleur_Palette_0[uIndex] & 0x000000FF)
                    );
            }
        }
    }
}

/**
* @fn void DoCopyPalette( char *pFirstFileData, unsigned int uFirstFileSize, char *pSecondFileData, unsigned int uSecondFileSize, unsigned int uBitPerPixels)
* @brief copy palette of bmp with 16 colors (4 bits per pixel) from pFirstFileData to pSecondFileData
*
* @param[in]        pFirstFileData
* @param[in]        uFirstFileSize
* @param[in]        pSecondFileData
* @param[in]        uSecondFileSize
* @param[in]        uBitPerPixels       : 4 for 16 colors (4 bits per pixel) or 8 for 256 colors (8 bits per pixel)
*/
void DoCopyPalette( char *pFirstFileData, unsigned int uFirstFileSize, char *pSecondFileData, unsigned int uSecondFileSize, unsigned int uBitPerPixels)
{
    FormatBMP       *pBmpIn16ColorsImageOne = NULL;
    FormatBMP       *pBmpIn16ColorsImageTwo = NULL;
    FormatBMP256    *pBmpIn256ColorsImageOne = NULL;
    FormatBMP256    *pBmpIn256ColorsImageTwo = NULL;
    unsigned int     uIndex;

    if ((pFirstFileData) && (pSecondFileData) && (uFirstFileSize) && (uSecondFileSize))
    {
        if (uBitPerPixels == 4)
        {
            pBmpIn16ColorsImageOne = (FormatBMP *)pFirstFileData;
            pBmpIn16ColorsImageTwo = (FormatBMP *)pSecondFileData;

            for (uIndex = 0; uIndex < 16; uIndex++)
            {
                pBmpIn16ColorsImageTwo->Couleur_Palette_0[uIndex] = pBmpIn16ColorsImageOne->Couleur_Palette_0[uIndex];
            }
        }
        else if (uBitPerPixels == 8)
        {
            pBmpIn256ColorsImageOne = (FormatBMP256 *)pFirstFileData;
            pBmpIn256ColorsImageTwo = (FormatBMP256 *)pSecondFileData;

            for (uIndex = 0; uIndex < 16; uIndex++)
            {
                pBmpIn256ColorsImageTwo->Couleur_Palettes[uIndex] = pBmpIn256ColorsImageOne->Couleur_Palettes[uIndex];
            }
        }
    }
}

/**
* @fn char *DoExtractSprite( char *pInputFileData, unsigned int inputFileSize, ConvmArguments *pTheSprite, unsigned int *pDataSize)
* @brief extract eprite from a bmp to text file 
*
* @param[in]        pInputFileData
* @param[in]        inputFileSize
* @param[in]        pTheSprite   : uFromX, uFromY, uNumerOfLine, uLargeur, uHauteur
* @param[in]        uFromY       : position Y of the 1st sprite
* @param[in]        uNumerOfLine : total number of line to extract
* @param[in]        uLargeur     : of the sprite
* @param[in,out]    pDataSize size of data to save
*
* @return A new pointer pOutputFileData
*/
char *DoExtractSprite( char *pInputFileData, unsigned int inputFileSize, tConvmArguments *pTheSprite, unsigned int *pDataSize)
{
    char                *pOutputFileData = NULL;
    FormatBMP           *pBmpIn16ColorsImage = NULL;
    FormatBMP256        *pBmpIn256ColorsImage = NULL;
    char                 pTemp[64] = {0};
    static const char   *pSpriteBeginLineStr = "                    dc  i'";
    static const char   *pSpriteEndStr       = "                    dc  i'2,6'";
    int                  iVarBmpY = 199 - pTheSprite->uSwapColumnB;
    int                  iVarBmpX = pTheSprite->uSwapColumnA;
    unsigned int         uVarPicX = 0;
    unsigned int         uJndex;
    unsigned int         uIndex;
    unsigned int         uLoop;
    unsigned int         curl = 0;

    *pDataSize = 0;

    pOutputFileData = calloc( 1, (size_t )(98304));
    if ((pOutputFileData) && (pInputFileData))
    {
        pBmpIn16ColorsImage = (FormatBMP *)pInputFileData;
        if (pBmpIn16ColorsImage->Nbr_Bit_Par_Pixel == 4)
        {
            uJndex = pTheSprite->uNumerOfLine;
            while (uJndex > 0)
            {
                sprintf( pTemp, "\ngTheWait%02d          dc  i'%d,%d'\n", curl, pTheSprite->uHauteur, pTheSprite->uLargeur / 4);
                pOutputFileData = strcat( pOutputFileData, pTemp);
                pTemp[0] = '\0';

                // the sprite extraction
                for (uIndex = iVarBmpY; uIndex > iVarBmpY - pTheSprite->uHauteur; uIndex--)
                {
                    pOutputFileData = strcat( pOutputFileData, pSpriteBeginLineStr);
                    for (uLoop = (pTheSprite->uSwapColumnA / 2); uLoop < ((pTheSprite->uSwapColumnA + pTheSprite->uLargeur) / 2); uLoop++)
                    {
                        sprintf( pTemp, "%02x", pBmpIn16ColorsImage->bitmap[uIndex][uLoop]);
                        pOutputFileData = strcat( pOutputFileData, pTemp);
                        pTemp[0] = '\0';
                    }
                    pOutputFileData = strcat( pOutputFileData, "'\n");
                }
                pOutputFileData = strcat( pOutputFileData, "\n");

                // Compute the mask in live
                for (uIndex = iVarBmpY; uIndex > iVarBmpY - pTheSprite->uHauteur; uIndex--)
                {
                    pOutputFileData = strcat( pOutputFileData, pSpriteBeginLineStr);
                    for (uLoop = (pTheSprite->uSwapColumnA / 2); uLoop < ((pTheSprite->uSwapColumnA + pTheSprite->uLargeur) / 2); uLoop++)
                    {
                        if (pBmpIn16ColorsImage->bitmap[uIndex][uLoop] == 0)
                            sprintf( pTemp, "%s", "00");
                        else if (((pBmpIn16ColorsImage->bitmap[uIndex][uLoop] & 0xF0) == 0) && ((pBmpIn16ColorsImage->bitmap[uIndex][uLoop] & 0x0F) != 0))
                            sprintf( pTemp, "%s", "0f");
                        else if (((pBmpIn16ColorsImage->bitmap[uIndex][uLoop] & 0xF0) != 0) && ((pBmpIn16ColorsImage->bitmap[uIndex][uLoop] & 0x0F) == 0))
                            sprintf( pTemp, "%s", "f0");
                        else
                            sprintf( pTemp, "%s", "ff");
                        pOutputFileData = strcat( pOutputFileData, pTemp);
                        pTemp[0] = '\0';
                    }
                    pOutputFileData = strcat( pOutputFileData, "'\n");
                }
                pOutputFileData = strcat( pOutputFileData, pSpriteEndStr);

                iVarBmpY -= (pTheSprite->uHauteur + 1);
                curl++;
                uJndex -= (pTheSprite->uHauteur + 1);
                pOutputFileData = strcat( pOutputFileData, "\n");
            }
            *pDataSize = (unsigned int )strlen( pOutputFileData);
        }
        else
        {
            pBmpIn16ColorsImage = NULL;
            pBmpIn256ColorsImage = (FormatBMP256 *)pInputFileData;
        }
    }

    return pOutputFileData;
}
