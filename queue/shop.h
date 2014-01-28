#include <iostream>
using namespace std;


class shop
{
private:
	int front;
	int back;
	int size;
	int arrivet[10];
	int waitt[10];

public:

	shop(){front = 1; back = 0; size = 0; arrivet[10]; waitt[10];}
	
	bool is_empty(){ if(front == size)return false; else return true;}

	void set_size(int a){ size = a;}

	void set_array()
	{
		int a = 0;
		while( a <= 10)
		{
			waitt[a] = 999999;
			arrivet[a] = 999999;
			++a;
		}
	}

	void benqueue(int service)
	{
		back = (back +1) % 10;
		waitt[back] = service;
	}

	void enqueue(int entry, int service)
	{
		back = (back + 1) % 10;
		arrivet[back] = entry;
		waitt[back] = service;

	}

	bool adequeue(int & a, int & b)
	{
		    a = arrivet[front];
			b = waitt[front];
			front = (front + 1) % 10;
    		return true;
	}

	int f_queue()
	{
		int q = 0;
		q = arrivet[front];
		return q;
	}

	bool is_not_full()
	{
		if(front != back || is_empty())
			return false;
		else 
			return true;
	}

	bool bdequeue(int & a)
	{
		if(is_empty())
			return false;
		else
		{
			a = waitt[front];
			front = (front + 1) % 10;
			return true;
		}
	}


};

