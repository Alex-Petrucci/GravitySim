#pragma once

constexpr const char* TITLE     = "Gravity Sim";
constexpr int INITIAL_WIDTH     = 1280;
constexpr int INITIAL_HEIGHT    = 720;
constexpr int FPS_CAP           = 144;
constexpr float G               = 4000.0f; // this is just the initial value and can be changed in the sim
constexpr float C               = 300.0f;
constexpr float SOFTENING       = 0.1f;
constexpr float MERGE_TOLERANCE = 0.1f;