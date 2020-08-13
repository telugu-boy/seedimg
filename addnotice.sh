#!/bin/bash

# inset a file content at the beginning of each matching files

DIR='./'
HEADER_FILE='NOTICE'

#the matching files will be ignored

IGNORE_FILE="*.addnotice.sh"

insertHeader(){
   local targetfile=$1
   local headerfile=$2
   echo "$targetfile"
   while read line
    do
#          echo $line
           sed -i "$targetfile" -e "1i$line\ "
    done< <(tac "$headerfile")

}

export -f insertHeader
find "$DIR" -type f -name "*.hpp" ! -name "$IGNORE_FILE" -exec bash -c "insertHeader {} $HEADER_FILE"  \;
