
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
* @fn static unsigned int convert_to_pic( char *pOutputFileData, unsigned int uOutputFileSize, char *pInputFileData, unsigned int inputFileSize)
* @brief calcul where are the data to uncompress
*
* @param[in,out]    pOutputFileData
* @param[in]        uOutputFileSize
* @param[in]        pInputFileData
* @param[in]        inputFileSize
*
* @return return offset of data to decompress
*/
static unsigned int convert_to_pic(char *pOutputFileData, unsigned int uOutputFileSize, char *pInputFileData, unsigned int inputFileSize)
{
    FormatPIC          *pPicImage = NULL;
    FormatBMP          *pBmpImage = NULL;
    unsigned int        iOffset = 0;
    int                 iVarPicX = 0;
    int                 iVarBmpX = 199;
    unsigned int        uIndex = 0;
    unsigned short int  uValueF = 0;
    unsigned long int   uValueD = 0;
    unsigned char       uColorRed = 0;
    unsigned char       uColorGreen = 0;
    unsigned char       uColorBlue = 0;

    if ((pOutputFileData) && (uOutputFileSize > 0) && (pInputFileData))
    {
        pPicImage = (FormatPIC *)pOutputFileData;

        pBmpImage = (FormatBMP *)pInputFileData;

        int VarPicX = 0;
        int VarBmpX = 199;
        for (VarPicX = 0; VarPicX < 200; VarPicX++)
        {
            memcpy(pPicImage->MonImage[VarPicX], pBmpImage->MonImage[VarBmpX], 160);
            VarBmpX--;
        }

        char* pFun = pOutputFileData + 0x7D00 + 0xD0;
        (void)strncpy(pFun, "Frederic Mure", sizeof("Frederic Mure"));
        pFun = pOutputFileData + 0x7D00 + 0xE0;
        (void)strncpy(pFun, "and", sizeof("and"));
        pFun = pOutputFileData + 0x7D00 + 0xF0;
        (void)strncpy(pFun, "Renaud Malaval", sizeof("Renaud Malaval"));

        for (uIndex = 0; uIndex < 16; uIndex++)            // traitement des 16 couleurs de la palette
        {
            uValueD = pBmpImage->Couleur_Palette_0[uIndex];                             // 00 60 20 00 = 00206000     0=00 R=20 G=60 B=00
            uColorRed = (unsigned char)((uValueD & 0x00F00000) / 1048576);       // ne garde que le R et le décale en unitee
            uColorGreen = (unsigned char)((uValueD & 0x0000F000) / 4096);          // ne garde que le G et le décale en unitee
            uColorBlue = (unsigned char)((uValueD & 0x000000F0) / 16);            // ne garde que le B deja en unitee
            uValueF = (uColorRed * 256) + (uColorGreen * 16) + (uColorBlue);  // transformation de 00R0G0B0 en 0=0 R=2 G=6 B=0 Passage niveau couleur FF en F
            pPicImage->Couleur_Palette_0[uIndex] = uValueF;
        }

        iOffset = (unsigned int)sizeof(FormatPIC);
    }

    return iOffset;
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
* @return offset of data size
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
        pBmpImage->Offset_Image = sizeof( FormatBMP256) - sizeof( pBmpImage->MonImage );
        pBmpImage->Entete2 = 40;
        pBmpImage->Largeur_Image = 320;
        pBmpImage->Longueur_Image = 200;
        pBmpImage->Nbr_Plan = 1;
        pBmpImage->Nbr_Bit_Par_Pixel = 8;
        pBmpImage->Taille_Map = sizeof( pBmpImage->MonImage);
        pBmpImage->Resolution_H = 2835;
        pBmpImage->Resolution_V = 2835;

        for (iVarPicX = 0; iVarPicX < 200; iVarPicX++)
        {
            uIndex = 0;
            uPaletteOffset = pPicImage->SCB[iVarPicX] * 16;     // 16 colors in a table of int (16 bits)
            for (uLoop = 0; uLoop < 160; uLoop++)
            {
                pBmpImage->MonImage[iVarBmpX][uIndex++] = ((pPicImage->MonImage[iVarPicX][uLoop] & 0xF0) >> 4) + uPaletteOffset;
                pBmpImage->MonImage[iVarBmpX][uIndex++] = (pPicImage->MonImage[iVarPicX][uLoop] & 0x0F) + uPaletteOffset;
            }
            iVarBmpX--;
        }

        pPicPalette = (unsigned short int *)&pPicImage->Couleur_Palette_0;
        pBmpPalette = (unsigned long int *)&pBmpImage->Couleur_Palettes;
        // creation de la palette BMP a partir des palettes IIGS
        for (uIndex = 0; uIndex < 256; uIndex++)            // traitement des 256 couleurs de la palette
        {
            uValueD = *pPicPalette;
            uColorRed = (unsigned char)((uValueD & 0x0F00) >> 8) + (unsigned char)((uValueD & 0x0F00) >> 4);
            uColorGreen = (unsigned char)((uValueD & 0x00F0) >> 4)  + (unsigned char)(uValueD & 0x00F0);
            uColorBlue = (unsigned char)(uValueD & 0x000F) + (unsigned char)((uValueD & 0x000F) << 4);

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
* @return return offset of data to decompress
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
        pBmpImage->Taille_Image = sizeof(FormatBMP);    // 32118
        pBmpImage->Offset_Image = sizeof(FormatBMP) - sizeof(pBmpImage->MonImage);
        pBmpImage->Entete2 = 40;
        pBmpImage->Largeur_Image = 320;
        pBmpImage->Longueur_Image = 200;
        pBmpImage->Nbr_Plan = 1;
        pBmpImage->Nbr_Bit_Par_Pixel = 4;
        pBmpImage->Taille_Map = sizeof(pBmpImage->MonImage);
        pBmpImage->Resolution_H = 3780;
        pBmpImage->Resolution_V = 3780;

        for (iVarPicX = 0; iVarPicX < 200; iVarPicX++)
        {
            (void)memcpy(&pBmpImage->MonImage[iVarBmpX], &pPicImage->MonImage[iVarPicX], 160);
            iVarBmpX--;
        }

        // creation de la palette de 64 octets du PC contre 32 du IIGS
        for (uIndex = 0; uIndex < 16; uIndex++)                                 // traitement des 16 couleurs de la palette
        {
            uValueD = pPicImage->Couleur_Palette_0[uIndex];                     // 0x60 0x02 = 0x0260     0=0 R=2 G=6 B=0
            uColorRed = (unsigned char)((uValueD & 0x0F00) >> 8);              // ne garde que le R et le décale en unitee
            uColorGreen = (unsigned char)((uValueD & 0x00F0) / 4);             // ne garde que le G et le décale en unitee
            uColorBlue = (unsigned char)(uValueD & 0x000F);                     // ne garde que le B deja en unitee

            // transformation de 0RGB en 0=00 R=20 G=60 B=00 Passage niveau couleur F en FF
            pBmpImage->Couleur_Palette_0[uIndex] = (uColorRed * 1048576) + (uColorGreen * 4096) + (uColorBlue * 16);
        }
        iOffset = (unsigned int)sizeof(FormatBMP);
    }

    return iOffset;
}

