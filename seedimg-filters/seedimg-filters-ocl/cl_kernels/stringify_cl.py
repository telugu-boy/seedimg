import sys

def stringify(iname, oname):
  res_str = "";
  with open(iname, "r") as i, open(oname, "w") as o:
    for l in i:
      res_str += '"' + l.rstrip() + "\"\n"
    o.write(res_str)
    
if __name__ == "__main__":
    # .cl files are suffixed with _ocl.cl
    # resulting files are suffixed with _kernel.hpp
    names = ["rotate_hue"]
    for name in names:
        stringify(name+"_ocl.cl", name+"_kernel.hpp")
