#include "SteamGiftAcc.h"
#include <fstream>
#include <cstdio>

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
    SteamGiftAcc* b = SteamGiftAcc::getInstance();
    if(b->log_in(phpsessid)){
        while (true)
        {
            printf("Parsing data...\n");
            auto giveaways = b->parseGiveaways(); //parse first page
            printf("Data has parsed...\n");
            printf("Entering giveaways...\n");
            for(const auto& ga : giveaways){
                
                if(b->enterGA(ga) == ERROR::FUNDS_RAN_OUT)
                    break;
            
                switch(b->enterGA(ga)){
                    case ERROR::OK:{
                        printf("[%40s]\n", ga.name.c_str());
                        break;
                    }
                    case ERROR::PREVIOUSLY_WON:{
                        printf("[%40s] -- PREVIOUSLY_WON\n", ga.name.c_str());
                        break;
                    }
                    case ERROR::UNKNOWN:{
                        printf("[%40s] -- ERROR\n", ga.name);
                        std::cout << static_cast<std::string>(ga) << "\n\n";
                        break;
                    }
                }
            }
            printf("Entered giveaways.\n");
            std::cout << "Sleeping for 15 mins." << std::endl;
	        std::this_thread::sleep_for(std::chrono::minutes(15));
        }
    }else{
        std::cout << "Login failed.\n";
    }
    
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