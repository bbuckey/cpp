#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include "IFarray.h"


using namespace std;

int main()
{
	ifarray q;
	int num = 1;
	string qu, ments;
	string ast = "*";
	q.setarray();

	ifstream document("documents.txt");
	ifstream stopword("stopwords.txt");
    ifstream querie("queries.txt");
	ofstream outfile("output.txt");
	assert(document);
	assert(stopword);
	assert(querie);
	assert(outfile);


	while(stopword >> ments)
	{
		q.stop_insert(ments);
		ments.erase();
	}

	int r = 0;
	while(querie >> qu)
	{
		q.insert_s(qu,r);
		++r;
		qu.erase();
	}

	q.set_size(r);

	while(document >> qu)
	{
		if(qu == ast)
		{
			++num;
		}
		else if(q.f_stopw(qu))
		{	
			qu.erase();
		}
	    else if(qu != "")
		{
			q.head_insert(qu,num);
			qu.erase();
		}
	}

	q.find_words();
	q.print(outfile);

	document.close();
	stopword.close();
	querie.close();
	outfile.close();

	return 0;



}



/*	while(document >> qu)
	{
		if(qu == ast)
		{
			++num;
		}
		else
		{
			if(q.f_stopw(qu))
			{
				qu.erase();
			}
			else
			{
				if(q.checkif(qu,num))
				{
					qu.erase();
				}
				else if( qu != "")
				{
					int x = q.anum(qu);
					q.head_insert(qu,num,x);
					qu.erase();
				}
			}
		}
	}*/


