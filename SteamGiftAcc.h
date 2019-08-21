//
// Created by e on 09/08/18.
//

#ifndef STEAMGIFTSBOT_SGBOT_H
#define STEAMGIFTSBOT_SGBOT_H


#include <fstream>
#include <vector>
#include <string>
#include <curl/curl.h>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include "htmlcxx/html/ParserDom.h"
#include <thread>
#include <iomanip>
#include <sstream>
//#include "Duration.h"

#define SLEEP_PAGE_PARSE_MS 2500

using namespace std;

struct GiveAway {
	GiveAway() : copies(1), entries(0), price(0) {
	}

	string href;
	string name;
	int price;
	int copies;
	int entries;

	string getCode() const {
		return move(string(href.begin() + 10, href.begin() + 10 + 5));
	}
	void setName(const string& name) {
		this->name = name;
	}
	void setName(string&& name) {
		this->name = std::move(name);
	}
};

typedef std::vector<GiveAway> GArray;


enum ERROR{	PREVIOUSLY_WON = -10, FUNDS_RAN_OUT,
			UNKNOWN, OK	= 1};


class SteamGiftAcc {
private:
	const string SITEURL = "https://www.steamgifts.com";
	const char* USERAGENT = "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/68.0.3440.84 Safari/537.36";
	ostream clog;
	int funds = 0;
	string xsrf_token;
	string phpsessidCookie;
	bool logged;
	static SteamGiftAcc* instance;

	SteamGiftAcc();

	static size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *userp);
	static int parseInt(const string&);

	
	string get(const string& url, const string& cookie) const;
	string post(const string& url, const string& cookie, const string& postfields, const GiveAway& ga) const;
	void parseGiveaway(const string& url, GArray* array);

public:
	static SteamGiftAcc& getInstance();

	ERROR enterGA(const GiveAway& ga);
	GArray parseGiveaways(int pageCount = 1, int pageStart = 1);
	bool log_in(const string& phpsessid);
	//void enterAllGA();
};

#endif //STEAMGIFTSBOT_SGBOT_H