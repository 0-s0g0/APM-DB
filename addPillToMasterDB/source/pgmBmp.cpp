/*pgmBmp.cpp*/

#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<math.h>

#include"header.h"

/* 定数宣言 */
#define MAX_IMAGESIZE      2600 /* 想定する縦・横の最大画素数 */
#define MAX_BRIGHTNESS       255 /* 想定する最大階調値 */
#define GRAYLEVEL            256 /* 想定する階調数(=最大階調値+1) */
#define MAX_FILENAME         256 /* 想定するファイル名の最大長 */
#define MAX_BUFFERSIZE       256 /* 利用するバッファ最大長 */
#define PI 3.14159265359
#define R2 1.41421356237
#define LNum 5000
#define HEADERSIZE 54 /*ヘッダーのサイズ*/

/* bmp形式のファイルを読み込む関数 */
unsigned char* readBMP(char file_name[], int* width, int* height) {

	char buffer[MAX_BUFFERSIZE];  /* データ読み込み用作業変数 */
	FILE* fp; /* ファイルポインタ */
	int x, y; /* ループ変数 */
	unsigned char* image;/*画像を格納するポインタ*/
	unsigned char headBuf[HEADERSIZE]; //ヘッダー情報取り込み
	unsigned char* bmpdata;           //画像データ1行分
	unsigned int color = 0;           //24bitの画像データであるかどうか
	int press = 0;                    //圧縮正式
	int filesize = 0;                 //ファイルサイズ
	int real_width;                   //データ上の1行分のバイト数

	//ヘッダーファイル読み込み

	errno_t err = fopen_s(&fp, file_name, "rb");
	if (err != 0) {
		printf("その名前のファイルは存在しません．\n");
		exit(1);
	}

	//ヘッダーファイル読み込み
	fread(headBuf, sizeof(unsigned char), HEADERSIZE, fp);
	memcpy(buffer, headBuf, 2);
	//ファイルタイプ
	if (strncmp(buffer, "BM", 2) != 0) {
		printf("ファイルタイプが違います．\n");
		exit(1);
	}
	//1画素あたりのデータサイズ
	memcpy(&color, headBuf + 28, sizeof(unsigned int));
	if (color < 24) {
		printf("1画素あたり24bitの画像ではありません．\n");
		exit(1);
	}
	//圧縮形式
	memcpy(&press, headBuf + 30, sizeof(int));
	if (press != 0) {
		printf("圧縮形式が無圧縮ではありません．\n");
		exit(1);
	}

	//画像の幅
	memcpy(width, headBuf + 18, sizeof(int));
	//画像の縦幅
	memcpy(height, headBuf + 22, sizeof(int));

	/* パラメータの画面への表示 */
	//printf("横の画素数 = %d, 縦の画素数 = %d\n", *width, *height);

	/*行末の0を含めた一行分の長さ*/
	real_width = *width * 3 + *width % 4;

	filesize = (*height) * real_width + HEADERSIZE; //ファイルサイズ
	//ファイルサイズ
	memcpy(&filesize, headBuf + 2, sizeof(int));
	if (filesize != ((*height) * real_width + HEADERSIZE)) {
		//printf("ファイルサイズが記録されているものと異なります．\n");
		//exit(1);
	}

	//1行分のRGB情報を取ってくるためのバッファ
	bmpdata = (unsigned char*)malloc(sizeof(unsigned char) * real_width);
	if (bmpdata == NULL) {
		printf("malloc1エラー\n");
		return NULL;
	}

	//入力画像を保存するための領域確保
	image = (unsigned char*)malloc(sizeof(unsigned char) * real_width * (*height));
	if (image == NULL) {
		printf("malloc2エラー\n");
		return NULL;
	}

	//読み込み
	for (y = 0; y < *height; y++) {
		/*一行読み込み*/
		fread(bmpdata, 1, real_width, fp);
		for (x = 0; x < *width * 3; x++) {
			/*バッファから画素値のみを取り出しimageに保存*/
			image[(*height - y - 1) * (*width) * 3 + x] = bmpdata[x];
		}
	}

	//printf("入力ファイル： %s\n", file_name);

	free(bmpdata);

	fclose(fp);

	return image;
}


