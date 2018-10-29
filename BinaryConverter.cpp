#include "provided.h"
#include <string>
#include <vector>
using namespace std;

const size_t BITS_PER_UNSIGNED_SHORT = 16;

string convertNumberToBitString(unsigned short number);
bool convertBitStringToNumber(string bitString, unsigned short& number);

string BinaryConverter::encode(const vector<unsigned short>& numbers)
{
	string tabs_spaces, bits;

	//convert and store each number in bits
	int len = numbers.size();
	for (int k = 0; k < len; ++k)	
		bits += convertNumberToBitString(numbers[k]);

	//convert bits to tabs and spaces
	len = bits.size();
	for (int k = 0; k < len; ++k)	
		tabs_spaces += (bits[k] == '0' ? ' ' : '\t');
	

	return tabs_spaces;
}

bool BinaryConverter::decode(const string& bitString, vector<unsigned short>& numbers)
{
	int len = bitString.size();
	if (len % 16 != 0)	return false;

	numbers.clear();
	string bits;
	//for every bit check if not a tab or space return false
	//store bit int bits, when bits.size() == 16
	//convert to number and store in vector
	for (int k = 0; k < len; ++k)
	{
		char curr = bitString[k];
		if (curr != ' ' && curr != '\t')
			return false;
		bits += (curr == ' ' ? '0' : '1');
		if (bits.size() == 16)
		{
			unsigned short x;
			convertBitStringToNumber(bits, x);
			numbers.push_back(x);
			bits = "";
		}
	}
	return true; 
}

string convertNumberToBitString(unsigned short number)
{
	string result(BITS_PER_UNSIGNED_SHORT, '0');
	for (size_t k = BITS_PER_UNSIGNED_SHORT; number != 0; number /= 2)
	{
		k--;
		if (number % 2 == 1)
			result[k] = '1';
	}
	return result;
}

bool convertBitStringToNumber(string bitString, unsigned short& number)
{
	if (bitString.size() != BITS_PER_UNSIGNED_SHORT)
		return false;
	number = 0;
	for (size_t k = 0; k < bitString.size(); k++)
	{
		number *= 2;
		if (bitString[k] == '1')
			number++;
		else if (bitString[k] != '0')
			return false;
	}
	return true;
}

unsigned int computeHash(const string& s)
{
	unsigned int tot = 0;
	for (int k = 0; k < s.size(); ++k)
		tot = tot + (k + 1) * s[k];
	return tot;
}

unsigned int computeHash(const unsigned short& i)
{
	return static_cast<unsigned int>(i);
}
