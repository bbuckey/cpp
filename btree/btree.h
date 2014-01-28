#include <iostream>
#include <string>
#include "btreenode.h"
using namespace std;

#ifndef BTREE_H
#define BTREE_H


class btree
{
	private:
		btreenode *root_ptr;
		size_t count_nodes(btreenode * rp);
		void insert(string x, btreenode *&rp);
		size_t countl(btreenode * rp);

			
	public:
		btree() { root_ptr = NULL;}
		size_t count_nodes(){return count_nodes(root_ptr);}
		void insert(string x){ insert(x,root_ptr);}
		size_t countl(){return countl(root_ptr);}


};
#endif