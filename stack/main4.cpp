#include <iostream>
#include <string>
#include "stack.h"

using namespace std;

char operator= (char x, string y);

int main()
{
	string r;
	stack a;	
	string x;
	string p;

	cout << "Enter in the expresstion you want to make postfix" << endl;
	cin >> x;
	
	for(int i = 0; i < x.length(); ++i)
	{
		char h;
		h = x.substr(i,1);
		switch(h)
		{
		case '-':
			while( a.top_stack() != "(" && a.is_empty() == false)
				{
				  r = a.pop();
				  cout << r ;
				  r.erase();
				}
				a.push(x.substr(i,1));
				break;
		case x.substr(i,1) == '+':
			while( a.top_stack() != "(" && a.is_empty() == false)
				{
				  r = a.pop();
				  cout << r ;
				  r.erase();
				}
				a.push(x.substr(i,1));
				break;
		case x.substr(i,1) == '*':
			a.push(x.substr(i,1));
			break;
		case x.substr(i,1) == '/':
			a.push(x.substr(i,1));
			break;
		case x.substr(i,1) == '(':
			a.push(x.substr(i,1));
			break;
		case x.substr(i,1) == ')':
				
				while(a.top_stack() != ")" && a.is_empty() == false)
				  {
					if( a.top_stack() == "(")
					{ 
						p = a.pop();
						p.erase();
						break;
					}
				
					p = a.pop();
					cout << p;
					p.erase();
			
				  }
				break;
		default :
			p = "";
			p = x.substr(i,1);
			cout << p;
		    break;
		}
	}

	a.print();
	cout << endl;
	return 0;
}

char operator= (char x, string y)
{

}

