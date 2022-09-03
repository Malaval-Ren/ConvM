#!/bin/bash
#
# convert *.shr file to *.pic
#
# renaud.malaval@free.fr
#
version='1.0'

# definition all colors and styles to use with an echo

# Reset
Color_Off='\033[0m'       # Text Reset

# Regular Colors
Black='\033[0;30m'        # Black
Red='\033[0;31m'          # Red
Green='\033[0;32m'        # Green
Yellow='\033[0;33m'       # Yellow
Blue='\033[0;34m'         # Blue
Purple='\033[0;35m'       # Purple
Cyan='\033[0;36m'         # Cyan
White='\033[0;37m'        # White

# Bold
BBlack='\033[1;30m'       # Black
BRed='\033[1;31m'         # Red
BGreen='\033[1;32m'       # Green
BYellow='\033[1;33m'      # Yellow
BBlue='\033[1;34m'        # Blue
BPurple='\033[1;35m'      # Purple
BCyan='\033[1;36m'        # Cyan
BWhite='\033[1;37m'       # White

# Underline
UBlack='\033[4;30m'       # Black
URed='\033[4;31m'         # Red
UGreen='\033[4;32m'       # Green
UYellow='\033[4;33m'      # Yellow
UBlue='\033[4;34m'        # Blue
UPurple='\033[4;35m'      # Purple
UCyan='\033[4;36m'        # Cyan
UWhite='\033[4;37m'       # White

# Background
On_Black='\033[40m'       # Black
On_Red='\033[41m'         # Red
On_Green='\033[42m'       # Green
On_Yellow='\033[43m'      # Yellow
On_Blue='\033[44m'        # Blue
On_Purple='\033[45m'      # Purple
On_Cyan='\033[46m'        # Cyan
On_White='\033[47m'       # White

# High Intensity
IBlack='\033[0;90m'       # Black
IRed='\033[0;91m'         # Red
IGreen='\033[0;92m'       # Green
IYellow='\033[0;93m'      # Yellow
IBlue='\033[0;94m'        # Blue
IPurple='\033[0;95m'      # Purple
ICyan='\033[0;96m'        # Cyan
IWhite='\033[0;97m'       # White

# Bold High Intensity
BIBlack='\033[1;90m'      # Black
BIRed='\033[1;91m'        # Red
BIGreen='\033[1;92m'      # Green
BIYellow='\033[1;93m'     # Yellow
BIBlue='\033[1;94m'       # Blue
BIPurple='\033[1;95m'     # Purple
BICyan='\033[1;96m'       # Cyan
BIWhite='\033[1;97m'      # White

# High Intensity backgrounds
On_IBlack='\033[0;100m'   # Black
On_IRed='\033[0;101m'     # Red
On_IGreen='\033[0;102m'   # Green
On_IYellow='\033[0;103m'  # Yellow
On_IBlue='\033[0;104m'    # Blue
On_IPurple='\033[0;105m'  # Purple
On_ICyan='\033[0;106m'    # Cyan
On_IWhite='\033[0;107m'   # White

# how to do a if of numeric or string
#            Shell Script
#Boolean Operator     Numeric     String
#===================  =======     ======
#Equals                 -eq        =
#Not Equals             -ne        !=
#Greater Than           -gt        >
#Less Than              -lt        <
#Greater or Equals      -ge        >=
#Less Than or Equals    -le        <=

#define exit value 0 succes, other failed
NO_ERROR=0

ERROR_SH=50
ERROR_RECOIL=100
ERROR_DEBUG=200

ERROR_SH_param=$(($ERROR_SH+1))
ERROR_SH_deprend=$(($ERROR_SH+2))
ERROR_SH_reorg=$(($ERROR_SH+3))
ERROR_SH_folder=$(($ERROR_SH+4))
ERROR_SH_nothing=$(($ERROR_SH+5))

aError=$NO_ERROR

# Clear the terminal screen
# printf "\033c"

# update it following your directories
convm='/g/Collector/_Apple IIgs/_DiskCrackBand_/tools/convm.exe'

# ie
# folders input and output are in the same folder
#./searchConvert2bmp.sh ../dessin_test/ ../dessin_bmp/

