#!/bin/sh

jpeg_matrix="1.0 0.0 1.402 1.0 -0.34414 -0.71414 1.0 1.772 0.0"
jpeg_mattype="ry1 gcb1 bcr1 ry2 gcb2 bcr2 ry3 gcb3 bcr3"

yuv_lutfile="../include/seedimg-filters/rh/from_ycbcr_jpeg_lut.rh"

for jmat in $jpeg_mattype; do
    printf "static constexpr int const $jmat[] = {" > $yuv_lutfile
    
    jpeg_lutout=()

    for x in $(seq 1 3 ${#jpeg_matrix[*]})
        for y in $(seq 256); do
            jpeg_lutout+=( (( $y * $jpeg_matix[$x] )) 
                           (( ($y-128) * $jpeg_matix[$x + 1] ))
                           (( ($y-128) * $jpeg_matix[$x + 2] )) )
        done
    done

    IFS=, ; printf "${jpeg_lutout[*]}" >> $yuv_lutfile
    echo '};' >> $yuv_lutfile
done

