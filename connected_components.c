#include "connected_components.h"

#include <assert.h>
//交换两个数
#define SWAP(T,a,b) {\
T tmp = a; \
a = b; \
b = tmp; \
}
//查找数组中的最小值的位置
#define FIND_MIN_UNZERO(T,arr,len,idx) {\
idx=-1;\
int min = 99999;\
for (int i1 = 0 ; i1 < len ; i1++){\
if (arr[i1] > 0 && min > arr[i1]){\
min = arr[i1];\
idx = i1;\
}\
}\
}

/**
 @brief 快速排序算法。利用二分+递归的方式实现快速排序。
 参考：https://blog.csdn.net/vayne_xiao/article/details/53508973
 @param arr:输入数组指针
 @param left:需要排序数组的起始位置
 @param right:需要排序数组的结束位置
 @param index:记录排序后所引位置的变化.请提前分配好空间
 例如：排序前数组[3,1,2]的索引为[0,1,2],排序后数组变为[1 2 3],索引变为[1
 2 0]
 */
__attribute__((deprecated))
void quik_sort(BYTE* arr, int left, int right, int* index) {
	if (right - left < 1) //排序的元素个数小于2个的时候排序毫无意义
		return;
	int l = left, r = right;
	while (1) {
		//从右往左检测直到找到比左边小的值。r:记录位置
		while (arr[r] >= arr[left] && l != r)
			r--;
		//从左往右检测直到找到比左边大的值。l:记录位置
		while (arr[l] <= arr[left] && l != r)
			l++;

		//左右碰头结束
		if (r == l) {
			SWAP(BYTE, arr[left], arr[l]);
			SWAP(int, index[left], index[l]);
			break;
		}

		//交换左右两个值，以及他们的索引
		SWAP(BYTE, arr[l], arr[r]);
		SWAP(int, index[l], index[r]);
	}

	//递归进行碰头位置，左右两边在排序
	int idx = l;
	quik_sort(arr, left, idx - 1, index);
	quik_sort(arr, idx + 1, right, index);

}

/**
 @brief
 在Two-pass连通域标记,详情参考：https://blog.csdn.net/hemeinvyiqiluoben/article/details/39854315
 @param image:图像数据,可以不用是二值图像，但是背景的像素值必须为0
 @param width:图像的宽
 @param height:图像的高
 @param labels:输出的标记图像
 */
