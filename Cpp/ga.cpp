#include"ga.hpp"
#include<iostream>
#include<random>
#include<string>
#include<cstring>
#include<fstream>
#include<vector>
#include<algorithm>
#include<sys/stat.h>

std::string make_dir()
{
	std::string dir_name="Genes";
	int dir_i=0;
	//無限ループにした方がコード汚くならなそうだった
	while(true)
	{
		std::string temp_str=dir_name+std::to_string(dir_i);
		if(mkdir(temp_str.c_str(),0755)==0)
		{
			dir_name=temp_str;
			break;
		}
		dir_i++;
	}
	return dir_name;
}

//遺伝子の保存
void GA::gene_save(std::string dir_name)
{
	for(int i=0;i<6;i++)
	{
		std::string fileName=dir_name+"/gene"+std::to_string(num)+std::to_string(i)+".txt";
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
void GA::Init_gene(std::string dir_name,int ga_i)
{
	num=ga_i;
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
	gene_save(dir_name);
}
//評価の対象となるスコアの計算
void GA::Cal_score(int board[SIDE][SIDE],int my_turn)
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
int Rand(int min,int max)
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<> rand(min,max);
	return rand(mt);
}
//遺伝子の個体数の多い順にソート 
void Sort_genes(std::vector<GA>& genes,int left,int right)
{
	int i=left+1;
	int k=right;
	while(i<k)
	{
		while(genes[i].gene_population<genes[left].gene_population&&i<right)
		{
			i++;
		}
		while(genes[k].gene_population>=genes[left].gene_population&&k>left)
		{
			k--;
		}
		if(i<k)
		{
			std::swap(genes[i],genes[k]);
		}
	}
	if(genes[left].gene_population>genes[k].gene_population)
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
}
//期待値選択方式により、優秀な遺伝子の選別(残りは、ルーレット方式により選択)
std::vector<GA> Select_gene(std::vector<GA> genes)
{
	int score_total=0;
	for(int i=0;i<genes.size();i++)
	{
		score_total+=genes[i].score;	
	}
	int population_total=0;
	int progeny_total=0;
	for(int i=0;i<N_GENES;i++)
	{
		int val=Rand(0,100);
		if(val<=CROSSOVER_RATE) progeny_total++;
	}
	for(int i=0;i<genes.size();i++)
	{
		genes[i].gene_population=(int)((double)progeny_total*((double)genes[i].score/(double)score_total));
		population_total+=genes[i].gene_population;
	}
	if(progeny_total>population_total)
	{
		//残りをルーレット方式により選択する
		int gene_not_enough=progeny_total-population_total;
		for(int i=0;i<gene_not_enough;i++)
		{
			int val=Rand(0,population_total-1);
			for(int j=0;j<genes.size();j++)
			{
				val-=genes[j].gene_population;
				if(val<0)
				{
					genes[j].gene_population++;
					break;
				}
			}
			population_total++;
		}
	}
	return genes;
}

//交叉する遺伝子の組み合わせを決定する
std::vector<GA> Crossover_genes(std::vector<GA> genes,int* num1=nullptr,int* num2=nullptr)
{
	if(num1==nullptr||num2==nullptr)
	{
		return genes;
	}
	*num1=-1;
	*num2=-1;
	for(int i=0;i<genes.size();i++)
	{
		if(genes[i].gene_population>0)
		{
			if(*num1==-1)
			{
				*num1=i;
				genes[i].gene_population--;
			}
			else
			{
				*num2=i;
				genes[i].gene_population--;
				break;
			}
		}	
	}
	return genes;
}
//多点交叉法による遺伝子の交叉
std::vector<GA> MultiPoint(std::vector<GA> genes)
{
	std::vector<GA> next_genes(N_GENES);
	int progeny_total=0;
	for(int i=0;i<genes.size();i++)
	{
		progeny_total+=genes[i].gene_population;
		if(genes[i].gene_population==0)break;
	}
	int idx=0;
	int next_idx=0;
	for(next_idx=0;next_idx<N_GENES-progeny_total;next_idx++)
	{
		if(genes[idx].gene_population==0)idx=0;
		next_genes[next_idx]=genes[idx];
		idx++;
	}
	while(true)
	{
		int parent_num1,parent_num2;
		genes=Crossover_genes(genes,&parent_num1,&parent_num2);	
		if(parent_num1==-1&&parent_num2==-1)
		{
			break;
		}
		else if(parent_num2==-1)
		{
			parent_num2=parent_num1;
		}
		next_genes[next_idx]=genes[parent_num1];
		next_genes[next_idx+1]=genes[parent_num2];
		parent_num1=next_idx;
		parent_num2=next_idx+1;
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
						std::swap(next_genes[parent_num1].gene[i][j][k],next_genes[parent_num2].gene[i][j][k]);
						std::swap(next_genes[parent_num1].gene[i][SIDE-j-1][SIDE-k-1],next_genes[parent_num2].gene[i][SIDE-j-1][SIDE-k-1]);
						std::swap(next_genes[parent_num1].gene[i][SIDE-j-1][k],next_genes[parent_num2].gene[i][SIDE-j-1][k]);
						std::swap(next_genes[parent_num1].gene[i][j][SIDE-k-1],next_genes[parent_num2].gene[i][j][SIDE-k-1]);	
					}
				}
			}
		}
	}
	return genes;
}

