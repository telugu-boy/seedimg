__kernel void grayscale_avg(__global uchar4* inp_pix, __global uchar4* res_pix, ulong amt_pixels) {
      int num = get_global_id(0);
      if(num >= amt_pixels)
          return;
      
      uchar avg = (inp_pix[num].x + inp_pix[num].y + inp_pix[num].z) / 3;
          
	  res_pix[num].x = avg;
      res_pix[num].y = avg;
      res_pix[num].z = avg;
} 
 
