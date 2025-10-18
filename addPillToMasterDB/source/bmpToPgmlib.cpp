/* 階調画像(bmp形式)用ライブラリbmpToPgmlib.cpp */
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

#include"header.h"

/* 定数宣言 */
#define MAX_IMAGESIZE        2500 /* 想定する縦・横の最大画素数 */
#define MAX_BRIGHTNESS       255 /* 想定する最大階調値 */
#define GRAYLEVEL            256 /* 想定する階調数(=最大階調値+1) */
#define MAX_FILENAME         256 /* 想定するファイル名の最大長 */
#define MAX_BUFFERSIZE       256 /* 利用するバッファ最大長 */

#define HEADERSIZE 54 /*ヘッダーのサイズ*/
#define PI 3.14159265359

/* bmp形式のファイルを読み込む関数 */
unsigned char* readBMP(char file_name[], int* width, int* height) {

	char buffer[MAX_BUFFERSIZE];  /* データ読み込み用作業変数 */
	FILE* fp; /* ファイルポインタ */
	int x, y; /* ループ変数 */
	unsigned char* image;/*画像を格納するポインタ*/
	unsigned char headBuf[HEADERSIZE]; //ヘッダー情報取り込み
	unsigned char* bmpdata;            //画像データ1行分
	unsigned int color = 0;           //24bitの画像データであるかどうか
	int press = 0;                    //圧縮正式
	int filesize = 0;                 //ファイルサイズ
	int real_width;                   //データ上の1行分のバイト数

	//ヘッダーファイル読み込み
	if (fopen_s(&fp, file_name, "rb") != 0) { // ★修正点1: fopen を fopen_s に変更
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
	printf("横の画素数 = %d, 縦の画素数 = %d\n", *width, *height);

	/*行末の0を含めた一行分の長さ*/
	real_width = *width * 3 + *width % 4;

	filesize = (*height) * real_width + HEADERSIZE; //ファイルサイズ
	//ファイルサイズ
	memcpy(&filesize, headBuf + 2, sizeof(int));
	if (filesize != ((*height) * real_width + HEADERSIZE)) {
		printf("ファイルサイズが記録されているものと異なります．\n");
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

	printf("入力ファイル： %s\n", file_name);

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
	unsigned char* bmpdata;            //画像データ1行分
	unsigned int color;                //24bitの画像データであるかどうか
	unsigned int filesize;             //ファイルサイズ
	unsigned int offset;               //オフセット
	unsigned int headsize;             //ヘッダーサイズ
	unsigned int planes;               //プレーン数
	int real_width;                    //データ上の1行分のバイト数
	char fname[100];                   //出力ファイル名

	strcpy_s(fname, sizeof(fname), file_name); // ★修正点2: strcpy を strcpy_s に変更
	strcat_s(fname, sizeof(fname), ".bmp");  // ★修正点3: strcat を strcat_s に変更
	/*ファイルオープン*/
	if (fopen_s(&fp, fname, "wb") != 0) { // ★修正点4: fopen を fopen_s に変更
		printf("ファイルをオープンできません．\n");
		exit(1);
	}

	/*ヘッダーに書き込む情報の計算*/
	real_width = width * 3 + width % 4;        //データ上の1行分のバイト数
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
			bmpdata[x] = image[(height - y - 1) * width * 3 + x];  //データ部分をバッファに書き込み
		}
		for (x = width * 3; x < real_width; x++) {
			bmpdata[x] = 0;                                        //行末の0をバッファに書き込み
		}
		fwrite(bmpdata, sizeof(unsigned char), real_width, fp);    //ファイルにバッファの内容を書き込む
	}

	printf("出力ファイル(BMP)： %s\n", fname);

	free(bmpdata);
	fclose(fp);
}

/* pgm形式のファイルを出力する関数 */
void writePGM(char file_name[], unsigned char* image, int width, int height) {

	FILE* fp; /* ファイルポインタ */
	int x, y; /* ループ変数 */

	/*ファイルオープン*/
	if (fopen_s(&fp, file_name, "wb") != 0) { // ★修正点5: fopen を fopen_s に変更
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

	/*出力ファイルに画素値を書き込み*/
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			fputc(image[y * width + x], fp);
		}
	}

	fclose(fp);

	printf("出力ファイル出力完了\n");

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
int OtsuMethod(unsigned char* image_in, int width, int height) {
	long int hist[256] = { 0 };    /* ヒストグラム用１次元配列 */
	int x, y, i, thvalue;
	int t, sum = 0, sum1 = 0, sum2 = 0;
	double db, db_max = 0.0;//db:クラス間分散,db:クラス間分散の最大値
	double mt;//mt:全画素の平均値
	int w1 = 0, w2 = 0, m1, m2; //w1:黒画素クラスの画素数 w2 : 黒画素クラスの画素数 m1:黒画素クラスの平均値,m2:白画素クラスの平均値,

	//ヒストグラムの作成
	for (y = 0; y < height; y++)
		for (x = 0; x < width; x++)
			hist[image_in[x + width * y]]++; /* 累積を１増加 */


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