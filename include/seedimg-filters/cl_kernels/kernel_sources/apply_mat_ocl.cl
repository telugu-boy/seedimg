__kernel void apply_mat(float16 matvec, __global uchar4* inp_pix, __global uchar4* res_pix) {
    
      ulong spw = SIMG_OCL_PXAMT;
      ulong start = get_global_id(0);
      ulong stop = start + spw * get_local_size(0);
      
      for(ulong num = start; num < stop; num+=get_local_size(0)) {
          
          if(start != num)
              break;
    //ulong num = get_global_id(0);
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
