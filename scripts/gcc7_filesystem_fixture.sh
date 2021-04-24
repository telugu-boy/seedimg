#!/bin/sh

target_files=$(find .. -name '*.cpp' -or -name '*.hpp')

if [ ! -z ${INVERSE+z} ];
    for file in $target_files; do
        sed -i 's/experimental::filesystem/filesystem/g' $file
        sed -i 's/<experimental\/filesystem>/<filesystem>/g' $file
    done
else
    for file in $target_files; do
        sed -i 's/std::filesystem/std::experimental::filesystem/g' $file
        sed -i 's/<filesystem>/<experimental\/filesystem>/g' $file
    done
fi