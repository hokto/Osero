#include<iostream>
#include<vector>
#include<string>
#include<sstream>
#include<istream>
#include<fstream>
#define SIDE 10
#define MINF -1000000000
#define INF 1000000000

const int dirX[8]={1,0,-1,-1,-1,0,1,1};
const int dirY[8]={0,-1,-1,-1,0,1,1,1};

/*Osero*/
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
				case 1:
					printf(" ●");
					break;
				case -1:
					printf(" ○");
					break;
				case 0:
					printf(" □");
					break;
			}
		}
		putchar('\n');
	}
}

bool isBetween(Pos pos)
{
	if(0<=pos.x&&pos.x<=SIDE&&0<=pos.y&&pos.y<=SIDE)
	{
		return true;
	}
	return false;
}
bool canReturn(int board[SIDE][SIDE],int turn,Pos currentPos,int dx,int dy)
{
	if(board[currentPos.y][currentPos.x]==0||!isBetween(currentPos))
	{
		return false;
	}
	else if(board[currentPos.y][currentPos.x]==turn)
	{
		return true;
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
bool canPut(int board[SIDE][SIDE],int turn,Pos currentPos)
{
	int judge=false;
	for(int i=0;i<8;i++)
	{
		Pos nextPos(currentPos.x+dirX[i],currentPos.y+dirY[i]);
		if(isBetween(nextPos)&&board[currentPos.y][currentPos.x]==0&&board[nextPos.y][nextPos.x]==turn*-1)
		{
			bool flag=canReturn(board,turn,nextPos,dirX[i],dirY[i]);
			if(!judge) judge=flag;
		}
	}
	return judge;
}
bool isFullBoard(int board[SIDE][SIDE])
{
	for(int i=0;i<SIDE;i++)
	{
		for(int j=0;j<SIDE;j++)
		{
			int cp_board[SIDE][SIDE];
			memcpy(cp_board,board,sizeof(int)*SIDE*SIDE);
			if(canPut(cp_board,1,Pos(j,i))) return false;
			memcpy(cp_board,board,sizeof(int)*SIDE*SIDE);
			if(canPut(cp_board,-1,Pos(j,i))) return false;
		}
	}
	return true;
}
Pos best_pos(-1,-1);
/*Algorithm*/
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
int Alpha(int board[SIDE][SIDE],int eva_board[SIDE][SIDE],int turn,int depth,int board_info,int alpha,int beta)
{
	if(depth==0)
	{
		return CarcuEva(board,eva_board,turn,board_info);
	}
	Pos current_pos(-1,-1);
	std::vector<Pos> pos_vec=CanPut_vec(board,turn);
}
void AlphaBeta(int board[SIDE][SIDE],int turn,int depth,int board_info)
{
	best_pos.x=best_pos.y=-1;
	int eva_board[SIDE][SIDE];
	std::ifstream ifs("../EvaBoard.txt");	
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
	Alpha(board,eva_board,turn,depth,board_info,MINF,INF);
}
int main()
{
	int board[SIDE][SIDE]={0};
	std::string input_turn;
	int my_turn=0;
	int turn=-1;
	int turn_cnt=0;
	puts("Input turn.white or black");
	std::cin>>input_turn;
	if(input_turn=="white")
	{
		my_turn=1;
	}
	else if(input_turn=="black")
	{
		my_turn=-1;
	}
	board[SIDE/2-1][SIDE/2-1]=1;
	board[SIDE/2][SIDE/2]=1;
	board[SIDE/2-1][SIDE/2]=-1;
	board[SIDE/2][SIDE/2-1]=-1;
	AlphaBeta(board,turn,5,0);
	do
	{
		bool path_flag=false;
		print_board(board);
		puts("Input Coordinates.(path:-1 -1)");
		Pos input_pos(-1,-1);	
		std::cin>>input_pos.x>>input_pos.y;
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
			continue;
		}
		else if(canPut(board,turn,input_pos))
		{
			board[input_pos.y][input_pos.x]=turn;
		}
		else
		{
			//入力し直し
			continue;
		}
		turn*=-1;
		turn_cnt++;
	}while(!isFullBoard(board));
	return 0;
}
