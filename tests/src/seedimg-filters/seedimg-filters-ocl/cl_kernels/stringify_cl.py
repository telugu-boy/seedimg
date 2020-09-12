import sys
import pathlib

def stringify(iname, oname):
  res_str = "";
  with open(iname, "r") as i, open(oname, "w") as o:
    for l in i:
      res_str += '"' + l.rstrip() + "\"\n"
    o.write(res_str)
    
if __name__ == "__main__":
    # .cl files are suffixed with _ocl.cl
    # resulting files are suffixed with _kernel.clh
    names = [
        "rotate_hue",
        "grayscale_lum",
        "grayscale_avg",
    ]
    dirname = str(pathlib.Path(__file__).parent.absolute()) + '/'
    for name in names:
        stringify(dirname + "kernel_sources/"+name+"_ocl.cl", dirname+name+"_kernel.clh")
    
    print("Done stringifying kernels")
