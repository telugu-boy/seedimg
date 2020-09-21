inline float fmodulo(float x, float N) {
  return fmod((fmod(x, N) + N), N);
}

__kernel void rgb2hsv(ulong amt_pixels, __global uchar4* inp_pix, __global uchar4* res_pix) {
      int num = get_global_id(0);
      if(num >= amt_pixels)
          return;
      
      uchar rmax = max(inp_pix[num].x, max(inp_pix[num].y, inp_pix[num].z));
      float rp = (float)inp_pix[num].x / 255.0f;
      float gp = (float)inp_pix[num].y / 255.0f;
      float bp = (float)inp_pix[num].z / 255.0f;
      float cmax = fmax(rp, fmax(gp, bp));
      float cmin = fmin(rp, fmin(gp, bp));
      float delta = cmax - cmin;
      
      res_pix[num].x = 
      (inp_pix[num].x == rmax) * (uchar)(30*fmodulo((gp-bp)/delta,6.0f)) +
      (inp_pix[num].y == rmax) * (uchar)(60*((bp-rp)/delta+2)) +
      (inp_pix[num].z == rmax) * (uchar)(60*((rp-gp)/delta+4));
      res_pix[num].y = (rmax != 0) * (uchar)((delta/cmax)*100.0f);
      res_pix[num].z = (uchar)(cmax * 100.0f);
}
