/*edgeDetectFiltering.cpp*/
#include <opencv2/opencv.hpp> 

#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<math.h>

#include"header.h"
#include"labeling.h"


int check8neighborhood(int x, int y, unsigned int* label, int n, int width, int height) {
	int i, j;

	for (j = 0; j < 3; j++) {
		for (i = 0; i < 3; i++) {
			if (label[(y + j - 1) * width + x + i - 1] > 0 && label[(y + j - 1) * width + x + i - 1] <= n) {
				return 1;
			}
		}
	}
	return 0;
}



/*微分フィルタを用いて勾配の強さと方向を求める関数*/
void caSlope(unsigned char* image, unsigned int* label, int n, int width, int height, int fx[3][3], int fy[3][3], double* strength, double* direction) {
	int i, j, x, y;
	double dx, dy, s;

	for (y = 1; y < height - 1; y++) {
		for (x = 1; x < width - 1; x++) {
			dx = 0; dy = 0;
			if (check8neighborhood(x, y, label, n, width, height)) {
				for (j = 0; j < 3; j++) {
					for (i = 0; i < 3; i++) {
						dx += fx[j][i] * image[(y + j - 1) * width + x + i - 1];
						dy += fy[j][i] * image[(y + j - 1) * width + x + i - 1];
					}
				}
				strength[y * width + x] = sqrt(dx * dx + dy * dy);
				if (dx == 0) {
					if (dy > 0) {
						direction[y * width + x] = PI / 2.0;
					}
					else if (dy < 0) {
						direction[y * width + x] = PI / 2.0 * 3.0;
					}
					else {
						direction[y * width + x] = 999;
					}
				}
				else if (dy == 0) {
					if (dx > 0) {
						direction[y * width + x] = 0.0;
					}
					else if (dx < 0) {
						direction[y * width + x] = PI;
					}
				}
				else {
					if (atan2(dy, dx) < 0) {
						direction[y * width + x] = atan2(dy, dx) + 2.0 * PI;
					}
					else {
						direction[y * width + x] = atan2(dy, dx);
					}
				}
				//printf("%f,%f  %f\n", dx, dy, direction[y*width + x]);
			}
		}
	}

}

/*勾配の強さを0～255に正規化する関数*/
unsigned char* normalization(int width, int height, double* strength) {

	int x, y, a;
	double max, min;
	unsigned char* image;

	image = (unsigned char*)malloc(sizeof(unsigned char) * width * height);

	max = 0.0;
	min = 255.0;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			if (strength[y * width + x] > max)max = strength[y * width + x];
			if (strength[y * width + x] < min)min = strength[y * width + x];
		}
	}
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			a = (strength[y * width + x] - min) / (max + min) * 255;
			image[y * width + x] = a;
		}
	}

	return image;
}

