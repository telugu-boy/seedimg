uchar clmp_(float i){
    return (i > 255.0f) * 255 +
           (i < 0.0f) * 0 +
           (0.0f < i && i < 255.0f) * i;
}

__kernel void apply_mat(float16 matvec, float4 vecvec, __global uchar4* inp_pix, __global uchar4* res_pix, ulong amt_pixels) {
      int num = get_global_id(0);
      if(num >= amt_pixels)
          return;
      
      uchar4 pix = inp_pix[num];
      
	  res_pix[num].x = clmp_(vecvec.x +
                    matvec.s0 * pix.x +
                    matvec.s1 * pix.y +
                    matvec.s2 * pix.z +
                    matvec.s3 * pix.w);
                  
      res_pix[num].y = clmp_(vecvec.y +
                    matvec.s4 * pix.x +
                    matvec.s5 * pix.y +
                    matvec.s6 * pix.z +
                    matvec.s7 * pix.w);
      
      res_pix[num].z = clmp_(vecvec.z +
                    matvec.s8 * pix.x +
                    matvec.s9 * pix.y +
                    matvec.sa * pix.z +
                    matvec.sb * pix.w);
                  
      res_pix[num].w = clmp_(vecvec.w +
                    matvec.sc * pix.x +
                    matvec.sd * pix.y +
                    matvec.se * pix.z +
                    matvec.sf * pix.w);
                  
}
