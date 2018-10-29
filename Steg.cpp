#include "provided.h"
#include <string>
#include <iostream>
using namespace std;

bool isWhiteSpace(char x)
{
	return (x == ' ' || x == '\t');
}

bool Steg::hide(const string& hostIn, const string& msg, string& hostOut) 
{
	if (hostIn.empty()) return false;
	int numLines = 0;
	bool foundChar = false;
	string currLine;
	string whiteSpaces;
	string tempHost;
	int hostSize = hostIn.size();
	//count the number of lines
	//strip any trailing whitespaces and add to tempHost
	for (int k = 0; k < hostSize; ++k)
	{
		char curr = hostIn[k];
		if (curr == '\n')
		{
			if (!foundChar)
				tempHost += '\n';
			else //did find a letter
				tempHost = tempHost + currLine + '\n';
			foundChar = false;
			whiteSpaces = currLine = "";
			++numLines;
		}
		else if (curr == '\r')
			; //do nothing
		else if (isWhiteSpace(curr))
			whiteSpaces += curr;
		else // we have something that can be appended to tempHost
		{
			foundChar = true;
			currLine = currLine + whiteSpaces + curr;
			whiteSpaces = "";
		}
	}
	//if hostIn does not end in nullptr then process
	if (hostIn[hostSize - 1] != '\n')
	{
		if (!foundChar)
			tempHost += '\n';
		else //did find a letter
			tempHost = tempHost + currLine + '\n';
		++numLines;
	}

	//return if message is empty
	if (msg.empty())
	{
		hostOut = tempHost;
		return true;
	}

	vector<unsigned short> ints;
	Compressor::compress(msg, ints);
	string tabs_spaces = BinaryConverter::encode(ints);

	//first lines % length have (lines/length)+1 characters
	//last #lines have L/N characers
	int msg_length = tabs_spaces.size();
	int fl = msg_length % numLines;
	int fls = (msg_length / numLines) + 1;
	int lls = msg_length / numLines;
	int count = 0;
	hostOut = "";
	int col = 0;

	for (unsigned int k = 0; k < tempHost.size(); ++k)
	{
		char curr = tempHost[k];
		//if still adding to the lines % length lines
		if (curr == '\n' && fl > 0)
		{
			//add (lines/length)+1 tabs and spaces 
			for (int j = 0; j < fls; ++j)
			{
				hostOut = hostOut + tabs_spaces[col];
				++col;
				++count;
			}
			hostOut += '\n';
			--fl;
			continue;
		}
		//else adding to the rest of the lines
		else if (curr == '\n' && fl <= 0)
		{
			//add L/N tabs and spaces
			for (int j = 0; j < lls; ++j)
			{
				hostOut = hostOut + tabs_spaces[col];
				++col;
				++count;
			}
			hostOut += '\n';
			continue;
		}
		hostOut += curr;
	}
	return true;
}

void swap(char& x, char& y)
{
	char temp = x;
	x = y;
	y = temp;
}

bool Steg::reveal(const string& hostIn, string& msg) 
{
	string tabs_spaces;
	bool foundChar = false;
	//start from end of string
	//add only trailing tabs and spaces
	for (int k = hostIn.size() - 1; k >= 0; --k)
	{
		char curr = hostIn[k];
		//searching a new line reset foundChar
		if (curr == '\n')
			foundChar = false;
		else if (isWhiteSpace(curr) && !foundChar)
			tabs_spaces += curr;
		else if (curr == '\r')
			; // do nothing
		else
			foundChar = true;
	}

	//reverse the order of tabs and spaces
	int tss = tabs_spaces.size();
	for (int k = 0; k < tss / 2; ++k)
		swap(tabs_spaces[k], tabs_spaces[tss - k - 1]);

	//if can not decode return false
	vector<unsigned short> x;
	if (!BinaryConverter::decode(tabs_spaces, x))
		return false;

	string fin_msg;
	Compressor::decompress(x, fin_msg);
	msg = fin_msg;
	return true;
}
