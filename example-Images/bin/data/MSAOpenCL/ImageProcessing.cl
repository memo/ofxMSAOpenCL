/***********************************************************************
 
 Copyright (c) 2008, 2009, Memo Akten, www.memo.tv
 *** The Mega Super Awesome Visuals Company ***
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of MSA Visuals nor the names of its contributors 
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE. 
 *
 * ***********************************************************************/ 


//--------------------------------------------------------------
__kernel void msa_boxblur(read_only image2d_t srcImage, write_only image2d_t dstImage, const int offset) {                                                                                            
	int2 coords = (int2)(get_global_id(0), get_global_id(1));
	int2 dx		= (int2)(offset, 0);
	int2 dy		= (int2)(0, offset);
	
	sampler_t smp = CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

	float4 color1 = read_imagef(srcImage, smp, coords);
	float4 color2 = read_imagef(srcImage, smp, coords + dx);
	float4 color3 = read_imagef(srcImage, smp, coords - dx);
	float4 color4 = read_imagef(srcImage, smp, coords + dy);
	float4 color5 = read_imagef(srcImage, smp, coords - dy);
	float4 color6 = read_imagef(srcImage, smp, coords + dx + dy);
	float4 color7 = read_imagef(srcImage, smp, coords - dx + dy);
	float4 color8 = read_imagef(srcImage, smp, coords + dx - dy);
	float4 color9 = read_imagef(srcImage, smp, coords - dx - dy);
	float4 color = (color1 + color2 + color3 + color4 + color5 + color6 + color7 + color8 + color9) * 1.0f/9.0f;
	write_imagef(dstImage, coords, color);
}  

//--------------------------------------------------------------
__kernel void msa_flipx(read_only image2d_t srcImage, write_only image2d_t dstImage) {                                                                                            
	int i = get_global_id(0);
	int j = get_global_id(1);
	int2 coords1 = (int2)(i, j);
	int2 coords2 = (int2)(get_image_width(srcImage) - i - 1.0f, j);
	float4 color = read_imagef(srcImage, CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST, coords1);
	write_imagef(dstImage, coords2, color);
}  


//--------------------------------------------------------------
__kernel void msa_flipy(read_only image2d_t srcImage, write_only image2d_t dstImage) {                                                                                            
	int i = get_global_id(0);
	int j = get_global_id(1);
	int2 coords1 = (int2)(i, j);
	int2 coords2 = (int2)(i, get_image_height(srcImage) - j - 1.0f);
	float4 color = read_imagef(srcImage, CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST, coords1);
	write_imagef(dstImage, coords2, color);
}  

//--------------------------------------------------------------
__kernel void msa_greyscale(read_only image2d_t srcImage, write_only image2d_t dstImage) {                                                                                            
	int2 coords = (int2)(get_global_id(0), get_global_id(1));
	float4 color = read_imagef(srcImage, CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST, coords);
	float luminance = 0.3f * color.x + 0.59 * color.y + 0.11 * color.z;
	color = (float4)(luminance, luminance, luminance, 1.0f);
	write_imagef(dstImage, coords, color);                                     
} 

//--------------------------------------------------------------
__kernel void msa_invert(read_only image2d_t srcImage, write_only image2d_t dstImage) {                                                                                            
	int2 coords = (int2)(get_global_id(0), get_global_id(1));
	float4 color = read_imagef(srcImage, CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST, coords);
	color = (float4)(1.0f, 1.0f, 1.0f, 1.0f) - color;
	write_imagef(dstImage, coords, color);
}  


//--------------------------------------------------------------
__kernel void msa_threshold(read_only image2d_t srcImage, write_only image2d_t dstImage, const float thresholdLevel) {
	int2 coords = (int2)(get_global_id(0), get_global_id(1));
	float4 color	= read_imagef(srcImage, CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST, coords);
	write_imagei(dstImage, coords, isgreaterequal(color, thresholdLevel));   
}

