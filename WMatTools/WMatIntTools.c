#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "WMatInt.h"

WMatInt* bilinearSample(WMatInt* input, float scaleX, float scaleY)
{
	//For now we are only interpolating
	assert(input->width * scaleX > input->width && input->height * scaleY > input->height);
	
	const int numj_sub = round(input->width * (scaleX - 1) / (input->width - 1));
	const int numi_sub = round(input->height * (scaleY - 1) / (input->height - 1));
	
	const int resW = (input->width-1)*(numj_sub+1)+1;
	const int rewH = (input->height-1)*(numi_sub+1)+1;
	WMatInt* res = create_WMatInt(resW, rewH);
	
	for(int i = 0; i < input->height-1; i++) {
		for (int j = 0; j < input->width-1; j++) {
			
			int tL = input->data[i*input->width+j];
			int bL = input->data[(i+1)*input->width+j];
			int tR = input->data[i*input->width+j+1];
			int bR = input->data[(i+1)*input->width+j+1];
			int a00 = tL;
			int a10 = bL - tL;
			int a01 = tR - tL;
			int a11 = bR - bL - tR + tL;
						
			int x = 0;
			int y = 0;
			for(int i_sub = 0; i_sub <= numi_sub + 1; i_sub++){
				float x = (float) i_sub / ((float)(numi_sub+1));
				
				for(int j_sub = 0; j_sub <= numj_sub + 1; j_sub++) {
					float y = (float) j_sub / ((float) (numj_sub+1));
					
					int flatIdx = (i * (numi_sub+1) + i_sub) * resW + j * (numj_sub+1) + j_sub;
					//unit square (0 <= x , y <= 1)
					//f(x,y) = a00 + a10x + a01y + a11xy
					res->data[flatIdx] = a00 + a10*x + a01*y + a11*x*y;
				}
			}
		}
	}
	
	return res;
}


void print_WMatInt(WMatInt* in) 
{
	if(in == NULL)
		return;
	
	int maxSize = 0;
	for(int i = 0; i < in->height; i++) {
		int size = 0;
		for(int j = 0; j < in->width; j++) {
			size = abs(in->data[i*in->width+j]);
			if(size > maxSize)
				maxSize = size;
		}
	}
	
	const int spaceW = ceil(log10(maxSize));
	for(int i = 0; i < in->height; i++) {
		printf("[");
		int loc = 0;
		for(int j = 0; j < in->width; j++) {
			loc = i*in->width+j;
			if(j == in->width - 1)
				printf("%*d]\n", spaceW, in->data[loc]);
			else
				printf("%*d, ", spaceW, in->data[loc]);
		}
	}
}

