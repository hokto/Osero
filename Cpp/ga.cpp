#include<iostream>
#include<random>
#include<string>
#include<fstream>
#include<vector>
#include<algorithm>
#define SIDE 10
#define MAX_VAL 100
#define MIN_VAL -100
#define CROSSOVER_RATE 90
#define MUTATION_RATE 1
#define MULTI_POINT	10
//範囲内での乱数を返す関数
int Rand(int min,int max);
class GA
{
	private:
   	public:
		// 0:序盤有利盤面　1:序盤不利盤面　2:中盤有利盤面　3:中盤不利盤面　4:終盤有利盤面　5:終盤不利盤面 int genes[6][SIDE][SIDE];
		int gene[6][SIDE][SIDE];
		static int genes_total;
		//選択時に評価するスコア
		int score=0;
		//選択時に計算する最終的なこの遺伝子の個体数
		int gene_population=0;
		int num;
		GA()
		{
			num=genes_total;
			genes_total++;
		}
		//遺伝子の保存
		void gene_save()
		{
			for(int i=0;i<6;i++)
			{
				std::string fileName="Genes/gene"+std::to_string(num)+std::to_string(i)+".txt";
				std::ofstream write_file;
				write_file.open(fileName,std::ios::out);
				for(int j=0;j<SIDE;j++)
				{
					for(int k=0;k<SIDE;k++)
					{
						write_file<<std::to_string(gene[i][j][k])+",";
					}
					write_file<<std::endl;
				}	
			}
		}
		//遺伝子の初期化
		void Init_gene()
		{
			for(int i=0;i<6;i++)
			{
				for(int j=0;j<SIDE/2;j++)
				{ 
					for(int k=0;k<SIDE/2;k++)
					{
						int val=Rand(MIN_VAL,MAX_VAL);
						gene[i][j][k]=val;
						gene[i][SIDE-j-1][SIDE-k-1]=val;
						gene[i][SIDE-j-1][k]=val;
						gene[i][j][SIDE-1-k]=val;	
					}
				}
			}
			gene_save();
		}
		//評価の対象となるスコアの計算
		void Cal_score(int board[SIDE][SIDE],int my_turn)
		{
			int none_cnt=0;
			for(int i=0;i<SIDE;i++)
			{
				for(int j=0;j<SIDE;j++)
				{
					if(my_turn==board[i][j])
					{
						score++;
					}
					else if(my_turn==board[i][j]*-1)
					{
						score--;
					}
					else
					{
						none_cnt++;
					}
				}
			}
			if(score<0)
			{
				//負けていたらスコア0
				score=0;
			}
			else
			{
				//勝っていたときに、空きマス分を評価する
				score+=none_cnt;
			}
		}
};
int GA::genes_total=0;
int Rand(int min,int max)
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<> rand(min,max);
	return rand(mt);
} //遺伝子の個体数の多い順にソート 
std::vector<GA> Sort_genes(std::vector<GA> genes,int left,int right)
{
	int i=left+1;
	int k=right;
	while(i<k)
	{
		while(genes[i].gene_population>=genes[left].gene_population&&i<right)
		{
			i++;
		}
		while(genes[k].gene_population<genes[left].gene_population&&k>left)
		{
			k--;
		}
		if(i<k)
		{
			std::swap(genes[i],genes[k]);
		}
	}
	if(genes[left].gene_population<=genes[k].gene_population)
	{
		std::swap(genes[left],genes[k]);
	}
	if(left<k-1)
	{
		Sort_genes(genes,left,k-1);
	}
	if(k+1<right)
	{
		Sort_genes(genes,k+1,right);
	}
	return genes;
}
//期待値選択方式により、優秀な遺伝子の選別(残りは、ルーレット方式により選択)
std::vector<GA> Select_gene(std::vector<GA> genes)
{
	int score_total=0;
	for(GA gene : genes)
	{
		score_total+=gene.score;	
	}
	int population_total=0;
	int progeny_total=0;
	for(int i=0;i<GA::genes_total;i++)
	{
		int val=Rand(0,100);
		if(val<=CROSSOVER_RATE) progeny_total++;
	}
	for(GA gene : genes)
	{
		gene.gene_population=(GA::genes_total-progeny_total)*(gene.score/score_total);
		population_total+=gene.gene_population;
	}
	if(GA::genes_total-progeny_total>population_total)
	{
		//残りをルーレット方式により選択する
		int gene_not_enough=GA::genes_total-progeny_total-population_total;
		for(int i=0;i<gene_not_enough;i++)
		{
			int val=Rand(0,population_total-1);
			for(GA gene : genes)
			{
				val-=gene.gene_population;
				if(val<0)
				{
					gene.gene_population++;
				}
			}
			population_total++;
		}
	}
	return genes;
}

