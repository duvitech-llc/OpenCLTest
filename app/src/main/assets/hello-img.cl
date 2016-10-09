

void addx(int a, int b, __global char *str, int idx)
{
	str[idx] = a + b + 1;
}

__kernel void hello(__global unsigned char* ostr, __global int *width)
{
	//unsigned int x = get_global_id(0);
	//unsigned int y = get_global_id(1);
	//unsigned int index = x + (y * width[0]);
	//unsigned int index = (get_global_id(1) * get_global_size(0)) + get_global_id(0);
	unsigned int index = get_global_id(0);
	if(index > width[0])
		return;
	//unsigned int index = x;
	if(ostr[index] > 215) {
		ostr[index] = 10;
	}
	if(ostr[index] < 10) {
		ostr[index] = 215;
	}
}

__kernel void imageinvert(__global unsigned char* ostr, const unsigned int nsize)
{
	//int x = get_global_id(0);
	//int y = get_global_id(1);
	//now invert colors
	int cnt = 0;
	for(cnt = 0; cnt < nsize; cnt++)
	{
		if(ostr[cnt] > 205) {
			ostr[cnt] = 10;
		}
		if(ostr[cnt] < 10) {
			ostr[cnt] = 205;
		}
	}
}

__kernel void copyimg(__read_only image2d_t srcImage, __write_only image2d_t destImage
		, sampler_t sampler)
{
	const sampler_t Xsampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
	int2 coords = (int2)(get_global_id(0), get_global_id(1));
	float4 pixel = read_imagef(srcImage, sampler, coords);
	//if(get_image_channel_order(srcImage) == CLK_RGBA)
	pixel.w = 1.0;
	if(pixel.x > 0.52) {
		pixel.x = pixel.x * 0.65;
	} else { // assume black
		pixel.x = (pixel.x + 0.025) * 1.8;
	}
	if(pixel.y > 0.56) {
		pixel.y = pixel.y * 0.65;
	} else { // assume black
		pixel.y = (pixel.y + 0.025) * 1.8;
	}
	if(pixel.z > 0.51) {
		pixel.z = pixel.z * 0.65;
	} else { // assume black
		pixel.z = (pixel.z + 0.025) * 1.8;
	}
	float4 outColor = (float4)(pixel.x, pixel.y, pixel.z, pixel.w); //r, g, b, a
	write_imagef(destImage, coords, outColor);
}

float4 wire(float4 in_pixel, float in_min, float in_max, float4 in_val) {
	float4 out_pixel;
	if(in_pixel.x < in_min || in_pixel.x > in_max)
		out_pixel.x = in_val.x;
	if(in_pixel.y < in_min || in_pixel.y > in_max)
		out_pixel.y = in_val.y;
	if(in_pixel.z < in_min || in_pixel.z > in_max)
		out_pixel.z = in_val.z;
	if(in_pixel.w < in_min || in_pixel.w > in_max)
		out_pixel.w = in_val.w;
	return out_pixel;
}

// Gaussian filter of image
__kernel void gaussian_filter(__read_only image2d_t srcImg,
		__write_only image2d_t dstImg,
		sampler_t sampler,
		int width, int height)
{
	// Gaussian Kernel is:
	// 1  2  1
	// 2  4  2
	// 1  2  1
	float kernelWeights[9] = { 1.0f, 2.0f, 1.0f,
		2.0f, 4.0f, 2.0f,
		1.0f, 2.0f, 1.0f };

	int2 startImageCoord = (int2) (get_global_id(0) - 1, get_global_id(1) - 1);
	int2 endImageCoord   = (int2) (get_global_id(0) + 1, get_global_id(1) + 1);
	int2 outImageCoord = (int2) (get_global_id(0), get_global_id(1));

	if (outImageCoord.x < width && outImageCoord.y < height)
	{
		int weight = 0;
		float4 outColor = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
		for( int y = startImageCoord.y; y <= endImageCoord.y; y++)
		{
			for( int x = startImageCoord.x; x <= endImageCoord.x; x++)
			{
				outColor += (read_imagef(srcImg, sampler, (int2)(x, y)) * (kernelWeights[weight] / 16.0f));
				weight += 1;
			}
		}

		// Write the output value to image
		write_imagef(dstImg, outImageCoord, outColor);
	}
}
