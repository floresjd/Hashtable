#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
#include "HashTable.h"
using namespace std;

//followed pseudocode
void Compressor::compress(const string& s, vector<unsigned short>& numbers)
{
	unsigned short c = ((s.size() / 2 + 512 < 16384) ? s.size() / 2 + 512 : 16384);
	HashTable<string, unsigned short> H(c*2, c);
	for (unsigned int k = 0; k < 256; ++k)
	{
		string temp;
		temp += static_cast<char>(k);
		H.set(temp, k, true);
	}
	int nextFreeID = 256;
	string runSoFar;
	vector<unsigned short> us;
	unsigned short size = (s.size());
	for (unsigned short k = 0; k < size; ++k)
	{
		string expandedRun = runSoFar + s[k];
		unsigned short x;
		if (H.get(expandedRun, x))
		{
			runSoFar = expandedRun;
			continue;
		}

		H.get(runSoFar, x);
		us.push_back(x);
		H.touch(runSoFar);
		runSoFar = "";
		string temp; temp += s[k];
		unsigned short cv;
		H.get(temp, cv);
		us.push_back(cv);
		if (!H.isFull())
		{
			H.set(expandedRun, nextFreeID);
			++nextFreeID;
		}
		else
		{
			string nothing;
			unsigned short id;
			H.discard(nothing, id);
			H.set(expandedRun, id);
		}
	}//end for
	if (!runSoFar.empty())
	{
		unsigned short x;
		H.get(runSoFar, x);
		us.push_back(x);
	}
	us.push_back(c);
	numbers = us;
	//apend the capacity of the string to be compresssed
} //end Compressor::encode


//followed pseudo code
bool Compressor::decompress(const vector<unsigned short>& numbers, string& s)
{
	if (numbers.empty())
		return false;
	HashTable<unsigned short, string> H(numbers.back() * 2, numbers.back());
	for (unsigned int k = 0; k < 256; ++k)
	{
		string temp;
		temp += static_cast<char>(k);
		H.set(k, temp, true);
	}
	unsigned int nextFreeID = 256;
	string runSoFar, output;
	unsigned int sz = numbers.size();
	for (unsigned short k = 0; k < sz - 1; ++k)
	{
		string x;
		H.get(numbers[k], x);
		if (numbers[k] <= 255)
		{
			output += x;
			string value;
			H.get(numbers[k], value);
			if (runSoFar.empty())
			{
				runSoFar += value;
				continue;
			}
			string expandedRun = runSoFar + value;
			if (!H.isFull())
			{
				H.set(nextFreeID, expandedRun);
				++nextFreeID;
			}
			else
			{
				string nothing;
				unsigned short id;
				H.discard(id, nothing);
				H.set(id, expandedRun);
			}
			runSoFar = "";
			continue;
		}
		string t;
		if (!H.get(numbers[k], t))
		{
			std::cout << output << "   HashTable does not contain assosication\n";
			return false;
		}
		H.touch(numbers[k]);
		string S;
		H.get(numbers[k], S);
		output += S;
		runSoFar = S;
	}//end for
	//output holds decompressed message
	s = output;
	return true;
} // end Compressor::decode
