#pragma once

// General
#define FPS 60

// Window

#define WINDOW_TITLE "Protegon"
#define WINDOW_X SDL_WINDOWPOS_CENTERED + 200
#define WINDOW_Y SDL_WINDOWPOS_CENTERED
#define WINDOW_WIDTH 1152
#define WINDOW_HEIGHT 800
#define WORLD_WIDTH 1300
#define WORLD_HEIGHT 1000
#define WINDOW_FLAGS SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN

// Tile IDs
#define PLAYER_ID 0 
#define UNKNOWN_TILE -1
#define FALLING_TILE_ID 3
#define KILL_TILE_ID 4
#define WIN_TILE_ID 5