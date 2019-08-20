//
// Created by e on 09/08/18.
//

#include "SteamGiftAcc.h"
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
#define D(str) do { } while ( false )
#endif

size_t SteamGiftAcc::WriteCallback(char *contents, size_t size, size_t nmemb, void *userp) {
	((std::string *) userp)->append((char *)contents, size * nmemb);
	return size * nmemb;
}

bool SteamGiftAcc::log_in(const string& phpsessid) {
	const string loggedFlag = "<div class=\"nav__row is-clickable js__logout\"";
	const string xsrfFlag = R"(<input type="hidden" name="xsrf_token" value=")";
	const string fundsFlag = R"(<span class="nav__points">)";

	this->phpsessidCookie = phpsessid;
	string resp(std::move(get(SITEURL, phpsessidCookie)));
	//ofstream fo("html.html");
//	cout << get(SITEURL, phpsessidCookie) << endl;
	logged = resp.find(loggedFlag) != string::npos;
	if (logged) {
		size_t index = resp.find(xsrfFlag);
		xsrf_token = string(resp.begin() + index + xsrfFlag.size(),
			resp.begin() + index + xsrfFlag.size() + 32);
		D("Logged: true\txsrf_token: " << xsrf_token);

		index = resp.find(fundsFlag);
		funds = parseInt(string(resp.begin() + index + fundsFlag.size(), resp.begin() + index + xsrfFlag.size() + 4));
		D("Funds: " << funds);
		return true;
	}
	return false;
}

SteamGiftAcc::SteamGiftAcc() : logged(false), funds(-1), clog(nullptr) {
	clog.rdbuf(cout.rdbuf());
	clog << setfill(' ');
}

int SteamGiftAcc::parseInt(const string& str) const {
	stringstream ss;
	for (char i : str) {
		if (isdigit(i)) 
			ss << i;
	}
	if (ss.tellp() == 0)
		return 0;
	return stoi(ss.str());
}

bool SteamGiftAcc::enterGA(const GiveAway& ga) {
	if ((funds - ga.price >= 0)) {
		
		string url(move(SITEURL + "/ajax.php"));
		string postfields(move("xsrf_token=" + xsrf_token+ "&do=entry_insert&" +"code=" + ga.getCode() ));
		string resp(move(post(url, phpsessidCookie, postfields, ga)));
		bool rez = resp.find("success") != string::npos;
		if (!rez) {
			if (resp.find("Previously Won") == string::npos) {
				static int errors = 0;
				errors++;
				D("Funds: " << funds << "\tPrice: " << ga.price << "\tHref: " << ga.href);
				D("Resp: " << resp);
				D("Errors catched: " << errors);
				if (errors > 3) {
					D("Too many errors.");
					throw runtime_error((__FUNCTION__ + string(" - error response.")).c_str());
				}
			}
		}
		else {
			Log("Log: " << left << setw(40) <<
				ga.name << right << "Price/Funds: " <<
				setw(3) << ga.price  << '/' << left << setw(3) << funds << right
				<< "  Copies/Entries: " << setw(4) << ga.copies << 
				'/' << left << setw(5) << ga.entries << right);
			//funds = funds - ga.price;
			auto index = resp.find("points");
			funds = parseInt(resp.substr(index, resp.size() - index));
			return true;
		}
		 //parseInt(string(resp.begin() + resp.find("points") + 9, resp.end()));
		
	}
	//    else if(funds == 0){
	//        throw runtime_error((__FUNCTION__ + string(" - out of funds.")).c_str());
	//    }
	return false;
}

string SteamGiftAcc::get(const string& url, const string& cookie) const{
	struct curl_slist *list = nullptr;
	string resp;
	CURL *curl;
	CURLcode res;

	list = curl_slist_append(list, "Accept: text/html, application/xhtml+xml, application/xml");
	//list = curl_slist_append(list, "Accept-Encoding: gzip, deflate, br");
	list = curl_slist_append(list, "Accept-Language: en,en-US;q=0.8,uk;q=0.5,ru;q=0.3");
	list = curl_slist_append(list, "Connection: close");
	//list = curl_slist_append(list, "Connection: close");
	list = curl_slist_append(list, ("Cookie: PHPSESSID="+cookie).c_str());
	list = curl_slist_append(list, "DNT: 1");
	list = curl_slist_append(list, "Host: www.steamgifts.com");
	list = curl_slist_append(list, "Referer: www.steamgifts.com");
	list = curl_slist_append(list, "TE: Trailers");
	list = curl_slist_append(list, "Upgrade-Insecure-Requests: 1");
	list = curl_slist_append(list, USERAGENT);




	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if (curl) {

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
	//	curl_easy_setopt(curl, CURLOPT_COOKIE, ("PHPSESSID="+cookie).c_str());
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
		curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
		
		res = curl_easy_perform(curl);

		if (res != CURLE_OK)
			throw runtime_error(string(curl_easy_strerror(res)));

	}
	curl_easy_cleanup(curl);
	//curl_global_cleanup();
	return std::move(resp);
}