//BLX-α法による遺伝子の交叉
//next_idxが大きくなり過ぎたたため、その対策をしたが、うまく交叉しなかったらそこが原因かも
std::vector<GA> Blx_Alpha(std::vector<GA> genes)
{
	std::vector<GA> next_genes(N_GENES);
	int progeny_total=0;
	for(int i=0;i<genes.size();i++)
	{
		progeny_total+=genes[i].gene_population;
		if(genes[i].gene_population==0)break;
	}
	int idx=0;
	int next_idx=0;
	for(next_idx=0;next_idx<N_GENES-progeny_total;next_idx++)
	{
		if(genes[idx].gene_population==0)idx=0;
		next_genes[next_idx]=genes[idx];
		idx++;
	}
	int cnt=0;
	while(true)
	{
		if(next_idx>=N_GENES)break;
		int parent_num1,parent_num2;
		genes=Crossover_genes(genes,&parent_num1,&parent_num2);	
		if(parent_num1==-1&&parent_num2==-1)
		{
			break;
		}
		else if(parent_num2==-1)
		{
			parent_num2=parent_num1;
		}
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
					next_genes[next_idx].gene[i][j][k]=val;
					next_genes[next_idx].gene[i][SIDE-j-1][SIDE-k-1]=val;
					next_genes[next_idx].gene[i][SIDE-j-1][k]=val;
					next_genes[next_idx].gene[i][j][SIDE-k-1]=val;
					val=Rand(min_c,max_c);
					if(next_idx+1<N_GENES)
					{
						next_genes[next_idx+1].gene[i][j][k]=val;
						next_genes[next_idx+1].gene[i][SIDE-j-1][SIDE-k-1]=val;
						next_genes[next_idx+1].gene[i][SIDE-j-1][k]=val;
						next_genes[next_idx+1].gene[i][j][SIDE-k-1]=val;
					}
				}
			}	
		}
		cnt++;
		next_idx+=2;
		printf("cnt:%d",cnt);
	}
	return next_genes;
}

std::vector<GA> Mutation(std::vector<GA> genes)
{
	for(int i=0;i<genes.size();i++)
	{
		for(int j=0;j<6;j++)
		{
			for(int k=0;k<SIDE/2;k++)
			{
				for(int l=0;l<SIDE/2;l++)
				{
					if(Rand(0,100)<=MUTATION_RATE)
					{
						puts("Mutation!");
						int val=Rand(MIN_VAL,MAX_VAL);
						genes[i].gene[j][k][l]=val;
						genes[i].gene[j][SIDE-k-1][SIDE-l-1]=val;
						genes[i].gene[j][SIDE-k-1][l]=val;
						genes[i].gene[j][k][SIDE-l-1]=val;
					}
				}
			}
		}
	}
	return genes;
}
