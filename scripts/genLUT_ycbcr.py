def write_rh(l, typ, names, filename):
    arr = []
    for inner in range(len(l)):
        arr.append(f"static constexpr {typ} const {names[inner]}[] = {{ {', '.join(map(str, l[inner]))} }};")
        
    with open("../src/seedimg-filters/seedimg-filters-cconv/" + filename, "w") as f:
        for s in arr:
            f.write(s + "\n")

#ycbcr_jpeg2rgb
#this will generate a .rh (resource header) in the respective folder, which can be included:
#it will have the nine lookup tables needed for this operation.
#which are: the nth row of the jpeg matrix * y, cb, cr of the pixel.
def gen_jpeg():
    jpeg_mat = [1.0, 0.0, 1.402, 
                1.0, -0.34414, -0.71414, 
                1.0, 1.772, 0.0]
    results_jpeg = [[] for _ in range(len(jpeg_mat))]
    for i in range(0, len(jpeg_mat), 3):
        for j in range(256):
            results_jpeg[i+0].append(int(j*jpeg_mat[i+0]))
            results_jpeg[i+1].append(int((j-128)*jpeg_mat[i+1]))
            results_jpeg[i+2].append(int((j-128)*jpeg_mat[i+2]))
    return results_jpeg


if __name__ == "__main__":
    write_rh(gen_jpeg(), "int", ["jpeg_ry1", "jpeg_gcb1", "jpeg_bcr1", 
                        "jpeg_ry2", "jpeg_gcb2", "jpeg_bcr2", 
                        "jpeg_ry3", "jpeg_gcb3", "jpeg_bcr3"], "from_ycbcr_jpeg_lut.rh")
    print("Done.")

