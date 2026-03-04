#pragma once
#include "imgui.h"
#include <array>
#include <string>
#include <vector>

struct FallingNote {
    int lane = 0;
    float y = 0.0f;
};

class GameScene {
public:
    // Receives song information when game starts.
    void Start(std::string title, int bpm);
    
    // Renders game scene every frame.
    void Update();
    
    // Signals whether game should return to menu.
    bool shouldExit = false;

private:
    std::string songTitle;
    int songBpm = 120;
    float gameTime = 0.0f; // Elapsed game time

    std::vector<FallingNote> notes;
    float spawnTimer = 0.0f;
    float spawnInterval = 0.5f;
    int nextLane = 0;
    float noteSpeed = 280.0f;

    int score = 0;
    int combo = 0;
    int maxCombo = 0;
    int perfectCount = 0;
    int goodCount = 0;
    int missCount = 0;

    std::string lastJudgeText = "";
    float judgeTextTimer = 0.0f;
    std::array<float, 4> laneFlashTimers = { 0.0f, 0.0f, 0.0f, 0.0f };
    std::array<int, 4> laneFlashKinds = { 0, 0, 0, 0 };
};