/**
* @fn static unsigned int countNumberOfScb(char *pInputFileData, unsigned int inputFileSize)
* @brief calcul where are the data to uncompress
*
* @param[in]        pInputFileData
* @param[in]        inputFileSize
*
* @return number of SCB
*/
static unsigned int countNumberOfScb(char *pInputFileData, unsigned int inputFileSize)
{
    char           *pInputRunner;
    char           *pScb;
    unsigned int    uScbNumber = 0;
    //unsigned int    uBegin = 0;
    //unsigned int    uEnd = 0;
    unsigned int    uPalette = 0;
    unsigned int    uIndex;

    if (pInputFileData)
    {
        pInputRunner = pInputFileData + (200 * 160);

        //uBegin = 0;
        //uEnd = 0;
        uPalette = *pInputRunner;
        pScb = pInputRunner;
        for (uIndex = 0; uIndex < 200; uIndex++)
        {
            if ((uPalette != *pInputRunner) || (uIndex == 199))
            {
                uScbNumber++;
                //(void)printf("Line %03d to %03d  use palette %02d  -  %02d times\n", uBegin, uEnd - 1, uPalette, (uEnd - uBegin));
                //uBegin = uEnd;
                //uEnd++;
                uPalette = *pInputRunner;
            }
            //else
            //{
            //    uEnd++;
            //}
            pInputRunner++;
        }
    }

    return uScbNumber;
}

static void picColorsUsage( char *pInputFileData, unsigned int inputFileSize)
{
    /*
    FormatPIC* pPicImage = NULL;
    int                 iVarPicX = 0;
    int                 iVarBmpX = 199;
    unsigned int        uOffset = 0;
    unsigned int        uIndex;
    unsigned int        uLoop;
    unsigned int        uPaletteOffset;
    unsigned short int  uIndexLessUsed[256] = { 0 };

    pPicImage = (FormatPIC*)pInputFileData;

    for (iVarPicX = 0; iVarPicX < 200; iVarPicX++)
    {
        uIndex = 0;
        uPaletteOffset = pPicImage->SCB[iVarPicX] * 16;     // 16 colors in a table of int (16 bits)
        for (uLoop = 0; uLoop < 160; uLoop++)
        {
            uIndexLessUsed[((pPicImage->MonImage[iVarPicX][uLoop] & 0xF0) >> 4) + uPaletteOffset] += 1;
            uIndexLessUsed[(pPicImage->MonImage[iVarPicX][uLoop] & 0x0F) + uPaletteOffset] += 1;
        }
        iVarBmpX--;
    }
    */
    return;
}

