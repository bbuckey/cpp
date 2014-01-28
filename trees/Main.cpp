#include <iostream>
#include <fstream>
#include <string>
#include "btreenode.h"
#include "btree.h"
#include "ubtree.h"
#include "pbtree.h"
using namespace std;

void words_often(ifstream,ifstream,ofstream,float);

int main()
{
	string a,b;
	float countlet, counts, percent;
	btree z;
	ubtree y;
	pbtree x;
	size_t nodes = 0;


	ifstream infilea("inputa.txt");
	ifstream infileb("inputb.txt");
	ofstream outfile("output.txt");



	while (infilea >> a)
	{
		if(a == "." || a == "?" || a == "!")
			x.pinsert(a);
		else
			z.insert(a);
		if(a.length() > 3)
			++percent;
		a.erase();

	}



	while (infileb >> b)
	{
		if(b.length() > 3)
			++nodes;
		y.uinsert(b);
		b.erase();

	}

	countlet = z.countl() / z.count_nodes() ;
	counts = z.count_nodes() / x.count_nodes() ;


	outfile << "1. The number of words in the document are: " << z.count_nodes() << endl;
	outfile << "2. The number of unique words are: " << y.count_nodes() << endl;
	outfile << "3. The number of unique words with a length great then 3 are: " << nodes << endl; 
	outfile << "4. The average word length is: " << countlet << endl;
	outfile << "5. The average sentance length is: " << counts << endl;
	outfile << "6. The abc order of words with the amount they appear: "; 
	y.in_order(outfile); 
//	outfile << " " << inordernumber << endl;
	words_often(infilea, infileb, outfile, percent);
	if( counts > 10)
		outfile << "8. Sentence length to long" << endl;
	if ( countlet > 5)
		outfile << "9. Words to big" << endl; 
	infilea.close();
	infileb.close();
	outfile.close();
	return 0;
}


void words_often(ifstream infilea, ifstream infileb, ofstream outfile, float p )
{
	string x, y;
	size_t count = 0;

	while(infileb >> x)
	{

		while(infilea >> y)
		{
			if (x == y)
				++count;
			if (count >= (10*(p/100)) && x.length() > 3)
				outfile << "Word used to often: " << x << " " << endl;
		}
		count = 0;
	}
}

