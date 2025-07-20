/* SPDX - License - Identifier: GPL - 3.0 - or -later
 *
 * A tool to help cross dev for Apple II GS.
 *
 * Copyright(C) 2024 ChatGPT
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


/*
phase donner a chatgpt

Code en C pour modifier une image au format bmp avec 8 bits par pixel, de 320 pixels par ligne, 200 ligne maximum.
R�duire le nombre de couleur par ligne a 16 au maximum, en cr�ant des couleurs interm�diaire entre 2 couleurs existant dans la ligne.
Le nombre total de couleurs est de 256 maximum. Ce qui veux dire un maximum de 16 lignes de 16 couleurs.

il me faut quelques am�lioration :
l'octet de poids fort de la structure RGBQUAD est le champ rgbReserved et reste � z�ro.
la variable new_colors doit �tre initialiser � z�ro
Les couleurs interm�diaire doit �tre calculer sur 2 couleurs proche des 16 couleurs pour une ligne

Je souhaite que le caract�re { soit � la ligne et un espace toujours apr�s le caract�re (

l'image de r�sultat est trop fonc�, peux tu augment� les contrastes.
D�place les couleurs non utiliser apr�s la derni�re couleur utiliser.

tu as oubli� cette regle dans ton nouveau code :
Espace apr�s les parenth�ses ( : Un espace est ajout� apr�s chaque parenth�se ouvrante (.


Synthese de chatgpt
Voici une synth�se des modifications apport�es et des aspects sp�cifiques � tester ou ajuster pour votre projet BMP :

Synth�se des Modifications
Cr�ation des Palettes :

Fonction create_palettes :
G�n�ration de palettes de 16 couleurs chacune pour une image BMP de 256 couleurs.
D�grad�s d�finis pour diff�rentes parties de l'image :
�tendard : D�grad� de vert.
Chevalier : D�grad� de gris.
Cape et Selle : D�grad� de rouge.
Bouclier : Couleur bleue uniforme.
Herbe : D�grad� de vert du vert clair au vert fonc�.
Application des Palettes :

Fonction map_palette_for_line :
Application de la sous-palette appropri�e � chaque ligne de l'image BMP.
Utilisation de l'index de ligne pour s�lectionner la sous-palette.
D�grad�s de Couleurs :

Fonction generate_gradient :
Cr�ation de d�grad�s de couleurs pour les palettes.
Param�trage des couleurs de d�but et de fin pour les d�grad�s.
Format du Code :

Espaces ajout�s apr�s toutes les parenth�ses ouvrantes (.
Correction des niveaux d'indirection pour �viter les avertissements de compilation.
Gestion des Couleurs Non Utilis�es :

Les couleurs non utilis�es doivent �tre d�plac�es apr�s la derni�re couleur utilis�e dans la palette BMP.
Suppression de l'Augmentation du Contraste :

Retrait de la fonctionnalit� d'augmentation du contraste, retour � la gestion normale des couleurs.
Aspects � Tester ou Ajuster
Validation des Palettes :

Assurez-vous que les palettes sont correctement cr��es avec les d�grad�s et les couleurs d�finies.
V�rifiez que les palettes sont appliqu�es correctement � chaque partie de l'image.
Application Correcte des Sous-Palettes :

Testez avec diff�rentes images BMP pour v�rifier si les sous-palettes sont bien appliqu�es aux lignes correspondantes.
Assurez-vous que les indices de couleur sont mapp�s correctement et que les couleurs de l'image sont visibles comme pr�vu.
R�sultats Visuels :

Comparez l'image r�sultante avec l'image originale pour v�rifier les changements visuels.
Assurez-vous que les parties sp�cifiques de l'image (comme l'�tendard, le chevalier, et l'herbe) montrent les d�grad�s et les couleurs attendus.
Gestion des Couleurs Non Utilis�es :

V�rifiez que les couleurs non utilis�es sont correctement d�plac�es dans la palette BMP, ce qui pourrait n�cessiter une r�vision du mappage des indices de couleur.
Code et Compilation :

Assurez-vous que le code se compile sans avertissements ou erreurs.
Testez diff�rentes configurations pour v�rifier la robustesse du code.
V�rification des D�grad�s :

Testez les d�grad�s avec diff�rentes configurations pour s'assurer qu'ils apparaissent correctement dans l'image finale.
Documentation pour Reprendre le Travail
Code Source :
Conservez le code source complet avec les modifications apport�es.
Configurations et Param�tres :
Notez les configurations sp�cifiques utilis�es pour les palettes et les d�grad�s.
Observations :
Documentez les r�sultats observ�s lors des tests, y compris les diff�rences avec l'image originale.
Questions ou Probl�mes :
Notez toute question ou probl�me rencontr� pour pouvoir les adresser lors de la reprise du travail.
Avec cette synth�se, vous devriez pouvoir reprendre le projet facilement et effectuer les ajustements n�cessaires. N'h�sitez pas � poser d'autres questions si vous avez besoin de plus d'aide !

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

// _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 4996)

// BMP file header structure
#pragma pack ( push, 1 )
typedef struct
{
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BITMAPFILEHEADER;

typedef struct
{
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizepImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BITMAPINFOHEADER;

typedef struct
{
    uint8_t rgbBlue;
    uint8_t rgbGreen;
    uint8_t rgbRed;
    uint8_t rgbReserved;
} RGBQUAD;
#pragma pack ( pop )

// Function prototypes
void create_palettes ( RGBQUAD palettes[16][16] );
void map_palette_for_line ( RGBQUAD* palette, uint8_t* pImage, int width, int line, int num_palettes );
void generate_gradient ( RGBQUAD* palette, uint8_t start_r, uint8_t start_g, uint8_t start_b, uint8_t end_r, uint8_t end_g, uint8_t end_b, int num_colors );

int reducenbrcolors( int argc, char* argv[] )
{
    if ( argc != 3 )
    {
        printf ( "Usage: %s <input.bmp> <output.bmp>\n", argv[0] );
        return 1;
    }

    FILE                *pFileIn = NULL;
    FILE                *pFileOut = NULL;
    uint8_t             *pImage = NULL;
    BITMAPFILEHEADER     fileHeader;
    BITMAPINFOHEADER     infoHeader;
    RGBQUAD              palette[256];
    RGBQUAD              palettes[16][16];

    // Open input file
    if ( fopen_s ( &pFileIn, argv[1], "rb" ) != 0 )
    {
        perror ( "Error opening input file" );
        return 1;
    }

    // Read file header
    if (fread( &fileHeader, sizeof( BITMAPFILEHEADER), 1, pFileIn) != sizeof( BITMAPFILEHEADER))
    {
        perror ( "Error reading file BITMAPFILEHEADER" );
        fclose ( pFileIn );
        return 1;
    }

    if ( fileHeader.bfType != 0x4D42 )
    {
        fprintf ( stderr, "Error: Not a BMP file\n" );
        fclose ( pFileIn );
        return 1;
    }

    // Read info header
    if ( fread ( &infoHeader, sizeof ( BITMAPINFOHEADER ), 1, pFileIn ) != sizeof( BITMAPINFOHEADER))
    {
        perror ( "Error reading info BITMAPINFOHEADER" );
        fclose ( pFileIn );
        return 1;
    }

    // Check if it's an 8-bit BMP
    if ( infoHeader.biBitCount != 8 )
    {
        fprintf ( stderr, "Error: Only 8-bit BMP files are supported\n" );
        fclose ( pFileIn );
        return 1;
    }

    // Read palette
    fread ( palette, sizeof ( RGBQUAD ), 256, pFileIn );

    // Ensure rgbReserved is zero
    for ( int i = 0; i < 256; i++ )
    {
        palette[i].rgbReserved = 0;
    }

    // Allocate memory for the pImage
    int imageSize = infoHeader.biWidth * abs ( infoHeader.biHeight );
    pImage = ( uint8_t *)malloc ( imageSize );
    if ( pImage == NULL )
    {
        perror ( "Error allocating memory" );
        fclose ( pFileIn );
        return 1;
    }

    // Read pImage data
    fread ( pImage, sizeof ( uint8_t ), imageSize, pFileIn);
    if ( pFileIn)
        fclose ( pFileIn );

    // Create color palettes for different parts of the pImage
    create_palettes ( palettes );

    // Process each line
    for ( int y = 0; y < abs ( infoHeader.biHeight ); y++ )
    {
        int palette_index = y / ( abs ( infoHeader.biHeight ) / 16 );
        map_palette_for_line ( palettes[palette_index], pImage, infoHeader.biWidth, y, 16 );
    }

    // Open output file
    if ( fopen_s( &pFileOut, argv[2], "wb" ) != 0 )
    {
        perror ( "Error opening output file" );
        free ( pImage );
        return 1;
    }

    // Write file header
    fwrite ( &fileHeader, sizeof ( BITMAPFILEHEADER ), 1, pFileOut );

    // Write info header
    fwrite ( &infoHeader, sizeof ( BITMAPINFOHEADER ), 1, pFileOut );

    // Write palette
    for ( int i = 0; i < 256; i++ )
    {
        fwrite ( &palette[i], sizeof ( RGBQUAD ), 1, pFileOut );
    }

    // Write pImage data
    fwrite ( pImage, sizeof ( uint8_t ), imageSize, pFileOut );
    if ( pFileOut)
        fclose ( pFileOut );

    // Free memory
    if ( pImage)
        free ( pImage );

    return 0;
}

void create_palettes ( RGBQUAD palettes[16][16] )
{
    // Define gradients and colors for each palette
    generate_gradient ( palettes[0], 0, 100, 0, 0, 255, 0, 16 ); // Green gradient for banner
    generate_gradient ( palettes[1], 50, 50, 50, 200, 200, 200, 16 ); // Gray gradient for knight
    generate_gradient ( palettes[2], 150, 0, 0, 255, 0, 0, 16 ); // Red gradient for cape and saddle
    for ( int i = 0; i < 16; i++ )
    {
        palettes[3][i].rgbRed = 0;
        palettes[3][i].rgbGreen = 0;
        palettes[3][i].rgbBlue = 255;
        palettes[3][i].rgbReserved = 0; // Blue for shield
    }
    generate_gradient ( palettes[4], 0, 100, 0, 0, 50, 0, 16 ); // Green gradient for grass

    // Fill remaining palettes with default colors (optional)
    for ( int i = 5; i < 16; i++ )
    {
        generate_gradient ( palettes[i], 0, 0, 0, 255, 255, 255, 16 ); // Example gradient from black to white
    }
}

void map_palette_for_line ( RGBQUAD* palette, uint8_t* pImage, int width, int line, int num_palettes )
{
    // Map line color indices to new palette indices
    for ( int x = 0; x < width; x++ )
    {
        uint8_t color_index = pImage[line * width + x];
        // Map the color index to the correct sub-palette
        pImage[line * width + x] = color_index % num_palettes; // Ensure index is within palette bounds
    }
}

void generate_gradient ( RGBQUAD* palette, uint8_t start_r, uint8_t start_g, uint8_t start_b, uint8_t end_r, uint8_t end_g, uint8_t end_b, int num_colors )
{
    for ( int i = 0; i < num_colors; i++ )
    {
        float ratio = ( float )i / ( num_colors - 1 );
        palette[i].rgbRed = ( uint8_t )( start_r + ratio * ( end_r - start_r ) );
        palette[i].rgbGreen = ( uint8_t )( start_g + ratio * ( end_g - start_g ) );
        palette[i].rgbBlue = ( uint8_t )( start_b + ratio * ( end_b - start_b ) );
        palette[i].rgbReserved = 0;
    }
}
