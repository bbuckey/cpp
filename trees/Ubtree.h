#include <iostream>
#include <string>
#include <fstream>
#include "btreenode.h"

using namespace std;

#ifndef UBTREE_H
#define UBTREE_H

class ubtree
{
	private:
		btreenode * uroot_ptr;
		void uinsert(string x, btreenode *& urp);
		void in_order(ofstream &outfile, btreenode *urp);
		float count_nodes(btreenode *urp);
 
			
	public:

		ubtree(){uroot_ptr = NULL;}
		void uinsert(string x){ uinsert(x,uroot_ptr);}
		void in_order(ofstream &outfile){ in_order(outfile, uroot_ptr);}
		float count_nodes(){return count_nodes(uroot_ptr);}
		

};
#endif