#include <iostream>
#include <string>

using namespace std;

class link
{
private:
link * next;
string data;
public:

link(){next = NULL; data = "";}
void set_data(string _data){data = _data;}
void set_link(link *& a){next = a;}
link * r_link(){return next;}
string r_data(){return data;}

};
