#include "provided.h"
#include <string>
using namespace std;

#include "http.h"

bool WebSteg::hideMessageInPage(const string& url, const string& msg, string& host)
{
	string contents;
	HTTP().get(url, contents);
	string temp;
	if (Steg::hide(contents, msg, temp))
	{
		host = temp;
		return true;
	}
	return false;
}

bool WebSteg::revealMessageInPage(const string& url, string& msg)
{
	string temp, contents;
	HTTP().get(url, contents);
	if (Steg::reveal(contents, temp))
	{
		msg = temp;
		return true;
	}
	return false;
}
