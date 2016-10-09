
#ifdef _INTEL
float4 yuv2rgb_convert1(float4 in_pixel, __global float *m_mx) 
{
	float4 out_pixel1;
	float base = 255.0;
	float a = 16.0/base;
	float b = 128.0/base;
	out_pixel1.x = m_mx[4] * (in_pixel.x - a) +                              m_mx[0] * (in_pixel.y - b);
	out_pixel1.y = m_mx[4] * (in_pixel.x - a) - m_mx[1] * (in_pixel.w - a) - m_mx[2] * (in_pixel.y - b);
	out_pixel1.z = m_mx[4] * (in_pixel.x - a) + m_mx[3] * (in_pixel.w - a);
	out_pixel1.w = 1.0; // a
	return out_pixel1;
}

float4 yuv2rgb_convert2(float4 in_pixel, __global float *m_mx) 
{
	float4 out_pixel2;
	float base = 255.0;
	float a = 16.0/base;
	float b = 128.0/base;
	out_pixel2.x = m_mx[4] * (in_pixel.z - a) +                              m_mx[0] * (in_pixel.y - b);
	out_pixel2.y = m_mx[4] * (in_pixel.z - a) - m_mx[1] * (in_pixel.w - a) - m_mx[2] * (in_pixel.y - b);
	out_pixel2.z = m_mx[4] * (in_pixel.z - a) + m_mx[3] * (in_pixel.w - a);
	out_pixel2.w = 1.0; // a
	return out_pixel2;
}

__kernel void convert(__read_only image2d_t srcImage2, 
		__write_only image2d_t destImage , sampler_t sampler, __read_only __global float *m_mx)
{
	//char Y = current[0]; char U = current[1];
	//char Y2 = current[2]; char V = current[3];

	int2 in_coords = (int2)(get_global_id(0), get_global_id(1));
	float4 in_pixel = read_imagef(srcImage2, sampler, in_coords);

	int2 out_coords1 = (int2)(get_global_id(0) * 2, get_global_id(1));
	float4 out_pixel1 = yuv2rgb_convert1(in_pixel, m_mx);
	if((out_coords1.x < 640) && (out_coords1.y < 480)) {
		write_imagef(destImage, out_coords1, out_pixel1);
	}
	int2 out_coords2 = (int2)(out_coords1.x+1, out_coords1.y);
	float4 out_pixel2 = yuv2rgb_convert2(in_pixel, m_mx);
	if((out_coords2.x < 640) && (out_coords2.y < 480)) {
		write_imagef(destImage, out_coords2, out_pixel2);
	}
}
#endif //_INTEL

#ifdef _FREESCALE
float4 yuv2rgb_convert1(float4 in_pixel, float *m_mx) 
{
	float4 out_pixel1;
	float base = 255.0;
	float a = 16.0/base;
	float b = 128.0/base;
	out_pixel1.x = m_mx[4] * (in_pixel.x - a) +                              m_mx[0] * (in_pixel.y - b);
	out_pixel1.y = m_mx[4] * (in_pixel.x - a) - m_mx[1] * (in_pixel.w - a) - m_mx[2] * (in_pixel.y - b);
	out_pixel1.z = m_mx[4] * (in_pixel.x - a) + m_mx[3] * (in_pixel.w - a);
	out_pixel1.w = 1.0; // a
	return out_pixel1;
}

float4 yuv2rgb_convert2(float4 in_pixel, float *m_mx) 
{
	float4 out_pixel2;
	float base = 255.0;
	float a = 16.0/base;
	float b = 128.0/base;
	out_pixel2.x = m_mx[4] * (in_pixel.z - a) +                              m_mx[0] * (in_pixel.y - b);
	out_pixel2.y = m_mx[4] * (in_pixel.z - a) - m_mx[1] * (in_pixel.w - a) - m_mx[2] * (in_pixel.y - b);
	out_pixel2.z = m_mx[4] * (in_pixel.z - a) + m_mx[3] * (in_pixel.w - a);
	out_pixel2.w = 1.0; // a
	return out_pixel2;
}

__kernel void convert(__read_only image2d_t srcImage2, 
		__write_only image2d_t destImage , sampler_t sampler)
{
	//char Y = current[0]; char U = current[1];
	//char Y2 = current[2]; char V = current[3];
	float m_mx[5] = { 0.75, 0.2, 2.15, 0.499501, 1.164, };

	int2 in_coords = (int2)(get_global_id(0), get_global_id(1));
	float4 in_pixel = read_imagef(srcImage2, sampler, in_coords);

	//printf("mx0 =  %f\n", m_mx[0]);
	int2 out_coords1 = (int2)(get_global_id(0) * 2, get_global_id(1));
	float4 out_pixel1 = yuv2rgb_convert1(in_pixel, m_mx);
	if((out_coords1.x < 640) && (out_coords1.y < 480)) {
		write_imagef(destImage, out_coords1, out_pixel1);
	}
	int2 out_coords2 = (int2)(out_coords1.x+1, out_coords1.y);
	float4 out_pixel2 = yuv2rgb_convert2(in_pixel, m_mx);
	if((out_coords2.x < 640) && (out_coords2.y < 480)) {
		write_imagef(destImage, out_coords2, out_pixel2);
	}
}
#endif //_FREESCALE

