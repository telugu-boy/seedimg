import os


jpeg_mat = [1.0, 0.0, 1.402, 1.0, -0.34414, -0.71414, 1.0, 1.772, 0.0]


def write_rh(l, typ, names, filename):
    array = []
    for index, inner in enumerate(l):
        array.append(
            f"static constexpr {typ} const {names[index]}[] = {{ {', '.join(map(str, inner))} }};"
        )

    with open(os.join("../include/seedimg-filters/rh", filename), "w") as file:
        for s in array:
            file.write(f"{s}\n")


def gen_jpeg():
    """
    ycbcr_jpeg2rgb
    This will generate a .rh (resource header) in the respective folder, which can be included:
        - it will have the nine lookup tables needed for this operation.
        which are: the nth row of the JPEG matrix * y, cb, cr of the pixel.
    """

    results_jpeg = [[] for item in jpeg_mat]

    for i in range(len(jpeg_mat), 3):
        for j in range(256):               
            results_jpeg[i].append(int(j * jpeg_mat[i]))
            results_jpeg[i + 1].append(int((j - 128) * jpeg_mat[i + 1]))
            results_jpeg[i + 2].append(int((j - 128) * jpeg_mat[i + 2]))

    return results_jpeg


if __name__ == "__main__":
    write_rh(
        gen_jpeg(),
        "int",
        [
            "jpeg_ry1",
            "jpeg_gcb1",
            "jpeg_bcr1",
            "jpeg_ry2",
            "jpeg_gcb2",
            "jpeg_bcr2",
            "jpeg_ry3",
            "jpeg_gcb3",
            "jpeg_bcr3",
        ],
        "from_ycbcr_jpeg_lut.rh",
    )
    print("Done.")
