#include "nong.h"

int idSong = 0, idAuto = 0, previousSong = 0;
std::string youtubeLink = "", audioQuality = "128k";
const char *qualityList[] = { "128k\0", "192k\0", "256k\0", "320k\0" };

bool __fastcall PlayLayer::initHook(gd::PlayLayer* self, int edx, gd::GJGameLevel* level) {
    idAuto = self->m_level->songID;
    return init(self, level);
}

DWORD WINAPI mainThread(void* hModule) {
    using namespace MegaHackExt;
    Window* window = Window::Create("qNoNG | 1.3");

    if (MH_Initialize() == MH_OK) {
        MH_CreateHook(
            (PVOID)(gd::base + 0x01FB780),
            PlayLayer::initHook,
            (LPVOID*)&PlayLayer::init
        );

        MH_EnableHook(MH_ALL_HOOKS);
    }

    TextBox* inputIdSong = TextBox::Create("Song ID"); auto idSongstr = std::to_string(idSong); inputIdSong->set(idSongstr.c_str());
    inputIdSong->setCallback([](TextBox* a, const char* Slider) {
        idSong = atoi(Slider);
    });

    CheckBox* boxAutoidSong = CheckBox::Create("Auto ID");
    boxAutoidSong->setCallback([](CheckBox* a, bool status) {
        if (status) {
            previousSong = idSong;
            idSong = idAuto;
        } else {
            idSong = previousSong;
        } 
    });

    TextBox* inputLink = TextBox::Create("YouTube Link"); inputLink->set(youtubeLink.c_str());
    inputLink->setCallback([](TextBox* a, const char* Slider) {
        youtubeLink = Slider;
    });

    ComboBox *qualityCombo = ComboBox::Create("Quality: ", nullptr);
	qualityCombo->setCallback([](ComboBox *obj, int index, const char *str) {
        if (index == 0) audioQuality = "128k";
        else if (index == 1) audioQuality = "192k";
        else if (index == 2) audioQuality = "256k";
        else if (index == 3) audioQuality = "320k";
    });
	
    qualityCombo->setValues(qualityList);

    Button* downloadSong  = Button::Create("Download");
	downloadSong ->setCallback([](Button* a) {
        if (youtubeLink.find("https://www.youtube.com/watch?v=") != std::string::npos)
            youtubeLink = youtubeLink.substr(32, 11);
        else if (youtubeLink.find("https://youtu.be/") != std::string::npos)
            youtubeLink = youtubeLink.substr(17, 11);
        else if (youtubeLink.find("https://www.youtube.com/watch?v=") != std::string::npos)
            youtubeLink = youtubeLink.substr(0, 11);

        std::string outputLine = "start qNoNG\\start.bat " + std::to_string(idSong) + " " + audioQuality + " " + youtubeLink;
        system(outputLine.c_str());
    });

    Button* deleteSong = Button::Create("Delete");
	deleteSong->setCallback([](Button* a) {
        std::string delCommand = "Resources\\" + std::to_string(idSong) + ".mp3";
        int result = std::filesystem::remove(delCommand.c_str());

        if (result == 0) {
            std::string messageText = "Song: \"" + std::to_string(idSong) + "\" failed to delete.";
            MessageBoxA(NULL, messageText.c_str(), "qNoNG 1.3", MB_OK | MB_ICONERROR);
        } else {
            std::string messageText = "Song: \" " + std::to_string(idSong) + " \" deleted!";
            MessageBoxA(NULL, messageText.c_str(), "qNoNG 1.3", MB_OK | MB_ICONINFORMATION);
        }
    });
    
    window->add(HorizontalLayout::Create(downloadSong, deleteSong));
    window->add(qualityCombo); window->add(inputLink);
    window->add(HorizontalLayout::Create(inputIdSong, boxAutoidSong));
    Client::commit(window);

    return 1;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpvReserved ) {
    if (dwReason != DLL_PROCESS_ATTACH) return 0;
    
    auto mThread = CreateThread(0, 0, mainThread, hModule, 0, 0);
    if (mThread) CloseHandle(mThread);
    else return 0;

    return 1;
}
