#ifndef _GA_H_
#define _GA_H_
#define SIDE	10
#define MAX_VAL	100
#define MIN_VAL	-100
#define CROSSOVER_RATE 	90
#define MUTATION_RATE 	1
#define MULTI_POINT	10
#define N_GENES	100
#define GENERATION	50
#define MINF -1000000000
#define INF 1000000000
#define WHITE -1
#define BLACK 1
#define NONE 0
#include<iostream>
#include<random>
#include<string>
#include<fstream>
#include<vector>
#include<algorithm>
class GA
{
	private:
   	public:
		// 0:序盤有利盤面　1:序盤不利盤面　2:中盤有利盤面　3:中盤不利盤面　4:終盤有利盤面　5:終盤不利盤面
		int gene[6][SIDE][SIDE];
		//選択時に評価するスコア
		int score=0;
		//選択時に計算する最終的なこの遺伝子の個体数
		int gene_population=0;
		int num;
		//遺伝子の保存
		void gene_save(std::string dir_name);
		//遺伝子の初期化
		void Init_gene(std::string dir_name,int ga_i);
		//評価の対象となるスコアの計算
		void Cal_score(int board[SIDE][SIDE],int my_turn);
};

//範囲内での乱数を返す関数
int Rand(int min,int max);
//遺伝子の個体数の多い順にソート 
void Sort_genes(std::vector<GA>& genes,int left,int right);
//期待値選択方式により、優秀な遺伝子の選別(残りは、ルーレット方式により選択)
std::vector<GA> Select_gene(std::vector<GA> genes);
//多点交叉法による遺伝子の交叉
std::vector<GA> MultiPoint(std::vector<GA> genes);
//BLX-α法による遺伝子の交叉
std::vector<GA> Blx_Alpha(std::vector<GA> genes);
//突然変異による遺伝子の変更を行う関数
std::vector<GA> Mutation(std::vector<GA> genes);
//遺伝子を格納するフォルダを作成
std::string make_dir();

#endif
