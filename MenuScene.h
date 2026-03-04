#pragma once

#include "imgui.h"
#include <string>
#include <vector>

struct SongInfo {
    std::string title;
    int bpm;
};

class MenuScene {
public:
    void Update();

    bool startRequested = false;
    SongInfo selectedSong;

private:
    std::vector<SongInfo> songs = {
        {"Tutorial Beat", 120},
        {"Night Drive", 140},
        {"Skyline", 160}
    };
    int selectedIndex = 0;
};