/* bmp形式のファイルを出力する関数 */
void writeBMP(char file_name[], unsigned char* image, int width, int height) {

	char buffer[MAX_BUFFERSIZE];  /* データ読み込み用作業変数 */
	FILE* fp; /* ファイルポインタ */
	int x, y, i; /* ループ変数 */
	unsigned char headBuf[HEADERSIZE]; //ヘッダー情報取り込み
	unsigned char* bmpdata;           //画像データ1行分
	unsigned int color;               //24bitの画像データであるかどうか
	unsigned int filesize;            //ファイルサイズ
	unsigned int offset;              //オフセット
	unsigned int headsize;            //ヘッダーサイズ
	unsigned int planes;              //プレーン数
	int real_width;                   //データ上の1行分のバイト数
	char fname[100];                  //出力ファイル名

	strcpy_s(fname, sizeof(fname), file_name);
	strcat_s(fname, sizeof(fname), ".bmp");  //出力ファイル名の後ろに拡張子をつける
	/*ファイルオープン*/
	errno_t err = fopen_s(&fp, fname, "wb");
	if (err != 0) {
		printf("ファイルをオープンできません．\n");
		exit(1);
	}

	/*ヘッダーに書き込む情報の計算*/
	real_width = width * 3 + width % 4;       //データ上の1行分のバイト数
	filesize = height * real_width + HEADERSIZE; //ファイルサイズ
	offset = HEADERSIZE;                       //ヘッダのサイズ
	headsize = 40;                             //情報ヘッダのサイズ
	planes = 1;                                //プレーン数
	color = 24;                                //1画素あたりのデータサイズ

	/*ヘッダー部分をバッファに書き込み*/
	headBuf[0] = 'B';
	headBuf[1] = 'M';
	memcpy(headBuf + 2, &filesize, 4);
	headBuf[6] = 0;
	headBuf[7] = 0;
	headBuf[8] = 0;
	headBuf[9] = 0;
	memcpy(headBuf + 10, &offset, 4);
	memcpy(headBuf + 14, &headsize, 4);
	memcpy(headBuf + 18, &width, 4);
	memcpy(headBuf + 22, &height, 4);
	memcpy(headBuf + 26, &planes, 2);
	memcpy(headBuf + 28, &color, 2);
	for (i = 30; i < HEADERSIZE; i++) {
		headBuf[i] = 0;
	}
	//ヘッダー部分をファイルに書き込み
	fwrite(headBuf, sizeof(unsigned char), HEADERSIZE, fp);

	//1行分のRGB情報保存するためのバッファ
	bmpdata = (unsigned char*)malloc(sizeof(unsigned char) * real_width);
	if (bmpdata == NULL) {
		printf("malloc1エラー\n");
		fclose(fp);
		exit(1);
	}
	/*1行ずつ書き込み*/
	for (y = 0; y < height; y++) {
		for (x = 0; x < width * 3; x++) {
			/*
			switch (x % 3)
			{
			case 0: bmpdata[x + 2] = image[(height - y - 1)*width * 3 + x];  //データ部分をバッファに書き込み
				break;
			case 1:bmpdata[x] = image[(height - y - 1)*width * 3 + x];  //データ部分をバッファに書き込み
				break;
			case 2:bmpdata[x - 2] = image[(height - y - 1)*width * 3 + x];  //データ部分をバッファに書き込み
				break;
			}
			*/

			bmpdata[x] = image[(height - y - 1) * width * 3 + x];

		}


		for (x = width * 3; x < real_width; x++) {
			bmpdata[x] = 0;                              //行末の0をバッファに書き込み
		}
		fwrite(bmpdata, sizeof(unsigned char), real_width, fp);    //ファイルにバッファの内容を書き込む
	}

	printf("出力ファイル(BMP)： %s\n", fname);

	free(bmpdata);
	fclose(fp);
}

/* pgm形式のファイルを読み込む関数 */
unsigned char* readPGM(char file_name[], int* width, int* height) {

	char buffer[MAX_BUFFERSIZE];  /* データ読み込み用作業変数 */
	FILE* fp; /* ファイルポインタ */
	int max_gray; /* 最大階調値 */
	int x, y; /* ループ変数 */
	unsigned char* image;/*画像を格納するポインタ*/

	errno_t err = fopen_s(&fp, file_name, "rb");
	if (err != 0) {
		printf("その名前のファイルは存在しません．\n");
		exit(1);
	}

	/* ファイルタイプ(=P5)の確認 */
	fgets(buffer, MAX_BUFFERSIZE, fp);
	if (buffer[0] != 'P' || buffer[1] != '5') {
		printf("ファイルのフォーマットが P5 とは異なります．\n");
		exit(1);
	}
	/* width[n], height[n] の代入（#から始まるコメントは読み飛ばす） */
	while (*width == 0 || *height == 0) {
		fgets(buffer, MAX_BUFFERSIZE, fp);
		if (buffer[0] != '#')
			sscanf_s(buffer, "%d %d", width, height);
	}
	/* max_gray の代入（#から始まるコメントは読み飛ばす） */
	max_gray = 0;
	while (max_gray == 0) {
		fgets(buffer, MAX_BUFFERSIZE, fp);
		if (buffer[0] != '#')
			sscanf_s(buffer, "%d", &max_gray);
	}
	/* パラメータの画面への表示 */
	printf("横の画素数 = %d, 縦の画素数 = %d\n", *width, *height);
	printf("最大階調値 = %d\n", max_gray);
	if (*width > MAX_IMAGESIZE || *height > MAX_IMAGESIZE) {
		printf("想定値 %d x %d を超えています．\n",
			MAX_IMAGESIZE, MAX_IMAGESIZE);
		printf("もう少し小さな画像を使って下さい．\n");
		exit(1);
	}
	if (max_gray != MAX_BRIGHTNESS) {
		printf("最大階調値が不適切です．\n");  exit(1);
	}
	/* 画像データを読み込んで画像用配列に代入する */
	image = (unsigned char*)malloc((*width) * (*height) * sizeof(unsigned char));

	for (y = 0; y < *height; y++) {
		for (x = 0; x < *width; x++) {
			image[x + y * (*width)] = (unsigned char)fgetc(fp);
		}
	}

	fclose(fp);
	printf("画像は正常に読み込まれました．\n");

	return image;
}

