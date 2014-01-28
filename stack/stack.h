#include <iostream>
#include <string>
#include "link.h"

using namespace std;

class stack
{
private:
	link * top;
public:

stack(){top = NULL;}

void push(string a)
{
	link * newnode = new link();
	newnode->set_data(a);
	newnode->set_link(top);
	top = newnode;
}

bool is_empty()
{
	if(top == NULL)
	return true;
	else
	return false;
}

string pop()
{
	string elem;
	if(is_empty())
	return "";
	else
	{
		link * p1;
		elem = top->r_data();
		p1 = top;
		top = top->r_link();
		delete p1;
		return elem;
	}
}

string top_stack()
{
  if(top != NULL)
    return top->r_data();
  else
    return "";
}

void print()
{
	while(is_empty() != true)
	{
		string x = "";
		x = pop();
		cout << x;
	}
}


};
