#include "SteamGiftAcc.h"
#include <fstream>


#define NK_IMPLEMENTATION
#include "dependencies/nuklear/nuklear.h"


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

    cout << "Phpsessid given: " << phpsessid << endl;
	while (true) {
		try {
			SteamGiftAcc b;
			if (!b.log_in(phpsessid)) {
				throw runtime_error("Error. Not logged in.");
			}
			b.parseGiveaways(pages);
			b.enterAllGA();
		}
		catch (exception& ex) {
			cout << ex.what() << endl;
			break;
		}
		cout << "\tsleeping for 15 mins." << std::endl;
		std::this_thread::sleep_for(std::chrono::minutes(15));
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

void GuiMain(){

    struct nk_context ctx;
    nk_init_fixed(&ctx, calloc(1, MAX_MEMORY), MAX_MEMORY, &font);

    enum {EASY, HARD};
    static int op = EASY;
    static float value = 0.6f;
    static int i =  20;

    if (nk_begin(&ctx, "Show", nk_rect(50, 50, 220, 220),
                 NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_CLOSABLE)) {
        /* fixed widget pixel width */
        nk_layout_row_static(&ctx, 30, 80, 1);
        if (nk_button_label(&ctx, "button")) {
            /* event handling */
        }

        /* fixed widget window ratio width */
        nk_layout_row_dynamic(&ctx, 30, 2);
        if (nk_option_label(&ctx, "easy", op == EASY)) op = EASY;
        if (nk_option_label(&ctx, "hard", op == HARD)) op = HARD;

        /* custom widget pixel width */
        nk_layout_row_begin(&ctx, NK_STATIC, 30, 2);
        {
            nk_layout_row_push(&ctx, 50);
            nk_label(&ctx, "Volume:", NK_TEXT_LEFT);
            nk_layout_row_push(&ctx, 110);
            nk_slider_float(&ctx, 0, &value, 1.0f, 0.1f);
        }
        nk_layout_row_end(&ctx);
    }
    nk_end(&ctx);

}