/* pgm形式のファイルを出力する関数 */
void writePGM(char file_name[], unsigned char* image, int width, int height) {

	FILE* fp; /* ファイルポインタ */
	int x, y; /* ループ変数 */
	char name[10] = ".pgm";
	char fname[100];              //出力ファイル名

	strcpy_s(fname, sizeof(fname), file_name);
	strcat_s(fname, sizeof(fname), name);
	errno_t err = fopen_s(&fp, fname, "wb");
	if (err != 0) {
		printf("ファイルをオープンできません．\n");
		exit(1);
	}

	/* ファイル識別子 "P5" を先頭に出力する */
	fputs("P5\n", fp);
	/* # で始まるコメント行（省略可能） */
	fputs("# Created by IrfanView\n", fp);
	/* 画像の横幅，縦幅の出力 */
	fprintf(fp, "%d %d\n", width, height);
	/* 最大階調値の出力 */
	fprintf(fp, "%d\n", MAX_BRIGHTNESS);
	/* 画像データの出力 */


	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			fputc(image[x + y * (width)], fp);
		}
	}

	fclose(fp);
	printf("画像は正常に出力されました．\n");
}

/* 連結領域一つ一つをpgm形式のファイルを出力する関数 */
void writePGM2(char file_name[], unsigned int* image, int width, int height, int n) {

	FILE* fp; /* ファイルポインタ */
	int x, y, i, a; /* ループ変数 */
	unsigned char max = 0, min = 255;
	char filename[200], name[10] = ".pgm";
	char fname[100];              //出力ファイル名

	for (i = 1; i <= n; i++) {
		sprintf_s(filename, sizeof(filename), "%s%d%s", file_name, i, name);
		errno_t err = fopen_s(&fp, filename, "wb");
		if (err != 0) {
			printf("ファイルをオープンできません．\n");
			exit(1);
		}

		/* ファイル識別子 "P5" を先頭に出力する */
		fputs("P5\n", fp);
		/* # で始まるコメント行（省略可能） */
		fputs("# Created by IrfanView\n", fp);
		/* 画像の横幅，縦幅の出力 */
		fprintf(fp, "%d %d\n", width, height);
		/* 最大階調値の出力 */
		fprintf(fp, "%d\n", MAX_BRIGHTNESS);
		/* 画像データの出力 */

		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				if (image[x + y * (width)] == (short)i) fputc(255, fp);
				else fputc(0, fp);
			}
		}

		for (y = 0; y < height; y++)
			for (x = 0; x < width; x++)
				fputc(image[x + y * (width)], fp);

		fclose(fp);

	}
	printf("画像は正常に出力されました．\n");
}

/*カラー画像をグレースケールに変換する関数*/
unsigned char* RGBtoGray(unsigned char* image_in, int width, int height) {
	int x, y;
	/*出力画像を格納するポインタ*/
	unsigned char* image_out;

	/*出力画像の領域確保*/
	image_out = (unsigned char*)malloc(width * height * sizeof(unsigned char));
	/*出力ファイルに画素値を書き込み*/
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			image_out[y * width + x] = (image_in[y * width * 3 + x * 3 + 0] + image_in[y * width * 3 + x * 3 + 1] + image_in[y * width * 3 + x * 3 + 2]) / 3;
		}
	}

	return image_out;


}



/* 判別分析法に基づきしきい値を選択する関数 */
int OtsuMethod(unsigned int* label, int n, unsigned char* image_in, int width, int height) {
	long int hist[256] = { 0 };    /* ヒストグラム用１次元配列 */
	int x, y, i, thvalue = 0;
	int t, sum = 0, sum1 = 0, sum2 = 0;
	double db, db_max = 0.0;//db:クラス間分散,db:クラス間分散の最大値
	double mt;//mt:全画素の平均値
	int w1 = 0, w2 = 0, m1, m2; //w1:黒画素クラスの画素数 w2 : 黒画素クラスの画素数 m1:黒画素クラスの平均値,m2:白画素クラスの平均値,

	//ヒストグラムの作成
	for (y = 0; y < height; y++)
		for (x = 0; x < width; x++)
			if (label[y * width + x] >= 1 && label[y * width + x] <= n)hist[image_in[x + width * y]]++; /* 累積を１増加 */
	//if(label[y*width+x] == n)hist[image_in[x + width*y]]++; /* 累積を１増加 */


	for (t = 0; t < 256; t++) {

		//平均値
		sum = 0; sum1 = 0; sum2 = 0; w1 = 0; w2 = 0;
		for (i = 0; i < 256; i++) {
			sum += i;
			if (i < t) {
				sum1 += i;
				w1 += hist[i];
			}
			else {
				sum2 += i;
				w2 += hist[i];
			}
		}
		mt = sum / 256;
		if (t == 0) m1 = 0;
		else m1 = sum1 / t;
		m2 = sum2 / (256 - t);
		//printf("%3d: m1= %d m2= %d \n", t,m1, m2);

		//クラス間分散
		db = (double)((w1 * (m1 - mt) * (m1 - mt) + w2 * (m2 - mt) * (m2 - mt)) / (w1 + w2));

		//printf("%3d: db= %f \n\n",t,db);
		//クラス間分散が最大のtを求める
		if (db > db_max) {
			db_max = db;
			thvalue = t;
		}

	}

	return thvalue;
}

