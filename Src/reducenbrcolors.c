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
Réduire le nombre de couleur par ligne a 16 au maximum, en créant des couleurs intermédiaire entre 2 couleurs existant dans la ligne.
Le nombre total de couleurs est de 256 maximum. Ce qui veux dire un maximum de 16 lignes de 16 couleurs.

il me faut quelques amélioration :
l'octet de poids fort de la structure RGBQUAD est le champ rgbReserved et reste à zéro.
la variable new_colors doit être initialiser à zéro
Les couleurs intermédiaire doit être calculer sur 2 couleurs proche des 16 couleurs pour une ligne

Je souhaite que le caractère { soit à la ligne et un espace toujours après le caractère (

l'image de résultat est trop foncé, peux tu augmenté les contrastes.
Déplace les couleurs non utiliser après la dernière couleur utiliser.

tu as oublié cette regle dans ton nouveau code :
Espace après les parenthèses ( : Un espace est ajouté après chaque parenthèse ouvrante (.


Synthese de chatgpt
Voici une synthèse des modifications apportées et des aspects spécifiques à tester ou ajuster pour votre projet BMP :

Synthèse des Modifications
Création des Palettes :

Fonction create_palettes :
Génération de palettes de 16 couleurs chacune pour une image BMP de 256 couleurs.
Dégradés définis pour différentes parties de l'image :
Étendard : Dégradé de vert.
Chevalier : Dégradé de gris.
Cape et Selle : Dégradé de rouge.
Bouclier : Couleur bleue uniforme.
Herbe : Dégradé de vert du vert clair au vert foncé.
Application des Palettes :

Fonction map_palette_for_line :
Application de la sous-palette appropriée à chaque ligne de l'image BMP.
Utilisation de l'index de ligne pour sélectionner la sous-palette.
Dégradés de Couleurs :

Fonction generate_gradient :
Création de dégradés de couleurs pour les palettes.
Paramétrage des couleurs de début et de fin pour les dégradés.
Format du Code :

Espaces ajoutés après toutes les parenthèses ouvrantes (.
Correction des niveaux d'indirection pour éviter les avertissements de compilation.
Gestion des Couleurs Non Utilisées :

Les couleurs non utilisées doivent être déplacées après la dernière couleur utilisée dans la palette BMP.
Suppression de l'Augmentation du Contraste :

Retrait de la fonctionnalité d'augmentation du contraste, retour à la gestion normale des couleurs.
Aspects à Tester ou Ajuster
Validation des Palettes :

Assurez-vous que les palettes sont correctement créées avec les dégradés et les couleurs définies.
Vérifiez que les palettes sont appliquées correctement à chaque partie de l'image.
Application Correcte des Sous-Palettes :

Testez avec différentes images BMP pour vérifier si les sous-palettes sont bien appliquées aux lignes correspondantes.
Assurez-vous que les indices de couleur sont mappés correctement et que les couleurs de l'image sont visibles comme prévu.
Résultats Visuels :

Comparez l'image résultante avec l'image originale pour vérifier les changements visuels.
Assurez-vous que les parties spécifiques de l'image (comme l'étendard, le chevalier, et l'herbe) montrent les dégradés et les couleurs attendus.
Gestion des Couleurs Non Utilisées :

Vérifiez que les couleurs non utilisées sont correctement déplacées dans la palette BMP, ce qui pourrait nécessiter une révision du mappage des indices de couleur.
Code et Compilation :

Assurez-vous que le code se compile sans avertissements ou erreurs.
Testez différentes configurations pour vérifier la robustesse du code.
Vérification des Dégradés :

Testez les dégradés avec différentes configurations pour s'assurer qu'ils apparaissent correctement dans l'image finale.
Documentation pour Reprendre le Travail
Code Source :
Conservez le code source complet avec les modifications apportées.
Configurations et Paramètres :
Notez les configurations spécifiques utilisées pour les palettes et les dégradés.
Observations :
Documentez les résultats observés lors des tests, y compris les différences avec l'image originale.
Questions ou Problèmes :
Notez toute question ou problème rencontré pour pouvoir les adresser lors de la reprise du travail.
Avec cette synthèse, vous devriez pouvoir reprendre le projet facilement et effectuer les ajustements nécessaires. N'hésitez pas à poser d'autres questions si vous avez besoin de plus d'aide !

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
