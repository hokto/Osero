#include<iostream>
#include<random>

int main()
{
	for(int i=0;i<100;i++)
	{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<> rand(0,1000000000);
	printf("%d\n",rand(mt));
	}
	return 0;
}