/*勾配の方向を画像にする関数*/
unsigned char* colorDirection(int width, int height, double* direction) {

	int x, y, a;
	unsigned char* image;
	image = (unsigned char*)malloc(sizeof(unsigned char) * width * height * 3);

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			if ((direction[y * width + x] > 0.0 && direction[y * width + x] < PI / 4.0) || (direction[y * width + x] <= -PI * 7.0 / 4.0 && direction[y * width + x] > -2.0 * PI)) {
				image[(y * width + x) * 3 + 0] = 255;
				image[(y * width + x) * 3 + 1] = 127;
				image[(y * width + x) * 3 + 2] = 0;
			}
			else if ((direction[y * width + x] >= PI / 4.0 && direction[y * width + x] < PI * 2.0 / 4.0) || (direction[y * width + x] <= -PI * 6.0 / 4.0 && direction[y * width + x] > -PI * 7.0 / 4.0)) {
				image[(y * width + x) * 3 + 0] = 127;
				image[(y * width + x) * 3 + 1] = 255;
				image[(y * width + x) * 3 + 2] = 0;
			}
			else if ((direction[y * width + x] >= PI * 2.0 / 4.0 && direction[y * width + x] < PI * 3.0 / 4.0) || (direction[y * width + x] <= -PI * 5.0 / 4.0 && direction[y * width + x] > -PI * 6.0 / 4.0)) {
				image[(y * width + x) * 3 + 0] = 0;
				image[(y * width + x) * 3 + 1] = 255;
				image[(y * width + x) * 3 + 2] = 0;
			}
			else if ((direction[y * width + x] >= PI * 3.0 / 4.0 && direction[y * width + x] < PI * 4.0 / 4.0) || (direction[y * width + x] <= -PI * 4.0 / 4.0 && direction[y * width + x] > -PI * 5.0 / 4.0)) {
				image[(y * width + x) * 3 + 0] = 0;
				image[(y * width + x) * 3 + 1] = 255;
				image[(y * width + x) * 3 + 2] = 255;
			}
			else if ((direction[y * width + x] >= PI * 4.0 / 4.0 && direction[y * width + x] < PI * 5.0 / 4.0) || (direction[y * width + x] <= -PI * 3.0 / 4.0 && direction[y * width + x] > -PI * 4.0 / 4.0)) {
				image[(y * width + x) * 3 + 0] = 0;
				image[(y * width + x) * 3 + 1] = 127;
				image[(y * width + x) * 3 + 2] = 255;
			}
			else if ((direction[y * width + x] >= PI * 5.0 / 4.0 && direction[y * width + x] < PI * 6.0 / 4.0) || (direction[y * width + x] <= -PI * 2.0 / 4.0 && direction[y * width + x] > -PI * 3.0 / 4.0)) {
				image[(y * width + x) * 3 + 0] = 127;
				image[(y * width + x) * 3 + 1] = 0;
				image[(y * width + x) * 3 + 2] = 255;
			}
			else if ((direction[y * width + x] >= PI * 6.0 / 4.0 && direction[y * width + x] < PI * 7.0 / 4.0) || (direction[y * width + x] <= -PI * 1.0 / 4.0 && direction[y * width + x] > -PI * 2.0 / 4.0)) {
				image[(y * width + x) * 3 + 0] = 255;
				image[(y * width + x) * 3 + 1] = 0;
				image[(y * width + x) * 3 + 2] = 255;
			}
			else if ((direction[y * width + x] >= PI * 7.0 / 4.0 && direction[y * width + x] <= PI * 8.0 / 4.0) || (direction[y * width + x] < 0 && direction[y * width + x] >-PI * 1.0 / 4.0)) {
				image[(y * width + x) * 3 + 0] = 255;
				image[(y * width + x) * 3 + 1] = 0;
				image[(y * width + x) * 3 + 2] = 255;
			}
			else {
				image[(y * width + x) * 3 + 0] = 0;
				image[(y * width + x) * 3 + 1] = 0;
				image[(y * width + x) * 3 + 2] = 0;
			}

		}
	}

	return image;
}



/*方向ヒストグラムを作成する関数*/
void makeDirectionHist(double* direction, int* center, double* diameter, int* directionHist, unsigned int* label, int width, int height, int n) {
	int x, y, i, b, lnum = 0;
	double a, c, j;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			a = 0; b = 0; c = 0; lnum = label[y * width + x];
			if (lnum != 0 && direction[y * width + x] != 999) {
				a = (x - center[lnum * 2]) * (x - center[lnum * 2]) + (y - center[lnum * 2 + 1]) * (y - center[lnum * 2 + 1]);
				c = sqrt(a) / (diameter[lnum] / 2.0);
				b = c * 100;
				if (b > 100)b = 100;
				for (j = 0.0; j < TheNumberOfClass; j += 1.0) {
					if (direction[y * width + x] >= (PI / 18.0) * j && direction[y * width + x] < (PI / 18.0) * (j + 1.0)) {
						if (b <= WEIGHT) directionHist[lnum * TheNumberOfClass + (int)j]++;
					}
				}
			}

		}
	}
}

/*自分のc個回りの角度も自分に加算する*/
void remakeDirectionHist(double* directionHist, double* ArtificialDirectionHist, int n, int c) {
	int j, a, b;
	c = 1;
	for (j = 1; j <= n; j++) {
		for (a = 0; a < TheNumberOfClass; a++) {
			for (b = 1; b <= c; b++) {
				ArtificialDirectionHist[j * TheNumberOfClass + a] += directionHist[j * TheNumberOfClass + (TheNumberOfClass + a - b) % TheNumberOfClass] + directionHist[j * TheNumberOfClass + (TheNumberOfClass + a + b) % TheNumberOfClass];
			}
			ArtificialDirectionHist[j * TheNumberOfClass + a] += directionHist[j * TheNumberOfClass + a];
		}
	}
}