originFolder=$(pwd)

filelist=""

checkingExt="*.pic"

convertedNbr=0

iixPresent=$(where iix)
if [ -z "$iixPresent" ]
then
	echo
	echo -e $BPurple " Graphic convert .pic to .bmp" $Color_Off
	echo
else
echo -e $BPurple
	iix gothic " Graphic convert .pic to .bmp"
echo -e $Color_Off
fi

echo -e $BGreen "Your eyes are alone to check if it's a true success!" $Color_Off

echo
if [[ ! -f $convm ]]
then
	echo
	echo -e $BRed "$convm.exe does not exist" $Color_Off	
	echo
	exit $ERROR_SH_folder
fi

if [ ! -d "$1" ]  
then
	echo
	echo -e $BRed "$1 Source does not exist" $Color_Off	
	echo
	exit $ERROR_SH_folder
fi

if [ ! -d "$2" ]  
then
	echo
	echo -e $BRed "$2 Target does not exist" $Color_Off	
	echo
	exit $ERROR_SH_folder
fi

cd "$1"
echo
ls
echo

# ###################################################################################################################### 
# from : https://stackoverflow.com/questions/14505047/loop-through-all-the-files-with-a-specific-extension
# -iname is like -name, but the match is case insensitive

# for file in `find . -type f \( -iname \$checkingExt -o -iname \$checkingExt2 \)`
for file in `find . -type f -iname "$checkingExt"`	# recursive but missing base founded
do
	filepath=$(echo "$file")
	filepath="${filepath#?}" 	# removes first character
	filepath="${filepath#?}" 	# removes first character
	srcpathname=$sourcefolder$filepath
    # echo "$srcpathname"
	filelist=$filelist" "$srcpathname
done

if [ -z "$filelist" ]
then
	cd "$originFolder"
	echo
	echo -e $BYellow "No files with extension" "$checkingExt" "found" $Color_Off
	echo
	exit $ERROR_SH_param
else
	echo $filelist
fi

# Iterate the string variable using for loop
for findedFile in $filelist
do
	# echo -e $Green $findedFile '\t\t' $( basename $findedFile ) $Color_Off

	checkoriginfile="./"$( basename $findedFile )	# just file name
	# echo -e $Green $checkoriginfile $Color_Off

	echo -e $Green "Doing : " "$convm" "-rled" "+lower" "$checkoriginfile" "$2" $Color_Off
	# "$convm" "-rled" "+lower" $checkoriginfile 2> /dev/null
	"$convm" "-2bmp" "+lower" "$checkoriginfile" "$2"
	aError=$?
	if [ $aError -ne 0 ]
	then
		echo
		echo -e $BYellow "Picture :" "$checkoriginfile" "not compatible. error =" $aError $Color_Off
		echo
	else
		convertedNbr=$((convertedNbr+1))
	fi
	echo

done

cd "$originFolder"

echo
echo -e $BGreen "Converted" $convertedNbr "files is done" $Color_Off

sleep 1  #Wait 1 seconds

exit $NO_ERROR























