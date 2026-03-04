#include "MenuScene.h"

void MenuScene::Update() {
    startRequested = false;

    ImGui::Begin("Menu");
    ImGui::Text("Rhythm Game");
    ImGui::Separator();

    if (songs.empty()) {
        ImGui::Text("No songs available.");
        ImGui::End();
        return;
    }

    if (selectedIndex < 0 || selectedIndex >= static_cast<int>(songs.size())) {
        selectedIndex = 0;
    }

    std::vector<const char*> songTitles;
    songTitles.reserve(songs.size());
    for (const auto& song : songs) {
        songTitles.push_back(song.title.c_str());
    }

    ImGui::Text("Select Song");
    ImGui::ListBox("##SongList", &selectedIndex, songTitles.data(), static_cast<int>(songTitles.size()), 5);

    const SongInfo& song = songs[static_cast<size_t>(selectedIndex)];
    ImGui::Text("Selected: %s (%d BPM)", song.title.c_str(), song.bpm);

    if (ImGui::Button("Start")) {
        selectedSong = song;
        startRequested = true;
    }

    ImGui::End();
}