/**
* @fn static unsigned int convert_to_bmp( char *pOutputFileData, unsigned int uOutputFileSize, char *pInputFileData, unsigned int inputFileSize)
* @brief calcul where are the data to uncompress
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
    FormatBMP   *pBmpImage;

    if ((pInputFileData) && (pFilePathname) && (inputFileSize > sizeof( FormatBMP)))
    {
        pBmpImage = (FormatBMP *)pInputFileData;
        printf("\nDisplay content of bmp header\n\n");
        printf(" %s\n\n", pFilePathname);

        printf("Signature          = %c%c\n", (char)(pBmpImage->Signature & 0x00FF), (char)((pBmpImage->Signature & 0xFF00) >> 8));
        printf("Taille_Image       = %ld\n", pBmpImage->Taille_Image);
        printf("Offset_Image       = %ld\n", pBmpImage->Offset_Image);
        printf("Entete2            = %ld\n", pBmpImage->Entete2);
        printf("Largeur_Image      = %ld\n", pBmpImage->Largeur_Image);
        printf("Longueur_Image     = %ld\n", pBmpImage->Longueur_Image);
        printf("Nbr_Plan           = %ld\n", pBmpImage->Nbr_Plan);
        printf("Nbr_Bit_Par_Pixel  = %ld\n", pBmpImage->Nbr_Bit_Par_Pixel);
        printf("Taille_Map         = %ld\n", pBmpImage->Taille_Map);
        printf("Resolution_H       = %ld\n", pBmpImage->Resolution_H);
        printf("Resolution_V       = %ld\n", pBmpImage->Resolution_V);

        printf("\n");
    }

    return;
}

/**
* @fn char *DoBmpJob( char *pInputFileData, unsigned int inputFileSize, unsigned int command, unsigned int *pDataSize)
* @brief parse pInputFileData of inputFileSize size following command, and return the data size of pOutputFileData in pDataSize
*
* @param[in]        pInputFileData
* @param[in]        inputFileSize
* @param[in]        command
* @param[in,out]    pDataSize
*
* @return A new pointer pOutputFileData
*/
char* DoBmpJob(char* pInputFileData, unsigned int inputFileSize, unsigned int command, unsigned int* pDataSize)
{
    char    *pOutputFileData = NULL;
    char    *pInputRunner = NULL;

    pOutputFileData = calloc(1, (size_t)(98304));
    if (pOutputFileData)
    {
        pInputRunner = pInputFileData;

        switch (command)
        {
        case TO_BMP:
            *pDataSize = convert_to_bmp( pOutputFileData, (unsigned int)98304, pInputFileData, inputFileSize);
            break;

        case TO_PIC:
            *pDataSize = convert_to_pic(pOutputFileData, (unsigned int)98304, pInputFileData, inputFileSize);
            break;

        default:
            break;
        }
    }
    return pOutputFileData;
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
BOOL CheckBmpFileFormat(char* pInputFileData, unsigned int inputFileSize)
{
    FormatBMP* pBmpStruct = NULL;
    BOOL                bResult = FALSE;

    if ((pInputFileData) && (inputFileSize > 0))
    {
        pBmpStruct = (FormatBMP*)pInputFileData;

        if (pBmpStruct->Signature == 19778)
        {
            if (pBmpStruct->Taille_Image == sizeof(FormatBMP))
            {
                if (pBmpStruct->Largeur_Image == 320)
                {
                    if (pBmpStruct->Longueur_Image == 200)
                    {
                        if (pBmpStruct->Nbr_Bit_Par_Pixel == 4)
                        {
                            if (pBmpStruct->Taille_Map == 32000)
                            {
                                if (pBmpStruct->Offset_Image == 118)
                                {
                                    bResult = TRUE;
                                }
                                else
                                {
                                    printf("BMP : Offset_Image is wrong\n");
                                }
                            }
                            else
                            {
                                printf("BMP : Taille_Map is wrong\n");
                            }
                        }
                        else
                        {
                            printf("BMP : Nbr_Bit_Par_Pixel is wrong\n");
                        }
                    }
                    else
                    {
                        printf("BMP : Longueur_Image is wrong\n");
                    }
                }
                else
                {
                    printf("BMP : Largeur_Image is wrong\n");
                }
            }
            else
            {
                printf("BMP : Taille_Image is wrong\n");
            }
        }
        else
        {
            printf("BMP : Signature is wrong\n");
        }
    }

    return bResult;
}
