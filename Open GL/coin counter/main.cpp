#include <iostream>
#include "coins.h"
using namespace std;


int main()
{
	int i = 0;
	int j = 0;
	int p = 0;
	char a;
	cout << "Please Choose weather you would like to use the greedy(g) or dynamic(d) algorithm" << endl;
	cin >> a;
	cout << endl;

	if(a == 'g' || a == 'G')
	{
		greedy b;
		cout << "You have chosen the greedy algorithm for coin changing " << endl;

		cout << "How many denominations of coins would you like? " << endl;
		cin >> i;
		cout << endl;
		int * coin;
		coin = (int*) new int[i];
		cout << "Please enter the denominations of coins from greatest to smallest " << endl;
		for(j = 0; j < i; j++)
		{
			cout << "The " << j+1 << " denomination is?" << endl;
			cin >> p;
			cout << endl;
			coin[j] = p;
		}
		cout << "Please enter the coin amount you wish to change " << endl;
		cin >> p;
		cout << endl;
		b.makecoinarray(coin,p,i);
		b.greedycoinchange();
	}
	else
	{
		dynamic c;
		cout << "You have chosen the dynamic algorithm for coin changing " << endl;

		cout << "How many denominations of coins would you like? " << endl;
		cin >> i;
		cout << endl;
		int * coin;
		coin = (int*) new int[i];
		cout << "Please enter the denominations of coins from greatest to smallest " << endl;
		for(j = 0; j < i; j++)
		{
			cout << "The " << j+1 << " denomination is?" << endl;
			cin >> p;
			cout << endl;
			coin[j] = p;
		}
		cout << "Please enter the coin amount you wish to change " << endl;
		cin >> p;
		cout << endl;
		c.makedenomarray(coin,p,i);
		c.makecoinarray();
		c.fillarray();
	}

return 0;
}


