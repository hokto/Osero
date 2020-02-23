#include<iostream>
#include<thread>
#include<vector>
#include<random>

int Rand(int min,int max)
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<> rand(min,max);
	return rand(mt);
}
int main()
{
	std::vector<std::thread> threads;
	int arr[3][10]={0};
	for(int i=0;i<3;i++)
	{
		threads.emplace_back([i,&arr]()
				{
					for(int j=0;j<10;j++)
					{
						arr[i][j]=Rand(0,100);
						printf("%d,%d:%d\n",i,j,arr[i][j]);
					}
				});
	}
	puts("Debug1");
	for(auto& t : threads)
	{
		t.join();
	}
	puts("Debug2");
	return 0;
}
