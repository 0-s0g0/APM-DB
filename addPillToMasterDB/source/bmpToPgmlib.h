#include"header.h"



/* 関数のプロトタイプ宣言 */
/* bmp形式のファイルを読み込む関数 */
unsigned char* readBMP(char file_name[], int* width, int* height);
/*引数:  name[]:ファイル名,width:画像の横幅,height:画像の縦幅*/
/*戻り値:画像を格納した領域の先頭アドレス*/

/* bmp形式のファイルを出力する関数 */
void writeBMP(char file_name[], unsigned char* image, int width, int height);
/*引数:  name[]:ファイル名,image:画像を格納した領域の先頭アドレス,width:画像の横幅,height:画像の縦幅*/
/*戻り値:なし*/

/* pgm形式のファイルを出力する関数 */
void writePGM(char file_name[], unsigned char* image, int width, int height);
/*引数:  name[]:ファイル名,image:画像を格納した領域の先頭アドレス,width:画像の横幅,height:画像の縦幅*/
/*戻り値:なし*/

/*カラー画像をグレースケールに変換する関数*/
unsigned char* RGBtoGray(unsigned char* image_in, int width, int height);
/*image_in:入力画像を格納した領域の先頭アドレス,width:画像の横幅,height:画像の縦幅*/
/*戻り値:出力画像を格納した領域の先頭アドレス*/

/* 判別分析法に基づきしきい値を選択する関数 */
int OtsuMethod(unsigned char* image_in, int width, int height);
/*引数:  image_in:入力画像を格納した領域の先頭アドレス,width:画像の横幅,height:画像の縦幅*/
/*戻り値:しきい値*/

/*しきい値に基づき画像を2値化する関数*/
unsigned char* binarization(unsigned char* image_in, int width, int height, int thvalue);
/*image_in:入力画像を格納した領域の先頭アドレス,width:画像の横幅,height:画像の縦幅,thvalue:しきい値*/
/*戻り値:出力画像を格納した領域の先頭アドレス*/