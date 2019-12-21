#include<iostream>
#include<string>
#define SIDE 10


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
	int dirX[8]={1,0,-1,-1,-1,0,1,1};
	int dirY[8]={0,-1,-1,-1,0,1,1,1};
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
void AlphaBeta(int board[SIDE][SIDE],int turn,int depth,int board_info)
{
	best_pos.x=best_pos.y=-1;
	int eva_board[SIDE][SIDE];
	std::ifstream ifs("../EvaBoard.txt");	
	if(ifs.fail())
	{
		printf("Failed to open file.\n");
		return -1;
	}
	std::string line;
	int i=0;
	while(std::getline(ifs,line))
	{
		std::stringstream ss{line};
		std::string line_sp;
		int j=0;
		while(std::getline(ss,line_sp,','))
		{
			eva_board[i][j]=atoi(line_sp.c_str());
			j++;	
		}
		i++;
	}
	for(int i=0;i<SIDE;i++)
	{
		for(int j=0;j<SIDE;j++)
		{
			printf(" %d",eva_board[i][j]);
		}
		putchar("\n");
	}
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
