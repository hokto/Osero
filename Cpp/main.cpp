#include"ga.hpp"
#include<iostream>
#include<algorithm>
#include<vector>
#include<string>
#include<cstring>
#include<sstream>
#include<istream>
#include<fstream> 
#include<cmath>
#include<thread>
#include<chrono>

#define N_ISLAND 2 //島モデル数
int Alpha(int board[SIDE][SIDE],int eva_board[SIDE][SIDE],int turn,int depth,int board_info,int alpha,int beta);
int Beta(int board[SIDE][SIDE],int eva_board[SIDE][SIDE],int turn,int depth,int board_info,int alpha,int beta);
const int dirX[8]={1,1,0,-1,-1,-1,0,1};
const int dirY[8]={0,-1,-1,-1,0,1,1,1};

/*Osero*/
//盤面上の座標格納に使用
class Pos
{
	public:
		int x;
		int y;
	Pos(int x0,int y0)
	{
		x=x0;
		y=y0;
	}
};


//現在の盤面を見やすいように表示
void print_board(int board[SIDE][SIDE])
{
	printf("  ");
	for(int i=0;i<SIDE;i++)
	{
		printf(" %d",i);
	}
	putchar('\n');
	for(int i=0;i<SIDE;i++)
	{
		printf(" %d",i);
		for(int j=0;j<SIDE;j++)
		{
			switch(board[i][j])
			{
				case WHITE:
					printf(" ●");
					break;
				case BLACK:
					printf(" ○");
					break;
				case NONE:
					printf(" □");
					break;
			}
		}
		putchar('\n');
	}
}

//複数の条件比較を避けるため実装
bool isBetween(Pos pos)
{
	if(0<=pos.x&&pos.x<SIDE&&0<=pos.y&&pos.y<SIDE)
	{
		return true;
	}
	return false;
}

//駒をひっくり返せるなら返して、trueを返す
bool canReturn(int board[SIDE][SIDE],int turn,Pos currentPos,int dx,int dy)
{
	if(board[currentPos.y][currentPos.x]==0||!isBetween(currentPos))
	{
		return false;
	}
	else if(board[currentPos.y][currentPos.x]==turn)
	{ return true;
	}
	else
	{
		Pos nextPos(currentPos.x+dx,currentPos.y+dy);
		if(canReturn(board,turn,nextPos,dx,dy))
		{
			board[currentPos.y][currentPos.x]=turn;
			return true;
		}
		return false;
	}
}

//指定された場所においてみて、canReturn()がtrueならばtrueを返す
bool canPut(int board[SIDE][SIDE],int turn,Pos currentPos)
{
	int put_judge=false;
	for(int i=0;i<8;i++)
	{
		Pos nextPos(currentPos.x+dirX[i],currentPos.y+dirY[i]);
		if(isBetween(nextPos)&&board[currentPos.y][currentPos.x]==0&&board[nextPos.y][nextPos.x]==turn*-1)
		{
			bool reverse_judge=canReturn(board,turn,nextPos,dirX[i],dirY[i]);

			//put_judgeがtrueだった場合にfalseに更新されないようにする
			if(!put_judge) put_judge=reverse_judge; 
		}
	}
	return put_judge;
}


bool isFullBoard(int board[SIDE][SIDE])
{
	for(int i=0;i<SIDE;i++)
	{
		for(int j=0;j<SIDE;j++)
		{
			int cp_board[SIDE][SIDE];
			//自分の駒と相手の駒で盤面がすべてうまっているかどうか
			memcpy(cp_board,board,sizeof(int)*SIDE*SIDE);
			if(canPut(cp_board,1,Pos(j,i))) return false;
			memcpy(cp_board,board,sizeof(int)*SIDE*SIDE);
			if(canPut(cp_board,-1,Pos(j,i))) return false;
		}
	}
	return true;
}


void countPieces(int board[SIDE][SIDE],int *white=nullptr,int *black=nullptr)
{
	if(white==nullptr||black==nullptr)return;
	*white=0;
	*black=0;
	for(int i=0;i<SIDE;i++)
	{
		for(int j=0;j<SIDE;j++)
		{
			switch(board[i][j])
			{
				case WHITE:
					*white+=1;
					break;
				case BLACK:
					*black+=1;
					break;
			}	
		}
	}
}
Pos best_pos(-1,-1);
int level=0;
/*Algorithm*/

