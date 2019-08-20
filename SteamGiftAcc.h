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


class SteamGiftAcc {
private:
	const string SITEURL = "https://www.steamgifts.com";
	const char* USERAGENT = "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/68.0.3440.84 Safari/537.36";
	ostream clog;
	int funds = 0;
	vector<GiveAway> giveaways;
	string xsrf_token;
	string phpsessidCookie;
	bool logged;

	static size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *userp);
	int parseInt(const string&) const;
	bool enterGA(const GiveAway& ga);
	string get(const string& url, const string& cookie) const;
	string post(const string& url, const string& cookie, const string& postfields, const GiveAway& ga) const;
	void parseGiveaway(const string& url);

public:
	SteamGiftAcc();
	void parseGiveaways(int pageStart, int pageCount);
	void parseGiveaways(int pages);
	bool log_in(const string& phpsessid);
	void enterAllGA() {
		static int entered = 0;
		//fstream fileLog("log.txt", fstream::out | fstream::app);

		sort(giveaways.begin(), giveaways.end(), [](const GiveAway& ga1, const GiveAway& ga2) {
			return ga1.copies / (double)ga1.entries > ga2.copies / (double)ga2.entries;
		});

		for (const auto& g : giveaways) {
			// if (g.copies / (float)g.entries >= 0.005f) {
				if (enterGA(g)) {
					++entered;
					std::this_thread::sleep_for(std::chrono::milliseconds(500));
				}
				//fileLog << g.copies << '\t' << g.entries << '\t' << g.price << '\t' << g.copies / (double)g.entries << '\t' << g.name << '\n';
			//}
			
		}
		//fileLog.close();

		giveaways.clear();
		this->clog << string(80, '-') << endl;
		this->clog << "Entered: " << entered;
	}
};


#endif //STEAMGIFTSBOT_SGBOT_H
