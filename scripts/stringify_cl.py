import sys
import os

def stringify(iname, oname):
  with open(iname, "r") as i, open(oname, "w") as o:
    o.write('R"(' + i.read() + ')\"');
    
if __name__ == "__main__":
    # .cl files are suffixed with _ocl.cl
    # resulting files are suffixed with _kernel.clh
    names = [
        "filters/rotate_hue",
        "filters/grayscale_lum",
        "filters/grayscale_avg",
        
        "cconv/rgb2hsv",
        "cconv/hsv2rgb",
    ]
    try:
        os.mkdir("../src/seedimg-filters/seedimg-filters-ocl/cl_kernels/cconv")
        os.mkdir("../src/seedimg-filters/seedimg-filters-ocl/cl_kernels/filters")
    except:
        pass
    for name in names:
        stringify("../src/seedimg-filters/seedimg-filters-ocl/cl_kernels/kernel_sources/"+name+"_ocl.cl",
        "../src/seedimg-filters/seedimg-filters-ocl/cl_kernels/"+name+"_kernel.clh")
    
    print("Done.")
