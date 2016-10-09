
#ifdef _INTEL
float is_range(float pixel, float range, float min_val, float max_val, float out_val, float def_val) {
	float hi_val = pixel + range;
	float lo_val = pixel - range;
	if(hi_val > max_val) {
		out_val = def_val;
	}
	if(lo_val < min_val) {
		out_val = def_val;
	}
	return out_val;
}

__kernel void find_target(__read_write image2d_t in_image, 
		__global char *pos, sampler_t sampler, __global int *width)
{
	int2 coords_src = (int2)(get_global_id(0), get_global_id(1));
	float4 pixel = read_imagef(in_image, sampler, coords_src);
	char flag_val = 0;
	float4 outColor;

	outColor.x = is_range(pixel.x, 0.10 * pixel.x, 0.1, 0.8, pixel.x, pixel.x * 0.0);
	outColor.y = is_range(pixel.y, 0.10 * pixel.y, 0.1, 0.8, pixel.y, pixel.y * 0.0);
	outColor.z = is_range(pixel.z, 0.10 * pixel.z, 0.1, 0.8, pixel.z, pixel.z * 0.0);
	outColor.w = 1.0;
	if((pixel.x > 0.8) && (pixel.y > 0.6) && (pixel.z > 0.7)) {
		flag_val = 1;
//		outColor = (0.85, 0.85, 0.85, 1.0);
	} else {
		flag_val = 0;
//		outColor = (0.0, pixel.y, 0.0, 0.4);
	}
	pos[(coords_src.y * width[0]) + coords_src.x] = flag_val;
	write_imagef(in_image, coords_src, outColor);
}

__kernel void find_target2(__read_write image2d_t in_image, 
		__global char *pos, sampler_t sampler, __global int *width)
{
	int2 coords_src = (int2)(get_global_id(0), get_global_id(1));
	float4 pixel = read_imagef(in_image, sampler, coords_src);
	float4 outColor;

	if((pixel.x > 0.8) && (pixel.y > 0.6) && (pixel.z > 0.7)) {
		pos[(coords_src.y * width[0]) + coords_src.x] = 1;
		outColor = (0.85, 0.85, 0.85, 1.0);
	} else {
		pos[(coords_src.y * width[0]) + coords_src.x] = 0;
		//float4 outColor = (pixel.x, pixel.y, pixel.z, 0.7);
		outColor = (0.0, 0.0, 0.0, 0.4);
	}
	write_imagef(in_image, coords_src, outColor);
}
#else
// ok seem read/write for image is ocl2.0 feature
__kernel void find_target(__read_only image2d_t in_image, 
		__global char *pos, __global int *width,
		__write_only image2d_t out_image)
{
	const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	int2 coords_src = (int2)(get_global_id(0), get_global_id(1));
	float4 pixel = read_imagef(in_image, sampler, coords_src);
	char flag_val = 0;
	float4 outColor;
/*
	outColor.x = is_range(pixel.x, 0.10 * pixel.x, 0.1, 0.8, pixel.x, pixel.x * 0.0);
	outColor.y = is_range(pixel.y, 0.10 * pixel.y, 0.1, 0.8, pixel.y, pixel.y * 0.0);
	outColor.z = is_range(pixel.z, 0.10 * pixel.z, 0.1, 0.8, pixel.z, pixel.z * 0.0);
	outColor.w = 1.0;
	if((pixel.x > 0.8) && (pixel.y > 0.6) && (pixel.z > 0.7)) {
		flag_val = 1;
//		outColor = (0.85, 0.85, 0.85, 1.0);
	} else {
		flag_val = 0;
//		outColor = (0.0, pixel.y, 0.0, 0.4);
	}
	pos[(coords_src.y * width[0]) + coords_src.x] = flag_val;
	write_imagef(in_image, coords_src, outColor);
	*/

	write_imagef(out_image, coords_src, pixel);
}

#endif //_INTEL

float pixel_limit(float range, float in_val, float ck_val, float out_val, float def_val) {
	float chg = in_val * range;
	float chg_min = in_val - chg;
	float chg_max = in_val + chg;
	if(ck_val < chg_min) {
		return out_val;
	}
	if(ck_val > chg_max) {
		return out_val;
	}
	return def_val;
}

