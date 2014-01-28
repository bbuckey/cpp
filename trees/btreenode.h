#include <iostream>
#include <string>

using namespace std;

#ifndef BTREENODE_H
#define BTREENODE_H


class btreenode
{
	private:
		btreenode * left_child;
		btreenode * right_child;
		string data;

	public:
		btreenode() {left_child = NULL; right_child = NULL; data = "";}
		string get_data(){return data;}
		const btreenode * get_left() const{return left_child;}
		btreenode *& get_left() {return left_child;}
		const btreenode * get_right() const{return right_child;}
		btreenode *& get_right() {return right_child;}
		void set_data(string new_data) {data=new_data;}
		void set_left(btreenode * new_left) {left_child = new_left;}
		void set_right(btreenode * new_right) {right_child = new_right;}
		bool is_leaf() const {return (left_child == NULL) && (right_child == NULL);}

};
#endif

