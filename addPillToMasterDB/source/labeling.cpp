/* labeling.cpp */
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<math.h>

#include"header.h"

#define MAXLABEL 99999



// ラベリングをする関数
int labeling(unsigned char* input, unsigned int* output, int width, int height)
{

	int i = 0, F, j = 0, k = 0, p = 0, q = 0, b0 = 0, buffer = 0;
	int Lambda = 1; // 連結成分番号を表す変数
	int count = 2;
	int T[MAXLABEL]; // ラベル表
	int l[5]; // 操作済み近傍のラベル
	int L[2]; // (iii-1)のラベルのバッファ

	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			if (input[j * width + i] >= 1) {
				input[j * width + i] = 1;
			}
			output[j * width + i] = 0;
		}
	}

	for (i = 0; i < MAXLABEL; i++) {
		T[i] = 0;
	}

	for (j = 1; j < height - 1; j++) {
		for (i = 1; i < width - 1; i++) {

			F = input[j * width + i];

			switch (F) {

			case 0: {
				output[j * width + i] = 0;
			}
				  break;
			case 1: {
				// ----- (iii)				
				l[0] = 0;
				l[1] = output[(j - 1) * width + (i - 1)];
				l[2] = output[(j - 1) * width + i];
				l[3] = output[(j - 1) * width + (i + 1)];
				l[4] = output[j * width + (i - 1)];

				k = 0;

				for (q = 0; q < 2; q++) L[q] = 0;

				for (p = 1; p <= 4; p++) {
					if (l[p] != 0) {
						if (k == 0) {
							L[0] = T[l[p]];
							k++;
						}
						if (k == 1 && L[0] != T[l[p]]) {
							L[1] = T[l[p]];
							k++;
						}
					}
				}

				if (k == 2 && L[0] > L[1]) {
					buffer = L[0];
					L[0] = L[1];
					L[1] = buffer;
				}

				switch (k) {
				case 0: {
					// ----- (iii-2)
					Lambda++;
					if (Lambda == MAXLABEL) {
						printf("> label count is over.\n");
						exit(0);
					}
					T[Lambda] = Lambda;
					output[j * width + i] = Lambda;
				}
					  break;
				case 1: {
					// ----- (iii-3)
					output[j * width + i] = L[0];
				}
					  break;
				case 2: {
					// ----- (iii-4)
					output[j * width + i] = L[0];
					for (q = 2; q <= Lambda; q++) {
						if (T[q] == L[1]) {
							T[q] = L[0];
						}
					}
				}
					  break;
				}
			}
			}
		}
	}

	// ----- (v)

	for (i = 2; i <= Lambda; i++) {
		if (T[i] == i) {
			T[i] = count;
			count++;
		}
		else {
			T[i] = T[T[i]];
		}
	}
	b0 = count;

	// ----- (vi)

	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			if (output[j * width + i] > 0) {
				output[j * width + i] = T[output[j * width + i]];
			}
		}
	}

	//
	// 上記のままだと，ラベル番号(出力画像の画素値)は2から始まるため，1から始まるよう画素値を1減らす
	//
	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			if (output[j * width + i] >= 1) {
				output[j * width + i] -= 1;
			}
		}
	}

	return(b0 - 2);

}

//面積の小さい連結成分を削除する関数
int deleteSmallArea(unsigned int* label, int n, int width, int height, int num) {
	int i, a, x, y, newlabelnum = 0, sum;

	for (i = 1; i <= n; i++) {
		sum = 0;
		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				if (label[y * width + x] == i)sum++;
			}
		}
		//if(sum> 100)printf("%2d:sum=%d \n", i, sum);
		if (sum < num) {
			for (y = 0; y < height; y++) {
				for (x = 0; x < width; x++) {
					if (label[y * width + x] == i)label[y * width + x] = 0;
				}
			}
		}
		else {
			newlabelnum++;
			for (y = 0; y < height; y++) {
				for (x = 0; x < width; x++) {
					if (label[y * width + x] == i)label[y * width + x] = newlabelnum;
				}
			}
		}
	}
	return newlabelnum;
}

//薬の側面を削除したラベリングを行う関数
unsigned int* reLabeling(unsigned int* input, int width, int height, int n, inputTablet* tablet, unsigned char* inputBmp) {
	int i, j, x, y, x_min = 3000, x_max = 0, y_min = 3000, y_max = 0, radius, x_center, y_center;
	double r;
	unsigned int* label;//再ラベル画像
	inputTablet* p;
	p = tablet;

	//初期化
	label = (unsigned int*)malloc(sizeof(unsigned int) * width * height);
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			label[y * width + x] = 0;
		}
	}

	for (i = 1; i <= n; i++) {
		x_min = 3000; x_max = 0; y_min = 3000; y_max = 0;
		radius = 0; x_center = 0; y_center = 0;
		r = 0.0;
		//薬の中心座標を見つける
		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				if (input[y * width + x] == i) {
					if (x_min > x)x_min = x;
					if (x_max < x)x_max = x;
					if (y_min > y)y_min = y;
					if (y_max < y)y_max = y;
				}
			}
		}
		radius = (y_max - y_min) / 2;
		y_center = y_min + radius;
		if (x_max < width / 2)x_center = x_min + radius;
		else x_center = x_max - radius;

		(p + i)->x = x_center;
		(p + i)->y = y_center;
		(p + i)->diameter = y_max - y_min;
		//printf("%2d:(%4d,%4d) %3d\n", i, x_center, y_center,radius);
		//再ラベリングを行う
		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				if (input[y * width + x] == i) {
					if (x_max <= width / 2) {
						if (x <= x_center) {
							label[y * width + x] = i;
						}
						else
						{
							r = (x_center - x) * (x_center - x) + (y_center - y) * (y_center - y);
							if (sqrt(r) <= radius)label[y * width + x] = i;
						}
					}
					else {
						if (x >= x_center) {
							label[y * width + x] = i;
						}
						else
						{
							r = (x_center - x) * (x_center - x) + (y_center - y) * (y_center - y);
							if (sqrt(r) <= radius)label[y * width + x] = i;
						}
					}
				}
			}
		}
	}
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			if (label[y * width + x] == 0) {
				inputBmp[(y * width + x) * 3 + 0] = 0;
				inputBmp[(y * width + x) * 3 + 1] = 0;
				inputBmp[(y * width + x) * 3 + 2] = 0;
			}
		}
	}


	return label;

}
