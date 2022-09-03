
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
static unsigned int convert_to_pic(char* pOutputFileData, unsigned int uOutputFileSize, char* pInputFileData, unsigned int inputFileSize)
{
    FormatPIC          *pPicImage = NULL;
    FormatBMP          *pBmpImage = NULL;
    unsigned int        iOffset = 0;
    int                 iVarPicX = 0;
    int                 iVarBmpX = 199;
    unsigned int        iIndex = 0;
    unsigned short int  uValueF = 0;
    unsigned long int   uValueD = 0;
    unsigned char       uValueR = 0;
    unsigned char       uValueG = 0;
    unsigned char       uValueB = 0;

    if ((pOutputFileData) && (uOutputFileSize > 0) && (pInputFileData))
    {
        pPicImage = (FormatPIC*)pOutputFileData;

        pBmpImage = (FormatBMP*)pInputFileData;

        int VarPicX = 0;
        int VarBmpX = 199;
        for (VarPicX = 0; VarPicX < 200; VarPicX++)
        {
            memcpy(pPicImage->MonImage[VarPicX], pBmpImage->MonImage[VarBmpX], 160);
            VarBmpX--;
        }

        char* pFun = pOutputFileData + 0x7D00 + 0xD0;
        (void)strncpy(pFun, "Fréderic Mure", sizeof("Fréderic Mure"));
        pFun = pOutputFileData + 0x7D00 + 0xE0;
        (void)strncpy(pFun, "and", sizeof("and"));
        pFun = pOutputFileData + 0x7D00 + 0xF0;
        (void)strncpy(pFun, "Renaud Malaval", sizeof("Renaud Malaval"));

        for (int iIndex = 0; iIndex < 16; iIndex++)            // traitement des 16 couleurs de la palette
        {
            uValueD = pBmpImage->Couleur_Palette_0[iIndex];                             // 00 60 20 00 = 00206000     0=00 R=20 G=60 B=00
            uValueR = (unsigned char)((uValueD & 0x00F00000) / 1048576);       // ne garde que le R et le décale en unitee
            uValueG = (unsigned char)((uValueD & 0x0000F000) / 4096);          // ne garde que le G et le décale en unitee
            uValueB = (unsigned char)((uValueD & 0x000000F0) / 16);            // ne garde que le B deja en unitee
            uValueF = (uValueR * 256) + (uValueG * 16) + (uValueB);  // transformation de 00R0G0B0 en 0=0 R=2 G=6 B=0 Passage niveau couleur FF en F
            pPicImage->Couleur_Palette_0[iIndex] = uValueF;
        }

        iOffset = (unsigned int)sizeof(FormatPIC);
    }

    return iOffset;
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
static unsigned int convert_to_bmp(char* pOutputFileData, unsigned int uOutputFileSize, char* pInputFileData, unsigned int inputFileSize)
{
    FormatPIC          *pPicImage = NULL;
    FormatBMP          *pBmpImage = NULL;
    unsigned int        iOffset = 0;
    int                 iVarPicX = 0;
    int                 iVarBmpX = 199;
    unsigned int        iIndex = 0;
    unsigned long int   uValueF = 0;
    unsigned short int  uValueD = 0;
    unsigned char       uValueR = 0;
    unsigned char       uValueG = 0;
    unsigned char       uValueB = 0;

    if ((pOutputFileData) && (uOutputFileSize > 0) && (pInputFileData))
    {
        pPicImage = (FormatPIC *)pInputFileData;

        pBmpImage = (FormatBMP *)pOutputFileData;
        pBmpImage->Signature = 19778;
        pBmpImage->Taille_Image = sizeof(FormatBMP);    // 32118
        pBmpImage->Offset_Image = 118;
        pBmpImage->Entete2 = 40;
        pBmpImage->Largeur_Image = 320;
        pBmpImage->Longueur_Image = 200;
        pBmpImage->Nbr_Plan = 1;
        pBmpImage->Nbr_Bit_Par_Pixel = 4;
        pBmpImage->Taille_Map = 32000;
        pBmpImage->Resolution_H = 3780;
        pBmpImage->Resolution_V = 3780;

        for (iVarPicX = 0; iVarPicX < 200; iVarPicX++)
        {
            (void)memcpy(&pBmpImage->MonImage[iVarBmpX], &pPicImage->MonImage[iVarPicX], 160);
            iVarBmpX--;
        }

        // creation de la palette de 64 octets du PC contre 32 du IIGS
        for (int iIndex = 0; iIndex < 16; iIndex++)            // traitement des 16 couleurs de la palette
        {
            uValueD = pPicImage->Couleur_Palette_0[iIndex];                     // 60 02 = 0260     0=0 R=2 G=6 B=0
            uValueR = (unsigned char)((uValueD & 0x0F00) / 256);                // ne garde que le R et le décale en unitee
            uValueG = (unsigned char)((uValueD & 0x00F0) / 16);                 // ne garde que le G et le décale en unitee
            uValueB = (unsigned char)(uValueD & 0x000F);                        // ne garde que le B deja en unitee
            uValueF = (uValueR * 1048576) + (uValueG * 4096) + (uValueB * 16);  // transformation de 0RGB en 0=00 R=20 G=60 B=00 Passage niveau couleur F en FF
            pBmpImage->Couleur_Palette_0[iIndex] = uValueF;
        }
        iOffset = (unsigned int)sizeof(FormatBMP);
    }

    return iOffset;
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
    FormatBMP          *pBmpStruct = NULL;
    BOOL                bResult = FALSE;

    if ((pInputFileData) && (inputFileSize > 0))
    {
        pBmpStruct = (FormatBMP *)pInputFileData;

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

    pOutputFileData = calloc(1, (size_t)(65535));
    if (pOutputFileData)
    {
        pInputRunner = pInputFileData;

        switch (command)
        {
        case TO_BMP:
            *pDataSize = convert_to_bmp( pOutputFileData, (unsigned int)65535, pInputFileData, inputFileSize);
            break;

        case TO_PIC:
            *pDataSize = convert_to_pic(pOutputFileData, (unsigned int)65535, pInputFileData, inputFileSize);
            break;

        default:
            break;
        }
    }
    return pOutputFileData;
}