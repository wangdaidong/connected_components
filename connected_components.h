#ifndef __CONNECTEDCOMPO__
#define __CONNECTEDCOMPO__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef _NEW
#define _NEW(T,DATA,SIZE,VAL) {\
DATA = (T*)malloc(SIZE*sizeof(T));\
memset(DATA,VAL,SIZE*sizeof(T));\
}
#endif

#ifndef _DELETE
#define _DELETE(DATA) {\
if(DATA){\
	free(DATA);\
	DATA=NULL;\
}\
}
#endif

typedef struct {
	float x;
	float y;
} _point;

typedef struct {
	double sum_xf;
	double sum_yf;
	double sum;
	double x;
	double y;
} _centroid;

typedef struct {
	_point start;
	_point end;
	_centroid centroid;
	int total;
} _status;

/**
 @brief
 在Two-pass连通域标记,详情参考：https://blog.csdn.net/hemeinvyiqiluoben/article/details/39854315
 @param image:图像数据,可以不用是二值图像，但是背景的像素值必须为0
 @param width:图像的宽
 @param height:图像的高
 @param labels:输出的标记图像
 */
#ifdef __cplusplus
extern "C"
#endif
void connected_components(BYTE *image, int width, int height, int **labels,
		_status ** status, int *count);

#endif // !CONNECTEDCOMPO
