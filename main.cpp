//Test program for tiny3d
#include "tiny3d.h"
#include <string>
#include <iostream>
using namespace std;

int main(int argc, char** argv)
{
	for(int i = 1; i < argc; i++)
	{
		cout << "Converting " << argv[i] << endl;
		string s = argv[i];
		s.erase(s.size()-4);
		s.append(".tiny3d");
		tiny3d::fromObjToTiny(argv[i], s.c_str());
	}
	
	return 0;
}