/*勾配強さヒストグラムを作成する関数*/
void makeStrengthHist(double* strength, inputTablet* tablet, int* strengthHist, unsigned int* label, int width, int height, int n) {
	int x, y, j, b, lnum = 0;
	double a, c;
	inputTablet* p;
	p = tablet;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			a = 0; b = 0; c = 0; lnum = label[y * width + x];
			if (lnum != 0 && strength[y * width + x] != 0) {
				a = (x - (p + lnum)->x) * (x - (p + lnum)->x) + (y - (p + lnum)->y) * (y - (p + lnum)->y);
				c = sqrt(a) / ((p + lnum)->diameter / 2.0);
				b = c * 100;
				if (b > 100)b = 100;
				if (b <= WEIGHT) {
					strengthHist[lnum * STRENGTHMAX + (int)strength[y * width + x]]++;
				}
			}
		}
	}
}

/*しきい値を適用した勾配強さヒストグラムを作成する関数*/
void reMakeStrengthHist(double* strength, inputTablet* tablet, int* strengthHist, unsigned int* label, int width, int height, int n) {
	int x, y, j, b, lnum = 0;
	double a, c;
	inputTablet* p;
	p = tablet;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			a = 0; b = 0; c = 0; lnum = label[y * width + x]; p = tablet + lnum;
			if (lnum != 0 && strength[y * width + x] != 0) {
				a = (x - p->x) * (x - p->x) + (y - p->y) * (y - p->y);
				c = sqrt(a) / (p->diameter / 2.0);
				b = c * 100;
				if (b > 100)b = 100;
				if (b <= WEIGHT) {
					strengthHist[lnum * STRENGTHMAX + (int)strength[y * width + x]]++;
				}
			}
		}
	}
}

/*勾配強さヒストグラムのしきい値を求め,しきい値を適用した強さ画像を作成する関数*/
void reMakeStrengthImage(int* strengthHist, unsigned int* label, int width, int height, int n, inputTablet* tablet, double m, double* strength, unsigned char* strengthImage) {
	int x, y, i, j, lnum = 0, b, sum = 0, sum1 = 0, o;
	double a, c;
	unsigned int* strengthlabel;
	unsigned char* image;
	image = (unsigned char*)malloc(sizeof(char) * width * height);
	inputTablet* p;
	p = tablet;

	for (j = 1; j <= n; j++) {
		sum = 0;
		for (i = 0; i < STRENGTHMAX; i++) {
			sum += strengthHist[j * STRENGTHMAX + i];
		}
		sum1 = sum;
		sum = 0;
		for (i = 0; i < STRENGTHMAX && sum < sum1 * (1.0 - m); i++) {
			sum += strengthHist[j * STRENGTHMAX + i];
		}
		tablet[j].threshold = i;
		//printf("しきい値：%d",i);
	}
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			a = 0; b = 0; c = 0; lnum = label[y * width + x]; p = tablet + lnum;
			if (lnum != 0 && strength[y * width + x] >= tablet[lnum].threshold) {
				a = (x - p->x) * (x - p->x) + (y - p->y) * (y - p->y);
				c = sqrt(a) / (p->diameter / 2.0);
				b = c * 100;
				if (b > 100)b = 100;
			}
			if (lnum != 0 && strength[y * width + x] >= p->threshold && b <= WEIGHT) {
				image[y * width + x] = lnum;
			}
			else
			{
				image[y * width + x] = 0;
			}
		}
	}
	strengthlabel = (unsigned int*)malloc(sizeof(int) * width * height);
	o = labeling(image, strengthlabel, width, height);
	o = deleteSmallArea(strengthlabel, o, width, height, 2);

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			if (strengthlabel[y * width + x] != 0) {
				strengthImage[y * width + x] = 255;
			}
			else
			{
				strengthImage[y * width + x] = 0;
			}
		}
	}

	/* ★修正点: IplImage* から cv::Mat に変更 */
	/*
	cv::Mat outImg(height, width, CV_8UC1);
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			outImg.at<unsigned char>(y, x) = strengthImage[y*width + x];
		}
	}
	cv::namedWindow("output");
	cv::imshow("output", outImg);
	cv::waitKey(0);

	cv::destroyWindow("output");
	*/
}