/*しきい値に基づき画像を2値化する関数*/
unsigned char* binarization(unsigned char* image_in, int width, int height, int t) {
	int x, y;
	/*出力画像を格納するポインタ*/
	unsigned char* image_out;

	/*出力画像の領域確保*/
	image_out = (unsigned char*)malloc(width * height * sizeof(unsigned char));

	/* 画像を２値化 */
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			if (image_in[x + width * y] <= t) image_out[x + width * y] = 0;
			else image_out[x + width * y] = 255;
		}
	}

	return image_out;

}

/*面積を求める関数*/
void calArea(unsigned int* label, inputTablet* tablet, int n, int width, int height) {
	int i, x, y;
	inputTablet* p;
	p = tablet;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			if (label[x + y * (width)] != 0) (p + label[x + y * (width)])->area++;
		}
	}
}


void calPerimeter(int* lx, int* ly, double* perimeter, int n) {
	int i = 1;
	int rely = ly[0], relx = lx[0];
	//printf("(%d,%d) ", lx[0], ly[0]);

	while (1) {
		if (ly[i] == rely || lx[i] == relx) {
			perimeter[n]++;
		}
		else perimeter[n] += R2;
		rely = ly[i];
		relx = lx[i];
		i++;
		if ((ly[0] == rely && lx[0] == relx && i != 1) || (ly[i] == -1 || lx[i] == -1))break;
	}
	perimeter[n] *= 0.95;
}

/*周囲長を求める関数2*/
void boder_s(unsigned char* image, int ys, int xs, int ds, int* ly, int* lx, int ln, int* lp, int width, int height) {
	static char yp[8] = { 0, -1, -1, -1, 0, 1, 1, 1 }, xp[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
	int y, x, yw, xw, nb, n, nc, flag = 0;
	y = ys; yw = ys;
	x = xs; xw = xs;
	n = (ds + 4) % 7;
	ds = -1;
	nb = -1;
	nc = image[(y - 1) * width + x - 1] + image[(y - 1) * width + x] + image[(y - 1) * width + x + 1] + image[(y)*width + x - 1] + image[(y)*width + x + 1] + image[(y + 1) * width + x - 1] + image[(y + 1) * width + x] + image[(y + 1) * width + x + 1];
	if (nc > 0) {
		do {
			//printf("(xw,yw)=(%d,%d) (x,y)=(%d,%d) n=%d ds=%d nb=%d\n", xw, yw, x, y, n, ds, nb);
			lx[*lp] = xw; x = xw;
			ly[*lp] = yw; y = yw;
			image[yw * width + xw] = 3;
			(*lp)++;
			if (flag == 1 || n != 1)n = (n + 4) % 7;
			flag = 1;
			do {
				n = (n + 1) % 8;
				yw = y + yp[n];
				xw = x + xp[n];
			} while (image[yw * width + xw] == 0);
			if (ds < 0) {
				if (nb < 0)nb = n;
				else ds = nb;
			}
			if (*lp >= ln) {
				*lp = -1;
				return;
			}
		} while ((y != ys) || (x != xs) || (n != ds));
	}
	else {
		lx[*lp] = xs; ly[*lp] = ys;
		image[y * width + x] = 2;
		(*lp)++;
	}
	lx[*lp] = -1;
	ly[*lp] = -1;
	(*lp)++;
	//printf("(xw,yw)=(%d,%d) (x,y)=(%d,%d) n=%d ds=%d nb=%d\n", xw, yw, x, y, n, ds, nb);
}

/*周囲長を求める関数1*/
void border(unsigned char* image, int* ly, int* lx, int ln, int* lp, int width, int height) {
	int y, x, y1, x1, ds = 0, nc;

	y1 = height - 1;
	x1 = width - 1;
	*lp = 0;
	for (y = 0; y < height; y++) {
		image[y * width + 0] = 0;
		image[y * width + x1] = 0;
	}
	for (x = 0; x < width; x++) {
		image[x] = 0;
		image[y1 * width + x] = 0;
	}
	for (y = 1; y < y1; y++) {
		for (x = 1; x < x1; x++) {
			if (image[y * width + x] == 1) {
				nc = image[y * width + x + 1] * image[(y - 1) * width + x] * image[y * width + x - 1] * image[(y + 1) * width + x];
				if (nc == 0) {
					if (image[(y + 1) * width + x] == 0)ds = 6;
					else if (image[y * width + x - 1] == 0)ds = 4;
					else if (image[(y - 1) * width + x] == 0)ds = 2;
					else if (image[y * width + x + 1] == 0)ds = 0;
					boder_s(image, y, x, ds, ly, lx, ln, lp, width, height);
				}
			}
		}
	}
	for (y = 1; y < y1; y++) {
		for (x = 1; x < x1; x++) {
			if (image[y * width + x] > 0)image[y * width + x] = 1;
		}
	}
}


/*直径度を求める関数*/
void caldiameter(double* diameter, double* perimeter, int n) {
	int i;
	for (i = 1; i <= n; i++) {
		diameter[i] = perimeter[i] / PI;
	}
}

/*円形度を求める関数*/
void calCircularity(double* circularity, int* area, double* perimeter, int n) {
	int i;

	for (i = 1; i <= n; i++) {
		circularity[i] = 4.0 * PI * (double)area[i] / (perimeter[i] * perimeter[i]);
	}

}

/* 輪郭線のみの画像を作る関数 */
void makerinkakuimage(unsigned char* image, int width, int height, int* ly, int* lx) {

	int i = 0; /* ループ変数 */

	while (1) {
		image[ly[i] * width + lx[i]] = 255;
		i++;
		if (ly[i] == ly[0] && lx[i] == lx[0])break;
	}

}

/* 錠剤の色をヒストグラムで表す関数 test */
void makeRGBHist(unsigned char* image_in, int width, int height, unsigned int* label, int rHist[], int gHist[], int bHist[])
{
	int x, y, lnum = 0;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			lnum = label[y * width + x];
			if (lnum != 0) {
				rHist[image_in[x * 3 + (y * width * 3) + 2]]++;
				gHist[image_in[x * 3 + (y * width * 3) + 1]]++;
				bHist[image_in[x * 3 + (y * width * 3)]]++;
			}
		}
	}
}


