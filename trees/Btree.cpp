#include <iostream>
#include <string>
#include <fstream>
#include "btreenode.h"
#include "btree.h"
#include "ubtree.h"
#include "pbtree.h"
using namespace std;


float btree::count_nodes(btreenode *rp)
{
	if(rp == NULL)
		return 0;
	else
		return (count_nodes(rp->get_left()) + count_nodes(rp->get_right()) + 1);
}


void btree::insert(string x,btreenode *&rp)
{
	if(rp == NULL)
	{
		rp = new btreenode;
		rp->set_data(x);
	
	}
	else if(x < rp->get_data())
		insert(x, rp->get_left());
	else
		insert(x, rp->get_right());
}


float btree::countl(btreenode * rp)
{
	if ( rp == NULL)
		return 0;
	return (rp->get_data().length() + countl(rp->get_left()) + countl(rp->get_right()));
}


void ubtree::in_order(ofstream &outfile, btreenode * urp)
{
	if(urp == NULL)
		return;
    in_order(outfile, urp->get_left()); 
	outfile << urp->get_data(); 
	in_order(outfile, urp->get_right());
}

void ubtree::uinsert(string x,btreenode *&urp)
{

	if(urp == NULL)
	{
		urp = new btreenode;
		urp->set_data(x);
	}
	else if(x < urp->get_data())
		uinsert(x, urp->get_left());
	else
		uinsert(x, urp->get_right());
}


float ubtree::count_nodes(btreenode *urp)
{
	if(urp == NULL)
		return 0;
	else
		return (count_nodes(urp->get_left()) + count_nodes(urp->get_right()) + 1);
}

float pbtree::count_nodes(btreenode *prp)
{
	if(prp == NULL)
		return 0;
	else
		return (count_nodes(prp->get_left()) + count_nodes(prp->get_right()) + 1);
}


void pbtree::pinsert(string x,btreenode *&prp)
{
	if(prp == NULL)
	{
		prp = new btreenode;
		prp->set_data(x);
	
	}
	else if(x < prp->get_data())
		pinsert(x, prp->get_left());
	else
		pinsert(x, prp->get_right());
}

