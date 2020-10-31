import sys
import os

def stringify(iname, oname):
  with open(iname, "r") as i, open(oname, "w") as o:
    o.write('R"(' + i.read().replace("SIMG_OCL_PXAMT", ')" SIMG_EXPAND_WRAP_IN_QUOTES(SIMG_OCL_PXAMT) R"(') + ')"');
    
if __name__ == "__main__":
    # .cl files are suffixed with _ocl.cl
    # resulting files are suffixed with _kernel.clh
    names = [
        "apply_mat",
        "rgb2hsv",
        "hsv2rgb",
    ]
    try:
        os.mkdir("../include/seedimg-filters/cl_kernels")
        os.mkdir("../include/seedimg-filters/cl_kernels")
    except:
        pass
    for name in names:
        stringify("../include/seedimg-filters/cl_kernels/kernel_sources/"+name+"_ocl.cl",
        "../include/seedimg-filters/cl_kernels/"+name+"_kernel.clh")
    
    print("Done.")