void connected_components(BYTE *image, int width, int height, int **labels,
		_status ** status, int *count) {
	assert(
			image && width > 0 && height > 0 && width % 4 == 0 && labels
					&& status && count);

	if (!*labels)
		_NEW(int, *labels, width * height, 0)
	else
		memset(*labels, 0, width * height * sizeof(int));

	int *parent = NULL; //第一次搜索标记后的索引号的父母
	int beta = 1; //索引号的长度倍数，最开始为1024，
	//如果label的个数超过了1024，就重新加倍的分配
	_NEW(int, parent, 1024 * beta, 0);

	//当前的标记数值
	int current_label = 0;
	for (int h = 0; h < height; h++) {
		for (int w = 0; w < width; w++) {
			BYTE x11 = image[width * h + w]; //当前图像中心位置
			if (x11 == 0) //是背景的话，直接跳到下一个位置
				continue;

			int x[4] = { 0 }; //四个位置的数值
			int index[4] = { 0 }; //四个位置的数组索引值

			index[0] = width * (h - 1) + w - 1;
			x[0] = index[0] >= 0 && w > 0 ? labels[0][index[0]] : 0; //左上,00

			index[1] = width * (h - 1) + w;
			x[1] = index[1] >= 0 ? labels[0][index[1]] : 0; //上,01

			index[2] = width * (h - 1) + w + 1;
			x[2] = ((index[2] >= 0) && w + 1 < width) ? labels[0][index[2]] : 0; //右上,02

			index[3] = width * h + w - 1;
			x[3] = index[3] >= 0 && w > 0 ? labels[0][index[3]] : 0; //左,10

			int idx = -1;
			FIND_MIN_UNZERO(int, x, 4, idx);
			if (idx < 0) { //如果全为0，则这是一个全新的连通域
				labels[0][width * h + w] = ++current_label;
				//数组长度不够，重新加倍分配
				if (current_label >= 1024 * beta) {
					beta += 1;
					int *pnt = NULL;
					_NEW(int, pnt, 1024 * beta, 0);
					memcpy(pnt, parent, 1024 * (beta - 1) * sizeof(int));
					_DELETE(parent);
					parent = pnt;
				}
			} else { //当前位置标记为最小的label号
				labels[0][width * h + w] = labels[0][index[idx]];
				//如果发现当前位置存在多个label，则将数值大的哪一个的父类设置为最小的那个数值
				int p[4] = { 0 }; //每个位置的最终父类
				for (int i = 0; i < 4; i++) {
					int ii = x[i];
					while (parent[ii] != 0)
						ii = parent[ii];
					if (x[i] > 0)
						p[i] = ii;
				}

				//把其它位置的最终父类，设置为最小的哪一个最终父类
				idx = -1;
				FIND_MIN_UNZERO(int, p, 4, idx);
				if (idx > -1)
					for (int i = 0; i < 4; i++)
						if (p[i] > 0 && i != idx && p[i] > p[idx])
							parent[p[i]] = p[idx];
			}

		}
	}

	//resort,为重新排序之后的重1开始顺序递增的索引号
	//比如最终的label的编号为1，5，10;重新排列后应该变为1，2，3
	int *resort = NULL;
	*count = 0;
	_NEW(int, resort, (current_label + 1), 0);
	//找到babel的最终父类，比如1->2;2->3，3->0，那么最终1的父类为3
	//应为3没有父类所以它可悲成为一个根节点
	for (int i = 1; i <= current_label; i++) {
		int idx = i;
		//找到最终的父类
		while (parent[idx] != 0)
			idx = parent[idx];
		//建立这个根节点的重排编号
		if (resort[idx] == 0)
			resort[idx] = ++(*count);
		//修改最终父类
		if (i != idx)
			parent[i] = idx;
	}

	_status *stus = NULL;
	++(*count);
	_NEW(_status, stus, *count, 0);
	*status = stus;

	//重排labels.先将同父类的label数值合并。
	//然后重启排序， 比如最终的label的编号为1，5，10;
	//重新排列后应该变为1，2，3
	for (int i = 0; i < width * height; i++) {
		int *label = labels[0] + i;
		//合并同父类的label
		int pnt = parent[*label];
		if (pnt != 0)
			*label = pnt;
		//重排labels的顺序
		int r = resort[*label];
		if (r != *label && r != 0) {
			*label = r;
		}

		//t统计每个连通域的起点、终点、质心、总数
		float *s_x = &stus[*label].start.x, *s_y = &stus[*label].start.y, *e_x =
				&stus[*label].end.x, *e_y = &stus[*label].end.y;
		int w = i % width, h = i / width;

		if (stus[*label].total == 0) {
			*s_x = *e_x = w;
			*s_y = *e_y = h;
		} else {
			*s_x > w ? *s_x = w : (*e_x < w ? *e_x = w : w);
			*s_y > h ? *s_y = h : (*e_y < h ? *e_y = h : h);
		}

		stus[*label].total++;
		stus[*label].centroid.sum += 1;
		stus[*label].centroid.sum_xf += w;
		stus[*label].centroid.sum_yf += h;
		stus[*label].centroid.x = stus[*label].centroid.sum_xf
				/ stus[*label].centroid.sum;
		stus[*label].centroid.y = stus[*label].centroid.sum_yf
				/ stus[*label].centroid.sum;
	}

	_DELETE(parent);
	_DELETE(resort);
}

