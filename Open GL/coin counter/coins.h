#include <iostream>
using namespace std;


class greedy
{
private:
	int * coinarray;
	int amount;
	int clen;

public:
	greedy(){amount =0; clen=0;}

	void makecoinarray(int *coin, int _amount, int len)
	{
		clen = len;
		amount = _amount;
	coinarray = (int*) new int[len];
	for(int i = 0; i < len; ++i)
	{
		coinarray[i] = coin[i];
	}
	}

	void greedycoinchange()
	{
		int _amount = amount;
		int i = 0;
			while(_amount > 0)
			{
				if(_amount >= coinarray[i])
				{
					if(coinarray[i] == 1)
					{
				cout << "The amount is " << _amount << " coin value " << coinarray[i] << " will be subtracted from the amount" << endl;
				cout << "There was " << _amount << " 1 used"<< endl;
				_amount = _amount - _amount
					cout << endl;
					}
					else
					{
					cout << "The amount is " << _amount << " coin value " << coinarray[i] << " will be subtracted from the amount" << endl;
					_amount = _amount - coinarray[i];
					cout << "The new value is " << _amount << endl;
					cout << endl;
					}
				}
				else
				{ 
					cout << "The amount " << _amount << " is less then " << coinarray[i] << " moveing to the next denomination " << endl;
					i++;
					cout << "The next denomination is " << coinarray[i] << endl;
					cout << endl;
				}
				char a;
				while(a != y)
				{
				cout << "would you like to go to the next step or skip to end? (y,n,s)"<< endl;
				cin >> a;
				if(a == 's')
					a = 'y';
				else
					a = 'n';
				cout << endl;
				cout << endl;

			}
	}
};

class dynamic
{
private:
	int ** coinarray;
	int * denom;
	int amount;
	int len;

public:
	dynamic(){amount =0; len =0;}

	void makedenomarray(int *coin, int _amount, int _len)
	{
		len = _len;
		amount = _amount;
	denom = (int*) new int[_len];
	for(int i = 0; i < _len; ++i)
	{
		denom[i] = coin[i];
	}
	}

	void makecoinarray()
	{
		int i,j;
		coinarray = (int**) new int*[len];
		for(i = 0; i < len; i++)
			coinarray[i] = new int[amount+1];

		for(i = 0; i < len; i++)
			for(j = 0; j < amount+1; j++)
				coinarray[i][j]=0;
	}

	void fillarray()
	{
		char a = 'n';
		int i =0;
		cout << "Filling in the lowest denomination row. From 0 to " << amount <<endl;
		for( i = 0; i < amount+1; i++)
		{
			coinarray[len-1][i] = i;
		}
		
		for( i = len-2; i >= 0; i--)
		{
			for(int j = 0; j < amount+1; j++)
			{	if(denom[i] > j || coinarray[i+1][j] < 1+coinarray[i][j-denom[i]])
				{cout << "the value " << denom[i] << " is greater then " << j << " or the value " << coinarray[i+1][j] << " is less then " << coinarray[i][j-denom[i]] << endl;
				coinarray[i][j]=coinarray[i+1][j];
				cout << "The value " << coinarray[i+1][j] << " was placed in location" << i << ", " << j << endl;  }
				else
				{coinarray[i][j]= 1+coinarray[i][j-denom[i]];
					cout << "The value " << 1+coinarray[i][j-denom[i]] << " was placed in location" << i << ", " << j << endl; }
				cout << endl;
				while(a != y)
				{
				cout << "would you like to go to the next step or skip to end? (y,n,s)"<< endl;
				cin >> a;
				if(a == 's')
					a = 'y';
				else
					a = 'n';
				}
			}
		}

		cout << "The coin matrix is " << endl;
		cout << "     ";

		for( i = 0; i < amount + 1;i++)
		{
			if(i < 10)
			{
			cout << i << "  ";
			}
			else
			cout << i << " ";
		}

		for( i = 0; i < len; i++)
		{
			cout << endl;
			if(denom[i] < 10)
			{
			cout << denom[i] << "  ";
			}
			else
			cout << denom[i] << " ";

			for(int j = 0; j < amount+1; j++)
			{
				if(coinarray[i][j] < 10)
				{
				cout << "  " << coinarray[i][j];
				}
				else
				cout << " " << coinarray[i][j];
			}
		}
		cout << endl;
		cout << "The martix tells you the optimal solution to the problem for that coin type" << endl;
		cout << "if the denomination is greater then the value the matrix tells you the best solution" << endl;
		cout << "for a lower coin value ... as you can see if you have the value 10 and only the amount 1" << endl;
		cout << "you will see a 1 in the matrix this is because you only need 1 coin to solve the solution" << endl;
		
	}



};