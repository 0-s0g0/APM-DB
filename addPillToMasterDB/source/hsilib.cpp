/* 階調画像(hsi形式)用ライブラリ hsilib.h */
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

/* 定数宣言 */
#define MAX_IMAGESIZE       1280 /* 想定する縦・横の最大画素数 */
#define MAX_BRIGHTNESS       255 /* 想定する最大階調値 */
#define GRAYLEVEL            256 /* 想定する階調数(=最大階調値+1) */
#define MAX_FILENAME         256 /* 想定するファイル名の最大長 */
#define MAX_BUFFERSIZE       256 /* 利用するバッファ最大長 */
#define HEADERSIZE 54 /*ヘッダーのサイズ*/
#define PI (3.14159265359)




/*RGB画像からHSI画像に変換する(6角錐モデル)関数*/
void RGBtoHSI(unsigned char* image, int width, int height, double* h, double* s, unsigned char* i) {
	int x, y, a, ImaxRGB;/* ループ変数 */
	unsigned char Imax, Imin, I, rgb[3];
	double S, H;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			Imax = 0;
			Imin = 255;
			for (a = 0; a < 3; a++) {
				rgb[a] = image[y * width * 3 + x * 3 + a];
				if (rgb[a] >= Imax) {
					Imax = rgb[a];
					ImaxRGB = a;
				}
				if (rgb[a] <= Imin) {
					Imin = rgb[a];
					//printf("Imin= %d\n",Imin);
				}
			}
			//if (x==0 && y == 0)printf("r=%d g=%d b=%d \n", rgb[0], rgb[1], rgb[2]);
			I = Imax;
			if (Imax == 0) {
				S = 0.0;
				H = 0.0;
			}
			else {
				S = (double)(Imax - Imin) / (double)Imax;
				if (Imax == Imin) {
					H = 0;
				}
				else {
					switch (ImaxRGB)
					{
					case 0:H = (double)(rgb[1] - rgb[2]) / (Imax - Imin) * (PI / 3.0);
						break;
					case 1:H = (double)(rgb[2] - rgb[0]) / (Imax - Imin) * (PI / 3.0) + (2.0 / 3.0) * PI;
						break;
					case 2:H = (double)(rgb[0] - rgb[1]) / (Imax - Imin) * (PI / 3.0) + (4.0 / 3.0) * PI;
						break;
					}
				}
				if (H < 0) {
					H += 2.0 * PI;
				}
				if (H >= 2.0 * PI) {
					H -= 2.0 * PI;
				}

			}
			//printf("(x,y)=(%d,%d) H= %f S= %f I= %d Imax= %d Imin= %d\n", x, y, H, S, I, Imax, Imin);
			h[y * width + x] = H;
			//printf("H= %f ", h[y * width + x]);
			s[y * width + x] = S;
			//printf("S= %f ", s[y * width + x]);
			i[y * width + x] = I;
			//printf("I= %d \n", i[y * width + x]);
			//if (x == 0 && y == 0)printf("h=%f s=%f i=%d \n", H, S, I);
		}
	}
}

/*引数以下の最大の整数を返す関数*/
int floorhsi(double x) {
	double i = 0.0;

	while (i <= x) {
		i++;
	}

	return (int)(i - 1.0);
}

