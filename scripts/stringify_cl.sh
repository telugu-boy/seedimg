#!/bin/sh

cl_kernel_ns="../include/seedimg-filters/cl_kernels"

for file in $(find $cl_kernel_ns/kernel_sources -type f -name '*_ocl.cl'); do
    outfile="$cl_kernel_ns/$(basename ${file%*_ocl.cl})_kernel.clh"

    printf 'R"(' > $outfile
    sed 's/SIMG_OCL_PXAMT/)" SIMG_EXPAND_WRAP_IN_QUOTES(SIMG_OCL_PXAMT) R"(/g' $file >> $outfile
    printf ')"' >> $outfile
done