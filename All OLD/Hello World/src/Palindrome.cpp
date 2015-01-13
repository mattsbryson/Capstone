//============================================================================
// Name        : Hello.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <array>
using namespace std;


int main()
{
	string in;
	cout << "Please enter a number to be checked for palidrome status: " << endl;
	cin >> in;


	int len=in.size();
	array<int,len> input;
	for (int i=0; i < len; i++)
	{
		input[i]=in[i];
	}

	for(int i = 0; i < input.size(); i++)
	{
		if(!(len[i] == len[input.size() - i]))
		{
			cout << "Not a palidrome." << endl;
			return 0;
		}
	}

	cout << "Palidrome." << endl;

	return 0;
}

