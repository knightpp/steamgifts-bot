#include "SteamGiftAcc.h"
#include <fstream>

using namespace std;

bool ReadFromFile(const string& path, string* lhs);
bool WriteToFile(const string& path, const string& data);


int main(int argc, char** argv) {
    string phpsessid;
    int pages = 1;
	cout << "[Build date] " << __DATE__ << ", " << __TIME__ <<endl;
    if(ReadFromFile("data.ini", &phpsessid)){
        if(argc >= 2 && isdigit(argv[1][0])){
            pages = stoi(argv[1]);
        }
    }else {
        if(argc >= 2){
            phpsessid = argv[1];
        }else{
            cout << "Enter phpsessid: ";
            cin >> phpsessid;
			WriteToFile("data.ini", phpsessid);
        }
    }

    cout << "Phpsessid: " << phpsessid << endl;
    SteamGiftAcc& b = SteamGiftAcc::getInstance();
    if(b.log_in(phpsessid)){
        auto giveaways = b.parseGiveaways(); //parse first page

    }else{
        std::cout << "Login failed.\n";
    }

	// 	cout << "\tsleeping for 15 mins." << std::endl;
	// 	std::this_thread::sleep_for(std::chrono::minutes(15));
    
    cout << "Exiting..." << endl;
	curl_global_cleanup();
#ifdef WIN
    cout << "Press ENTER...";
    cin.get();
#endif
    return 0;
}

bool WriteToFile(const string& path, const string& data) {
	ofstream fo(path);
	fo << data;
	fo.close();
	return fo.good();
}
bool ReadFromFile(const string& path, string* lhs) {
	ifstream f(path);
	f >> (*lhs);
	f.close();
	return f.good();
}