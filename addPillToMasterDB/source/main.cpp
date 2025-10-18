/*
マスタデータ作成用プログラム
*/

#include <opencv2/opencv.hpp> 

#include<stdio.h>
#include<math.h>
#include"pgmBmp.h"  /* 階調画像用ライブラリのインクルード */
#include"labeling.h"  /* ラベリング用ライブラリのインクルード */
#include"edgeDetectFiltering.h"/*エッジ検出フィルタ処理用ライブラリのインクルード*/
#include"hsilib.h"/*hsi用ライブラリのインクルード*/
#include"header.h"

int main(int argc, char* argv[])
{
	printf("start。\n");
	FILE* fp_test;
	if (fopen_s(&fp_test, "test.txt", "w") != 0) {
		printf("テストファイルの作成に失敗しました。\n");
	}
	else {
		fprintf(fp_test, "ファイル書き込みテスト成功！\n");
		fclose(fp_test);
		printf("test.txt を作成しました。\n");
	}
	unsigned char* inputPgm, * inputBmp, * gray, * strengthImage, * image, * output, * ai, * BilateralImage; /*input:入力画像 gray:濃淡画像  BilateralImage:バイラテラルフィルタ処理後の画像*/
	unsigned int* label, * relabel; //ラベル画像
	int width, height, masterNum, vectorNum, n, x, y, i, j; /*width:入力画像の横幅 height:入力画像の縦幅 n:錠剤数*/
	char fileNameInBmp[100], fileNameOut[100]; /*file_name_in:入力画像のファイルの名前 file_name_out : 出力画像のファイルの名前*/
	char inputPath[] = "../../image2/bmp/"; //入力画像のパス
	inputTablet* tablet;
	masterData* master;
	int fx[3][3] = { { 0, 0, 0 }, { 0, -1, 1 }, { 0, 0, 0 } };
	int fy[3][3] = { { 0, 1, 0 }, { 0, -1, 0 }, { 0, 0, 0 } };
	double* strength, * direction, * s, * directionHist2, * directionHist1, * directionHist0;
	int* strengthHist, rHist[256], gHist[256], bHist[256];
	FILE* fp, * fpStrengthHist;


	//引数エラーチェック
	if (argc != 4) {
		printf("引数を正しく入力してください.\n");
		exit(1);
	}

	master = 0;
	//マスターファイルを読み込む
	master = readMaster(&masterNum, &vectorNum, argv[3]);

	// ★追加: readMasterが失敗した場合のエラーチェック
	if (master == NULL) {
		printf("マスターファイルの読み込み、またはメモリの確保に失敗しました。\n");
		exit(1);
	}

	//width,heightの初期化
	width = 0;
	height = 0;

	/* 入力画像をファイルから読み込む */
	inputBmp = readBMP(argv[1], &width, &height);
	//追加(test)：inputBmpが正しく作成されているか確認するためにinputBmpを表示
	//writeBMP(argv[4], inputBmp, width, height);

	//RGBをgray画像に変換
	gray = RGBtoGray(inputBmp, width, height);
	//writePGM("gray", gray, width, height);
	/*しきい値に基づき画像を2値化する*/
	inputPgm = binarization(gray, width, height, BinarizationNum);
	/*領域確保*/
	label = (unsigned int*)malloc(sizeof(unsigned int) * width * height);
	/*ラベリング処理*/
	n = labeling(inputPgm, label, width, height);
	n = deleteSmallArea(label, n, width, height, deleteSmallAreaNum);
	//printf("n=%d",n);
	/*領域確保*/
	tablet = (inputTablet*)malloc(sizeof(inputTablet) * (n + 1));
	for (i = 0; i <= n; i++) {
		tablet[i].x = 0;
		tablet[i].y = 0;
		tablet[i].area = 0.0;
		tablet[i].diameter = 0.0;
		tablet[i].Rave = 0;
		tablet[i].Gave = 0;
		tablet[i].Bave = 0;
		tablet[i].Rmed = 0;
		tablet[i].Gmed = 0;
		tablet[i].Bmed = 0;
		for (j = 0; j < TheNumberOfClass; j++) {
			tablet[i].directionHist[j] = 0.0;
		}
		tablet[i].threshold = 0;
		for (j = 0; j < DivisionNumber; j++) {
			tablet[i].diividedEdgepixelFrequency[j] = 0.0;
		}
	}

	//再ラベリング化
	relabel = reLabeling(label, width, height, n, tablet, inputBmp);

	//RGBをgray画像に変換
	gray = RGBtoGray(inputBmp, width, height);
	//writePGM("gray2", gray, width, height);

	/*面積*/
	calArea(relabel, tablet, n, width, height);

	//RGB
	calRGBcolor(relabel, inputBmp, width, height, n, tablet);

	/*初期化*/
	strength = (double*)malloc(sizeof(double) * width * height);
	direction = (double*)malloc(sizeof(double) * width * height);
	strengthHist = (int*)malloc(sizeof(int) * (n + 1) * STRENGTHMAX);
	strengthImage = (unsigned char*)malloc(sizeof(unsigned char) * width * height);
	directionHist0 = (double*)malloc(sizeof(double) * (n + 1) * TheNumberOfClass);
	directionHist1 = (double*)malloc(sizeof(double) * (n + 1) * TheNumberOfClass);
	directionHist2 = (double*)malloc(sizeof(double) * (n + 1) * TheNumberOfClass);
	BilateralImage = (unsigned char*)malloc(sizeof(unsigned char) * width * height);

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			strength[y * width + x] = 0;
			direction[y * width + x] = 0;
		}
	}

	for (i = 0; i < 256; i++) {
		rHist[i] = 0;
		gHist[i] = 0;
		bHist[i] = 0;
	}

	/*追加：バイラテラルフィルタをgray画像に対して行う*/
	Bilateral(gray, width, height, BilateralImage);

	//勾配の強さと方向を求める
	caSlope(BilateralImage, relabel, n, width, height, fx, fy, strength, direction);

	//初期化
	for (i = 0; i <= n; i++) {
		for (j = 0; j < STRENGTHMAX; j++) {
			strengthHist[i * STRENGTHMAX + j] = 0.0;
		}
	}

	// 錠剤上のカラーヒストグラムを作成する
	// relabel: ラベル画像(入力)
	makeRGBHist(inputBmp, width, height, relabel, rHist, gHist, bHist);

	//勾配強さヒストグラムを作成する
	// tablet: 錠剤情報管理構造体(入力)
	// n: 錠剤の個数(入力)
	// strengthHist: 勾配強さヒストグラム(出力)
	makeStrengthHist(strength, tablet, strengthHist, relabel, width, height, n);

	//勾配強さヒストグラムのしきい値を求めしきい値を適用した強さ画像を作成する
	reMakeStrengthImage(strengthHist, relabel, width, height, n, tablet, areaOfEdge, strength, strengthImage);

	/*追加：Cannyを用いた刻印の方向ヒストグラム及び強さヒストグラムの作成*/
	//CannyStrengthImage(BilateralImage, relabel, width, height, n, tablet, areaOfEdge,strengthImage);

	//追加(test)：刻印の方向ベクトルをHSIのHで表現し、画像として出力する
	//test = (double*)malloc(sizeof(double)*width*height);
	//output = (unsigned char*)malloc(sizeof(unsigned char)*width*height*3);
	///*HSI用配列初期化*/
	//
	//s = (double *)malloc(sizeof(double)*width*height);
	//ai = (unsigned char*)malloc(sizeof(unsigned char)*width*height);
	//for (y = 0; y < height; y++){
	//	for (x = 0; x < width; x++){
	//		s[y*width + x] = 1.0;
	//		ai[y*width + x] = 255;
	//		
	//	}
	//}
	//for (y = 0; y < height; y++){
	//	for (x = 0; x < width; x++){
	//		if (strengthImage[y*width + x] == 0){
	//			test[y*width + x] = 0.0;
	//		}
	//		else{
	//			test[y*width + x] = direction[y*width + x];
	//		}
	//	}
	//}
	///*勾配の方向を画像にする*/
	//output = HSItoRGB(width, height, test, s, ai);

	////追加(test)：勾配の方向画像を出力する
	//writeBMP(argv[4], output, width, height);

	//初期化
	for (i = 0; i <= n; i++) {
		for (j = 0; j < TheNumberOfClass; j++) {
			directionHist0[i * TheNumberOfClass + j] = 0;
			directionHist1[i * TheNumberOfClass + j] = 0;
			directionHist2[i * TheNumberOfClass + j] = 0;
		}
	}

	//刻印の部分のみを抽出した方向ヒストグラムを作成する
	makeMarkingDirectionHist(direction, strengthImage, strength, tablet, directionHist0, relabel, width, height, n);


	//追加(test)：directionHist0が 0～1の範囲に正規化されているか確かめるために表示
	/*for (i = 1; i <= n; i++) {
		for (j = 0; j < TheNumberOfClass; j++) {
			printf("directionHist0:%lf\n", directionHist0[i * TheNumberOfClass + (int)j]);
		}
	}*/

	////追加(test)：下記の画像を.bmpで出力する
	//writePGM(argv[5], gray, width, height);
	/*writePGM(argv[6], BilateralImage, width, height);
	writePGM(argv[7], strengthImage, width, height);*/

	//追加(test)：カラー画像、濃淡画像、バイラテラル処理後の画像、二値化処理後の画像を表示する
	//IplImage *inImg, *inImg_colar, *outImg, *nitika;
	////int x, y;

	//inImg = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
	//inImg_colar = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
	//outImg = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
	//nitika = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);

	//for (y = 0; y < height; y++) {
	//	for (x = 0; x < width; x++) {
	//		inImg->imageData[y*inImg->widthStep + x] = (char)gray[y*width + x];
	//		nitika->imageData[y*nitika->widthStep + x] = (char)strengthImage[y*width + x];
	//	}
	//}

	//for (y = 0; y < height; y++) {
	//	for (x = 0; x < width * 3; x += 3) {
	//		inImg_colar->imageData[y*inImg_colar->widthStep + x] = (char)inputBmp[y*width * 3 + x];
	//		inImg_colar->imageData[y*inImg_colar->widthStep + x + 1] = (char)inputBmp[y*width * 3 + x + 1];
	//		inImg_colar->imageData[y*inImg_colar->widthStep + x + 2] = (char)inputBmp[y*width * 3 + x + 2];
	//	}
	//}
	//for (y = 0; y < height; y++) {
	//	for (x = 0; x < width; x++) {
	//		outImg->imageData[y*outImg->widthStep + x] = inImg->imageData[y*inImg->widthStep + x];
	//	}
	//}
	//cvNamedWindow("input");
	//cvShowImage("input", inImg);
	//cvNamedWindow("colar");
	//cvShowImage("colar", inImg_colar);
	//cvNamedWindow("output");
	//cvShowImage("output", outImg);
	//cvNamedWindow("nitika");
	//cvShowImage("nitika", nitika);
	//cvWaitKey(0);


	//cvDestroyWindow("input");
	//cvDestroyWindow("output");
	//cvDestroyWindow("colar");
	//cvDestroyWindow("nitika");
	//cvReleaseImage(&inImg_colar);
	//cvReleaseImage(&inImg);
	//cvReleaseImage(&outImg);
	//cvReleaseImage(&nitika);


	//自分のc個回りの角度も自分に加算する
	//remakeDirectionHist(directionHist0, directionHist1, n, 1);


	//方向ヒストグラムの最大度数の階級を0にあわせてヒストグラムを作成
	changeStartPoint(directionHist0, directionHist2, tablet, n);

	int flag = 0;
	/*錠剤の中心から輪郭までを同心円状にa分割した時の各分割に存在するエッジ画素の頻度を求める関数*/
	calDiividedEdgepixelFrequency(tablet, relabel, strengthImage, strength, width, height, n, flag);

	flag = 1;


	//追加(test)：方向ヒストグラムの主軸を合わせる前と後，およびエッジ画素の頻度を表示 //////////////////////////////
	/*IplImage *inImg, *inImg2;
	inImg = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
	inImg2 = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);*/
	for (i = 0; i <= n; i++) {
		for (j = 0; j < DivisionNumber; j++) {
			(tablet + i)->diividedEdgepixelFrequency_misuno[j] = 0.0; //初期化
		}
	}
	for (i = 0; i <= n; i++) {
		for (j = 0; j < DivisionNumber; j++) {
			(tablet + i)->diividedEdgepixelFrequency_misuno[j] = (tablet + i)->diividedEdgepixelFrequency[j];
		}
	}

	for (i = 0; i <= n; i++) {
		for (j = 0; j < DivisionNumber; j++) {
			(tablet + i)->diividedEdgepixelFrequency[j] = 0.0; //初期化
		}
	}

	calDiividedEdgepixelFrequency(tablet, relabel, strengthImage, strength, width, height, n, flag);

	/*for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				inImg->imageData[y*inImg->widthStep + x] = (char)strengthImage[y*width + x];
				inImg2->imageData[y*inImg2->widthStep + x] = (char)gray[y*width + x];
			}
	}
*/
	for (i = 1; i <= n; i++) {
		printf("\n薬:%d\n回転後DirectionHist:", i);
		for (j = 0.0; j < TheNumberOfClass; j += 1.0) {
			printf("%4.lf　    ", directionHist0[i * TheNumberOfClass + (int)j]);
		}
		printf("\n");
		/*printf("回転後DirectionHist:");
		for (j = 0.0; j < TheNumberOfClass; j += 1.0) {
			printf("%lf　    ", directionHist2[i * TheNumberOfClass + (int)j]);
		}
		printf("\n");*/
		printf("エッジ頻度(満野):");
		for (j = 0; j < DivisionNumber; j++) {
			printf("%lf　", (tablet + i)->diividedEdgepixelFrequency_misuno[j]);
		}
		printf("\n");
		printf("エッジ頻度(藤間):");
		for (j = 0; j < DivisionNumber; j++) {
			printf("%lf　", (tablet + i)->diividedEdgepixelFrequency[j]);
		}
		printf("\n\n\n");
		/*cvNamedWindow("input");
		cvShowImage("input", inImg);
		cvNamedWindow("gray");
		cvShowImage("gray", inImg2);
		if (i % 5 == 0) {
			cvWaitKey(0);
			cvDestroyWindow("input");
			cvDestroyWindow("gray");
			cvReleaseImage(&inImg);
			cvReleaseImage(&inImg2);
		}*/

	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*マスターデータに入力画像の情報を追加する*/
	addMaster(tablet, master, &masterNum, n, argv[2]);

	//マスターファイルに書き込む関数
	writeMaster(masterNum, vectorNum, master, argv[3]);

	//色ヒストグラム書き込み関数(test)
	//writeRGBHist(argv[4], rHist, gHist, bHist);

	return 0;
}