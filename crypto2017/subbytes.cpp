#include "stdafx.h"
#include "cipher.h"
#include <string>
#include <vector>
#include <bitset>
#include <iostream>
using namespace std;

//int sbox[256] = {
//	0x63,0x7C,0x77,0x7B,0xF2,0x6B,0x6F,0xC5,0x30,0x01,0x67,0x2B,0xFE,0xD7,0xAB,0x76,
//	0xCA,0x82,0xC9,0x7D,0xFA,0x59,0x47,0xF0,0xAD,0xD4,0xA2,0xAF,0x9C,0xA4,0x72,0xC0,
//	0xB7,0xFD,0x93,0x26,0x36,0x3F,0xF7,0xCC,0x34,0xA5,0xE5,0xF1,0x71,0xD8,0x31,0x15,
//	0x04,0xC7,0x23,0xC3,0x18,0x96,0x05,0x9A,0x07,0x12,0x80,0xE2,0xEB,0x27,0xB2,0x75,
//	0x09,0x83,0x2C,0x1A,0x1B,0x6E,0x5A,0xA0,0x52,0x3B,0xD6,0xB3,0x29,0xE3,0x2F,0x84,
//	0x53,0xD1,0x00,0xED,0x20,0xFC,0xB1,0x5B,0x6A,0xCB,0xBE,0x39,0x4A,0x4C,0x58,0xCF,
//	0xD0,0xEF,0xAA,0xFB,0x43,0x4D,0x33,0x85,0x45,0xF9,0x02,0x7F,0x50,0x3C,0x9F,0xA8,
//	0x51,0xA3,0x40,0x8F,0x92,0x9D,0x38,0xF5,0xBC,0xB6,0xDA,0x21,0x10,0xFF,0xF3,0xD2,
//	0xCD,0x0C,0x13,0xEC,0x5F,0x97,0x44,0x17,0xC4,0xA7,0x7E,0x3D,0x64,0x5D,0x19,0x73,
//	0x60,0x81,0x4F,0xDC,0x22,0x2A,0x90,0x88,0x46,0xEE,0xB8,0x14,0xDE,0x5E,0x0B,0xDB,
//	0xE0,0x32,0x3A,0x0A,0x49,0x06,0x24,0x5C,0xC2,0xD3,0xAC,0x62,0x91,0x95,0xE4,0x79,
//	0xE7,0xC8,0x37,0x6D,0x8D,0xD5,0x4E,0xA9,0x6C,0x56,0xF4,0xEA,0x65,0x7A,0xAE,0x08,
//	0xBA,0x78,0x25,0x2E,0x1C,0xA6,0xB4,0xC6,0xE8,0xDD,0x74,0x1F,0x4B,0xBD,0x8B,0x8A,
//	0x70,0x3E,0xB5,0x66,0x48,0x03,0xF6,0x0E,0x61,0x35,0x57,0xB9,0x86,0xC1,0x1D,0x9E,
//	0xE1,0xF8,0x98,0x11,0x69,0xD9,0x8E,0x94,0x9B,0x1E,0x87,0xE9,0xCE,0x55,0x28,0xDF,
//	0x8C,0xA1,0x89,0x0D,0xBF,0xE6,0x42,0x68,0x41,0x99,0x2D,0x0F,0xB0,0x54,0xBB,0x16
//};

int addVar = 198;
int sBox[256] = { 0 };
bool firstTime = true;
int affineConst = 143;
int irreducable = 0x11B;
//int state[256] = { 0x01, 0x01, 0x02 };

int productInFiniteField(int num1, int num2) {

	int i = 7;
	int product = 0;
	bitset<16> prod = 0;
	bitset<8> num_1 = num1;
	bitset<16> num_2 = num2;

	while (i >= 0) {
		if (num_1[i] == 1) {
			prod = prod ^ (num_2 << i);
		}
		i--;
	}
	return (int)(prod.to_ulong());
}


int mulInverse(int init) {
	if (init == 0) {
		return 0;
	}

	int temp = irreducable;
	int q = 0;
	int prev = 0;
	int present = 1;

	while (temp > 1) {
		int q1 = 1;
		int pos1 = position(init);//finding position of first 1 in the divisor
		int pos2 = position(temp);// finding position of first 1 in the divident

		if (pos1 <= pos2) {//if position of dividend is greater than the position of the divisor, divisor has to be multiplied (or shifted, in case of binary)
			q1 = q1 << (pos2 - pos1); //Finding quotient for the first xor
			int temp2 = init << (pos2 - pos1);
			temp = temp^temp2;//xor the values to get the remainer (Here, temp is the remainder)
		}
		else {//here we are trying to calculate the intermediate inverse of the function using quotients
			int random = prev;
			prev = present;//storing previous intermediate inverse
			present = productInFiniteField(present, q); //product in finite field to get the new intermediate inverse
			present = random ^ present;
			int temporary = temp;
			temp = init;
			init = temporary;
			q1 = 0;
			q = 0;
		}
		q += q1;
	}

	int random = prev;
	prev = present;
	present = productInFiniteField(present, q);//finding the final inverse by finding the product in finite field and xoring it with the previous intermediate inverse.
	present = random ^ present;
	return present;
}
int affineTrans(int k) {
	if (k == 0x11a) {//since 01 doesn't have an inverse that can be represented in 8 bits.
		return 0x7c;
	}

	int i = 0, j = 0;
	bitset<8> prod, finalVal = 0;
	bitset<8> affineConstBit = affineConst;
	bitset<8> value = k;
	prod = value;

	for (int i = 0; i < 8; i++) {//reversing the bits for easy calculation
		value[7 - i] = prod[i];
	}

	while (i < 8) {//Affine transformation done here. Matrix multiplication+ addition of the matrix
		prod = affineConstBit & value;

		while (j < 8) {
			finalVal[i] = finalVal[i] ^ prod[j];
			j++;
		}

		j = 0;
		int temp = affineConstBit[0];
		affineConstBit = affineConstBit >> 1;
		affineConstBit[7] = temp;
		i++;
	}

	prod = finalVal;
	for (int i = 0; i < 8; i++) {
		finalVal[7 - i] = prod[i];
	}

	bitset<8> addVarBit = addVar;
	finalVal = finalVal ^ addVarBit;
	prod = finalVal;

	for (int i = 0; i < 8; i++) {
		finalVal[7 - i] = prod[i];
	}

	return (int)(finalVal.to_ulong());
}
void SubBytes(int state[][4]) {
	for (int i = 0; i < 4; i++) {
		/*if (i % 16 == 0) {
			cout << endl;
		}*/
		for (int j = 0; j < 4; j++) {
			state[i][j] = affineTrans(mulInverse(state[i][j]));//generating state here using sbox
			//cout << std::hex << state[i][j] << " ";
		}
		cout << endl;
	}
}

//
//void SubBytes(int state[][4])
//{
//	int k = mulInverse(0x10);
//	bitset<8> kk = k;
//	int r = affineTrans(k);
//	int m;
//	m = 44;
//	byteSub();
//	cin >> m;
//}