string SteamGiftAcc::post(const string& url, const string& cookie, const string& postfields, const GiveAway& ga) const{
	struct curl_slist *list = nullptr;
	string resp;
	CURL *curl;
	CURLcode res;

	//Accept-Encoding: gzip, deflate, br ,......//////////////////////////////////////////////////

//	list = curl_slist_append(list, "Accept: application/json, text/javascript, */*; q=0.01");
	list = curl_slist_append(list, "Host: www.steamgifts.com");
	list = curl_slist_append(list, USERAGENT);
	list = curl_slist_append(list, "Accept: application/json, text/javascript, */*; q=0.01");
	list = curl_slist_append(list, ("Referer: " + SITEURL + ga.href).c_str());
	list = curl_slist_append(list, "Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
	list = curl_slist_append(list, "X-Requested-With: XMLHttpRequest");
	list = curl_slist_append(list, "Content-Length: 70");
	list = curl_slist_append(list, "DNT: 1");
	list = curl_slist_append(list, "Connection: close");

	list = curl_slist_append(list, ("Cookie: PHPSESSID=" + cookie).c_str());
	list = curl_slist_append(list, "TE: Trailers");


	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		//curl_easy_setopt(curl, CURLOPT_COOKIE, cookie.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postfields.c_str());
		curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
			throw runtime_error(string(curl_easy_strerror(res)));
		curl_easy_cleanup(curl);
	}
	//curl_global_cleanup();
	return std::move(resp);
}

void SteamGiftAcc::parseGiveaway(const string& url){
	/// переписать нормально

	using namespace htmlcxx;
	HTML::ParserDom parser;
	string html = move(get(url, phpsessidCookie));
	tree<HTML::Node> dom = parser.parseTree(html);
	tree<HTML::Node>::iterator it = dom.begin();
	tree<HTML::Node>::iterator end = dom.end();


	//string href;
	//string name;
	//int price;
	//int copies;
	//int entries;

	for (; it != end; ++it) {
		if (it->tagName() == "div") {
			it->parseAttributes();
			auto attr = it->attribute("class");
			if (attr.first) {
				if (attr.second.find("is-faded") == string::npos
					&& attr.second.find("giveaway__row-inner-wrap") != string::npos) {
					it += 6;
					it->parseAttributes();

					GiveAway ga; // Should i initialize by constructor?  /// by ????
					ga.href = it->attribute("href").second;
					ga.setName((++it)->text());
					--it;
					it += 3;
					it->parseAttributes();


					if (it->text().find("Copies") != string::npos) {
						ga.copies = parseInt(it->text());

						it += 2;
						ga.price = parseInt(it->text());
					}
					else {
						ga.price = parseInt(it->text());
					}
					while (it != end) {
						string text(it->text());
						if (text.find(" entries") != string::npos) {
							ga.entries = parseInt(it->text());
							break;
						}
						++it;
					}
					giveaways.push_back(move(ga));
				}
			}
		}
	}
	D("Parsed url: " << url);
}

void SteamGiftAcc::parseGiveaways(const int pages) {
	D("Parsing pages: " << pages);

	giveaways.reserve(pages * 60);
	static const string urlPage("https://www.steamgifts.com/giveaways/search?page=");
	for (int i = 1; i <= pages; i++) {
		parseGiveaway(urlPage + to_string(i));
		std::this_thread::sleep_for(std::chrono::milliseconds(2500));
	}
}

void SteamGiftAcc::parseGiveaways(const int pageStart, const int pageCount) {
	D("Parsing pages from: " << pageStart << " to " << pageStart + pageCount - 1);

	giveaways.reserve((pageCount + pageStart) * 50);
	static const string urlPage("https://www.steamgifts.com/giveaways/search?page=");
	for (int i = pageStart; i < pageCount + pageStart; i++) {
		parseGiveaway(urlPage + to_string(i));
		std::this_thread::sleep_for(std::chrono::milliseconds(2500));
	}
}