/* 色ヒストグラムをメモ帳に書く  test  */
void writeRGBHist(char filename[], int rHist[], int gHist[], int bHist[])
{
	int i, j;
	FILE* f;
	errno_t err = fopen_s(&f, filename, "w");
	if (err != 0) {
		printf("ファイルをオープンできません．\n");
		exit(1);
	}

	for (j = 0; j < 3; j++) {
		for (i = 0; i < 256; i++) {
			if (j == 0) {
				fprintf(f, "%d", rHist[i]);
			}
			else if (j == 1) {
				fprintf(f, "%d", gHist[i]);
			}
			else if (j == 2) {
				fprintf(f, "%d", bHist[i]);
			}
			fprintf(f, "\n");
		}
		fprintf(f, "\n\n");
	}
	fclose(f);
}

/*qsort用比較関数*/
int compare_int(const void* a, const void* b)
{
	return *(int*)a - *(int*)b;
}


/*各ラベルに対してRGBの中央値を求める関数*/
void calRGBcolor(unsigned int* label, unsigned char* bmp, int width, int height, const int n, inputTablet* tablet) {
	int x, y, i, j, a, * R, * G, * B, sumr, sumg, sumb, labelNum;
	inputTablet* p;
	p = tablet;

	for (i = 1; i <= n; i++) {
		labelNum = 0;
		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				if (label[y * width + x] == i)labelNum++;
			}
		}
		R = (int*)malloc(sizeof(int) * (labelNum + 1));
		G = (int*)malloc(sizeof(int) * (labelNum + 1));
		B = (int*)malloc(sizeof(int) * (labelNum + 1));
		/*初期化*/
		for (j = 0; j <= labelNum; j++) {
			R[j] = 0;
			G[j] = 0;
			B[j] = 0;
		}
		a = 0;
		sumr = 0;
		sumg = 0;
		sumb = 0;
		/*RGBをそれぞれの配列に保存*/
		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				if (label[x + y * (width)] == i) {
					R[a] = bmp[y * width * 3 + x * 3 + 2];
					G[a] = bmp[y * width * 3 + x * 3 + 1];
					B[a] = bmp[y * width * 3 + x * 3 + 0];
					a++;
				}
			}
		}
		/*ソート*/
		qsort(R, a, sizeof(int), compare_int);
		qsort(G, a, sizeof(int), compare_int);
		qsort(B, a, sizeof(int), compare_int);
		/*中央値*/
		(tablet + i)->Rmed = R[a / 2];
		(tablet + i)->Gmed = G[a / 2];
		(tablet + i)->Bmed = B[a / 2];
		/*平均値*/
		for (j = 0; j < a; j++) {
			sumr += R[j];
			sumg += G[j];
			sumb += B[j];
		}
		(tablet + i)->Rave = (double)sumr / a;
		(tablet + i)->Gave = (double)sumg / a;
		(tablet + i)->Bave = (double)sumb / a;
		/*分散値*/
		sumr = 0;
		sumg = 0;
		sumb = 0;
		for (j = 0; j < a; j++) {
			sumr += (R[j] - (p + i)->Rave) * (R[j] - (p + i)->Rave);
			sumg += (G[j] - (p + i)->Gave) * (G[j] - (p + i)->Gave);
			sumb += (B[j] - (p + i)->Bave) * (B[j] - (p + i)->Bave);
		}
		//dis[i * 3 + 0] = (double)sumr / a;
		//dis[i * 3 + 1] = (double)sumg / a;
		//dis[i * 3 + 2] = (double)sumb / a;

		// メモリ解放
		free(R);
		free(G);
		free(B);
	}
}



/*qsort用比較関数*/
int compare_double(const void* p, const void* q)
{
	if (*(double*)p > *(double*)q) return 1;
	if (*(double*)p < *(double*)q) return -1;
	return 0;
}

/*中心座標を求めるプログラム*/
void calCenter(int* center, double* diameter, int n, unsigned int* label, int width, int height) {
	int i, x0, y0, x1, y1, x, y, start;
	for (i = 1; i <= n; i++) {
		start = 0;
		x0 = 0; y0 = 0;
		x1 = 0; y1 = 0;
		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				if (label[y * width + x] == i) {
					if (start == 0) {
						x0 = x;
						y0 = y;
						start = 1;
					}
					if (y0 == y) {
						x1 = x;
						y1 = y;
					}

				}

			}
		}
		center[i * 2] = (x0 + x1) / 2;
		center[i * 2 + 1] = y0 + diameter[i] / 2;
		//printf("%2d: %d, %d\n", i, center[i * 2], center[i * 2 + 1]);
	}
}

