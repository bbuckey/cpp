#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include "hash.h"


int main()
{
	cout << endl;
	hash b;
	string fname;
	string lname;
	int key = 0;
	ifstream number("phonenum.txt");

	b.setarray();
	while(number >> key)
	{

		number >> lname;
		number >> fname;
		b.hashfun(key,fname,lname);
	}

	char nums[4];
	while(true)
	{
		cout << ">";
		cin >> nums;
		cout << endl;
		if(nums[0] == 'q')
			break;
		int r = atoi(nums);
		b.findhash(r);
	}

	number.close();

	return 0;
}