float4 pixelchange(float4 in_pixel, float4 ck_pixel) {
	float4 outColor;
	float range = 0.35;
	outColor.x = pixel_limit(range * 1.0, in_pixel.x, ck_pixel.x, 0.05, in_pixel.x * 1.0);
	outColor.y = pixel_limit(range * 1.1, in_pixel.y, ck_pixel.y, 0.05, in_pixel.y * 1.0);
	outColor.z = pixel_limit(range * 1.5, in_pixel.z, ck_pixel.z, 0.85, in_pixel.z * 1.0);
	outColor.w = 1.0;
	return outColor;
}

#ifdef _INTEL
// motion detect with red highlights
__kernel void find_motion(__read_only image2d_t in_image, __read_only image2d_t last_image, 
		__write_only image2d_t out_image, sampler_t sampler, __global char *pos, __global int *width)
{
	int2 coords_src = (int2)(get_global_id(0), get_global_id(1));
	int2 coords_dest = (int2)(get_global_id(0), get_global_id(1));

	float4 pixel1 = read_imagef(in_image, sampler, coords_src);
	float4 pixel2 = read_imagef(last_image, sampler, coords_src);
	float4 outColor = pixelchange(pixel1, pixel2);
	if((outColor.x != pixel1.x) && (outColor.y != pixel1.y) && (outColor.z != pixel1.z)) {
		pos[(coords_src.y * width[0]) + coords_src.x] = 8;
	}
	
	if(pos[(coords_src.y * width[0]) + coords_src.x] > 0) {
		pos[(coords_src.y * width[0]) + coords_src.x]--; // remove stale data
	} else {
		pos[(coords_src.y * width[0]) + coords_src.x] = 0;
	}
	write_imagef(out_image, coords_dest, pixel1);
}

__kernel void find_motion_with_color(__read_only image2d_t in_image, __read_only image2d_t last_image, 
		__write_only image2d_t out_image, sampler_t sampler, __global char *pos, __global int *width)
{
	int2 coords_src = (int2)(get_global_id(0), get_global_id(1));
	int2 coords_dest = (int2)(get_global_id(0), get_global_id(1));

	float4 pixel1 = read_imagef(in_image, sampler, coords_src);
	float4 pixel2 = read_imagef(last_image, sampler, coords_src);
	float4 outColor = pixelchange(pixel1, pixel2);
	if((outColor.x != pixel1.x) && (outColor.y != pixel1.y) && (outColor.z != pixel1.z)) {
		pos[(coords_src.y * width[0]) + coords_src.x] = 1;
	}
	
	if(pos[(coords_src.y * width[0]) + coords_src.x] > 0) {	
		write_imagef(out_image, coords_dest, outColor);
		pos[(coords_src.y * width[0]) + coords_src.x]--;
	} else {
		write_imagef(out_image, coords_dest, pixel1);
		pos[(coords_src.y * width[0]) + coords_src.x] = 0;
	}
}
#endif //_INTEL

#ifdef _FREESCALE
//[WIP]
// motion detect with red highlights
__kernel void find_motion(__read_only image2d_t in_image, __read_only image2d_t last_image, 
		__write_only image2d_t out_image, sampler_t sampler, global char *pos, global int *width)
{
	//const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	int2 coords_src = (int2)(get_global_id(0), get_global_id(1));
	int2 coords_dest = (int2)(get_global_id(0), get_global_id(1));
	float4 pixel1 = read_imagef(in_image, sampler, coords_src);
	float4 pixel2 = read_imagef(last_image, sampler, coords_src);
	float4 outColor = pixelchange(pixel1, pixel2);
	if((outColor.x != pixel1.x) && (outColor.y != pixel1.y) && (outColor.z != pixel1.z)) {
		pos[(coords_src.y * width[0]) + coords_src.x] = 5;
	}

	if(pos[(coords_src.y * width[0]) + coords_src.x] > 0) {	
		write_imagef(out_image, coords_dest, outColor);
		pos[(coords_src.y * width[0]) + coords_src.x]--;
	} else {
		write_imagef(out_image, coords_dest, pixel1);
		pos[(coords_src.y * width[0]) + coords_src.x] = 0;
	}
}
#endif //_FREESCALE

