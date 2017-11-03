// crypto2017.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <vector>
#include <iostream>
#include "cipher.h"

using namespace std;

int main()
{
	vector <int> cipherKey = {
		0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c
	};

	vector<vector<int>> expandedKeys = KeyExpansion(cipherKey, 4);

	vector <vector <int>> state = {
		{0x32,0x88,0x31,0xe0},
		{0x43,0x5a,0x31,0x37},
		{0xf6,0x30,0x98,0x07},
		{0xa8,0x8d,0xa2,0x34}
	};

	state = SubBytes(state, 4);
	state = ShiftRows(state, 4);
	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			cout << hex << state[row][column] << ' ';
		}
		cout << endl;
	}
	cout << dec;
	system("pause");
	return 0;
}
