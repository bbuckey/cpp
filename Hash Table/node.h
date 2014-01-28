#include <iostream>
using namespace std;

 
class node 
{
private:
	
	node * next;
	string fname;
	string lname;
public:
	node(string _lname, string _fname){lname = _lname; fname = _fname; next = NULL;}
	node * get_next(){return next;}
	void set_next(node * _next){ next = _next;}
	string get_fname(){return fname;}
	string get_lname(){return lname;}
};
