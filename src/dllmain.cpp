
#include "nong.h"

int songId = 0, autoId = 0;
std::string youtubeLink = "";
std::string audioQuality = "128k";
const char *qualityList[] = { "128k\0", "192k\0", "256k\0", "320k\0", nullptr };

bool __fastcall PlayLayer::initHook(gd::PlayLayer* self, int edx, gd::GJGameLevel* level) {
	bool out = PlayLayer::init(self, level); if (!out) { return out; }

    autoId = gd::GameManager::sharedState()->getPlayLayer()->m_level->songID;

    return out;
}

DWORD WINAPI mainThread(void* hModule) {
    using namespace MegaHackExt;

    Window* window = Window::Create("qNoNG | 1.2");

    TextBox* inputSongId = TextBox::Create("Song ID");
    inputSongId->setCallback([](TextBox* a, const char* Slider) {
        songId = atoi(Slider);
    });

    CheckBox* boxAutoSongID = CheckBox::Create("Auto ID");
    boxAutoSongID->setCallback([](CheckBox* a, bool status) {
        if (status)
            songId = autoId;
    });

    TextBox* inputLink = TextBox::Create("YouTube Link");
    inputLink->setCallback([](TextBox* a, const char* Slider) {
        youtubeLink = Slider;
    });

    ComboBox *qualityCombo = ComboBox::Create("Quality: ", nullptr);
	qualityCombo->setCallback([](ComboBox *obj, int index, const char *str) {
        if (index == 0)
            audioQuality = "128k";
        else if (index == 1)
            audioQuality = "192k";
        else if (index == 2)
            audioQuality = "256k";
        else if (index == 3)
            audioQuality = "320k";
    });
	qualityCombo->setValues(qualityList);

    Button* downloadSong  = Button::Create("Download");
	downloadSong ->setCallback([](Button* a) {
        std::string outputLine = "start qNoNG\\start.bat " + std::to_string(songId) + " " + audioQuality + " " + youtubeLink;
        system(outputLine.c_str());
    });

    Button* deleteSong = Button::Create("Delete");
	deleteSong->setCallback([](Button* a) {
        std::string delCommand = "Resources\\" + std::to_string(songId) + ".mp3";
        int result = std::filesystem::remove(delCommand.c_str());

        if (result == 0) {
            std::string messageText = "Song: \"" + std::to_string(songId) + "\" not found.";
            MessageBoxA(NULL, messageText.c_str(), "qNoNG 1.2", MB_OK | MB_ICONERROR);
        } else {
            std::filesystem::remove(delCommand.c_str());
            std::string messageText = "Song: \" " + std::to_string(songId) + " \" deleted!";
            MessageBoxA(NULL, messageText.c_str(), "qNoNG 1.2", MB_OK | MB_ICONINFORMATION);
        }
    });
    
	window->add(HorizontalLayout::Create(downloadSong, deleteSong));
    window->add(qualityCombo);
    window->add(inputLink);
    window->add(HorizontalLayout::Create(inputSongId, boxAutoSongID));

    Client::commit(window);

    return TRUE;
}

void mh_init() {
    MH_CreateHook(
        (PVOID)(gd::base + 0x01FB780),
        PlayLayer::initHook,
        (LPVOID*)&PlayLayer::init
    );
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if (MH_Initialize() == MH_OK) {
            mh_init();
            MH_EnableHook(MH_ALL_HOOKS);
        }

        CreateThread(0, 0x1000, mainThread, hModule, 0, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
