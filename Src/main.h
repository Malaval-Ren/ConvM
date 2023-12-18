
#ifndef MY_GLOBAL_H
#define MY_GLOBAL_H

/*
extracted from file
minwindef.h

here:
C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0\shared\minwindef.h

#define MAKEWORD(a, b)      ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))
#define LOWORD(l)           ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#define HIWORD(l)           ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#define LOBYTE(w)           ((BYTE)(((DWORD_PTR)(w)) & 0xff))
#define HIBYTE(w)           ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))
*/

#define BYTE_TO_BINARY( octet)  \
    ((octet) & 0x80 ? '1' : '0'), \
    ((octet) & 0x40 ? '1' : '0'), \
    ((octet) & 0x20 ? '1' : '0'), \
    ((octet) & 0x10 ? '1' : '0'), \
    ((octet) & 0x08 ? '1' : '0'), \
    ((octet) & 0x04 ? '1' : '0'), \
    ((octet) & 0x02 ? '1' : '0'), \
    ((octet) & 0x01 ? '1' : '0') 

enum eCommandNumber
{
    eNONE = 0,
    eCRLF,
    eLFCR,
    eDOUBLE_0A,
    eDOUBLE_0D,
    eDETAB,
    eDUMP,
    eEXT_TXT,
    eRLE_COMP,
    eRLE_DECO,
    eTO_BMP,
    eTO_PIC,
    eNUMCOLORPERLINE,
    eADDPALBMP4,
    eINSERTPALBMP8,
    eSWAP2COLOR,
    eCOMPPALET,
    eCOPYPALET,
    eEXTSPRITE,

    eLast               // max value never used
};

#pragma pack(1)     // structure without padding....
typedef struct
{
    unsigned long int   Longueur_Fichier;             // 4 octets pour définir la longueur du fichier, incluant la longueur elle même...
    unsigned char       Signature[5];                 // 5 octets pour la chaine en Pacscal de signature $4 + "MAIN" = 04 4D 41 49 4E
    unsigned short int  Mode_Maitre;                  // 2 octets pour le SCB maitre (0)
    unsigned short int  Nbr_Pixel_Par_Ligne;          // 2 octets pour le nombre de pixel par ligne (320)
    unsigned short int  Nbr_Table_Couleur;            // 2 octets pour le nombre de tables de couleurs, peut etre a zero (1) !!!! doit etre 1 !!!
    unsigned short int  Couleur_Palette_0[16];        // une seule palette de 16 couleurs sur 2 octets A=0 R=x G=x B=x
                                                      // et d'autre palette si Nbr_Table_Couleur >1, mais dans ce cas la, on ne traite pas...
} HeaderSHR;

typedef struct 
{
    unsigned short int  Nbr_Scan_Ligne;               // 2 octets pour le nombre de Scan ligne, ne peux pas etre a zero (200)
    unsigned char       ScanLineDirectory[4 * 200];   // 4 octets * Nbr_Scan_Ligne (200), 2 pour le nombre de bytes à decompressé et 2 pour le mode SCB de la ligne
    unsigned char       PackedScanlines[160 * 200];   // prevision max de 320*200, chacune des 200 lignes est compressé de lg donnée par ScanLigneDirectory
} DataSHR;

typedef struct
{
    signed short int    NumberOfBytes;
    unsigned short int  mode;
} DirEntry;

typedef struct
{
    unsigned char       MonImage[200][160];             // 32000 car 2 pixel par octets -> 200 * (320 / 2) = 32000
    unsigned char       SCB[200];                       // 200 SCB donnant le numero de palette de couleur pour chaque ligne 
    unsigned char       Libre[56];                      // espace libre utiliser pour indiquer quel logiciel... Par exemple en [25] == "816/Paint"
    unsigned short int  Couleur_Palette_0[16];          // une seule palette de 16 couleurs sur 2 octets A=0 R=x G=x B=x
    unsigned short int  Couleur_Palette_1a15[16 * 15];  // les autres 15 palettes restantes... 16 couleurs sur 2 octets * 15
} FormatPIC;

typedef struct
{
    // BITMAPFILEHEADER
    unsigned short int  Signature;                  // 2 octets pour la signature "BM" = 19778
    unsigned long int   Taille_Image;               // 4 octets pour la taille de l'image (pas toujours fiable)
    unsigned long int   Reserves;                   // 4 octets réservés
    unsigned long int   Offset_Image;               // 4 octets pour l'offset du début de l'image (en prenant en compte la taille de la palette)
    // DIB Header BITMAPV5HEADER
    unsigned long int   Entete2;                    // 4 octets pour la taille de la seconde entête (généralement 40 octets >> "0x28" )
    unsigned long int   Largeur_Image;              // 4 octets pour la largeur de l'image (colonne)
    unsigned long int   Longueur_Image;             // 4 octets pour la longueur de l'image (ligne)
    unsigned short int  Nbr_Plan;                   // 2 octets pour le nombre de plan (toujours à 1)
    unsigned short int  Nbr_Bit_Par_Pixel;          // 2 octets pour le nombre de bit par pixel (1=2 couleurs, 4=16 couleurs, 8=256 couleurs, 16,24,32)
    unsigned long int   Type_Compression;           // 4 octets pour le type de compression (0=aucune, 1=RLE-8, 2=RLE-4)
    unsigned long int   Taille_Map;                 // 4 octets pour la taille de la carte de bit en octet
    unsigned long int   Resolution_H;               // 4 octets pour résolution horizontale en pixel par mètre (pas toujours fiable)
    unsigned long int   Resolution_V;               // 4 octets pour résolution verticale en pixel par mètre (pas toujours fiable)
    unsigned long int   Nbr_Couleur_Image;          // 4 octets pour le nombre de couleurs dans l'image (ou 0 pour une image RVB)
    unsigned long int   Nbr_Couleur_Importante;     // 4 octets pour le nombre de couleurs importante dans l'image (0 si toutes)
} BMPHeader;