/*追加：バイラテラルフィルタをgray画像に対して行う*/
void Bilateral(unsigned char* gray, int width, int height, unsigned char* BilateralImage)
{
	// ★修正点: IplImage* から cv::Mat に変更
	cv::Mat inImg(height, width, CV_8UC1);
	cv::Mat outImg(height, width, CV_8UC1);
	int x, y;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			// ★修正点: ポインタアクセスから at() メソッドに変更
			inImg.at<unsigned char>(y, x) = gray[y * width + x];
		}
	}

	// バイラテラルフィルタの場合
	// ★修正点: cvSmooth から cv::bilateralFilter に変更
	// 元コードの引数 (1, 5, 50, 100) の意図を汲み、カーネル径5, 色空間のシグマ50, 座標空間のシグマ100として設定
	cv::bilateralFilter(inImg, outImg, 5, 50, 100);

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			// ★修正点: ポインタアクセスから at() メソッドに変更
			BilateralImage[y * width + x] = outImg.at<unsigned char>(y, x);
		}
	}

	/* ★修正点: ウィンドウ表示関連の関数をC++形式に変更 */
	/*
	cv::namedWindow("input");
	cv::imshow("input", inImg);
	cv::namedWindow("output");
	cv::imshow("output", outImg);
	cv::waitKey(0);


	cv::destroyWindow("input");
	cv::destroyWindow("output");
	*/
	// cv::Matは自動でメモリ解放されるため cvReleaseImage は不要
}


/*追加：Cannyを用いた刻印の方向ヒストグラム及び強さヒストグラムの作成*/
void CannyStrengthImage(unsigned char* gray, unsigned int* label, int width, int height, int n, inputTablet* tablet, double m, unsigned char* strengthImage) {
	int x, y, i, j, lnum = 0, b, o;
	double a, c;
	unsigned int* strengthlabel;
	unsigned char* image;
	image = (unsigned char*)malloc(sizeof(char) * width * height);
	inputTablet* p;
	p = tablet;
	// ★修正点: IplImage* から cv::Mat に変更
	cv::Mat inImg(height, width, CV_8UC1);
	cv::Mat outImg(height, width, CV_8UC1);

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			// ★修正点: ポインタアクセスから at() メソッドに変更
			inImg.at<unsigned char>(y, x) = gray[y * width + x];
		}
	}

	// ★修正点: cvCanny から cv::Canny に変更 (引数は同じ)
	cv::Canny(inImg, outImg, 10.0, 50.0, 3);

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			a = 0; b = 0; c = 0; lnum = label[y * width + x];
			p = tablet + lnum;
			if (lnum != 0) {
				a = (x - p->x) * (x - p->x) + (y - p->y) * (y - p->y);
				c = sqrt(a) / (p->diameter / 2.0);
				b = c * 100;
				if (b > 100)b = 100;

				if (b >= WEIGHT)image[y * width + x] = 0;
				else image[y * width + x] = lnum;

				/*
				a = 0; b = 0; c = 0; lnum = image[y*width + x];
				p = tablet + lnum;
				if (lnum != 0) {
					a = (x - p->x)*(x - p->x) + (y - p->y)*(y - p->y);
					c = sqrt(a) / (p->diameter / 2.0);
					b = c * 100;
					if (b > 100)b = 100;

					if (b >= WEIGHT)image[y*width + x] = 0;
				}
				*/
			}
		}
	}

	strengthlabel = (unsigned int*)malloc(sizeof(int) * width * height);
	o = labeling(image, strengthlabel, width, height);

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			if (strengthlabel[y * width + x] == 0)
				// ★修正点: ポインタアクセスから at() メソッドに変更
				outImg.at<unsigned char>(y, x) = 0;

			// ★修正点: ポインタアクセスから at() メソッドに変更
			strengthImage[y * width + x] = outImg.at<unsigned char>(y, x);
		}
	}

	// ★修正点: ウィンドウ表示関連の関数をC++形式に変更
	cv::namedWindow("input");
	cv::imshow("input", inImg);
	cv::namedWindow("canny");
	cv::imshow("canny", outImg);
	cv::waitKey(0);

	cv::destroyWindow("input");
	cv::destroyWindow("canny");
	// cv::Matは自動でメモリ解放されるため cvReleaseImage は不要
}

