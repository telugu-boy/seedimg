__kernel void brightness_a(int intensity, __global uchar4* inp_pix, __global uchar4* res_pix) {
    
    ulong num = get_global_id(0);
    res_pix[num] = (uchar4)
        (
            (uchar)clamp((int)inp_pix[num].x + intensity, 0, 255),
            (uchar)clamp((int)inp_pix[num].y + intensity, 0, 255),
            (uchar)clamp((int)inp_pix[num].z + intensity, 0, 255),
            (uchar)clamp((int)inp_pix[num].w + intensity, 0, 255)
        );
} 
