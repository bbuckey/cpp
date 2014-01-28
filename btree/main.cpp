#include <iostream>
#include <fstream>
#include <string>
#include "btreenode.h"
#include "btree.h"
#include "ubtree.h"
#include "pbtree.h"
using namespace std;


int main()
{
	string a,b,c;

	btree z;
	ubtree y;
	pbtree x;



	ifstream infilea("inputa.txt");
	ifstream infileb("inputb.txt");
	ofstream outfile("output.txt");



	while (infilea >> a)
	{
		if(a == "." || a == "?" || a == "!")
			x.pinsert(a);
		else
			z.insert(a);
		a.erase();

	}



	while (infilea >> b)
	{

		y.uinsert(b);
		b.erase();

	}


/*	countlet = z.count_nodes() / z.countl();
	counts = z.count_nodes() /  ;

	outfile << "1. The number of words in the document are: " << z.count_nodes() << endl;
	outfile << "2. The number of unique words in the document are: " << << endl;
	outfile << "3. The number of unique words with a length great then 3 are: " <<  << endl; ;
	outfile << "4. The average word length is: " <<  << endl;
	outfile << "5. The average sentance length is: " <<  << endl;
//	outfile << "6. The abc order of words with the amount they appear: " <<  << " " <<  << endl;
//	outfile << "7. Words that were used to often are: " << "" << endl;
	if( > 10)
		outfile << "8. Sentence length to long" << endl;
	if (  > 5)
		outfile << "9. Words to big" << endl; */
	infilea.close();
	infileb.close();
	outfile.close();
	return 0;
}


