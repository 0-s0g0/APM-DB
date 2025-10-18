/*edgeDetectFiltering.h*/
#include"header.h"


/*微分フィルタを用いて勾配の強さと方向を求める関数*/
void caSlope(unsigned char* image, unsigned int* label, int n, int width, int height, int fx[3][3], int fy[3][3], double* strength, double* direction);
/*引数 image:入力画像（グレー画像), label:ラベル画像, n:ラベル数, width,height:画像の縦横幅, fx:微分フィルタ（横方向）, fy:微分フィルタ（縦方向）,strength:勾配の強さを保存する配列, direction:方向を保存する配列*/
/*戻り値: なし*/

/*勾配の強さを0～255に正規化する関数*/
unsigned char* normalization(int width, int height, double* strength);
/*引数  width,height:画像の縦横幅,strength:勾配の強さを保存する配列*/
/*戻り値: 正規化画像*/

/*勾配の方向を画像にする関数*/
unsigned char* colorDirection(int width, int height, double* direction);
/*引数  width,height:画像の縦横幅,direction:勾配の強さを保存する配列*/
/*戻り値: bmp画像*/

/*ある画素の８近傍にラベルがあるかどうかをチェックする関数*/
int check8neighborhood(int x, int y, unsigned int* label, int n, int width, int height);
/*引数 x,y:探索画素,label:ラベル画像,n:ラベル数,width,height:画像の縦横幅*/
/*戻り値：ある1 なし0*/

/*方向ヒストグラムを作成する関数*/
void makeDirectionHist(double* direction, int* center, double* diameter, int* directionHist, unsigned int* label, int width, int height, int n);
/*引数: direction:方向が保存してある配列,center:中心座標が保存してある配列,diameter:直径が保存してある関数,directionHist:方向ヒストグラムを保存する配列,label:ラベル画像,n:ラベル数,width,height:画像の縦横幅*/
/*戻り値: 無し*/

/*自分のc個回りの角度も自分に加算する*/
void remakeDirectionHist(double* directionHist, double* ArtificialdirectionHist, int n, int c);
/*戻り値: 無し*/

/*勾配強さヒストグラムを作成する関数*/
void makeStrengthHist(double* strength, inputTablet*, int* strengthHist, unsigned int* label, int width, int height, int n);
/*引数: strength:強さが保存してある配列　strengthHist:方向ヒストグラムを保存する配列,label:ラベル画像,n:ラベル数,width,height:画像の縦横幅*/
/*戻り値: 無し*/

/*勾配強さヒストグラムのしきい値を求め,しきい値を適用した強さ画像を作成する関数*/
void reMakeStrengthImage(int* strengthHist, unsigned int* label, int width, int height, int n, inputTablet*, double m, double* strength, unsigned char*);
/*引数: strengthHist:方向ヒストグラムを保存する配列,label:ラベル画像,n:ラベル数,width,height:画像の縦横幅 threshold;しきい値を保存する配列,m:面積のm%が刻印*/
/*戻り値: 無し*/

/*追加：バイラテラルフィルタをgray画像に対して行う*/
void Bilateral(unsigned char* gray, int width, int height, unsigned char* BilateralImage);

/*追加：Cannyを用いた刻印の方向ヒストグラム及び強さヒストグラムの作成*/
void CannyStrengthImage(unsigned char* gray, unsigned int* label, int width, int height, int n, inputTablet* tablet, double m, unsigned char* strengthImage);


/*刻印部分のみを抽出した方向ヒストグラムを作成する関数*/
void makeMarkingDirectionHist(double* direction, unsigned char* strengthImage, double* strength, inputTablet* tablet, double* MarkingDirectionHist, unsigned int* label, int width, int height, int n);
/*引数: direction;方向が保存してある配列　strength:強さが保存してある配列　threshold;しきい値が保存してある配列,center:中心座標が保存してある配列,diameter:直径が保存してある関数,　MarkingDirectionHist:刻印部分のみの方向ヒストグラムを保存する配列,label:ラベル画像,n:ラベル数,width,height:画像の縦横幅*/
/*戻り値: 無し*/

/*方向ヒストグラムの最大度数の階級を0にくるように合わせる関数*/
void changeStartPoint(double* inHist, double* outHist, inputTablet* tablet, int n);
/*戻り値: 無し*/
