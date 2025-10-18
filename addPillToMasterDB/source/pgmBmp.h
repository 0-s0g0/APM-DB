/*pgmBmp.h*/

#pragma once // ヘッダーの重複インクルードを防止

/* 定数宣言 */
#define MAX_IMAGESIZE       2600
#define MAX_BRIGHTNESS        255
#define GRAYLEVEL             256
#define MAX_FILENAME          256
#define MAX_BUFFERSIZE        256
#define PI 3.14159265359
#define R2 1.41421356237
#define LNum 5000
#define HEADERSIZE 54

// main.cppで必要となる定数
#define BinarizationNum 128
#define deleteSmallAreaNum 100
#define TheNumberOfClass 36
#define DivisionNumber 10
#define STRENGTHMAX 256
#define areaOfEdge 0.2
#define WEIGHT 90


#include "header.h" 


/* 関数のプロトタイプ宣言 (pgmBmp1.cppの実装に一致させたもの) */

/* bmp形式のファイルを読み込む関数 */
unsigned char* readBMP(char file_name[], int* width, int* height);

/* bmp形式のファイルを出力する関数 */
void writeBMP(char file_name[], unsigned char* image, int width, int height);

/* pgm形式のファイルを読み込む関数 */
unsigned char* readPGM(char file_name[], int* width, int* height);

void addMaster(inputTablet* tablet, masterData* master, int* masterNum, int n, char pillName[]);

/* pgm形式のファイルを出力する関数 */
void writePGM(char file_name[], unsigned char* image, int width, int height);

/* 連結領域一つ一つをpgm形式のファイルを出力する関数 */
void writePGM2(char file_name[], unsigned int* image, int width, int height, int n);

/*カラー画像をグレースケールに変換する関数*/
unsigned char* RGBtoGray(unsigned char* image_in, int width, int height);

/* 判別分析法に基づきしきい値を選択する関数 */
int OtsuMethod(unsigned int* label, int n, unsigned char* image_in, int width, int height);

/*しきい値に基づき画像を2値化する関数*/
unsigned char* binarization(unsigned char* image_in, int width, int height, int thvalue);

/*面積を求める関数*/
void calArea(unsigned int* label, inputTablet* tablet, int n, int width, int height);

/*各ラベルに対してRGBの中央値,平均,分散を求める関数*/
void calRGBcolor(unsigned int* label, unsigned char* bmp, int width, int height, const int n, inputTablet* tablet);

/*マスターファイルを読み込む関数*/
masterData* readMaster(int* masterNum, int* vectorNum, char filename[]);

//マスターファイルに書き込む関数
void writeMaster(int masterNum, int vectorNum, masterData* master, char filename[]);

/*入力配列の平均、標準偏差，中央値を求める関数*/
void calAvePVMedi(double* data, double* ave, double* PV, double* medi, int n);

/*マスターデータに入力画像の情報を追加する*/
void addMaster(inputTablet* tablet, masterData* master, int* masterNum, int n, char pillName[]);

// 以下の関数はmain.cppからは直接呼び出されていないが、他のライブラリで必要
// edgeDetectFiltering.h で必要
void caSlope(unsigned char* image, unsigned int* label, int n, int width, int height, int fx[3][3], int fy[3][3], double* strength, double* direction);
void makeStrengthHist(double* strength, inputTablet* tablet, int* strengthHist, unsigned int* label, int width, int height, int n);
void reMakeStrengthImage(int* strengthHist, unsigned int* label, int width, int height, int n, inputTablet* tablet, double m, double* strength, unsigned char* strengthImage);
void makeMarkingDirectionHist(double* direction, unsigned char* strengthImage, double* strength, inputTablet* tablet, double* MarkingDirectionHist, unsigned int* label, int width, int height, int n);
void changeStartPoint(double* inHist, double* outHist, inputTablet* tablet, int n);
void calDiividedEdgepixelFrequency(inputTablet* tablet, unsigned int* label, unsigned char* strengthImage, double* strength, int width, int height, int n, int flag);
void Bilateral(unsigned char* gray, int width, int height, unsigned char* BilateralImage);

// labeling.h で必要
int labeling(unsigned char* image, unsigned int* label, int width, int height);
int deleteSmallArea(unsigned int* label, int n, int width, int height, int th);
unsigned int* reLabeling(unsigned int* label, int width, int height, int n, inputTablet* tablet, unsigned char* bmp);

// test用の関数 (main.cppで使用)
void makeRGBHist(unsigned char* image_in, int width, int height, unsigned int* label, int rHist[], int gHist[], int bHist[]);
void writeRGBHist(char filename[], int rHist[], int gHist[], int bHist[]);