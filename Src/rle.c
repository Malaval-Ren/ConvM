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
* @fn static unsigned int rle4_encode( char *pOutputFileData, const char *pInputFileData, unsigned int uLen)
* @brief encode in pInputFileData in out pOutputFileData
*
* @param[in,out]    pOutputFileData
* @param[in]        pInputFileData
* @param[in]        uLen
*
* @@return return size of encoded data
*/
static unsigned int rle4_encode( char *pOutputFileData, const char *pInputFileData, unsigned int uLen)
{
    unsigned int    uTotalEncodedData = 0;

    if ((pOutputFileData) && (pInputFileData) && (uLen))
    {
    }

    return uTotalEncodedData;
}

/**
* @fn unsigned int rle4_decode( char *pOutputFileData, unsigned int uOutputFileSize, char *pInputFileData, unsigned int uLen)
* @brief SHR decode from pInputFileData pointer to pOutputFileData pointer
*
* @param[in,out]    pOutputFileData
* @param[in]        pInputFileData
* @param[in]        uLen
*
* @@return return size of decoded data
*/
unsigned int rle4_decode( char *pOutputFileData, unsigned int uOutputFileSize, char *pInputFileData, unsigned int uLen)
{
    char           *pRunning;
    unsigned int    uTotalDecodedData = 0;
    unsigned char   type;
    unsigned char   count;
    unsigned char   uIndex;

    if ((pOutputFileData) && (pInputFileData))
    {
        while (uLen > 0)
        {
            type = ((unsigned char)*pInputFileData & 0xc0) >> 6;
            count = ((unsigned char)*pInputFileData & 0x3f) + 1;
            pInputFileData++;
            uLen--;
            // printf("pOutputFileData        : %p\tsize = %d\twrite = %d\tuLen = %d\n", (void*)pOutputFileData, uOutputFileSize, nbrWrite, uLen);

            switch (type)
            {
                case 0: // AllDifferent
                {
                    uTotalDecodedData += count;
                    for (uIndex = 0; uIndex < count; uIndex++)
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
                    for (uIndex = 0; uIndex < count; uIndex++)
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
                    for (uIndex = 0; uIndex < count; uIndex++)
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
                    for (uIndex = 0; uIndex < count; uIndex++)
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
    unsigned int            uIndex;
    unsigned int            uLoop;
    unsigned int            totalOfBytesToUnpack = 0;

    if (pInputFileData)
    {
        pShrStruct = (HeaderSHR *)pInputFileData;

        (void )printf( "HEADER\n");
        (void )printf( " - File size         : %lu\n", pShrStruct->Longueur_Fichier);
        if (pShrStruct->Signature[0] != 0)
        {
            pBlockName = (char *)&pString;
            (void )strncpy_s( pBlockName, 32, (char *)(&pShrStruct->Signature[1]), pShrStruct->Signature[0]);
            (void )printf( " - Block Name        : %s\n", pBlockName);
        }

        if (pBlockName)
        {
            if (!strcmp( (const char *)pString, "MAIN"))
            {
                (void )printf( " - Master mode high  : %d\n", (pShrStruct->Mode_Maitre && 0xFF00));
                (void )printf( " - Master mode low   : %d\n", (pShrStruct->Mode_Maitre && 0x00FF));
                (void )printf( " - PixelsPerScanLine : %d\n", pShrStruct->Nbr_Pixel_Par_Ligne);
                (void )printf( " - NumColorTables    : %d\n", pShrStruct->Nbr_Table_Couleur);
                if (pShrStruct->Nbr_Table_Couleur > 0)
                {
                    (void )printf( " - COLORS Tables\n");
                    pColors = (unsigned short int*)&pShrStruct->Couleur_Palette_0[0];
                    for (uIndex = 0; uIndex < pShrStruct->Nbr_Table_Couleur; uIndex++)
                    {
                        for (uLoop = 0; uLoop < 16; uLoop++)
                        {
                            if (uLoop == 0)
                                (void )printf( "   0x%04X", *pColors);
                            else
                                (void )printf( " 0x%04X", *pColors);
                            pColors++;
                        }
                    }
                    (void )printf( "\n");
                }

                (void )printf( "\n + offset            : %d = 0x%04X\n\n", (int )((char *)pColors - pInputFileData), (int)((char*)pColors - pInputFileData));

                if (pColors)
                {
                    pShrDataStruct = (DataSHR *)pColors;
                }
                else
                {
                    pShrDataStruct = (DataSHR *)&pShrStruct->Couleur_Palette_0;
                }
                (void )printf( " - NumScanLines      : %d\n", pShrDataStruct->Nbr_Scan_Ligne);

                if (pShrDataStruct->Nbr_Scan_Ligne > 0)
                {
                    (void )printf( " - SCAN LINE\n");
                    pDirEntry = (DirEntry *)&pShrDataStruct->ScanLineDirectory;
                    uLoop = 0;
                    for (uIndex = 0; uIndex < pShrDataStruct->Nbr_Scan_Ligne; uIndex++)
                    {
                        if (uLoop == 0)
                            (void )printf( "   %03d %d %d", pDirEntry->NumberOfBytes, (pDirEntry->mode && 0xFF00), (pDirEntry->mode && 0x00FF));
                        else
                            (void )printf( "  %03d %d %d", pDirEntry->NumberOfBytes, (pDirEntry->mode && 0xFF00), (pDirEntry->mode && 0x00FF));
                        totalOfBytesToUnpack += pDirEntry->NumberOfBytes;
                        pDirEntry++;
                        uLoop++;
                        if (uLoop == 16)
                        {
                            uLoop = 0;
                            (void )printf( "\n");
                        }
                    }
                    (void )printf( "\n");
                }

                (void )printf( "\n + BytesToUnpack     : %d = 0x%04X\n", totalOfBytesToUnpack, totalOfBytesToUnpack);
                (void )printf( " + offset            : %d = 0x%04X\n\n", (int)((char *)pDirEntry - pInputFileData), (int)((char *)pDirEntry - pInputFileData));

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
static unsigned int createPic( char *pOutputFileData, unsigned int uOutputFileSize, char *pInputFileData, unsigned int inputFileSize)
{
    unsigned int        iOffset = 0;
    HeaderSHR          *pShrHeaderStruct;
    DataSHR            *pShrDataStruct;
    unsigned int        iHeaderSize = 0;
    unsigned short int *pColors = NULL;
    unsigned int        colorLen;
    DirEntry           *pDirEntry = NULL;
    unsigned int        totalOfBytesToUnpack = 0;
    unsigned int        uIndex;
    char               *pImageData;

    if (pInputFileData)
    {
        pShrHeaderStruct = (HeaderSHR *)pInputFileData;

        iHeaderSize = sizeof(HeaderSHR) - sizeof(pShrHeaderStruct->Couleur_Palette_0);

        pColors = (unsigned short int *)(pInputFileData + iHeaderSize);
        colorLen = pShrHeaderStruct->Nbr_Table_Couleur * sizeof(pShrHeaderStruct->Couleur_Palette_0);
        //(void )printf("pColors               : %p\n", (void *)pColors);
        //(void )printf("pColors palette size  : %d\n", colorLen);

        pShrDataStruct = (DataSHR *)(pInputFileData + iHeaderSize + colorLen);

        if (pShrDataStruct->Nbr_Scan_Ligne > 0)
        {
            pDirEntry = (DirEntry *)&pShrDataStruct->ScanLineDirectory;
            for (uIndex = 0; uIndex < pShrDataStruct->Nbr_Scan_Ligne; uIndex++)
            {
                totalOfBytesToUnpack += pDirEntry->NumberOfBytes;
                pDirEntry++;
            }
        }

        pImageData = (char *)pDirEntry;

        if ((pColors) && (colorLen >= 32) && (pImageData))
        {
            iOffset = rle4_decode(pOutputFileData, uOutputFileSize, pImageData, totalOfBytesToUnpack);
            if (iOffset)
            {
                char *pFun = pOutputFileData + 0x7D00 + 0xD0;
                (void )strncpy( pFun, "Renaud Malaval", sizeof( "Renaud Malaval"));
                pFun = pOutputFileData + 0x7D00 + 0xE0;
                (void)strncpy( pFun, "and", sizeof("and"));
                pFun = pOutputFileData + 0x7D00 + 0xF0;
                (void )strncpy( pFun, "Frederic Mure", sizeof( "Frederic Mure"));

                char* pPalette = pOutputFileData + 0x7E00;
                (void )memcpy(pPalette, pColors, colorLen);
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
            case eDUMP:
            {
                DumpShrHeader(pInputFileData, inputFileSize);
                *pDataSize = 0;
            }
            break;

            case eRLE_COMP:
                *pDataSize = rle4_encode( pOutputFileData, pInputFileData, inputFileSize);
            break;

            case eRLE_DECO:
                *pDataSize = createPic( pOutputFileData, 65535, pInputFileData, inputFileSize);
            break;
                
            default:
            break;
        }
    }
    return pOutputFileData;
}

/**
* @fn void doDumpPic(char* pInputFileData, unsigned int inputFileSize)
* @brief Dump the content of a pic file
*
* @param[in]        pFilePathname
* @param[in]        pInputFileData
* @param[in]        inputFileSize
*
*/
void doDumpPic( char *pFilePathname, char *pInputFileData, unsigned int inputFileSize)
{
    char           *pInputRunner;
    char           *pScb;
    char           *pPalette;
    char           *pNumPalette;
    char           *pColor;
    unsigned short *pColorRunning;
    unsigned int    uIndex;
    unsigned int    uLoop;
    unsigned int    uBegin = 0;
    unsigned int    uEnd = 0;
    unsigned int    uPalette = 0;
    unsigned int    uOffsetPalette;
    unsigned int    uOffsetCouleur;
    unsigned int    uNextLine;
    unsigned int    uIndexUsed[0x10] = {0};
    unsigned short  uColorUsed[256];

    if ( (pInputFileData) && (pFilePathname) )
    {
        (void )printf( "\nDisplay content of pic\n\n");
        (void )printf( " %s\n\n", pFilePathname);

        (void )printf( "\t- PIXELS -\n\n");
        pInputRunner = pInputFileData;
        for (uIndex = 0; uIndex < 200; uIndex++)
        {
            uNextLine = 20;
            (void )printf( "%03d: ", uIndex);
            for (uLoop = 0; uLoop < 160; uLoop++)
            {
                (void )printf( "%02d %02d ", ((*pInputRunner & 0xF0) >> 4), (*pInputRunner & 0x0F));
                pInputRunner++;
                if (uLoop == uNextLine - 1)
                {
                    (void )printf( "\n     ");
                    uNextLine += 20;
                }
            }
            (void )printf( "\n");
        }

        (void )printf( "\n\t- SCBS -\n\n");
        uBegin = 0;
        uEnd = 0;
        uPalette = *pInputRunner;
        pScb = pInputRunner;
        for (uIndex = 0; uIndex < 200; uIndex++)
        {
            if ((uPalette != *pInputRunner) || (uIndex == 199) )
            {
                (void )printf("Line %03d to %03d  use palette %02d  -  %02d times\n", uBegin, uEnd - 1, uPalette, (uEnd - uBegin));
                uBegin = uEnd;
                uEnd++;
                uPalette = *pInputRunner;
            }
            else
            {
                uEnd++;
            }
            pInputRunner++;
        }

        (void )printf( "\n\t- INFO -\n\n");
        uBegin = 0;
        for (uIndex = 0; uIndex < 56; uIndex++)
        {
            if (( *pInputRunner == 0) && (uBegin == 0))
            {
                ;
            }
            else if (( *pInputRunner == 0) && (uBegin == 1))
            {
                (void )printf( "\n");
                uBegin = 0;
            }
            else
            {
                (void )printf( "%c", *pInputRunner);
                uBegin = 1;
            }
            pInputRunner++;
        }

        (void )printf( "\n\n\t- PALETTE -\n");

        (void )memset( &uColorUsed, 0xFFFF, sizeof(uColorUsed));

        pColorRunning = (unsigned short *)(pInputFileData + 0x7E00);
        uLoop = 0;
        uBegin = 0;
        uEnd = 1;
        uColorUsed[uLoop] = *pColorRunning;
        for (uIndex = 0; uIndex < 16; uIndex++)
        {
            (void )printf( "\n%02d: ", uIndex);
            for (uLoop = 0; uLoop < 16; uLoop++)
            {
                (void )printf( "0x%04X ", *pColorRunning);

                // Fill array to be able to count number of color used
                while (uColorUsed[uBegin] != 0xFFFF)
                {
                    if (uColorUsed[uBegin] == *pColorRunning)
                        break;
                    uBegin++;
                }
                if (uColorUsed[uBegin] == 0xFFFF)
                {
                    uColorUsed[uBegin] = *pColorRunning;
                    uEnd++;
                }
                uBegin = 0;

                pColorRunning++;
            }
        }
        (void )printf( "\n\nNumber of different colors = %d\n\n", uEnd);

        (void )printf( "\t- COLOR INDEX USAGE -\n\n");
        pInputRunner = pInputFileData;
        pPalette = pInputFileData + 0x7E00;
        for (uIndex = 0; uIndex < 200; uIndex++)
        {
            (void )printf( "%03d: ", uIndex);
            for (uLoop = 0; uLoop < 160; uLoop++)
            {
                uIndexUsed[(*pInputRunner & 0xF0) >> 4] += 1;
                uIndexUsed[*pInputRunner & 0x0F] += 1;
                pInputRunner++;
            }

            for (uLoop = 0; uLoop < 0x10; uLoop++)
            {
                if (uIndexUsed[uLoop] == 0)
                {
                    (void)printf("    ");
                }
                else
                {
                    (void)printf("%03d ", uIndexUsed[uLoop]);
                }
            }
            (void )printf( " -  ");

            for (uLoop = 0; uLoop < 0x10; uLoop++)
            {
                if (uIndexUsed[uLoop] != 1)
                {
                    (void )printf("   .   ");
                }
                else
                {
                    pNumPalette = pScb + uIndex;   // numero de palette
                    uOffsetPalette = (*pNumPalette) * 0x1E;
                    uOffsetCouleur = uLoop << 1;
                    pColor = pPalette + uOffsetPalette + uOffsetCouleur;
                    (void )printf( "0x%04X ", *(unsigned short *)pColor);
                }
            }
            (void )printf( "\n");

            (void )memset( &uIndexUsed, 0x0, sizeof( uIndexUsed));
        }

    }

    return;
}

/**
* @fn char *doExtractTextFromBinay( char *pInputFileData, unsigned int uInputFileSize, unsigned int minSentenseLen, unsigned int *pDataSize)
* @brief Dump the content of a pic file
*
* @param[in]        pInputFileData
* @param[in]        inputFileSize
* @param[in]        minSentenseLen
* @param[in,out]    pDataSize
*
* @return a new allocated pointer pOutputFileData
*/
char *doExtractTextFromBinay( char *pInputFileData, unsigned int uInputFileSize, unsigned int minSentenseLen, unsigned int *pDataSize)
{
    char           *pOutputFileData = NULL;
    char           *pInputRunner;
    char           *pBeforeInputRunner;
    char           *pAfterInputRunner;
    char           *pOutputRunner = NULL;
    char            tempBuffer[512] = {0};
    char            convertChar;
    unsigned int    uOffset = 0;
    unsigned int    uIndex = 0;
    unsigned int    uLen;

    if (uInputFileSize <= 589824)   // 512 + 64
    {
        pOutputFileData = calloc(1, (size_t)(589824));    // allocate big place to work
        if (pOutputFileData)
        {
            pInputRunner = pInputFileData;
            pOutputRunner = pOutputFileData;

            while (uIndex < uInputFileSize)
            {
                if ( (*pInputRunner >= ' ') && (*pInputRunner <= '~') )
                {
                    convertChar = *pInputRunner;
                    if (convertChar == '*')
                    {
                        pBeforeInputRunner = pInputRunner - 1;
                        pAfterInputRunner = pInputRunner + 1;

                        if (*pAfterInputRunner == 't')
                        {
                            if ((*pBeforeInputRunner == ' ') ||
                                (*pBeforeInputRunner == 'b') ||
                                (*pBeforeInputRunner == 'r') ||
                                (*pBeforeInputRunner == 't') ||
                                (*pBeforeInputRunner == 'n') ||
                                (*pBeforeInputRunner == '-') ||
                                (*pBeforeInputRunner == '\''))
                            {
                                convertChar = 'ê';
                            }
                        }
                        if (*pAfterInputRunner == 'm')
                        {
                            if ((*pBeforeInputRunner == 'm') || (*pBeforeInputRunner == 'l'))
                            {
                                convertChar = 'ê';
                            }
                        }
                        else if ((*pBeforeInputRunner == 'g') && (*pAfterInputRunner == 'n'))
                            convertChar = 'ê';
                    }
                    tempBuffer[uOffset] = convertChar;
                    uOffset++;
                }
                else if ( (*pInputRunner >= (char )0x81) && (*pInputRunner <= (char )0xB4) )
                {
                    /*   convert : ASCII TABLE from Atari ST
                            _0   _1  _2  _3  _4  _5  _6  _7  _8  _9  _A  _B  _C  _D  _E  _F
                        2_       !   "   #   $   %   &   '   (   )   *   +   ,   -   .   /
                        3_   0   1   2   3   4   5   6   7   8   9   :   ;   <   =   >   ?
                        4_   @   A   B   C   D   E   F   G   H   I   J   K   L   M   N   O
                        5_   P   Q   R   S   T   U   V   W   X   Y   Z   [   \   ]   ^   _
                        6_   `   a   b   c   d   e   f   g   h   i   j   k   l   m   n   o
                        7_   p   q   r   s   t   u   v   w   x   y   z   {   |   }   ~   Δ
                        8_   Ç   ü   é   â   ä   à   å   ç   ê   ë   è   ï   î   ì   Ä   Å
                        9_   É   æ   Æ   ô   ö   ò   û   ù   ÿ   Ö   Ü   ¢   £   ¥   ß   ƒ
                        A_   á   í   ó   ú   ñ   Ñ   ª   º   ¿   ⌐   ¬   ½   ¼   ¡   «   »
                        B_   ã   õ   Ø   ø   œ   Œ   À   Ã   Õ   ¨   ´   †   ¶   ©   ®   ™
                        C_   ĳ   Ĳ   א   ב   ג   ד   ה   ו   ז   ח   ט   י   כ   ל   מ   נ
                        D_   ס   ע   פ   צ   ק   ר   ש   ת   ן   ך   ם   ף   ץ   §   ∧   ∞
                        E_   α   β   Γ   π   Σ   σ   µ   τ   Φ   Θ   Ω   δ   ∮   ϕ   ∈   ∩
                        F_   ≡   ±   ≥   ≤   ⌠   ⌡   ÷   ≈   °   •   ·   √   ⁿ   ²   ³   ¯
                    */
                    switch (*pInputRunner)
                    {
                        case (char )0x81:  convertChar = 'ü';   break;
                        case (char )0x82:  convertChar = 'é';   break;
                        case (char )0x83:  convertChar = 'â';   break;
                        case (char )0x84:  convertChar = 'ä';   break;
                        case (char )0x85:  convertChar = 'à';   break;
                        case (char )0x86:  convertChar = 'å';   break;
                        case (char )0x87:  convertChar = 'ç';   break;
                        case (char )0x88:  convertChar = 'ê';   break;
                        case (char )0x89:  convertChar = 'ë';   break;
                        case (char )0x8A:  convertChar = 'è';   break;
                        case (char )0x8B:  convertChar = 'ï';   break;
                        case (char )0x8C:  convertChar = 'î';   break;
                        case (char )0x8D:  convertChar = 'ì';   break;
                        //case (char )0x8E:  convertChar = ' ';   break;
                        //case (char )0x8F:  convertChar = ' ';   break;
                        //case (char )0x90:  convertChar = ' ';   break;
                        //case (char )0x91:  convertChar = ' ';   break;
                        //case (char )0x92:  convertChar = ' ';   break;
                        case (char )0x93:  convertChar = 'ô';   break;
                        case (char )0x94:  convertChar = 'ö';   break;
                        case (char )0x95:  convertChar = 'ò';   break;
                        case (char )0x96:  convertChar = 'û';   break;
                        case (char )0x97:  convertChar = 'ÿ';   break;
                        //case (char )0x98:  convertChar = ' ';   break;
                        //case (char )0x99:  convertChar = ' ';   break;
                        //case (char )0x9A:  convertChar = ' ';   break;
                        //case (char )0x9B:  convertChar = ' ';   break;
                        //case (char )0x9C:  convertChar = ' ';   break;
                        //case (char )0x9D:  convertChar = ' ';   break;
                        //case (char )0x9E:  convertChar = ' ';   break;
                        //case (char )0x9F:  convertChar = ' ';   break;
                        case (char )0xA0:  convertChar = 'á';   break;
                        case (char )0xA1:  convertChar = 'í';   break;
                        case (char )0xA2:  convertChar = 'ó';   break;
                        case (char )0xA3:  convertChar = 'ú';   break;
                        case (char )0xA4:  convertChar = 'ñ';   break;
                        //case (char )0xA5:  convertChar = ' ';   break;
                        //case (char )0xA6:  convertChar = ' ';   break;
                        //case (char )0xA7:  convertChar = ' ';   break;
                        //case (char )0xA8:  convertChar = ' ';   break;
                        //case (char )0xA9:  convertChar = ' ';   break;
                        //case (char )0xAA:  convertChar = ' ';   break;
                        //case (char )0xAB:  convertChar = ' ';   break;
                        //case (char )0xAC:  convertChar = ' ';   break;
                        //case (char )0xAD:  convertChar = ' ';   break;
                        case (char )0xAE:  convertChar = '"';   break;
                        case (char )0xAF:  convertChar = '"';   break;
                        case (char )0xB0:  convertChar = 'ã';   break;
                        case (char )0xB1:  convertChar = 'õ';   break;
                        case (char )0xB2:  convertChar = 'Ø';   break;
                        case (char )0xB3:  convertChar = 'ø';   break;
                        case (char )0xB4:  convertChar = 'œ';   break;

                        default:           convertChar = ' ';   break;
                    }

                    if (convertChar != ' ')
                    {
                        tempBuffer[uOffset] = convertChar;
                        uOffset++;
                    }
                    else
                    {
                        if (uOffset > 0)    // not managed characters in a string before '\0' drop it
                        {
                            uOffset = 0;
                            tempBuffer[uOffset] = 0;
                        }
                    }
                }
                else if (*pInputRunner == '\n')    // a return to line in a string
                {
                    tempBuffer[uOffset] = '\\';
                    uOffset++;
                    tempBuffer[uOffset] = 'N';
                    uOffset++;
                }
                else if (*pInputRunner == '\r')    // a return to line in a string
                {
                    tempBuffer[uOffset] = '\\';
                    uOffset++;
                    tempBuffer[uOffset] = 'R';
                    uOffset++;
                }
                else if (*pInputRunner == '\0')     // end of a string
                {
                    if (uOffset > 0)
                    {
                        tempBuffer[uOffset] = *pInputRunner;

                        uLen = (unsigned int)strlen(tempBuffer);
                        if ((uLen > 0) && (uLen >= minSentenseLen))
                        {
                            (void)memcpy(pOutputRunner, tempBuffer, uLen);
                            pOutputRunner += uLen;
                            *pOutputRunner = '\r';
                            pOutputRunner++;
                            *pOutputRunner = '\n';
                            pOutputRunner++;
                            *pDataSize += uLen + 2;
                        }
                        uOffset = 0;
                        tempBuffer[uOffset] = 0;
                    }
                }
                else     // not managed characters
                {
                    if (uOffset > 0)    // not managed characters in a string before '\0' drop it
                    {
                        uOffset = 0;
                        tempBuffer[uOffset] = 0;
                    }
                }

                pInputRunner++;
                uIndex++;
            }
        }
    }

    return pOutputFileData;
}
