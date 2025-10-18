#ifndef __SAMPLE_H__
#define __SAMPLE_H__

#define TheNumberOfClass 10 //方向ヒストグラムの階級数
#define DivisionNumber 10 //錠剤の中心から輪郭までを同心円状に分割した分割数
#define BinarizationNum 45 //濃淡画像を2値化するためのしきい値
#define deleteSmallAreaNum 500 //小領域を削除するときのしきい値
#define areaOfEdge 0.4//エッジの面積の割合
#define PI 3.14159265359 //円周率
#define WEIGHT 70 //　錠剤の中心からの有効範囲(70=70%)
#define STRENGTHMAX 370  //強さヒストグラムを作成する際の強さの最大値


typedef struct masterData{
	int id; // 錠剤の遠し番号
	char pillName[100]; //錠剤の名前
	int numOfPills; //錠剤の数
	double area[3]; // 0：平均値　1：母分散　2：中央値
	double diameter[3]; // 0：平均値　1：母分散　2：中央値
	double Rave[3]; double Gave[3]; double Bave[3]; // 0：平均値　1：母分散　2：中央値
	double Rmed[3]; double Gmed[3]; double Bmed[3]; // 0：平均値　1：母分散　2：中央値
	double *v; //特徴ベクトル
}  masterData;

typedef struct inputTablet{
	int x; //中心座標（x）
	int y; //中心座標（y）
	double area; //面積
	double diameter; //直径
	double Rave; double Gave; double Bave; //RGB値の各平均値(Rave[0]、Gave[0]、Bave[0])
	double Rmed; double Gmed; double Bmed;  //RGB値の各中央値(Rmed[2]、Gmed[2]、Bmed[2])
	double directionHist[TheNumberOfClass]; //方向ヒストグラム（0～1の範囲）
	int threshold; //勾配ヒストグラムの強さのしきい値
	double diividedEdgepixelFrequency[DivisionNumber]; //エッジ画素の頻度
	double diividedEdgepixelFrequency_misuno[DivisionNumber]; //追加test
} inputTablet;

#endif /* __SAMPLE_H__ */