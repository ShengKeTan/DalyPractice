#include <iostream>

#include "list.h"

int main(int argc, char** argv)
{
	list<int> mlist;
	mlist.push_back(1);
	mlist.push_back(2);
	mlist.push_back(3);
	std::cout << mlist.size() << std::endl;
	
	for (list<int>::iterator it = mlist.begin(); it != mlist.end(); ++it)
	{
		std::cout << (*it) << std::endl;
	}
	
	mlist.clear();
	std::cout << mlist.size() << std::endl;

	mlist.push_back(1);
	mlist.push_back(2);
	mlist.push_back(3);
	std::cout << mlist.size() << std::endl;
	for (list<int>::iterator it = mlist.begin(); it != mlist.end(); ++it)
	{
		std::cout << (*it) << std::endl;
	}

	return 0;
}