typedef struct
{
    // BITMAPFILEHEADER
    unsigned short int  Signature;                  // 2 octets pour la signature "BM" = 19778
    unsigned long int   Taille_Image;               // 4 octets pour la taille de l'image (pas toujours fiable)
    unsigned long int   Reserves;                   // 4 octets réservés
    unsigned long int   Offset_Image;               // 4 octets pour l'offset du début de l'image (en prenant en compte la taille de la palette)
    // DIB Header BITMAPV5HEADER
    unsigned long int   Entete2;                    // 4 octets pour la taille de la seconde entête (généralement 40 octets >> "0x28" )
    unsigned long int   Largeur_Image;              // 4 octets pour la largeur de l'image (colonne)
    unsigned long int   Longueur_Image;             // 4 octets pour la longueur de l'image (ligne)
    unsigned short int  Nbr_Plan;                   // 2 octets pour le nombre de plan (toujours à 1)
    unsigned short int  Nbr_Bit_Par_Pixel;          // 2 octets pour le nombre de bit par pixel (4=16 couleurs)
    unsigned long int   Type_Compression;           // 4 octets pour le type de compression (0=aucune, 1=RLE-8, 2=RLE-4)
    unsigned long int   Taille_Map;                 // 4 octets pour la taille de la carte de bit en octet
    unsigned long int   Resolution_H;               // 4 octets pour résolution horizontale en pixel par mètre (pas toujours fiable)
    unsigned long int   Resolution_V;               // 4 octets pour résolution verticale en pixel par mètre (pas toujours fiable)
    unsigned long int   Nbr_Couleur_Image;          // 4 octets pour le nombre de couleurs dans l'image (ou 0 pour une image RVB)
    unsigned long int   Nbr_Couleur_Importante;     // 4 octets pour le nombre de couleurs importante dans l'image (0 si toutes)
    // Color Table
    unsigned long int   Couleur_Palette_0[2];      // 0 - une seule palette de 16 couleurs sur 4 octets A=00 R=xx G=xx B=xx
    // BITMAP
    unsigned char       bitmap[200][40];           // 8000 car 2 pixels par octets -> 320/2*200 = 32000
} FormatBMP2;

typedef struct
{
    // BITMAPFILEHEADER
    unsigned short int  Signature;                  // 2 octets pour la signature "BM" = 19778
    unsigned long int   Taille_Image;               // 4 octets pour la taille de l'image (pas toujours fiable)
    unsigned long int   Reserves;                   // 4 octets réservés
    unsigned long int   Offset_Image;               // 4 octets pour l'offset du début de l'image (en prenant en compte la taille de la palette)
    // DIB Header BITMAPV5HEADER
    unsigned long int   Entete2;                    // 4 octets pour la taille de la seconde entête (généralement 40 octets >> "0x28" )
    unsigned long int   Largeur_Image;              // 4 octets pour la largeur de l'image (colonne)
    unsigned long int   Longueur_Image;             // 4 octets pour la longueur de l'image (ligne)
    unsigned short int  Nbr_Plan;                   // 2 octets pour le nombre de plan (toujours à 1)
    unsigned short int  Nbr_Bit_Par_Pixel;          // 2 octets pour le nombre de bit par pixel (4=16 couleurs)
    unsigned long int   Type_Compression;           // 4 octets pour le type de compression (0=aucune, 1=RLE-8, 2=RLE-4)
    unsigned long int   Taille_Map;                 // 4 octets pour la taille de la carte de bit en octet
    unsigned long int   Resolution_H;               // 4 octets pour résolution horizontale en pixel par mètre (pas toujours fiable)
    unsigned long int   Resolution_V;               // 4 octets pour résolution verticale en pixel par mètre (pas toujours fiable)
    unsigned long int   Nbr_Couleur_Image;          // 4 octets pour le nombre de couleurs dans l'image (ou 0 pour une image RVB)
    unsigned long int   Nbr_Couleur_Importante;     // 4 octets pour le nombre de couleurs importante dans l'image (0 si toutes)
    // Color Table
    unsigned long int   Couleur_Palette_0[16];      // 0 - une seule palette de 16 couleurs sur 4 octets A=00 R=xx G=xx B=xx
    // BITMAP
    unsigned char       bitmap[200][160];           // 32000 car 2 pixels par octets -> 320/2*200 = 32000
} FormatBMP;
// default value vor the struct FormatBMP
// { 19778, 32118, 0, 118, 40, 320, 200, 1, 4, 0, 32000, 3780, 3780, 0, 0}

