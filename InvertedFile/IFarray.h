#include <iostream>
#include <fstream>
#include <string>
#include "IFnode.h"
#include "IFdnode.h"
#include "IFstops.h"
using namespace std;


const int max = 100;
class ifarray
{
private:
	link * parray[max];
	string karray[max][max];
	link * marray[max];
	int size;
	int msize;
	dlink * head_ptr;
	slink * s_ptr;

public:

	ifarray(){size = 0; msize = max; head_ptr = NULL; s_ptr = NULL;}

	void set_size(int _size){size = _size;}
	int r_size(){ return size;}

	void setarray()
	{
		for(int i = 0; i < msize; ++i)
		{
			parray[i] = NULL;
			karray[i][i] = "";
			marray[i] = NULL;
		}

	}

	void stop_insert(string entry)
	{
		slink * newbox = new slink;
		newbox->set_stopw(entry);
		newbox->set_slink(s_ptr);
		s_ptr = newbox;
	}
	
	void list_head_insert(link *& a,int entry)
	{
		link * newbox = new link;
		newbox->setdoc(entry);
		newbox->set_link(a);
		a = newbox;
	}

	void insert_muti(link *& a,int entry)
	{
		link * newbox = new link;
		newbox->setdoc(entry);
		newbox->set_link(a);
		a = newbox;
	}

	void insert_s(string x, int i)
	{
		karray[i][i] = x;
		list_head_insert(parray[i],0);
		insert_muti(marray[i],0); 
	}

	void insert_d(int docn, int key)
	{
		if(parray[key]->rdocnum() == 0)
		{
			link * a = parray[key];
			parray[key] = NULL;
			delete a;
		}
		list_head_insert(parray[key],docn);
	}

	void insert_mu(int docn, int key)
	{
		if(marray[key]->rdocnum() == 0)
		{
			link * a = marray[key];
			marray[key] = NULL;
			delete a;
		}
		insert_muti(marray[key],docn);
	}

	bool checkdub(string x, int r)
	{
		if(r == 0)
			return false;
		for(int i = 0; i < r; ++i)
		{
			if( x == karray[i][i])
				return true;
		}
		return false;
	}

	void checkif(string x, int p)
	{
		for(int i = 0; i < size; ++i)
		{
			if(karray[i][i] == "")
				;
			else if(x == karray[i][i])
			{
				insert_d(p,i);
			}
		}
		return ;
	}


	void head_insert(string a, int number /*int anumber*/)
	{
		dlink * newbox = new dlink;
//		newbox->set_darray(anumber);
		newbox->set_sdoc(a);
		newbox->set_dnum(number);
		newbox->set_dlink(head_ptr);
		head_ptr = newbox;
	}

	int docnumb(){ return head_ptr->r_dnum();}


	bool f_stopw(string x)
	{
		for(slink * p1 = s_ptr; p1 != NULL; p1=p1->re_next())
		{
			if(x == p1->r_stopw())
				return true;
		}
		return false;
	}

	void print(ofstream &outfile)
	{
		string s;
		for(int i = 0; i < size; ++i)
		{	
			if(karray[i][i] == "")
				return;
			if(karray[i][i] == "*")
			{++i;}

			if(karray[i][i] != "*" && karray[i+1][i+1] != "*")
			{
				int x = 0;
				i = findingmuti(i);
				for(link * p1 = marray[i]; p1 != NULL; p1 = p1->rnext())
			{
				int q = 0;
				q = p1->rdocnum();
				outfile << q << " ";
			}
			}
			else if(karray[i][i] != "" && karray[i+1][i+1] == "*")
			{
				for(link * p1 = parray[i]; p1 != NULL; p1 = p1->rnext())
			{
				int x = 0;
				x = p1->rdocnum();
				outfile << x << " ";
			}
			}
			
			outfile << endl;
			outfile << "*" << endl;	
		}
	}

	 int findingmuti(int i)
	 {
		 if(karray[i][i] != "*" && karray[i+1][i+1] != "*")
			for(link * p1 = parray[i]; p1 != NULL; p1 = p1->rnext())
				{
				for(link * p2 = parray[i+1]; p2 != NULL; p2 = p2->rnext())
				{
					int x = 0;
					if( p1->rdocnum() == p2->rdocnum())
					{
					x = p1->rdocnum();
					insert_mu(x,i+1);
					}
				}
				}
			++i;
		 while(karray[i+1][i+1] != "*" && karray[i][i] != "*")
				{
				for(link * p3 = parray[i+1]; p3 != NULL; p3 = p3->rnext())
						{
						for(link * p10 = marray[i]; p10 != NULL; p10 = p10->rnext())
						{	int x = 0;
							if( p10->rdocnum() == p3->rdocnum())
							{
							x = p10->rdocnum();
							insert_mu(x,i+1);								
							}
						}
					}
				++i;
			 }
		return i;
	 }

	 void find_words()
	 { 
		 for( dlink * p7 = head_ptr; p7 != NULL; p7 = p7->next_d())
			 {
				 checkif(p7->r_doc(),p7->r_dnum()); 
			 }
		 
		 return;
	 }


};

