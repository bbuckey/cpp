#include <iostream>
#include <string>

using namespace std;

class dlink
{
private:
	int dnumber;
//	int darrayn;
	string document;
	dlink * nextd;

public:

	dlink(){dnumber = 0; document = ""; nextd = NULL; /*darrayn = 0;*/}

//	void set_darray(int _num){ darrayn = _num;}
	void set_dnum(int _doc){ dnumber = _doc;}
	void set_sdoc(string a){ document = a;}
	dlink * next_d(){ return nextd;}
	void set_dlink(dlink * _nextd){ nextd = _nextd;}
	string r_doc(){ return document;}
	int r_dnum(){ return dnumber;}
//	int r_darray(){ return darrayn;}

};
	
