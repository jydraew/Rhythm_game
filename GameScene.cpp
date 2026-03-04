#include "GameScene.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <random>
#include <thread>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

void GameScene::Start(std::string title, int bpm) {
    songTitle = title;
    songBpm = (bpm > 0) ? bpm : 120;
    shouldExit = false;
    gameTime = 0.0f;

    notes.clear();
    spawnTimer = 0.0f;
    spawnInterval = 60.0f / static_cast<float>(songBpm);
    nextLane = -1;

    score = 0;
    combo = 0;
    maxCombo = 0;
    perfectCount = 0;
    goodCount = 0;
    missCount = 0;
    lastJudgeText.clear();
    judgeTextTimer = 0.0f;
    laneFlashTimers = { 0.0f, 0.0f, 0.0f, 0.0f };
    laneFlashKinds = { 0, 0, 0, 0 };
}

void GameScene::Update() {
    const float deltaTime = ImGui::GetIO().DeltaTime;
    gameTime += deltaTime;

    spawnTimer += deltaTime;
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<int> laneDist(0, 3);
    while (spawnTimer >= spawnInterval) {
        spawnTimer -= spawnInterval;
        int spawnLane = laneDist(rng);
        if (spawnLane == nextLane) {
            spawnLane = laneDist(rng);
        }
        notes.push_back({ spawnLane, -20.0f });
        nextLane = spawnLane;
    }

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

    // Game play background window
    ImGui::Begin("GamePlay", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    // Top info
    ImGui::Text("Playing: %s (%d BPM)", songTitle.c_str(), songBpm);
    ImGui::SameLine();
    ImGui::Text("| Score: %d  Combo: %d", score, combo);
    if (ImGui::Button("Back to Menu")) {
        shouldExit = true;
    }
    ImGui::Text("P:%d  G:%d  M:%d  Max Combo:%d", perfectCount, goodCount, missCount, maxCombo);
    ImGui::Text("Keys: D F J K");

    ImGui::Separator();

    const ImVec2 playOrigin = ImGui::GetCursorScreenPos();
    const float contentWidth = ImGui::GetContentRegionAvail().x;
    const float playHeight = ImGui::GetContentRegionAvail().y - 20.0f;
    const float laneWidth = 80.0f;
    const float lanesTotalWidth = laneWidth * 4.0f;
    const float startXOffset = std::max(0.0f, (contentWidth - lanesTotalWidth) * 0.5f);
    const float laneTop = playOrigin.y + 8.0f;
    const float laneBottom = laneTop + std::max(120.0f, playHeight - 16.0f);
    const float judgmentY = laneBottom - 18.0f;
    const float perfectWindow = 12.0f;
    const float goodWindow = 28.0f;
    const float missWindow = 45.0f;
    const float noteHeight = 14.0f;
    const float flashDuration = 0.09f;

    ImDrawList* drawList = ImGui::GetWindowDrawList();

    for (float& timer : laneFlashTimers) {
        timer = std::max(0.0f, timer - deltaTime);
    }

    for (size_t lane = 0; lane < laneFlashTimers.size(); ++lane) {
        if (laneFlashTimers[lane] <= 0.0f) {
            laneFlashKinds[lane] = 0;
        }
    }

    auto registerJudgement = [&](const char* text) {
        lastJudgeText = text;
        judgeTextTimer = 0.7f;
    };

    auto triggerLaneFeedback = [&](int lane, int kind) {
        if (lane < 0 || lane >= 4) {
            return;
        }

        laneFlashTimers[static_cast<size_t>(lane)] = flashDuration;
        laneFlashKinds[static_cast<size_t>(lane)] = kind;
    };

    auto playLaneTone = [&](int lane) {
        if (lane < 0 || lane >= 4) {
            return;
        }

        static const DWORD keyTones[4] = { 440, 494, 523, 587 };
        const DWORD frequency = keyTones[static_cast<size_t>(lane)];
        std::thread([frequency]() {
            Beep(frequency, 8);
        }).detach();
    };

    auto processLaneInput = [&](int lane) {
        int bestIndex = -1;
        float bestAbsDistance = 1000000.0f;

        for (int index = 0; index < static_cast<int>(notes.size()); ++index) {
            if (notes[index].lane != lane) {
                continue;
            }

            const float noteCenterY = laneTop + notes[index].y + (noteHeight * 0.5f);
            const float distance = noteCenterY - judgmentY;
            const float absDistance = std::fabs(distance);

            if (absDistance < bestAbsDistance) {
                bestAbsDistance = absDistance;
                bestIndex = index;
            }
        }

        if (bestIndex < 0) {
            return;
        }

        if (bestAbsDistance <= perfectWindow) {
            score += 1000;
            combo += 1;
            maxCombo = std::max(maxCombo, combo);
            perfectCount += 1;
            registerJudgement("Perfect");
            triggerLaneFeedback(lane, 1);
            notes.erase(notes.begin() + bestIndex);
            return;
        }

        if (bestAbsDistance <= goodWindow) {
            score += 500;
            combo += 1;
            maxCombo = std::max(maxCombo, combo);
            goodCount += 1;
            registerJudgement("Good");
            triggerLaneFeedback(lane, 2);
            notes.erase(notes.begin() + bestIndex);
            return;
        }

        if (bestAbsDistance <= missWindow) {
            combo = 0;
            missCount += 1;
            registerJudgement("Miss");
            triggerLaneFeedback(lane, 3);
            notes.erase(notes.begin() + bestIndex);
        }
    };

    if (ImGui::IsKeyPressed(ImGuiKey_D, false)) {
        playLaneTone(0);
        processLaneInput(0);
    }
    if (ImGui::IsKeyPressed(ImGuiKey_F, false)) {
        playLaneTone(1);
        processLaneInput(1);
    }
    if (ImGui::IsKeyPressed(ImGuiKey_J, false)) {
        playLaneTone(2);
        processLaneInput(2);
    }
    if (ImGui::IsKeyPressed(ImGuiKey_K, false)) {
        playLaneTone(3);
        processLaneInput(3);
    }

    for (int lane = 0; lane < 4; ++lane) {
        const float laneX = playOrigin.x + startXOffset + lane * laneWidth;
        const bool isFlashing = laneFlashTimers[static_cast<size_t>(lane)] > 0.0f;
        ImU32 laneColor = IM_COL32(35, 35, 40, 255);
        if (isFlashing) {
            const int flashKind = laneFlashKinds[static_cast<size_t>(lane)];
            if (flashKind == 1) {
                laneColor = IM_COL32(255, 225, 90, 255);
            } else if (flashKind == 2) {
                laneColor = IM_COL32(90, 195, 255, 255);
            } else if (flashKind == 3) {
                laneColor = IM_COL32(255, 95, 95, 255);
            } else {
                laneColor = IM_COL32(70, 95, 130, 255);
            }
        }
        drawList->AddRectFilled(ImVec2(laneX + 2.0f, laneTop), ImVec2(laneX + laneWidth - 2.0f, laneBottom), laneColor);
        drawList->AddRect(ImVec2(laneX + 2.0f, laneTop), ImVec2(laneX + laneWidth - 2.0f, laneBottom), IM_COL32(90, 90, 100, 255));
    }

    for (auto& note : notes) {
        note.y += noteSpeed * deltaTime;

        const float noteX = playOrigin.x + startXOffset + note.lane * laneWidth + 10.0f;
        const float noteTop = laneTop + note.y;
        const float noteBottom = noteTop + noteHeight;

        if (noteBottom >= laneTop && noteTop <= laneBottom) {
            drawList->AddRectFilled(ImVec2(noteX, noteTop), ImVec2(noteX + laneWidth - 20.0f, noteBottom), IM_COL32(120, 210, 255, 255), 4.0f);
        }
    }

    notes.erase(
        std::remove_if(notes.begin(), notes.end(), [&](const FallingNote& note) {
            const float noteCenterY = laneTop + note.y + (noteHeight * 0.5f);
            const bool passedMissLine = noteCenterY > (judgmentY + missWindow);
            if (passedMissLine) {
                combo = 0;
                missCount += 1;
                registerJudgement("Miss");
                triggerLaneFeedback(note.lane, 3);
                return true;
            }
            return false;
        }),
        notes.end()
    );

    drawList->AddRectFilled(
        ImVec2(playOrigin.x + startXOffset + 4.0f, judgmentY),
        ImVec2(playOrigin.x + startXOffset + lanesTotalWidth - 4.0f, judgmentY + 4.0f),
        IM_COL32(255, 255, 255, 220)
    );

    if (judgeTextTimer > 0.0f && !lastJudgeText.empty()) {
        judgeTextTimer -= deltaTime;
        drawList->AddText(
            ImVec2(playOrigin.x + startXOffset + lanesTotalWidth + 20.0f, laneTop + 20.0f),
            IM_COL32(255, 230, 120, 255),
            lastJudgeText.c_str()
        );
    }

    ImGui::Dummy(ImVec2(0.0f, std::max(140.0f, playHeight)));

    ImGui::End();
}