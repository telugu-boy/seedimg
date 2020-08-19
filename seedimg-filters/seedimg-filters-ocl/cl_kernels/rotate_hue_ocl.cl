uchar clamp(float i){
    if(i > 255.0f)
        return 255;
    else if(i < 0.0f)
        return 0;
    else
        return (uchar)i;
}

__kernel void rotate_hue(__constant float* hue_kernel, __global uchar4* inp_pix, __global uchar4* res_pix) {
      int num = get_global_id(0);
	  res_pix[num].x = clamp(
                    hue_kernel[0] * inp_pix[num].x +
                    hue_kernel[1] * inp_pix[num].y +
                    hue_kernel[2] * inp_pix[num].z
                  );
      res_pix[num].y = clamp(
                    hue_kernel[3] * inp_pix[num].x +
                    hue_kernel[4] * inp_pix[num].y +
                    hue_kernel[5] * inp_pix[num].z
                  );
      res_pix[num].z = clamp(
                    hue_kernel[6] * inp_pix[num].x +
                    hue_kernel[7] * inp_pix[num].y +
                    hue_kernel[8] * inp_pix[num].z
                  );
}