//計算式=(自分の駒の総評価値+自分の置ける場所の総評価値)
//-(相手の駒の総評価値+相手の置ける場所の総評価値)
int CarcuEva(int board[SIDE][SIDE],int eva_board[SIDE][SIDE],int turn,int board_info)
{
	int my_val=0;
	int can_put_val=0;
	int enemy_val=0;
	int enemy_can_put_val=0;
	for(int i=0;i<SIDE;i++)
	{
		for(int j=0;j<SIDE;j++)
		{
			//隅に駒があれば、計算時に評価値を高くし、
			//序盤、中盤、終盤で計算による評価値の重さを変える
			if(board[i][j]==turn)
			{
				my_val+=eva_board[i][j];
				int none_cnt=0;
				for(int k=0;k<8;k++)
				{
					Pos nextPos(j+dirY[i],i+dirX[i]);
					if(!isBetween(nextPos))
					{
						none_cnt++;
					}
				}			
				if(none_cnt!=0)
				{
					my_val+=eva_board[i][j]*(none_cnt-1);
				}
			}
			else if(board[i][j]==turn*-1)
			{
				enemy_val+=eva_board[i][j];
			}
			else
			{
				int cp_board[SIDE][SIDE];
				memcpy(cp_board,board,sizeof(int)*SIDE*SIDE);
				if(canPut(cp_board,turn*-1,Pos(j,i)))
				{
					if(board_info!=2)
					{
						enemy_can_put_val+=eva_board[i][j]/2;
					}
					else
					{
						enemy_can_put_val+=eva_board[i][j];
					}
				}
				else if(board_info==2)
				{
					int cp_board[SIDE][SIDE];
					memcpy(cp_board,board,sizeof(int)*SIDE*SIDE);
					if(canPut(cp_board,turn,Pos(j,i)))
					{
						can_put_val+=eva_board[i][j]/2;
					}
				}
			}
		}
	}
	return my_val+can_put_val-(enemy_val+enemy_can_put_val);
}

std::vector<Pos> CanPut_vec(int board[SIDE][SIDE],int turn)
{
	std::vector<Pos> pos_vec;
	for(int i=0;i<SIDE;i++)
	{
		for(int j=0;j<SIDE;j++)
		{
			int cp_board[SIDE][SIDE];
			memcpy(cp_board,board,sizeof(int)*SIDE*SIDE);
			if(canPut(cp_board,turn,Pos(j,i)))
			{
				pos_vec.push_back(Pos(j,i));
			}
		}
	}
	return pos_vec;
}

//ソートを高速にしたかったのでクイックソートで実装
std::vector<Pos> QuickSort(std::vector<Pos> target_vec,std::vector<int> sort_vec,int left,int right,int ab_flag)
{
	int i=left+1;
	int k=right;
	while(i<k)
	{
		while(((sort_vec[i]>sort_vec[left]&&ab_flag==0)||(sort_vec[i]<sort_vec[left]&&ab_flag==1))&&i<right)
		{
			i++;
		}
		while(((sort_vec[k]<=sort_vec[left]&&ab_flag==0)||(sort_vec[k]>=sort_vec[left]&&ab_flag==1))&&k>left)
		{
			k--;
		}
		if(i<k)
		{
			std::swap(sort_vec[i],sort_vec[k]);
			std::swap(target_vec[i],target_vec[k]);
		}
	}
	if((sort_vec[left]<sort_vec[k]&&ab_flag==0)||(sort_vec[left]>sort_vec[k]&&ab_flag==1))
	{
		std::swap(sort_vec[left],sort_vec[k]);
		std::swap(target_vec[left],target_vec[k]);
	}
	if(left<k-1)
	{
		QuickSort(target_vec,sort_vec,left,k-1,ab_flag);
	}
	if(k+1<right)
	{
		QuickSort(target_vec,sort_vec,k+1,right,ab_flag);
	}
	return target_vec;
}

std::vector<Pos> SortNode(int board[SIDE][SIDE],int eva_board[SIDE][SIDE],int turn,int board_info,std::vector<Pos> pos_vec,int ab_flag)
{
	std::vector<int> eva_vec;
	for(Pos pos:pos_vec)
	{
		int cp_board[SIDE][SIDE];
		memcpy(cp_board,board,sizeof(int)*SIDE*SIDE);
		canPut(cp_board,turn,pos);
		cp_board[pos.y][pos.x]=turn;
		eva_vec.push_back(CarcuEva(cp_board,eva_board,turn,board_info));
	}
	return QuickSort(pos_vec,eva_vec,0,pos_vec.size()-1,ab_flag);
}