//マスターファイルを読み込む関数
//マスターファイルを読み込む関数
masterData* readMaster(int* masterNum, int* vectorNum, char filename[]) {
	int i, j;
	FILE* masterFile;
	masterData* master;

	// ★修正: ポインタを NULL で初期化する
	master = NULL;

	errno_t err = fopen_s(&masterFile, filename, "r");
	if (err != 0) {
		// ファイルが存在しない場合はエラーとせず、新しいマスターデータを作成する
		*masterNum = 0;
		*vectorNum = TheNumberOfClass + DivisionNumber;
		master = (masterData*)malloc(sizeof(masterData));
		if (master == NULL) { printf("mallocエラー\n"); exit(1); }
		master[0].id = 0;
		for (j = 0; j < 100; j++) {
			master[0].pillName[j] = 0;
		}
		master[0].numOfPills = 0;
		for (j = 0; j < 3; j++) {
			master[0].area[j] = 0.0;
			master[0].diameter[j] = 0.0;
			master[0].Rave[j] = 0.0;
			master[0].Gave[j] = 0.0;
			master[0].Bave[j] = 0.0;
			master[0].Rmed[j] = 0.0;
			master[0].Gmed[j] = 0.0;
			master[0].Bmed[j] = 0.0;
		}
		master[0].v = (double*)malloc(sizeof(double) * (*vectorNum));
		if (master[0].v == NULL) { printf("mallocエラー\n"); exit(1); }
		for (j = 0; j < *vectorNum; j++) {
			master[0].v[j] = 0.0;
		}
		return master;
	}

	if (fscanf_s(masterFile, "%d%d", masterNum, vectorNum) != EOF) {

		master = (masterData*)malloc(sizeof(masterData) * (*masterNum + 100)); // 少し多めに確保
		if (master == NULL) { printf("mallocエラー\n"); exit(1); }

		for (i = 0; i <= *masterNum; i++) {
			master[i].id = 0;
			for (j = 0; j < 100; j++) {
				master[i].pillName[j] = 0;
			}
			master[i].numOfPills = 0;
			for (j = 0; j < 3; j++) {
				master[i].area[j] = 0.0;
				master[i].diameter[j] = 0.0;
				master[i].Rave[j] = 0.0;
				master[i].Gave[j] = 0.0;
				master[i].Bave[j] = 0.0;
				master[i].Rmed[j] = 0.0;
				master[i].Gmed[j] = 0.0;
				master[i].Bmed[j] = 0.0;
			}
			master[i].v = (double*)malloc(sizeof(double) * (*vectorNum));
			if (master[i].v == NULL) { printf("mallocエラー\n"); exit(1); }
			for (j = 0; j < *vectorNum; j++) {
				master[i].v[j] = 0.0;
			}
		}

		for (i = 0; i < *masterNum; i++) {
			fscanf_s(masterFile, "%d", &((master + i)->id));
			fscanf_s(masterFile, "%s", (master + i)->pillName, (unsigned)_countof((master + i)->pillName));
			fscanf_s(masterFile, "%d", &((master + i)->numOfPills));
			for (j = 0; j < 3; j++) {
				fscanf_s(masterFile, "%lf", &((master + i)->area[j]));
			}
			for (j = 0; j < 3; j++) {
				fscanf_s(masterFile, "%lf", &((master + i)->diameter[j]));
			}
			for (j = 0; j < 3; j++) {
				fscanf_s(masterFile, "%lf", &((master + i)->Rave[j]));
			}
			for (j = 0; j < 3; j++) {
				fscanf_s(masterFile, "%lf", &((master + i)->Gave[j]));
			}
			for (j = 0; j < 3; j++) {
				fscanf_s(masterFile, "%lf", &((master + i)->Bave[j]));
			}
			for (j = 0; j < 3; j++) {
				fscanf_s(masterFile, "%lf", &((master + i)->Rmed[j]));
			}
			for (j = 0; j < 3; j++) {
				fscanf_s(masterFile, "%lf", &((master + i)->Gmed[j]));
			}
			for (j = 0; j < 3; j++) {
				fscanf_s(masterFile, "%lf", &((master + i)->Bmed[j]));
			}
			for (j = 0; j < *vectorNum; j++) {
				fscanf_s(masterFile, "%lf", &((master + i)->v[j]));
			}
		}
	}

	fclose(masterFile);
	return master;
}

//マスターファイルに書き込む関数
void writeMaster(int masterNum, int vectorNum, masterData* master, char filename[]) {
	int i, j;
	FILE* masterFile;
	masterData* p;

	errno_t err = fopen_s(&masterFile, filename, "w");
	if (err != 0) {
		printf("ファイルをオープンできません．\n");
		exit(1);
	}

	fprintf(masterFile, "%d %d\n", masterNum, vectorNum);

	p = master;
	for (i = 0; i < masterNum; i++) {
		fprintf(masterFile, "%d ", ((p + i)->id));
		fprintf(masterFile, "%s ", ((p + i)->pillName));
		fprintf(masterFile, "%d ", ((p + i)->numOfPills));
		for (j = 0; j < 3; j++) {
			fprintf(masterFile, "%lf ", ((p + i)->area[j]));
		}
		for (j = 0; j < 3; j++) {
			fprintf(masterFile, "%lf ", ((p + i)->diameter[j]));
		}
		for (j = 0; j < 3; j++) {
			fprintf(masterFile, "%lf ", ((p + i)->Rave[j]));
		}
		for (j = 0; j < 3; j++) {
			fprintf(masterFile, "%lf ", ((p + i)->Gave[j]));
		}
		for (j = 0; j < 3; j++) {
			fprintf(masterFile, "%lf ", ((p + i)->Bave[j]));
		}
		for (j = 0; j < 3; j++) {
			fprintf(masterFile, "%lf ", ((p + i)->Rmed[j]));
		}
		for (j = 0; j < 3; j++) {
			fprintf(masterFile, "%lf ", ((p + i)->Gmed[j]));
		}
		for (j = 0; j < 3; j++) {
			fprintf(masterFile, "%lf ", ((p + i)->Bmed[j]));
		}
		for (j = 0; j < vectorNum; j++) {
			fprintf(masterFile, "%lf ", ((p + i)->v[j]));
		}
		fprintf(masterFile, "\n");
	}

	fclose(masterFile);
}

