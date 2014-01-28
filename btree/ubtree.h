#include <iostream>
#include <string>
#include "btreenode.h"

using namespace std;

#ifndef UBTREE_H
#define UBTREE_H

class ubtree
{
	private:
		btreenode * uroot_ptr;
		void uinsert(string x, btreenode *& urp);
		string in_order(btreenode *urp);
		size_t count_nodes(btreenode *urp);
			
	public:

		ubtree(){uroot_ptr = NULL;}
		void uinsert(string x){ uinsert(x,uroot_ptr);}
		string in_order(){return in_order(uroot_ptr);}
		size_t count_nodes(){return count_nodes(uroot_ptr);}

};
#endif