//MinMaxより高速そうなAlphaBetaでAI部分の実装
void AlphaBeta(int board[SIDE][SIDE],int turn,int depth,int white,int black,int board_info,bool ga_flag,int gene1,int gene2,std::vector<GA> genes)
{
	best_pos.x=best_pos.y=-1;
	int eva_board[SIDE][SIDE];
	if(!ga_flag)
	{	
		int situation;
		if(turn==BLACK)
		{
			if(black>white)
			{
				situation=0;
			}
			else
			{
				situation=1;
			}
		}
		else
		{
			if(white>black)
			{
				situation=0;
			}
			else
			{
				situation=1;
			}
		}
		//テスト用
		std::ifstream ifs("Genes2/gene7"+std::to_string(situation+board_info*2)+".txt");	
		if(ifs.fail())
		{
			printf("Failed to open file.\n");
			return;
		}
		std::string line;
		int i=0;
		while(std::getline(ifs,line))
		{
			std::stringstream ss;
			ss<<line;
			std::string line_sp;
			int j=0;
			while(std::getline(ss,line_sp,','))
			{
				eva_board[i][j]=atoi(line_sp.c_str());
				j++;	
			}
			i++;
		}
	}
	else
	{
		for(int i=0;i<SIDE;i++)
		{
			for(int j=0;j<SIDE;j++)
			{
				int gene_num;
				int situation;
				if(turn==BLACK)
				{
					gene_num=gene1;
					if(black>white)
					{
						situation=0;
					}
					else
					{
						situation=1;
					}
				}
				else
				{
					gene_num=gene2;
					if(white>black)
					{
						situation=0;
					}
					else
					{
						situation=1;
					}
				} 
				eva_board[i][j]=genes[gene_num].gene[situation+board_info*2][i][j];
			}
		}
	}
	Alpha(board,eva_board,turn,depth,board_info,MINF,INF);
}
int Beta(int board[SIDE][SIDE],int eva_board[SIDE][SIDE],int turn,int depth,int board_info,int alpha,int beta)
{
	if(depth==0)
	{
		return CarcuEva(board,eva_board,turn,board_info);
	}
	Pos pos(-1,-1);
	std::vector<Pos> pos_vec=CanPut_vec(board,turn);
	if(pos_vec.size()!=0)
	{
		if(depth>=3)
		{
			pos_vec=SortNode(board,eva_board,turn,board_info,pos_vec,0);
		}
		for(Pos put_pos:pos_vec)
		{
			int cp_board[SIDE][SIDE];
			memcpy(cp_board,board,sizeof(int)*SIDE*SIDE);
			Pos current_pos(put_pos.x,put_pos.y);
			canPut(cp_board,turn,current_pos);
			cp_board[current_pos.y][current_pos.x]=turn;
			int score=Alpha(cp_board,eva_board,turn*-1,depth-1,board_info,alpha,beta);
			if(score<beta)
			{
				beta=score;
				if(beta<=alpha)
				{
					return alpha;
				}
			}
		}
	}
	return beta;
}
int Alpha(int board[SIDE][SIDE],int eva_board[SIDE][SIDE],int turn,int depth,int board_info,int alpha,int beta)
{
	if(depth==0)
	{
		return CarcuEva(board,eva_board,turn,board_info);
	}
	Pos pos(-1,-1);
	std::vector<Pos> pos_vec=CanPut_vec(board,turn);
	if(pos_vec.size()!=0)
	{
		if(depth>=3)
		{
			pos_vec=SortNode(board,eva_board,turn,board_info,pos_vec,1);
		}
		for(Pos put_pos:pos_vec)
		{
			int cp_board[SIDE][SIDE];
			memcpy(cp_board,board,sizeof(int)*SIDE*SIDE);
			Pos current_pos(put_pos.x,put_pos.y);
			canPut(cp_board,turn,current_pos);
			cp_board[current_pos.y][current_pos.x]=turn;
			int score=Beta(cp_board,eva_board,turn*-1,depth-1,board_info,alpha,beta);
			if(score>alpha)
			{
				alpha=score;
				pos=current_pos;
				if(alpha>=beta)
				{
					if(depth==level)
					{
						best_pos=current_pos;
					}
					return beta;
				}
			}
		}
	}
	if(depth==level)
	{
		best_pos=pos;
	}
	return alpha;
}