/*錠剤の中心から輪郭までを同心円状にa分割した時の各分割に存在するエッジ画素の頻度を求める関数*/
void calDiividedEdgepixelFrequency(inputTablet* tablet, unsigned int* label, unsigned char* strengthImage, double* strength, int width, int height, int n, int flag) {
	int x, y, i, j, lnum;
	long long sum = 0;
	int* x_edge, * y_edge, radius, x_min, y_min, x_max, y_max; //x_edge:刻印の中心x座標    y_edge:刻印の中心y座標  radius:半径
	double MinimumAreaRadius, a, c, * temp;
	inputTablet* p;
	p = tablet;

	temp = (double*)malloc(sizeof(double) * (n + 1) * DivisionNumber);
	x_edge = (int*)malloc(sizeof(int) * (n + 1));
	y_edge = (int*)malloc(sizeof(int) * (n + 1));

	//初期化
	for (i = 0; i <= n; i++) {
		for (j = 0; j < DivisionNumber; j++) {
			temp[i * DivisionNumber + j] = 0;
		}
	}
	for (i = 0; i <= n; i++) {
		x_edge[i] = 0;
		y_edge[i] = 0;
	}

	//錠剤の中心座標から見たエッジ画素の頻度
	if (flag == 0) {
		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				if (label[y * width + x] != 0 && strengthImage[y * width + x] != 0) {
					a = 0.0; c = 0.0; lnum = label[y * width + x];
					MinimumAreaRadius = (double)(((p + lnum)->diameter / 2.0) * ((double)WEIGHT / 100.0)) / DivisionNumber;
					a = (x - (p + lnum)->x) * (x - (p + lnum)->x) + (y - (p + lnum)->y) * (y - (p + lnum)->y);
					c = sqrt(a);
					if (c > (p + lnum)->diameter)c = (p + lnum)->diameter;
					for (i = 0; i < DivisionNumber; i++) {
						if (c >= MinimumAreaRadius * i && c < MinimumAreaRadius * (i + 1)) {
							temp[lnum * DivisionNumber + i]++;
						}
					}

				}

			}
		}
	}
	else {
		//追加：刻印の中心座標から見たエッジ画素の頻度
		for (i = 1; i <= n; i++) {
			x_min = 3000; x_max = 0;
			y_min = 3000; y_max = 0;
			radius = 0;

			for (y = 0; y < height; y++) {
				for (x = 0; x < width; x++) {
					if (label[y * width + x] == i && strengthImage[y * width + x] != 0) {
						if (x_min > x)x_min = x;
						if (x_max < x)x_max = x;
						if (y_min > y)y_min = y;
						if (y_max < y)y_max = y;
					}
				}
			}
			if (x_min > x_max) { // 刻印が見つからなかった場合
				x_edge[i] = (p + i)->x;
				y_edge[i] = (p + i)->y;
			}
			else {
				radius = (y_max - y_min) / 2;
				y_edge[i] = y_min + radius;
				if (x_max < width / 2)x_edge[i] = x_min + radius;
				else x_edge[i] = x_max - radius;
			}

			//if (i != 0)printf("薬:%d      x:%d  y:%d\n", i, x_edge[i], y_edge[i]);
		}
		//printf("\n\n\n");

		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				if (label[y * width + x] != 0 && strengthImage[y * width + x] != 0) {
					a = 0.0; c = 0.0; lnum = label[y * width + x];
					MinimumAreaRadius = (double)(((p + lnum)->diameter / 2.0) * ((double)WEIGHT / 100.0)) / DivisionNumber;

					//printf("薬:%d      x:%d  y:%d\n", lnum, x_edge[lnum], y_edge[lnum]);

					a = (x - x_edge[lnum]) * (x - x_edge[lnum]) + (y - y_edge[lnum]) * (y - y_edge[lnum]);
					c = sqrt(a);
					if (c > (p + lnum)->diameter)c = (p + lnum)->diameter;
					for (i = 0; i < DivisionNumber; i++) {
						if (c >= MinimumAreaRadius * i && c < MinimumAreaRadius * (i + 1)) {
							temp[lnum * DivisionNumber + i] += strength[y * width + x];
						}
					}

				}

			}
		}
	}

	for (i = 1; i <= n; i++) {
		sum = 0;
		for (j = 0; j < DivisionNumber; j++) {
			sum += (long long)temp[i * DivisionNumber + j] * temp[i * DivisionNumber + j];
		}
		double sqrt_sum = sqrt((double)sum);
		if (sqrt_sum == 0.0) { // ゼロ除算を回避
			for (j = 0; j < DivisionNumber; j++) {
				(p + i)->diividedEdgepixelFrequency[j] = 0.0;
			}
		}
		else {
			for (j = 0; j < DivisionNumber; j++) {
				(p + i)->diividedEdgepixelFrequency[j] = (double)temp[i * DivisionNumber + j] / sqrt_sum;
			}
		}
	}

	// メモリ解放
	free(temp);
	free(x_edge);
	free(y_edge);
}