/*HSI画像からRGB画像に変換する関数*/
unsigned char* HSItoRGB(int width, int height, double* h, double* s, unsigned char* i) {
	int x, y;/* ループ変数 */
	int fh;
	double p, q, t;
	unsigned char* image;

	//出力画像を保存するための領域確保
	image = (unsigned char*)malloc(sizeof(unsigned char) * width * height * 3);

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {

			if (h[y * width + x] == 2.0 * PI) {
				h[y * width + x] = 0;
			}

			fh = floorhsi((3.0 / PI) * h[y * width + x]);
			//fh = (int)((3.0 / PI)*h[y * width + x]);
			p = (double)i[y * width + x] * (1.0 - s[y * width + x]);
			q = (double)i[y * width + x] * (1.0 - s[y * width + x] * ((3.0 / PI) * h[y * width + x] - (double)fh));
			t = (double)i[y * width + x] * (1.0 - s[y * width + x] * (1.0 - (3.0 / PI) * h[y * width + x] + (double)fh));
			//if (x == 0 && y == 0)printf("h=%f s=%f i=%d \n", h[y * width + x], s[y * width + x], i[y * width + x]);
			//if (x == 0 && y == 0)printf("hf=%d p=%f q=%f t=%f \n", fh, p, q,t);
			switch (fh)
			{
			case 0:image[y * width * 3 + x * 3] = (unsigned char)i[y * width + x];
				image[y * width * 3 + x * 3 + 1] = (unsigned char)t;
				image[y * width * 3 + x * 3 + 2] = (unsigned char)p;
				break;
			case 1:image[y * width * 3 + x * 3] = (unsigned char)q;
				image[y * width * 3 + x * 3 + 1] = (unsigned char)i[y * width + x];
				image[y * width * 3 + x * 3 + 2] = (unsigned char)p;
				break;
			case 2:image[y * width * 3 + x * 3] = (unsigned char)p;
				image[y * width * 3 + x * 3 + 1] = (unsigned char)i[y * width + x];
				image[y * width * 3 + x * 3 + 2] = (unsigned char)t;
				break;
			case 3:image[y * width * 3 + x * 3] = (unsigned char)p;
				image[y * width * 3 + x * 3 + 1] = (unsigned char)q;
				image[y * width * 3 + x * 3 + 2] = (unsigned char)i[y * width + x];
				break;
			case 4:image[y * width * 3 + x * 3] = (unsigned char)t;
				image[y * width * 3 + x * 3 + 1] = (unsigned char)p;
				image[y * width * 3 + x * 3 + 2] = (unsigned char)i[y * width + x];
				break;
			case 5:image[y * width * 3 + x * 3] = (unsigned char)i[y * width + x];
				image[y * width * 3 + x * 3 + 1] = (unsigned char)p;
				image[y * width * 3 + x * 3 + 2] = (unsigned char)q;
				break;
			}
		}
	}

	return image;

}

/*HSI画像を正規化する関数*/
unsigned char* HSItoNormalizeHSI(int width, int height, double* h, double* s, unsigned char* i) {
	int x, y;/* ループ変数 */
	unsigned char* image;

	//出力画像を保存するための領域確保
	image = (unsigned char*)malloc(sizeof(unsigned char) * width * height * 3);

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			image[y * width * 3 + x * 3] = (unsigned char)(h[y * width + x] / (2.0 * PI) * 255.0);
			image[y * width * 3 + x * 3 + 1] = (unsigned char)(s[y * width + x] * 255.0);
			image[y * width * 3 + x * 3 + 2] = (unsigned char)i[y * width + x];
		}
	}

	return image;
}

/*RGB画像(unsigned char型の3成分を持つ画像)を，個々の成分の画像に分離する関数*/
void splitRGB(unsigned char* image, int width, int height, unsigned char* r, unsigned char* g, unsigned char* b) {
	int x, y;/* ループ変数 */

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			r[y * width + x] = image[y * width * 3 + x * 3];
			g[y * width + x] = image[y * width * 3 + x * 3 + 1];
			b[y * width + x] = image[y * width * 3 + x * 3 + 2];
		}
	}
}



/*色相を変換する関数*/
void Hue(int width, int height, double* h, double p) {
	int x, y;/* ループ変数 */
	double a;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			a = h[y * width + x] + p;
			if (a >= 2.0 * PI) a -= 2.0 * PI;
			else if (a < 0) a += 2.0 * PI;
			h[y * width + x] = a;
		}
	}
}

/*彩度を変換する関数*/
void Saturation(int width, int height, double* s, double p) {
	int x, y;/* ループ変数 */
	double a;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			a = s[y * width + x] * p;
			if (a > 255) a = 255.0;
			s[y * width + x] = a;
		}
	}
}

/*明度を変換する関数*/
void Intensity(int width, int height, unsigned char* i, double p) {
	int x, y, a;/* ループ変数 */

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			a = i[y * width + x] * p;
			if (a > 255) a = 255;
			i[y * width + x] = a;
		}
	}
}