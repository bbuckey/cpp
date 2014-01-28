#include <iostream>
#include <string>
#include "node.h"

using namespace std;

const size_t msize = 250;


class hash
{
private:
	node * harray[msize][1];
	int size;
public:
	
	hash(){size = 250;}

	void setarray(){ for(int i =0; i <= 250; ++i){ for (int j = 0; j <= 1; ++j)harray[i][j] = NULL;}}

	void hashfun(int entry, string fname, string lname)
	{
		int slot = 0;
		int key = entry % 250;
		cout << "Still here?";

		if(check_home(key,slot)) inserthash(key,slot,fname,lname);
		int h;
		h = entry % 250;
		int mods = 250/2;

		for(int step = 1; step < mods; ++step)
		{
			key = (h + step) % mods;
			if(check_home(key,slot))inserthash(key,slot,fname,lname);
			key = (mods + h - step) % mods;
			if(check_home(key,slot))inserthash(key,slot,fname,lname);
		}
	}

	bool check_home(int key, int & slot)
	{
		if(harray[key][0] == NULL) { slot = 0; return true;}
		else if(harray[key][1] == NULL){ slot = 1; return true;}
		else
			return false;

	}

	void list_head_insert(node *& box, string fname, string lname)
	{
		node * a = new node(lname,fname);
		a->set_next(box);
		box = a;
	}

	void inserthash(int key, int slot, string fname, string lname)
	{
		list_head_insert(harray[key][slot],fname,lname);
	}

	void findhash(int entry)
	{
		node  * r;
		int counter;
		int slot = 0;
		int key = entry % size;

		if(harray[key][0] != NULL) 
		{
			r = harray[key][0];
			cout << r->get_lname() << "," << r->get_fname() << endl;
		}
		if(harray[key][1] != NULL)
		{
			r = harray[key][1];
			cout << r->get_lname() << "," << r->get_fname() << endl;
		}

		int h = entry % size;
		int mods = size/2;

		for(int step = 1; step < mods; ++step)
		{
			counter = 0;
			key = (h + step) % mods;
			if(check_probe(key,slot))
			{
			r = harray[key][slot];
			cout << r->get_lname() << "," << r->get_fname() << endl;
			}
			else
				++counter;
			key = (mods + h - step) % mods;
			if(check_probe(key,slot))
			{
			r = harray[key][slot];
			cout << r->get_lname() << "," << r->get_fname() << endl;
			}
			else
				++counter;
			if(counter == 2)break;
		}
	}

	bool check_probe(int key,int & slot)
	{
		if(harray[key][0] != NULL)
		{
			slot = 0;
			return true;
		}
		if(harray[key][1] != NULL)
		{
			slot = 1;
			return true;
		}
		return false;
	}



};	