//置ける場所に適当に置くAI(GAが強いかどうか確かめる用)
void RandomAI(int board[SIDE][SIDE],int my_turn)
{
	best_pos.x=best_pos.y=-1;
	std::vector<Pos> pos_vec=CanPut_vec(board,my_turn);
	if(pos_vec.size()!=0)
	{
		int val=Rand(0,pos_vec.size()-1);
		best_pos=pos_vec[val];
	}
}

//ga_flagでGAを起動するかきめる
void Osero(int board[SIDE][SIDE],bool ga_flag,int gene1=-1,int gene2=-1,std::vector<GA> genes=std::vector<GA>())
{
	std::string input_turn;
	int my_turn=0;
	int turn=BLACK;
	int turn_cnt=0;
	int path_cnt=0;
	int board_info=0;
	int white=0;
	int black=0;
	if(!ga_flag)
	{
		puts("Input turn.white or black");
		std::cin>>input_turn;
		if(input_turn=="white")
		{
			my_turn=WHITE;
		}
		else if(input_turn=="black")
		{
			my_turn=BLACK;
		}
	}
	board[SIDE/2-1][SIDE/2-1]=1;
	board[SIDE/2][SIDE/2]=1;
	board[SIDE/2-1][SIDE/2]=-1;
	board[SIDE/2][SIDE/2-1]=-1;
	level=3;
	print_board(board);
	countPieces(board,&white,&black);
	printf("White:%d	Black:%d\n",white,black);
	do
	{
		if(path_cnt>=2)
		{
			break;
		}
		if(std::pow(SIDE,2/3)<=turn_cnt&&turn_cnt<=std::pow(SIDE,2/3)*2)
		{
			board_info=1;
		}
		else if(std::pow(SIDE,2/3)*2<=turn_cnt&&turn_cnt<=std::pow(SIDE,2)-1)
		{
			board_info=2;
		}
		bool path_flag=false;
		Pos input_pos(-1,-1);	
		if(my_turn==turn||ga_flag)
		{
			AlphaBeta(board,turn,level,white,black,0,ga_flag,gene1,gene2,genes);
			input_pos=best_pos;
		}
		else if(!ga_flag)
		{	
			//puts("Input Coordinates.(path:-1 -1)");
			//std::cin>>input_pos.x>>input_pos.y;
			//テスト用
			RandomAI(board,turn);
			input_pos=best_pos;
		}
		printf("x:%d y:%d\n",input_pos.x,input_pos.y);	
		if(input_pos.x==-1&&input_pos.y==-1)
		{
			path_flag=true;
		}
		else if(!isBetween(input_pos)) continue;
		if(path_flag)
		{
			turn*=-1;
			path_flag=false;
			path_cnt++;
			continue;
		}
		else if(canPut(board,turn,input_pos))
		{
			path_cnt=0;
			board[input_pos.y][input_pos.x]=turn;
		}
		else
		{
			//入力し直し
			continue;
		}
		turn*=-1;
		turn_cnt++;
		if(SIDE*SIDE-4-turn_cnt<level)
		{
			level=SIDE*SIDE-4-turn_cnt;
		}
		print_board(board);
		countPieces(board,&white,&black);
		printf("White:%d	Black:%d\n",white,black);
	}while(!isFullBoard(board));
}

