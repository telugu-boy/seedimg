def write_rh(l, typ, names, filename):
    arr = []
    for inner in range(len(l)):
        arr.append(f"static constexpr {typ} const {names[inner]}[] = {{ {', '.join(map(str, l[inner]))} }};")
        
    with open(filename, "w") as f:
        for s in arr:
            f.write(s + "\n")

#ycbcr_jpeg2rgb
#this will generate a .rh (resource header) in the respective folder, which can be included:
#it will have the nine lookup tables needed for this operation.
#which are: the nth row of the jpeg matrix * y, cb, cr of the pixel.
def gen_jpeg():
    jpeg_mat = [1.0, 0.0, 1.402, 1.0, -0.34414, -0.71414, 1.0, 1.772, 0.0]
    results_jpeg = [[] for _ in range(len(jpeg_mat))]
    for i in range(0, len(jpeg_mat), 3):
        for j in range(256):
            results_jpeg[i+0].append(int(j*jpeg_mat[i+0]))
            results_jpeg[i+1].append(int((j-128)*jpeg_mat[i+1]))
            results_jpeg[i+2].append(int((j-128)*jpeg_mat[i+2]))
    return results_jpeg

#read above comment
def gen_bt601():
    bt601_mat = [
    298.082 / 256.0, 0.0 / 256.0,      408.583 / 256.0,
    298.082 / 256.0, -100.291 / 256.0, -208.120 / 256.0,
    298.082 / 256.0, 516.412 / 256.0,  0.0 / 256.0]
    results_bt601 = [[] for _ in range(len(bt601_mat))]
    for i in range(0, len(bt601_mat), 3):
        for j in range(256):
            results_bt601[i+0].append(f"{j*bt601_mat[i+0]}f")
            results_bt601[i+1].append(f"{j*bt601_mat[i+1]}f")
            results_bt601[i+2].append(f"{j*bt601_mat[i+2]}f")
    return results_bt601

if __name__ == "__main__":
    write_rh(gen_jpeg(), "int", ["jpeg_ry1", "jpeg_gcb1", "jpeg_bcr1", 
                        "jpeg_ry2", "jpeg_gcb2", "jpeg_bcr2", 
                        "jpeg_ry3", "jpeg_gcb3", "jpeg_bcr3"], "ycbcr_jpeg_lut.rh")

    write_rh(gen_bt601(), "float", ["bt601_ry1", "bt601_gcb1", "bt601_bcr1", 
                        "bt601_ry2", "bt601_gcb2", "bt601_bcr2", 
                        "bt601_ry3", "bt601_gcb3", "bt601_bcr3"], "ycbcr_bt601_lut.rh")

