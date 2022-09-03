#!/bin/bash
#
# basic unit test for convm
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
ERROR_UNITTEST=100
ERROR_DEBUG=200

ERROR_SH_param=$(($ERROR_SH+1))
ERROR_SH_deprend=$(($ERROR_SH+2))
ERROR_SH_reorg=$(($ERROR_SH+3))
ERROR_SH_folder=$(($ERROR_SH+4))
ERROR_SH_nothing=$(($ERROR_SH+5))

aError=$NO_ERROR

# Clear the terminal screen
# printf "\033c"

iixPresent=$(where iix)
if [ -z "$iixPresent" ]
then
	echo
	echo -e $BPurple " Unit Test CONVM" $Color_Off
	echo
else
echo -e $BPurple
	iix gothic " Unit Test CONVM"
echo -e $Color_Off
fi

originFolder=$(pwd -P)
ut_src_folder=`pwd`"/unittest"
ut_work_folder=`pwd`"/ut_work"

if [[ ! -d $ut_src_folder ]]
then
	echo
	echo -e $BRed $convm "does not exist" $Color_Off
	echo
	exit $ERROR_SH_folder
fi

if [[ ! -d "$ut_work_folder" ]]
then
	mkdir "$ut_work_folder"
	if [[ ! -d $ut_work_folder ]]
	then
		exit $ERROR_SH_folder
	fi
fi

convm='/g/Collector/_Apple IIgs/_DiskCrackBand_/tools/x64/Debug/convm.exe'
if [[ ! -f $convm ]]
then
	convm='/g/Collector/_Apple IIgs/_DiskCrackBand_/tools/x64/Release/convm.exe'
	if [[ ! -f $convm ]]
	then
		convm=$originFolder'/../convm.exe'
		if [[ ! -f $convm ]]
		then
			echo
			echo -e $BRed $convm "does not exist" $Color_Off	
			echo
			exit $ERROR_SH_folder
		fi
	fi
fi

UnitTestNbr=0

# "$convm" $checkoriginfile 2> /dev/null
# TEST : not enough parameters
"$convm"
aError=$?
if [ $aError -eq 0 ]
then
	exit $ERROR_UNITTEST
fi
UnitTestNbr=$((UnitTestNbr+1))
echo

# TEST : illegal convmspec parameter
"$convm" "-toto" "$ut_work_folder/tartemolle.scr"
aError=$?
if [ $aError -eq 0 ]
then
	exit $ERROR_UNITTEST
fi
UnitTestNbr=$((UnitTestNbr+1))
echo

# TEST : illegal option parameter
"$convm" "-RlEd" "+upper" "$ut_work_folder/tartemolle.shr"
aError=$?
if [ $aError -eq 0 ]
then
	exit $ERROR_UNITTEST
fi
UnitTestNbr=$((UnitTestNbr+1))
echo

# TEST : input file does not exist
"$convm" "-RlEd" "$ut_work_folder/tartemolle.pic"
aError=$?
if [ $aError -eq 0 ]
then
	exit $ERROR_UNITTEST
fi
UnitTestNbr=$((UnitTestNbr+1))
echo

# TEST : size equal to 0 for file
touch "$ut_work_folder/tartemolle.pnt"
"$convm" "-RlEd" "$ut_work_folder/tartemolle.pnt"
aError=$?
if [ $aError -eq 0 ]
then
	exit $ERROR_UNITTEST
fi
if [[ -f "$ut_work_folder/tartemolle.pnt" ]]
then
	rm -f "$ut_work_folder/tartemolle.pnt"
fi
UnitTestNbr=$((UnitTestNbr+1))
echo

# TEST : file without extension
cp -f "$ut_src_folder/RED.DRAGON.gif" "$ut_work_folder/REDDRAGON"
"$convm" "-RlEd" "$ut_work_folder/REDDRAGON"
aError=$?
if [ $aError -eq 0 ]
then
	exit $ERROR_UNITTEST
fi
if [[ -f "$ut_work_folder/REDDRAGON" ]]
then
	rm -f "$ut_work_folder/REDDRAGON"
fi
UnitTestNbr=$((UnitTestNbr+1))
echo

