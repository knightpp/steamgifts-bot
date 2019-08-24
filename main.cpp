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
    if(!ReadFromFile("data.ini", &phpsessid)){
        if(argc >= 2){
            phpsessid = argv[1];
        }else{
            cout << "Enter phpsessid: ";
            cin >> phpsessid;
			WriteToFile("data.ini", phpsessid);
        }
    }

    //cout << "Phpsessid: " << phpsessid << endl;
    SteamGiftAcc* acc = SteamGiftAcc::getInstance();
    if(acc->log_in(phpsessid)){
        while (true)
        {
            int entered = 0;
            static int totalEntered = 0;
            printf("Parsing data...\n");
            auto giveaways(std::move(acc->parseGiveaways())); //parse first page
            std::sort(giveaways.begin(), giveaways.end(), 
            [](const GiveAway& ga1, const GiveAway& ga2){
                return ga1.getChancePercent() > ga2.getChancePercent();
            });

            printf("Parsed: %2d\n", giveaways.size());
            printf("Entering giveaways...\n");
            for(const auto& ga : giveaways){
                ERROR err = acc->enterGA(ga);
                if(err == ERROR::NOT_ENOUGH_POINTS)
                    break;
                switch(err){
                    case ERROR::OK:{
                        ++entered;
                        printf("[%40s] -- Price: %2d, Chance: %f%%\n", ga.name.c_str(), ga.price, ga.getChancePercent());
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
            printf("Entered: %3d\n", entered);
            printf("Total:   %3d\n", totalEntered += entered);
            time_t now = time(0);
            tm* ltm = localtime(&now);
            printf("%02d:%02d:%02d - sleeping for 15 mins.\n\n", 
               ltm->tm_hour, ltm->tm_min, ltm->tm_sec
            );
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
    char buf[103 + 1];
	ifstream f(path);
    if(!f.is_open())
        return false;
    f.getline(buf, 103 + 1);
	//f >> (*lhs);
	f.close();
    (*lhs) = std::string(buf);
	return strlen(buf) == 103;
}