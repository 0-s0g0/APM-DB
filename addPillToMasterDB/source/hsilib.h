/* hsilib.h */

/*RGB画像からHSI画像に変換する関数*/
void RGBtoHSI(unsigned char* image, int width, int height, double* h, double* s, unsigned char* i);
/*引数:  image:画像を格納した領域の先頭アドレス,width:画像の横幅,height:画像の縦幅,h:h画像を格納した領域へのポインタ,s:s画像を格納した領域へのポインタ,i:i画像を格納した領域へのポインタ*/
/*戻り値:なし*/

/*HSI画像からRGB画像に変換する関数*/
unsigned char* HSItoRGB(int width, int height, double* h, double* s, unsigned char* i);
/*引数:  width:画像の横幅,height:画像の縦幅,h:h画像を格納した領域へのポインタ,s:s画像を格納した領域へのポインタ,i:i画像を格納した領域へのポインタ*/
/*戻り値:画像を格納した領域の先頭アドレス*/

/*HSI画像を正規化する関数*/
unsigned char* HSItoNormalizeHSI(int width, int height, double* h, double* s, unsigned char* i);
/*引数:  width:画像の横幅,height:画像の縦幅,h:h画像を格納した領域,s:s画像を格納した領域へのポインタ,i:i画像を格納した領域へのポインタ*/
/*戻り値:正規化されたhsi画像を格納した領域の先頭アドレス*/

/*RGB画像(unsigned char型の3成分を持つ画像)を，個々の成分の画像に分離する関数*/
void splitRGB(unsigned char* image, int width, int height, unsigned char* r, unsigned char* g, unsigned char* b);
/*引数:  width:画像の横幅,height:画像の縦幅,r:r画像を格納した領域,g:g画像を格納した領域へのポインタ,b:b画像を格納した領域へのポインタ*/
/*戻り値:なし*/

/*引数以下の最大の整数を返す関数*/
int floorhsi(double x);
/*引数: x:実数*/
/*戻り値：整数*/

/*色相を変換する関数*/
void Hue(int width, int height, double* h, double p);
/*引数:　width:画像の横幅,height:画像の縦幅,h:h画像を格納した領域,p:角度*/
/*戻り値:なし*/

/*彩度を変換する関数*/
void Saturation(int width, int height, double* s, double p);
/*引数:　width:画像の横幅,height:画像の縦幅,s:s画像を格納した領域,p:パラメーター*/
/*戻り値:なし*/

/*明度を変換する関数*/
void Intensity(int width, int height, unsigned char* i, double p);
/*引数:　width:画像の横幅,height:画像の縦幅,i:i画像を格納した領域,p:パラメーター*/
/*戻り値:なし*/

