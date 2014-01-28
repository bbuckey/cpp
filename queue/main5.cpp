#include <iostream>
#include "shop.h"
using namespace std;

int main ()
{
	shop a;
	shop b;
	int time = 0;
	int people = 0;
	int next_arrive = 0;
	int next_completion = 999999;
	int c = 0;
	int d = 0;
	
	a.set_array();
	b.set_array();

	cin >> people;
	cout << endl;

	if(people > 10)
	{
		people = 10;
		cout << "You can not have more then 10 people" << endl;
	}
	int h = people;
	a.set_size(people);
	b.set_size(people);
	while( h > 0)
	{
		cin >> c;
		cout << endl;
		cin >> d;
		cout << endl;
		a.enqueue(c,d);
		--h;
		c = d = 0;
	}
	h= 0;
	h = a.f_queue();

	next_arrive = time + h;
	
	while(next_completion != 999999 || next_arrive != 999999)
	{
		c = d = 0;
		cout << next_arrive << "      " << next_completion << endl;
		if(next_arrive <= next_completion)
		{
			time = next_arrive;
			if(next_completion == 999999)
			{
				cout << "this part" << endl;
				next_completion = time;
				a.adequeue(c,d);
			}
			if(b.is_not_full())
			{
				cout << "help" << endl;
				b.benqueue(d);
				if(a.is_empty())
					next_arrive = 999999;
				else
				{
					h = 0;
					h = a.f_queue();
					next_arrive = time + h;
				}
			}
		}
		else
		{
			cout << "No this mess" << endl;
			time = next_completion;
			b.bdequeue(d);
			if(b.is_empty())
			next_completion = 999999;
			else
			next_completion = time + d;
			
		}
	}

	
	return 0;
}