/*刻印部分のみを抽出した方向ヒストグラムを作成する関数*/
void makeMarkingDirectionHist(double* direction, unsigned char* strengthImage, double* strength, inputTablet* tablet, double* MarkingDirectionHist, unsigned int* label, int width, int height, int n) {
	int x, y, i, b, lnum = 0;
	double a, c, j, sum; //sum：刻印の強度の総積算数
	inputTablet* p;
	p = tablet;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			a = 0; b = 0; c = 0; lnum = label[y * width + x];
			if (lnum != 0 && direction[y * width + x] != 999 && strengthImage[y * width + x] != 0) {
				a = (x - (p + lnum)->x) * (x - (p + lnum)->x) + (y - (p + lnum)->y) * (y - (p + lnum)->y);
				c = sqrt(a) / ((p + lnum)->diameter / 2.0);
				b = c * 100;
				if (b > 100)b = 100;
				for (j = 0.0; j < TheNumberOfClass; j += 1.0) {
					if (direction[y * width + x] >= (PI / (TheNumberOfClass / 2)) * j && direction[y * width + x] < (PI / (TheNumberOfClass / 2)) * (j + 1.0)) {
						//if (b <= 90) MarkingDirectionHist[lnum * TheNumberOfClass + (int)j]++;

						MarkingDirectionHist[lnum * TheNumberOfClass + (int)j] += strength[y * width + x];

						/*printf("j:%lf       strength:%lf       sum[%d]:%lf\n", j, strength[y*width + x],lnum, sum[lnum]);*/
					}
				}
			}

		}
	}
	//sumで方向ヒストグラムを割る処理（ヒストグラムの値を 0～1 の範囲に収める）
	for (i = 1; i <= n; i++) {
		sum = 0;
		for (j = 0; j < TheNumberOfClass; j++) {
			sum += MarkingDirectionHist[i * TheNumberOfClass + (int)j] * MarkingDirectionHist[i * TheNumberOfClass + (int)j];
		}
		for (j = 0; j < TheNumberOfClass; j++) {
			MarkingDirectionHist[i * TheNumberOfClass + (int)j] = (double)MarkingDirectionHist[i * TheNumberOfClass + (int)j] / sqrt(sum);
			//printf("marking:%lf\n", MarkingDirectionHist[i * TheNumberOfClass + (int)j]);
		}
	}
}

/*方向ヒストグラムの最大度数の階級を0にくるように合わせる関数*/
void changeStartPoint(double* inHist, double* outHist, inputTablet* tablet, int n) {
	int maxNum = 0, max = 0, j, a, b;
	inputTablet* p;
	p = tablet;

	for (j = 1; j <= n; j++) {
		maxNum = 0; max = 0;
		for (a = 0; a < TheNumberOfClass; a++) {
			//printf("Hist:%lf\n", inHist[j * TheNumberOfClass + a]);
			if (inHist[j * TheNumberOfClass + a] >= maxNum) {
				maxNum = inHist[j * TheNumberOfClass + a];
				max = a;
			}
		}
		//printf("%d:%d\n",j,max);
		b = max;
		for (a = 0; a < TheNumberOfClass; a++) {
			outHist[j * TheNumberOfClass + a] = inHist[j * TheNumberOfClass + b];
			b = (b + 1) % TheNumberOfClass;
		}
	}

	for (j = 1; j <= n; j++) {
		for (a = 0; a < TheNumberOfClass; a++) {
			(p + j)->directionHist[a] = outHist[j * TheNumberOfClass + a];
			//printf("directionHist:%lf\n", (p + j)->directionHist[a]);
		}
	}
}