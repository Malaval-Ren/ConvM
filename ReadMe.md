
# **ConvM**

![Alt text](./MM_PowerUp_256x256.png "ConvM") ![Alt text](./gplv3-127x51.png "license") 

## Convert many stuff

Just a little software to help cross dev for **Apple IIGS**

## help usage :

```bash
Usage: convm <convmspec> <option> "<filespec>" "<output folder or file>"

  <convmspec> is one of the following:
   -crlf                        - convert CR to LF
   -lfcr                        - convert LF to CR
   -dblf                        - replace 2 first $0A (LF) by one in a serie
   -dbcr                        - replace 2 first $0D (CR) by one in a serie
   -detab <col>                 - convert tabs to spaces (tab stop every COL columns)
   -dump                        - dump content of a supported file format
   -extxt <minlen>              - extract string of minlen from a binary file
   -rlec                        - not implemented
   -rled                        - decompress with rle algorithms file
   -2bmp                        - convert .scr, .shr, .pnt, .pic to .bmp
   -2pic                        - convert .bmp to .pic
   -brcc                        - reduce color chart from 0..256 in R G B (bmp) to 0..15 in R G B (pic)
   -rdic                        - Remove duplicate index to the same color
   -ncpl                        - display tle line number with color index not used in .pic and .bmp
   -apbm4                       - add palette to .bmp 16 to 256 colors
   -ipbm8 <lin> <lin>           - insert palette 16 colors to .bmp 256 colors at lines
   -swap <col> <col>            - swap 2 colors in palette and bitmap
   -cmpl                        - compare palette of bmp 4 bits per pixel
   -cppl                        - copy the 16 colors of bmp 4 bits per pixel and fist 16 colors of bmp 8 bits per pixel
   -xtrspr <col> <lin> <lin> <col> <lin> - extract sprite form BMP (4 bits per pixel) to text to .aii

  <option> is one of the following:
   +lower             - the output file name is in lower case

  <filespec> file extension could be:
   -crlf to -detab    : any type of text
   -dump              : any
   -rlec -rled        : .scr, .shr, .pnt, .pic
   -ncpl              : .pic, .bmp
   -apbm4 -ipbm8
   -swap -cmpl -cppl
   -brcc -rdic        : .bmp
```

## Convmspec Details

### -h

  Display help usage 

### -h

  Display version

### -crlf <file>

### -lfcr <file>

### -dblf <file>

### -dbcr <file>

### -detab <col> <file>

### -dump <file>

### -extxt <minlen> <file>

### -rled

### -2bmp

### -2pic

### -ncpl

### -ipbm4 

### -ipbm8 <lin> <lin> <file> <file>

  Copy 16 colors from the input file to the output file.
  if output file is a 4 bits per pixel convert it to 8 bits per pixel.
  ```bash
  ../../tools/convm.exe -ipbm8 1 2 ../dessin.bmp/tacadre.bmp ../dessin.bmp/tasetarc111.bmp
  ../../tools/convm.exe -ipbm8 1 3 ../dessin.bmp/tacadre.bmp ../dessin.bmp/tasetarc111.bmp
  ```

### -swap <col> <col> <file>

  Exchange 2 colors in the palette <col> <-> <col> and update offsets in bitmap
  The name of the output file the name is the input file name plus "-s.bmp"
  ```bash
  ../../tools/convm.exe -swap 17 26 ../../../Iron_Lord/dessin.bmp/tasetarc1.bmp
  ```

### -cmpl <file> <file>

### -cppl <file> <file>

### -xtrspr <col> <lin> <lin> <col> <lin> <file>


## Special thank's to :

co worker - **Frédéric Mure**

[Apple2Infinitum](https://app.slack.com/) - Digarok

[Brutal Deluxe](https://www.brutaldeluxe.fr/) - Antoine VIGNAU & Olivier ZARDINI

[The Byte Works](https://www.byteworks.us/Products.html) - Mike Westerfield

[GoldenGate](https://goldengate.gitlab.io/) - Kelvin W Sherlock


Have Fun :exclamation:
