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
#include <thread>
#include <iomanip>
#include <sstream>
#include "htmlcxx/html/ParserDom.h"
//#include "Duration.h"

#define FILL_WIDTH 80
#define SLEEP_PAGE_PARSE_MS 2500

#define LOG
//#undef DEBUG

#ifdef DEBUG
#define D(str) do { clog << str << std::endl; } while( false )
#else
#define D(str) do { } while ( false )
#endif

#ifdef LOG
#define Log(str) do { clog << str << std::endl; } while( false )
#else
#define Log(str) do { } while ( false )
#endif

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
	float getChancePercent() const{
		return ((float)copies/(float)(entries == 0 ? copies : entries)) * 100.0f;
	}

	explicit operator std::string() const{
		std::stringstream ss;
		ss << "{Href: " << href << "\nName: " << name << "\nPrice: " << 
			price << "\nCopies: " << copies << "\nEntries: " << entries << '}';
		return ss.str();
	}
};

typedef std::vector<GiveAway> GArray;


enum ERROR{	PREVIOUSLY_WON = -10, NOT_ENOUGH_POINTS, UNKNOWN, OK = 1};


/**
 * SteamGifts account abstraction
 * 
 * Currently cannot work with multiple accounts.
 */
class SteamGiftAcc {
private:
	const string SITEURL = "https://www.steamgifts.com";
	const char* USERAGENT = "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/68.0.3440.84 Safari/537.36";
	int points = 0;
	string xsrf_token;
	string phpsessidCookie;

	SteamGiftAcc();
	

	static size_t WriteCallback(char *contents, size_t size, size_t nmemb, void *userp);
	static int parseInt(const string&);

	string get(const string& url, const string& cookie) const;
	string post(const string& url, const string& cookie, const string& postfields, const GiveAway& ga) const;
	void parseGiveaway(const string& url, GArray* array) const;

public:
    SteamGiftAcc(const SteamGiftAcc&) = delete;
    SteamGiftAcc& operator=(const SteamGiftAcc&) = delete;
	static SteamGiftAcc* getInstance();
	/**
	 * Enter the giveaway.
	 * @param giveaway to enter
	 */
	ERROR enterGA(const GiveAway& ga);
	/**
	 * Parse data from the site.
	 * @param number of pages to parse
	 * @param page to start from
	 */
	GArray parseGiveaways(int pageCount = 1, int pageStart = 1) const;
	/**
	 * Login with cookie.
	 * 
	 * If failed returns false, otherwise true.
	 * 
	 * @param cookie string(no junk characters)
	 */
	bool log_in(const string& phpsessid);
	
	/**
	 * Last parsed number of points
	 *  
	 * @return last parsed number of points
	 */
	int getPoints() const;
};



#endif //STEAMGIFTSBOT_SGBOT_H