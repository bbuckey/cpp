#include <iostream>
#include <string>

using namespace std;


class slink
{
private:
	slink * snext;
	string stopw;

public:

	slink(){snext = NULL; stopw = "";}

	slink * re_next(){return snext;}
	string r_stopw(){return stopw;}
	void set_stopw(string docn){stopw = docn;}
	void set_slink(slink * newlink){ snext = newlink;}
};
