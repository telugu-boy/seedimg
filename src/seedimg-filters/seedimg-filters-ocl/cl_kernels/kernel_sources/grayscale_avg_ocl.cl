__kernel void grayscale_avg(__global uchar4* inp_pix, __global uchar4* res_pix) {
      int num = get_global_id(0);
      if(num >= get_global_size(0))
          return;
      
      uchar avg = (res_pix[num].x + res_pix[num].y + res_pix[num].z) / 3;
          
	  res_pix[num].x = avg;
      res_pix[num].y = avg;
      res_pix[num].z = avg;
} 
 
