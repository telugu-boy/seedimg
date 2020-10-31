__kernel void apply_mat(float16 matvec, __global uchar4* inp_pix, __global uchar4* res_pix) {
      ulong id = get_global_id(0) * SIMG_OCL_PXAMT;
      
      for(ulong num = id; num < id + SIMG_OCL_PXAMT; ++num) {
        uchar4 pix = inp_pix[num];
        
        res_pix[num].xyz = (uchar3)(
                        (uchar)clamp(
                        matvec.s0 * pix.x +
                        matvec.s4 * pix.y +
                        matvec.s8 * pix.z +
                        matvec.sc, 0.0f, 255.0f),
                    
                        (uchar)clamp(
                        matvec.s1 * pix.x +
                        matvec.s5 * pix.y +
                        matvec.s9 * pix.z +
                        matvec.sd, 0.0f, 255.0f),
        
                        (uchar)clamp(
                        matvec.s2 * pix.x +
                        matvec.s6 * pix.y +
                        matvec.sa * pix.z +
                        matvec.se, 0.0f, 255.0f));
        /*res_pix[num] = pix;*/
    }
}