typedef struct
{
    // BITMAPFILEHEADER
    unsigned short int  Signature;                  // 2 octets pour la signature "BM" = 19778
    unsigned long int   Taille_Image;               // 4 octets pour la taille de l'image (pas toujours fiable)
    unsigned long int   Reserves;                   // 4 octets réservés
    unsigned long int   Offset_Image;               // 4 octets pour l'offset du début de l'image (en prenant en compte la taille de la palette)
    // DIB Header BITMAPV5HEADER
    unsigned long int   Entete2;                    // 4 octets pour la taille de la seconde entête (généralement 40 octets >> "0x28" )
    unsigned long int   Largeur_Image;              // 4 octets pour la largeur de l'image (colonne)
    unsigned long int   Longueur_Image;             // 4 octets pour la longueur de l'image (ligne)
    unsigned short int  Nbr_Plan;                   // 2 octets pour le nombre de plan (toujours à 1)
    unsigned short int  Nbr_Bit_Par_Pixel;          // 2 octets pour le nombre de bit par pixel (8=256 couleurs)
    unsigned long int   Type_Compression;           // 4 octets pour le type de compression (0=aucune, 1=RLE-8, 2=RLE-4)
    unsigned long int   Taille_Map;                 // 4 octets pour la taille de la carte de bit en octet
    unsigned long int   Resolution_H;               // 4 octets pour résolution horizontale en pixel par mètre (pas toujours fiable)
    unsigned long int   Resolution_V;               // 4 octets pour résolution verticale en pixel par mètre (pas toujours fiable)
    unsigned long int   Nbr_Couleur_Image;          // 4 octets pour le nombre de couleurs dans l'image (ou 0 pour une image RVB)
    unsigned long int   Nbr_Couleur_Importante;     // 4 octets pour le nombre de couleurs importante dans l'image (0 si toutes)
    // Color Table
    unsigned long int   Couleur_Palettes[256];      // 0 - une seule palette de 256 couleurs sur 4 octets A=00 R=xx G=xx B=xx
    // BITMAP
    unsigned char       bitmap[200][320];           // 64000 car 1 pixel par octets -> 320*200 = 64000
} FormatBMP256;

/*
* structure from GIMP for BMP file, not used
* 
typedef struct _BitmapFileHeadGimp
{
    unsigned char       zzMagic[2];  // 00 "BM"
    unsigned long int   bfSize;      // 02
    unsigned short int  zzHotX;      // 06
    unsigned short int  zzHotY;      // 08
    unsigned long int   bfOffs;      // 0A
    unsigned long int   biSize;      // 0E
} BitmapFileHeadGimp;

typedef struct _BitmapHeadGimp
{
    long int            biWidth;     // 12
    long int            biHeight;    // 16
    unsigned short int  biPlanes;    // 1A
    unsigned short int  biBitCnt;    // 1C
    unsigned long int   biCompr;     // 1E
    unsigned long int   biSizeIm;    // 22
    unsigned long int   biXPels;     // 26
    unsigned long int   biYPels;     // 2A
    unsigned long int   biClrUsed;   // 2E
    unsigned long int   biClrImp;    // 32
    unsigned long int   masks[4];    // 36
} BitmapHeadGimp;

typedef struct _BitmapChannelGimp
{
    unsigned long int   mask;
    unsigned long int   shiftin;
    float               max_value;
} BitmapChannelGimp;
*/

#pragma pack()

typedef struct _tConvmArguments
{
    char           *pFullFilename;
    char           *pOutputPathname;
    unsigned int    uTabColumns;
    unsigned int    uMinSentenseLen;
    unsigned int    uSwapColumnA;
    unsigned int    uSwapColumnB;
    unsigned int    uNumerOfLine;
    unsigned int    uLargeur;
    unsigned int    uHauteur;
} tConvmArguments;

#endif

typedef struct _CustomPaletteHeader
{
    unsigned short int  uSignature;                  // 2 octets pour la signature "RM"
    unsigned short int  uNumberOfPalettes;
} CustomPaletteHeader;

typedef struct _CustomPalette
{
    unsigned char       uPaletteIndex;               // the palette number form 0 to 15
    unsigned char       uNumberOfUsage;              // number of usage of this palette not colled 
    unsigned char       uFromScbIndex;               // the 1st line
    unsigned char       uToScbIndex;                 // the last line
} CustomPalette;

typedef struct _tContextApp
{
    char                *pInputFileData;
    unsigned int         uInputFileSize;
    char                *pOutputFileData;
    unsigned int         uOutputFileSize;
} tContextApp;