//交叉する遺伝子の組み合わせを決定する
void Crossover_genes(std::vector<GA> genes,int *num1,int *num2)
{
	*num1=-1,*num2=-1;
	for(int i=0;i<genes.size();i++)
	{
		if(genes[i].gene_population!=0)
		{
			if(*num1==-1)
			{
				*num1=i;
			}
			else
			{
				*num2=i;
				break;
			}
		}	
	}
}
//多点交叉法による遺伝子の交叉
std::vector<GA> MultiPoint(std::vector<GA> genes)
{
	while(true)
	{
		int parent_num1,parent_num2;
		Crossover_genes(genes,&parent_num1,&parent_num2);	
		if(parent_num1==-1&&parent_num2==-1)
		{
			break;
		}
		else if(parent_num2==-1)
		{
			parent_num2=parent_num1;
		}
		//分割点
		int split_point[MULTI_POINT-1];
		int i=0;
		while(i<MULTI_POINT-1)
		{
			int val=Rand(1,SIDE/2*SIDE/2-1);	
			bool flag=false;
			for(int target : split_point)
			{
				if(val==target)
				{
					flag=true;
					break;
				}
			}
			if(flag)continue;
			split_point[i]=val;
			i++;
		}
		std::sort(split_point,split_point+MULTI_POINT-1);
		int split_idx=0;
		bool flag_crossover=false;
		for(int i=0;i<6;i++)
		{
			for(int j=0;j<SIDE/2;j++)
			{
				for(int k=0;k<SIDE/2;k++)
				{
					if((j+1)*5+k+1==split_point[split_idx])
					{
						//フラグの反転
						flag_crossover=!flag_crossover;
						split_idx++;
					}
					//遺伝子の一部を交換し、新しい遺伝子を作る
					if(flag_crossover)
					{
						std::swap(genes[parent_num1].gene[i][j][k],genes[parent_num2].gene[i][j][k]);
						std::swap(genes[parent_num1].gene[i][SIDE-j-1][SIDE-k-1],genes[parent_num2].gene[i][SIDE-j-1][SIDE-k-1]);
						std::swap(genes[parent_num1].gene[i][SIDE-j-1][k],genes[parent_num2].gene[i][SIDE-j-1][k]);
						std::swap(genes[parent_num1].gene[i][j][SIDE-k-1],genes[parent_num2].gene[i][j][SIDE-k-1]);	
					}
				}
			}
		}
	}
	return genes;
}

//BLX-α法による遺伝子の交叉
std::vector<GA> Blx_Alpha(std::vector<GA> genes)
{
	std::vector<GA> next_genes(GA::genes_total);
	while(true)
	{
		int parent_num1,parent_num2;
		Crossover_genes(genes,&parent_num1,&parent_num2);	
		if(parent_num1==-1&&parent_num2==-1)
		{
			break;
		}
		else if(parent_num2==-1)
		{
			parent_num2=parent_num1;
		}
		int idx=0;
		for(int i=0;i<6;i++)
		{
			for(int j=0;j<SIDE/2;j++)
			{
				for(int k=0;k<SIDE/2;k++)
				{
					int d=std::sqrt(genes[parent_num1].gene[i][j][k]-genes[parent_num2].gene[i][j][k]);
					int alpha=0.35;
					int min=(genes[parent_num1].gene[i][j][k]<genes[parent_num2].gene[i][j][k] ? genes[parent_num1].gene[i][j][k] : genes[parent_num2].gene[i][j][k]);
					int max=genes[parent_num1].gene[i][j][k]+genes[parent_num2].gene[i][j][k]-min;
					int min_c=min-alpha*d;
					int max_c=max+alpha*d;
					int val=Rand(min_c,max_c);
					next_genes[idx].gene[i][j][k]=val;
					next_genes[idx].gene[i][SIDE-j-1][SIDE-k-1]=val;
					next_genes[idx].gene[i][SIDE-j-1][k]=val;
					next_genes[idx].gene[i][j][SIDE-k-1]=val;
				}
			}	
		}
	}
	return next_genes;
}
int main()
{
	for(int i=0;i<5;i++)
	{
		GA gene;
		gene.Init_gene();
	}
	return 0;
}