if [ $# -eq 1 ]
then
	if [[ "$1" = "-atari" ]]
   then
		sourcefolder="../../Atari.IronLord/"
		extensioncatalog="./Atari_ext.txt"
		checkingext="*.ACP"		# *.CMP no good result; *.ACP somes ressult but nothing good 
		targetfolder="./"
	elif [[ "$1" = "-amiga" ]]  
   then
		sourcefolder="../../Amiga.IronLord/"
		extensioncatalog="./Amiga_ext.txt"
		checkingext="*.raw"
		targetfolder="./"
	else
		echo
		echo -e $BRed "Parameter :" $1 "is not managed" $Color_Off	
		echo
		exit $ERROR_SH_param		
	fi
else
	echo
	echo -e $BRed "One parameter is necessary {-atari} or {-amiga}" $Color_Off	
	echo
	exit $ERROR_SH_param
fi

# ###################################################################################################################### 

echo
echo -e $BGreen "Triing to convert all files found in" "$sourcefolder" "with ext" "$checkingext" "to here" "$targetfolder" $Color_Off
echo -e $BGreen "Your eyes are alone to check if it's a true success!" $Color_Off

echo
if [[ ! -f $recoil ]]
then
	echo
	echo -e $BRed "recoil2png.exe does not exist" $Color_Off	
	echo
   exit $ERROR_SH_folder
fi

if [ ! -d "$originFolder" ]  
then
	echo
	echo -e $BRed "$originFolder does not exist" $Color_Off	
	echo
   exit $ERROR_SH_folder
fi

if [ ! -d "$sourcefolder" ]  
then
	echo
	echo -e $BRed "$sourcefolder does not exist" $Color_Off	
	echo
   exit $ERROR_SH_folder
fi

if [ ! -d "$targetfolder" ]  
then
	echo
	echo -e $BRed "$targetfolder does not exist" $Color_Off	
	echo
   exit $ERROR_SH_folder
fi

cd $sourcefolder

# ###################################################################################################################### 
# from : https://stackoverflow.com/questions/14505047/loop-through-all-the-files-with-a-specific-extension
# -iname is like -name, but the match is case insensitive
for file in `find . -type f -iname "$checkingext"`	# recursive but missing base founded
do
	filepath=$(echo "$file")
	filepath="${filepath#?}" 	# removes first character
	filepath="${filepath#?}" 	# removes first character
	srcpathname=$sourcefolder$filepath
   # echo "$srcpathname"
	filelist=$filelist" "$srcpathname
done

# echo -e $Green "The ext is" "$checkingext" $Color_Off

if [ -z "$filelist" ]
then
	# ls
	echo
	echo -e $BYellow "No files with extension" "$checkingext" "found" $Color_Off
	echo
	exit $ERROR_SH_param
# else
	# echo $filelist
fi

# echo $filelist

cd $originFolder

# Iterate the string variable using for loop
for findedFile in $filelist
do
   # echo -e $Green $findedFile '\t\t' $( basename $findedFile ) $Color_Off

	checkoriginfile="./"$( basename $findedFile )	# just file name
   # echo -e $Green $checkoriginfile $Color_Off
	cp -f $findedFile $checkoriginfile

	# echo -e $Green "doing : " "$recoil" $checkoriginfile $Color_Off
	"$recoil" $checkoriginfile 2> /dev/null
	aError=$?
	if [ $aError -ne 0 ]
	then
		successCounter=0
		# Iterate the extention from file variable using for loop
		inputExt=$extensioncatalog
		while IFS= read -r line_ext
		do
			foo=$( basename $findedFile )				# just file name
			bar=${foo/./-}									# replace '.' by '-'
			checkfile="./"$bar'.'${line_ext::-1}	# new name "./" $bar '.' extension without last char

			cp -f $checkoriginfile $checkfile

			foo=$( basename $findedFile )						# just file name
			bar=${foo/./-}											# replace '.' by '-'
			outputfile="./"$bar'-'${line_ext::-1}".png"	# new name "./" $bar '-' extension without last char ".png"

	      # echo -e $Green "doing : " $recoil $checkfile -o "$outputfile" $Color_Off
			"$recoil" "-o" "$outputfile" "$checkfile" 2> /dev/null
			aError=$?
			if [ $aError -ne 0 ]
			then
				# echo -e $Yellow "Failed with ext : ""${line_ext::-1}""\t delete : "${outputfile} $Color_Off
				rm -f $checkfile
				rm -f $outputfile
			else
				echo
				echo -e $Green "Success from "$checkoriginfile '\tTO\t' $outputfile $Color_Off
				aError=$NO_ERROR
				successCounter=$((successCounter+1))
			fi
		done < "$inputExt"

		if [ $aError -ne 0 ] && [ $successCounter -eq 0 ];
		then
			echo
			echo -e $Yellow "All converts have failed for" "$checkoriginfile" $Color_Off
			rm -f $checkoriginfile
			aError=$ERROR_SH_nothing
		fi
	else
		echo
		echo -e $BGreen "Success for" "$checkoriginfile" $Color_Off
		aError=$NO_ERROR
	fi
	
	echo

done

echo -e $BGreen "All works are done" $Color_Off

sleep 1  #Wait 1 seconds

exit $NO_ERROR
