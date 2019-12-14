#include "SteamGiftAcc.h"
#include <fstream>
#include <cstdio>
#include <exception>
#include "cxxopts/include/cxxopts.hpp"
#include <windows.h>

using namespace std;

bool ReadFromFile(const string& path, string* lhs);
bool WriteToFile(const string& path, const string& data);

#define printFailFunction() printf("-> %s() failed\n", __FUNCTION__);

int main(int argc, char** argv) {
    printf("[Bild date] %s, %s\n", __DATE__, __TIME__);
    cxxopts::Options options("SteamGiftsBot", "Bot for http://steamgifts.com/");
    options.add_options()
        ("h,help", "Print help")
        ("c,cookie", "Auth cookie", cxxopts::value<std::string>())
        ("config", "Path to config", cxxopts::value<std::string>()->default_value("data.ini"));
    string phpsessid;
    string path;
    auto result = options.parse(argc, argv);


    if(result.count("help")){
        printf("%s\n", options.help().c_str());
        return 0;
    }

    if(result.count("config")){
        path = result["config"].as<std::string>();
    }else
    {
        #ifdef SNAP_BUILD
        auto env = std::getenv("SNAP_USER_DATA");
        if(env == nullptr){
            printf("-> SNAP_USER_DATA env variable not found! Run it as snap!\n");
            return 1;
        }
        path = string(env) + string("/data.ini");
        #else
        path = result["config"].as<std::string>();
        #endif
    }

    
    if(result.count("cookie")){
        phpsessid = result["cookie"].as<std::string>();
        WriteToFile(path, phpsessid);
    }else if(!ReadFromFile(path, &phpsessid)){
        //printf("->Can't open file \"%s\"\n", path.c_str());
        return 0;
    }

    SteamGiftAcc* acc = SteamGiftAcc::getInstance();

    bool logged;
    start:
    try{
    while ((logged = acc->log_in(phpsessid)))
    {
        int entered = 0;
        static int totalEntered = 0;
        printf("Parsing data...\n");
        {
            auto giveaways(std::move(acc->parseGiveaways())); //parse first page
            std::sort(giveaways.begin(), giveaways.end(),
                      [](const GiveAway& ga1, const GiveAway& ga2){
                          return ga1.getChancePercent() > ga2.getChancePercent();
                      });

            printf("Parsed giveaways: %2zu\n", giveaways.size());
            printf("Points available: %3d\n", acc->getPoints());
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
                        printf("[%40s] -- ERROR\n", ga.name.c_str());
                        printf("%s\n",static_cast<std::string>(ga).c_str());
                        break;
                    }
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
    }}
    catch (std::runtime_error &err){
        printf("-> Caught error\nWhat(): %s\n", err.what());
        printf("-> Next try in 5 mins.");
        std::this_thread::sleep_for(std::chrono::minutes(5));
        goto start;
    }
    if(!logged)
        printf("-> Login failed.\n");

    printf("Exiting...\n");
    curl_global_cleanup();
#ifdef WIN
    // cout << "Press ENTER...";
    system("pause");
    cin.get();
#endif
    return 0;
}

bool ReadFromFile(const string& path, string* lhs) {
    try{
        // char buf[103 + 1];
        // ifstream f(path);
        
        // if(!f.is_open()){
        //     char str[512];
        //     sprintf(str, "Can't open '%s'", path.c_str());
        //     throw std::runtime_error(str);
        // }
        // f.getline(buf, 103 + 1);
        // f.close();
        // (*lhs) = std::string(buf);
        // if(strlen(buf) != 103)
        //     throw std::runtime_error(R"(Insufficient lenght of contents of the file. 
        //     Assuming the cookie must be exactly 103 char long.)");
        ifstream f(path);
        if(!f.is_open()){
            char str[512];
            sprintf(str, "Can't open '%s'", path.c_str());
            throw std::runtime_error(str);
        f >> (*lhs);
        f.close();
        return true;
    }catch(std::runtime_error& err){
        printf("-> runtime_error. What(): %s\n", err.what());
    }catch(...){
        printf("-> Unknown error.\n");
    }
    printFailFunction();
    return false;
}

bool WriteToFile(const string& path, const string& data){
    try{
        ofstream f(path);
        f << data << std::endl;
        f.close();
        return true;
    }catch(std::ofstream::failure& writeErr){
        printf("-> What(): %s\n", writeErr.what());
    }catch(...){
        printf("-> Unknown error.\n");
    }
    printFailFunction();
    return false;
}
    