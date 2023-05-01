#!/bin/bash
#
# Clean Visual Studio folders
#
# renaud.malaval@free.fr
#
version='1.2'

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
	echo -e $BPurple " Clean Visual Studio stufs" $Color_Off
	echo
else
echo -e $BPurple
	iix gothic " Clean Visual Studio stufs"
echo -e $Color_Off
fi

originFolder=$(pwd)
project_Name=$(basename "$originFolder")

echo
echo -e $Green $project_Name $Color_Off
echo

if [[ -f "$originFolder""/""$project_Name"".sln" ]]
then
	echo -e $Green "Deleting files" $Color_Off
	if [[ -f "$originFolder""/"".data" ]]
	then
		rm -f "$originFolder""/"".data"
	fi
	if [[ -f "$originFolder""/"".pdata" ]]
	then
		rm -f "$originFolder""/"".pdata"
	fi
	if [[ -f "$originFolder""/"".rdata" ]]
	then
		rm -f "$originFolder""/"".rdata"
	fi
	if [[ -f "$originFolder""/"".reloc" ]]
	then
		rm -f "$originFolder""/"".reloc"
	fi
	if [[ -f "$originFolder""/"".text" ]]
	then
		rm -f "$originFolder""/"".text"
	fi

	echo
	echo -e $Green "Deleting directories" $Color_Off
	if [[ -d "$originFolder""/.vs/""$project_Name""/v16" ]]
	then
		rm -frd "$originFolder""/.vs/""$project_Name""/v16"
	fi
	if [[ -d "$originFolder""/""$project_Name""/Debug" ]]
	then
		rm -frd "$originFolder""/""$project_Name""/Debug"
	fi
	if [[ -d "$originFolder""/""$project_Name""/Release" ]]
	then
		rm -frd "$originFolder""/""$project_Name""/Release"
	fi
	if [[ -d "$originFolder""/""$project_Name""/x64" ]]
	then
		rm -frd "$originFolder""/""$project_Name""/x64"
	fi
	if [[ -d "$originFolder""/Debug" ]]
	then
		rm -frd "$originFolder""/Debug"
	fi
	if [[ -d "$originFolder""/Release" ]]
	then
		rm -frd "$originFolder""/Release"
	fi
	if [[ -d "$originFolder""/.rsrc" ]]
	then
		rm -frd "$originFolder""/.rsrc"
	fi

	if [[ -f "$originFolder""/x64/Release/""$project_Name"".exe" ]]
	then
		echo -e $Green "Copy release to up directory" $Color_Off
		cp -fp "$originFolder""/x64/Release/""$project_Name"".exe" "$originFolder""/../""$project_Name"".exe"
	fi

	if [[ -d "$originFolder""/x64" ]]
	then
		rm -frd "$originFolder""/x64"
	fi

	echo
	echo -e $BGreen "Done" $Color_Off
else
	echo
	echo -e $BGreen "Wrong folder, nothing is done" $Color_Off
fi

exit $NO_ERROR
