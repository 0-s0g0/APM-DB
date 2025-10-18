/* labeling.h */

#include"header.h"



// ラベリングをする関数
int labeling(unsigned char *input, unsigned int *output, int width, int height);
// 引数 input: 入力画像(8bit濃淡画像) output: 出力画像(32bitラベリング結果) width: 画像横幅 height: 画像縦幅
// 戻り値:ラベル数

//面積の小さい連結成分を削除する関数
int deleteSmallArea(unsigned int *label, int n, int width, int height, int num);
/*引数 label:ラベル画像 n:ラベル数 width: 画像横幅 height: 画像縦幅 num:最小面積*/
/*戻り値　新ラベル数*/

//薬の側面を削除したラベリングを行う関数
unsigned int *reLabeling(unsigned int *input, int width, int height, int n, inputTablet *tablet, unsigned char *inputBmp);
/*引数 label:ラベル画像  width: 画像横幅 height: 画像縦幅 n:ラベル数 n:中心座標保存配列 diameter:直径保存配列*/
/*戻り値　新ラベル画像*/
