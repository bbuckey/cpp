#include <iostream>
using namespace std;


class link
{
private:
	link * next;
	int docnum;

public:
	
	link(){next = NULL; docnum = 0;}

	link * rnext(){return next;}
	int rdocnum(){return docnum;}
	void setdoc(int docn){docnum = docn;}
	void set_link(link * newlink){ next = newlink;}


};