/*入力配列の平均、母分散，中央値を求める関数*/
void calAvePVMedi(double* data, double* ave, double* PV, double* medi, int n) {
	int i;
	double* sort, sum = 0, ss = 0;

	if (n <= 0) return; // データがない場合は何もしない

	sort = (double*)malloc(sizeof(double) * n);
	if (sort == NULL) { printf("mallocエラー\n"); exit(1); }


	for (i = 0; i < n; i++) {
		sort[i] = data[i];
	}

	qsort(sort, n, sizeof(double), compare_double);

	for (i = 0; i < n; i++) {
		sum += data[i];
	}
	*ave = sum / (double)n;
	*medi = sort[n / 2];

	sum = 0;
	for (i = 0; i < n; i++) {
		sum += (data[i] - *ave) * (data[i] - *ave);
	}

	if (n > 1) {
		ss = sum / (n - 1); // 標本分散
	}
	else {
		ss = 0;
	}

	*PV = ss;

	// メモリ解放
	free(sort);
}

/*マスターデータに入力画像の情報を追加する*/
void addMaster(inputTablet* tablet, masterData* master, int* masterNum, int n, char pillName[]) {
	double* data, ave = 0, pv = 0, med = 0, sum;
	int i, j;
	inputTablet* p;
	masterData* currentMaster;

	p = tablet;
	data = (double*)malloc(sizeof(double) * n);
	if (data == NULL) { printf("mallocエラー\n"); exit(1); }

	for (i = 0; i < n; i++) {
		data[i] = 0;
	}

	// 新しいマスターデータの格納先ポインタ
	currentMaster = &master[*masterNum];

	currentMaster->id = *masterNum;
	strcpy_s(currentMaster->pillName, _countof(currentMaster->pillName), pillName);
	currentMaster->numOfPills = n;

	for (i = 0; i < n; i++) {
		data[i] = (p + i + 1)->area;
	}
	calAvePVMedi(data, &ave, &pv, &med, n);
	currentMaster->area[0] = ave;
	currentMaster->area[1] = pv;
	currentMaster->area[2] = med;

	for (i = 0; i < n; i++) {
		data[i] = (p + i + 1)->diameter;
	}
	calAvePVMedi(data, &ave, &pv, &med, n);
	currentMaster->diameter[0] = ave;
	currentMaster->diameter[1] = pv;
	currentMaster->diameter[2] = med;

	for (i = 0; i < n; i++) {
		data[i] = (p + i + 1)->Rave;
	}
	calAvePVMedi(data, &ave, &pv, &med, n);
	currentMaster->Rave[0] = ave;
	currentMaster->Rave[1] = pv;
	currentMaster->Rave[2] = med;

	for (i = 0; i < n; i++) {
		data[i] = (p + i + 1)->Gave;
	}
	calAvePVMedi(data, &ave, &pv, &med, n);
	currentMaster->Gave[0] = ave;
	currentMaster->Gave[1] = pv;
	currentMaster->Gave[2] = med;

	for (i = 0; i < n; i++) {
		data[i] = (p + i + 1)->Bave;
	}
	calAvePVMedi(data, &ave, &pv, &med, n);
	currentMaster->Bave[0] = ave;
	currentMaster->Bave[1] = pv;
	currentMaster->Bave[2] = med;

	for (i = 0; i < n; i++) {
		data[i] = (p + i + 1)->Rmed;
	}
	calAvePVMedi(data, &ave, &pv, &med, n);
	currentMaster->Rmed[0] = ave;
	currentMaster->Rmed[1] = pv;
	currentMaster->Rmed[2] = med;

	for (i = 0; i < n; i++) {
		data[i] = (p + i + 1)->Gmed;
	}
	calAvePVMedi(data, &ave, &pv, &med, n);
	currentMaster->Gmed[0] = ave;
	currentMaster->Gmed[1] = pv;
	currentMaster->Gmed[2] = med;

	for (i = 0; i < n; i++) {
		data[i] = (p + i + 1)->Bmed;
	}
	calAvePVMedi(data, &ave, &pv, &med, n);
	currentMaster->Bmed[0] = ave;
	currentMaster->Bmed[1] = pv;
	currentMaster->Bmed[2] = med;

	for (i = 0; i < DivisionNumber; i++) {
		sum = 0.0;
		for (j = 0; j < n; j++) {
			sum += (p + j + 1)->diividedEdgepixelFrequency[i];
		}
		currentMaster->v[i] = sum / (double)n;
	}
	for (i = 0; i < TheNumberOfClass; i++) {
		sum = 0.0;
		for (j = 0; j < n; j++) {
			sum += (p + j + 1)->directionHist[i];
		}
		currentMaster->v[i + DivisionNumber] = sum / (double)n;
	}

	// 今回の処理でマスターデータが1件追加されたので、総数をインクリメント
	(*masterNum)++;

	// メモリ解放
	free(data);
}