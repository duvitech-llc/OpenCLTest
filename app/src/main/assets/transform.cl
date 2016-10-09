
float4 wire(float4 in_pixel, float4 in_val) {
	float in_min = 0.955;
  	float in_max = 0.111;
	float4 out_pixel;
	if(in_pixel.x > in_val.x)
		out_pixel.x = in_pixel.x - in_val.x;
	if(in_pixel.y > in_val.y)
		out_pixel.y = in_pixel.y - in_val.y;
	if(in_pixel.z > in_val.z)
		out_pixel.z = in_pixel.z - in_val.z;

	if(in_pixel.x < in_val.x)
		out_pixel.x = in_val.x - in_pixel.x;
	if(in_pixel.y < in_val.y)
		out_pixel.y = in_val.y - in_pixel.y;
	if(in_pixel.z < in_val.z)
		out_pixel.z = in_val.z - in_pixel.z;
	out_pixel.w = 1.0;
	return clamp(out_pixel, 0.001, 0.955);
}

__kernel void trans1(__read_only image2d_t in_image, __read_only image2d_t last_image, 
		__write_only image2d_t out_image, sampler_t sampler)
{
	const sampler_t Xsampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	int2 coords_src = (int2)(get_global_id(0), get_global_id(1));
	int2 coords_dest = (int2)(get_global_id(0), get_global_id(1));

	float4 pixel;
	float4 pixel1 = read_imagef(in_image, sampler, coords_src);
	float4 pixel2 = read_imagef(last_image, sampler, coords_dest);

	pixel.x = pixel1.x + pixel2.x;
	pixel.y = pixel1.y; // + pixel2.y;
	pixel.z = pixel1.z + pixel2.z;
	pixel.w = pixel1.w;
	float4 outColor = clamp(pixel, 0.001, 0.955);
	
	//float4 outColor = (float4)(pixel.x, pixel.y, pixel.z, pixel.w); //r, g, b, a
	write_imagef(out_image, coords_dest, outColor);
}

__kernel void trans2(__read_only image2d_t in_image, __read_only image2d_t last_image, 
		__write_only image2d_t out_image, sampler_t sampler)
{
	const sampler_t Xsampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	int2 coords_src = (int2)(get_global_id(0), get_global_id(1));
	int2 coords_dest = (int2)(get_global_id(0), get_global_id(1));

	float4 in_pixel = read_imagef(in_image, sampler, coords_src);
	float4 last_pixel = read_imagef(last_image, sampler, coords_dest);
	float4 outColor = wire(in_pixel, last_pixel);
	
	write_imagef(out_image, coords_dest, outColor);
}

// blur
__kernel void blur3(__read_only image2d_t in_image, __read_only image2d_t last_image, 
		__write_only image2d_t out_image, sampler_t sampler)
{
	const sampler_t Xsampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	int2 coords_src1 = (int2)(get_global_id(0) + 1, get_global_id(1) + 1);
	int2 coords_src2 = (int2)(get_global_id(0) - 1, get_global_id(1) - 1);
	int2 coords_dest = (int2)(get_global_id(0), get_global_id(1));

	float4 pixel1 = read_imagef(in_image, Xsampler, coords_src1);
	float4 pixel2 = read_imagef(last_image, Xsampler, coords_src2);

	float4 outColor = (pixel1 + pixel2)/2;
	
	write_imagef(out_image, coords_dest, outColor);
}

// blur 2
__kernel void blur4(__read_only image2d_t in_image, __read_only image2d_t last_image, 
		__write_only image2d_t out_image, sampler_t sampler)
{
	const sampler_t Xsampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	int2 coords_src1 = (int2)(get_global_id(0) + 1, get_global_id(1) + 1);
	int2 coords_src2 = (int2)(get_global_id(0) - 1, get_global_id(1) - 1);
	int2 coords_dest = (int2)(get_global_id(0), get_global_id(1));

	float4 pixel1 = read_imagef(in_image, Xsampler, coords_src1);
	float4 pixel2 = read_imagef(last_image, Xsampler, coords_src2);

	float4 outColor = (pixel1 + clamp(pixel2, 0.001, 0.325))/2;
	
	write_imagef(out_image, coords_dest, outColor);
}

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

// motion detect with red highlights
__kernel void motion5(__read_only image2d_t in_image, __read_only image2d_t last_image, 
		__write_only image2d_t out_image, sampler_t sampler)
{
	int2 coords_src = (int2)(get_global_id(0), get_global_id(1));
	int2 coords_dest = (int2)(get_global_id(0), get_global_id(1));

	float4 pixel1 = read_imagef(in_image, sampler, coords_src);
	float4 pixel2 = read_imagef(last_image, sampler, coords_src);
	float4 outColor = pixelchange(pixel1, pixel2);
	
	write_imagef(out_image, coords_dest, outColor);
}

float4 pixelchange2(float4 in_pixel, float4 ck_pixel) {
	float4 outColor;
	float range = 0.35;
	outColor.x = pixel_limit(range * 1.0, in_pixel.x, ck_pixel.x, ck_pixel.x, in_pixel.x * 0.0);
	outColor.y = pixel_limit(range * 1.1, in_pixel.y, ck_pixel.y, ck_pixel.y, in_pixel.y * 0.0);
	outColor.z = pixel_limit(range * 1.5, in_pixel.z, ck_pixel.z, ck_pixel.z, in_pixel.z * 0.0);
	outColor.w = 1.0;
	return outColor;
}

__kernel void trans6(__read_only image2d_t in_image, __read_only image2d_t last_image, 
		__write_only image2d_t out_image, sampler_t sampler)
{
	int2 coords_src = (int2)(get_global_id(0), get_global_id(1));
	int2 coords_dest = (int2)(get_global_id(0), get_global_id(1));

	float4 pixel1 = read_imagef(in_image, sampler, coords_src);
	float4 pixel2 = read_imagef(last_image, sampler, coords_src);
	float4 outColor = pixelchange2(pixel1, pixel2);
	
	write_imagef(out_image, coords_dest, outColor);
}

