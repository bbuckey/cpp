#include <iostream>
#include <string>
#include "btreenode.h"

using namespace std;

#ifndef PBTREE_H
#define PBTREE_H

class pbtree
{
	private:
		btreenode * proot_ptr;
		void pinsert(string x, btreenode *& prp);
		float count_nodes(btreenode *urp);
			
	public:

		pbtree(){proot_ptr = NULL;}
		void pinsert(string x){ pinsert(x,proot_ptr);}
		float count_nodes(){return count_nodes(proot_ptr);}

};
#endif