void ga_circle(std::vector<GA> &genes,std::string dir_name)
{
	int gene1=0;
	int gene2=0;
	for(int i=0;i<GENERATION;i++)
	{
		for(int j=0;j<N_GENES;j++)
		{
			genes[j].score=0;
			genes[j].gene_population=0;
		}
		int excellence_genes[N_GENES];
		memset(excellence_genes,0,sizeof(excellence_genes));
		int excellence_total=N_GENES;
		int current_idx=0;
		int candidate_genes[5];
		while(excellence_total>1)
		{
			int idx=0;
			while(idx<5&&current_idx+idx<N_GENES)
			{
				if(excellence_genes[current_idx+idx]==0)
				{
					candidate_genes[idx]=current_idx+idx;
				}
				else {
					current_idx++;
					continue;
				}
				idx++;
			}
			//遺伝子数5つによるトーナメント制
			for(int j=0;j<idx;j++)
			{
				gene1=candidate_genes[j];
				for(int k=0;k<idx;k++)
				{
					if(j==k)continue;
					gene2=candidate_genes[k];				
					int board[SIDE][SIDE]={0};
					Osero(board,true,gene1,gene2,genes);
					genes[gene1].Cal_score(board,BLACK);
					genes[gene2].Cal_score(board,WHITE);
				}
			}
			//選択した遺伝子を候補から外す
			for(int j=0;j<idx;j++)
			{
				excellence_genes[candidate_genes[j]]=1;
				excellence_total--;
			}
			current_idx+=idx;
			if(current_idx>=N_GENES)current_idx=0;
		}
		if(i==GENERATION)break;
		puts("SelectGene");
		genes=Select_gene(genes);
		puts("SortGene");
		Sort_genes(genes,0,N_GENES-1);
		std::reverse(genes.begin(),genes.end());
		if(i<GENERATION/2)
		{
			puts("MultiPoint");
			genes=MultiPoint(genes);
		}
		else
		{
			puts("BlxAlpha");
			genes=Blx_Alpha(genes);
			puts("Fin_BlxAlpha");
		}
		puts("Mutation");
		genes=Mutation(genes);
		for(int j=0;j<genes.size();j++)
		{
			genes[j].gene_save(dir_name);
		}
	}
}
void copy_gene(GA copy_gene,GA target_gene)
{
	for(int i=0;i<6;i++)
	{
		for(int j=0;j<SIDE;j++)
		{
			for(int k=0;k<SIDE;k++)
			{
				copy_gene.gene[i][j][k]=target_gene.gene[i][j][k];
			}
		}
	}		
}
std::vector<GA> transmigration(std::vector<std::vector<GA>> islands)
{
	std::vector<GA> new_island(N_GENES);
	int island1_selected[N_GENES];
	memset(island1_selected,0,sizeof(island1_selected));
	int island2_selected[N_GENES];
	memset(island2_selected,0,sizeof(island2_selected));
	int genes_i=0;
	for(int i=0;i<N_GENES/2;i++)
	{
		int gene_rand1;
		int gene_rand2;
		while(true)
		{
			gene_rand1=Rand(0,N_GENES-1);
			gene_rand2=Rand(0,N_GENES-1);
			if(island1_selected[gene_rand1]==0&&island2_selected[gene_rand2]==0)break;
		}
		copy_gene(new_island[genes_i],islands[0][gene_rand1]);
		copy_gene(new_island[genes_i+1],islands[1][gene_rand2]);
		island1_selected[gene_rand1]=1;
		island2_selected[gene_rand2]=1;
		genes_i+=2;	
	}
	printf("size:%d",new_island.size());
	return new_island;
}
std::vector<std::vector<GA>> islands;
int main()
{
	char input;
	//遺伝的アルゴリズムを用いた遺伝子の作成を行うかどうか
	puts("GA? (y/n)");
	std::cin>>input;
	if(input=='y')
	{
		std::vector<std::thread> threads;
		for(int thread_cnt=0;thread_cnt<N_ISLAND;thread_cnt++)
		{	
			threads.push_back(std::thread([thread_cnt]()
			{
				std::vector<GA> genes(N_GENES);
				//遺伝子を格納するフォルダを作る
				std::string dir_name=make_dir(); 
				for(int i=0;i<N_GENES;i++)
				{
					genes[i].Init_gene(dir_name,i);
				}
				ga_circle(genes,dir_name);
				islands.push_back(genes);
			}));
		}
		for(int i=0;i<N_ISLAND;i++)
		{
			threads[i].join();
			//処理の一時停止(島モデル実装のため)
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
		std::vector<GA> new_island=transmigration(islands);
		std::string dir_name=make_dir(); 
		for(int i=0;i<N_GENES;i++)
		{
			new_island[i].num=i;
			new_island[i].gene_save(dir_name);
		}	
		ga_circle(new_island,dir_name);
		int max=0;
		for(int i=1;i<new_island.size();i++)
		{
			if(new_island[max].score<new_island[i].score)
			{
				max=i;
			}
		}
		printf("Max:%d\n",max);
	}
	else
	{
		int board[SIDE][SIDE]={0};
		Osero(board,false);
	}
	return 0;
}