# TEST : file extension not supported
cp -f "$ut_src_folder/RED.DRAGON.gif" "$ut_work_folder/RED.DRAGON.tst"
"$convm" "-RlEd" "$ut_work_folder/RED.DRAGON.tst"
aError=$?
if [ $aError -eq 0 ]
then
	exit $ERROR_UNITTEST
fi
if [[ -f "$ut_work_folder/RED.DRAGON.tst" ]]
then
	rm -f "$ut_work_folder/RED.DRAGON.tst"
fi
UnitTestNbr=$((UnitTestNbr+1))
echo

# TEST : file extension not supported
cp -f "$ut_src_folder/LADY.TUT.SHR" "$ut_work_folder/LADYTUT.abc"
"$convm" "-RlEd" "$ut_work_folder/LADYTUT.abc"
aError=$?
if [ $aError -eq 0 ]
then
	exit $ERROR_UNITTEST
fi
if [[ -f "$ut_work_folder/LADYTUT.abc" ]]
then
	rm -f "$ut_work_folder/LADYTUT.abc"
fi
UnitTestNbr=$((UnitTestNbr+1))
echo

# TEST : <convmspec> not implemented
cp -f "$ut_src_folder/LADY.TUT.SHR" "$ut_work_folder/LADY.TUT.SHR"
"$convm" "-RlEc" "$ut_work_folder/LADY.TUT.SHR"
aError=$?
if [ $aError -eq 0 ]
then
	exit $ERROR_UNITTEST
fi
if [[ -f "$ut_work_folder/LADY.TUT.SHR" ]]
then
	rm -f "$ut_work_folder/LADY.TUT.SHR"
fi
UnitTestNbr=$((UnitTestNbr+1))
echo

# TEST : command already done on this file  "-rled"  "red.dragon.pic"
cp -f "$ut_src_folder/RED.DRAGON.SCR" "$ut_work_folder/RED.DRAGON.pic"
"$convm" "-RlEd" "$ut_work_folder/RED.DRAGON.pic"
aError=$?
if [ $aError -eq 0 ]
then
	exit $ERROR_UNITTEST
fi
if [[ -f "$ut_work_folder/RED.DRAGON.pic" ]]
then
	rm -f "$ut_work_folder/RED.DRAGON.pic"
fi
UnitTestNbr=$((UnitTestNbr+1))
echo

# TEST : command already done on this file  "-rled"  "red.dragon.scr"
cp -f "$ut_src_folder/RED.DRAGON.SCR" "$ut_work_folder/RED.DRAGON.SCR"
"$convm" "-RlEd" "$ut_work_folder/RED.DRAGON.SCR"
aError=$?
if [ $aError -eq 0 ]
then
	exit $ERROR_UNITTEST
fi
if [[ -f "$ut_work_folder/RED.DRAGON.SCR" ]]
then
	rm -f "$ut_work_folder/RED.DRAGON.SCR"
fi
UnitTestNbr=$((UnitTestNbr+1))
echo

# TEST : file not compatible with command  "-rled"  "astronut.bmp"
cp -f "$ut_src_folder/ASTRONUT.bmp" "$ut_work_folder/ASTRONUT.bmp"
"$convm" "-RlEd" "$ut_work_folder/ASTRONUT.bmp"
aError=$?
if [ $aError -eq 0 ]
then
	exit $ERROR_UNITTEST
fi
if [[ -f "$ut_work_folder/ASTRONUT.bmp" ]]
then
	rm -f "$ut_work_folder/ASTRONUT.bmp"
fi
UnitTestNbr=$((UnitTestNbr+1))
echo

# TEST : command already done on this file  "-2bmp"  "astronut.bmp"
cp -f "$ut_src_folder/ASTRONUT.bmp" "$ut_work_folder/astronut.bmp"
"$convm" "-2bmp" "$ut_work_folder/astronut.bmp"
aError=$?
if [ $aError -eq 0 ]
then
	exit $ERROR_UNITTEST
fi
if [[ -f "$ut_work_folder/astronut.bmp" ]]
then
	rm -f "$ut_work_folder/astronut.bmp"
fi
UnitTestNbr=$((UnitTestNbr+1))
echo


echo
echo -e $BGreen " " $UnitTestNbr "Unit tests from bach script are on SUCCESS !" $Color_Off

sleep 1  #Wait 1 seconds

exit $NO_ERROR
