/*******************************************************************************************
*
*   raylib - classic game: space invaders
*
*   Sample game developed by Ian Eito, Albert Martos and Ramon Santamaria
*
*   This game has been created using raylib v1.3 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2015 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

typedef enum {
	LIGHT,
	DARK
} FontTheme;

//------------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------------
// the universe is actually 50x30 but there is some extra space outside the viewport
#define extra_space 10
#define universe_width 50 + extra_space * 2
#define universe_height 30 + extra_space * 2
const int screen_width = 1000;
const int screen_height = 600;
const int cell_size = 20;
bool universe[universe_height][universe_width]; // a canvas containing our cells
bool is_simulating = false; // if the simulation of the game of life is running or not
float universe_tempo = 0.2f; // the update time of the universe
float universe_timer = 0.0f;
float key_press_timer = 0.0f;
float key_press_cooldown = 0.12f;
Color inactive_cell_color = { 255, 255, 255, 255 };
Color active_cell_color = { 0, 0, 0, 255 };
Color strong_text_color = { 0, 0, 0, 50 };
Color weak_text_color = { 0, 0, 0, 32 };
Vector2 click_position = { -1.0f, -1.0f };


//------------------------------------------------------------------------------------
// Non-raylib functions
//------------------------------------------------------------------------------------
void init_universe(); // initializes the universe to an empty grid
void update_cell(int x, int y); // switches a cell
void update_universe(); // simulates 1 step in the game of life
void render_universe(); // renders the cells
void process_keypress(int key_pressed);
Vector2 get_click();
int get_neighbors(bool universe[universe_height][universe_width], int x, int y);
void set_font_theme(FontTheme theme);


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
    InitWindow(screen_width, screen_height, "Jonyski's Game of Life");
    init_universe();
    SetTargetFPS(60);
    ClearBackground(inactive_cell_color);

    while (!WindowShouldClose()) {
    	BeginDrawing();
    	ClearBackground(inactive_cell_color);
    	DrawText("press SPACEBAR to run the simulation", 250, 288, 24, strong_text_color);
    	DrawText("press 0 - 9 to change the color scheme", 262, 322, 22, weak_text_color);
    	DrawText(TextFormat("universe updating every: %02.02fs", universe_tempo), 324, 10, 20, strong_text_color);

    	key_press_timer += GetFrameTime();
    	int key_pressed = GetKeyPressed();
    	process_keypress(key_pressed);

    	if(is_simulating) {
    		universe_timer += GetFrameTime();
    		if(universe_timer >= universe_tempo) {
    			update_universe();
	    		universe_timer = 0.0f;
    		}
    	}

        click_position = get_click(); // checking for mouse clicks
        if(click_position.x >= 0) {
        	// finding the indexes of the cell to switch
        	int cell_x = (int) floor(click_position.x / (screen_width / (universe_width - 2 * extra_space))) + extra_space;
        	int cell_y = (int) floor(click_position.y / (screen_height / (universe_height - 2 * extra_space))) + extra_space;
        	update_cell(cell_x, cell_y);
        }
        render_universe();

    	EndDrawing();
    }
    CloseWindow();

    return 0;
}


void init_universe() {
	for(int i = 0; i < universe_height; i++) {
		for(int j = 0; j < universe_width; j++) {
			universe[i][j] = false;
		}
	}
}

void update_cell(int x, int y) {
	universe[y][x] = universe[y][x] ? false : true;
}

void update_universe() {
	// creating a copy of the universe
	bool paralel_universe[universe_height][universe_width];
	for(int i = 0; i < universe_height; i++) {
		for(int j = 0; j < universe_width; j++) {
			paralel_universe[i][j] = universe[i][j];
		}
	}
	// updating the cells of the original universe
	int neighbors = 0;
	for(int i = 0; i < universe_height; i++) {
		for(int j = 0; j < universe_width; j++) {
			// getting the numbers of neighbors of a cell
			neighbors = get_neighbors(paralel_universe, j, i);
			// applying the Conway's game of life rules to the cell
			if(universe[i][j]) {
				if(neighbors < 2 || neighbors > 3) universe[i][j] = false;
			} else {
				if(neighbors == 3) universe[i][j] = true;
			}
		}
	}
}
void render_universe() {
	for(int i = extra_space; i < universe_height - extra_space; i++) {
		for(int j = extra_space; j < universe_width - extra_space; j++) {
			if(universe[i][j]) {
				int x = j - extra_space;
				int y = i - extra_space;
				DrawRectangle(x*cell_size, y*cell_size, cell_size, cell_size, active_cell_color);
			}
		}
	}
}

Vector2 get_click() {
	if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		click_position = GetMousePosition();
	else
		click_position = (Vector2) { -1.0f, -1.0f };

	return click_position;
}

void process_keypress(int key_pressed) {
	switch(key_pressed){
		// key that runs/pauses the simulation
		case KEY_SPACE:
			is_simulating = is_simulating ? false : true;
			break;
		// keys that change the simulation speed
		case KEY_MINUS:
			universe_tempo += 0.02;
			break;
		case KEY_EQUAL:
			if(universe_tempo - 0.02 >= 0.02) universe_tempo -= 0.02;
			break;
		// keys that change the color scheme
		case KEY_ZERO:
			inactive_cell_color = (Color) { 255, 255, 255, 255 };
			active_cell_color = (Color) { 0, 0, 0, 255 };
			set_font_theme(DARK);
			break;
		case KEY_ONE:
			inactive_cell_color = (Color) { 42, 47, 51, 255 };
			active_cell_color = (Color) { 61, 137, 192, 255 };
			set_font_theme(LIGHT);
			break;
		case KEY_TWO:
			inactive_cell_color = (Color) { 197, 92, 76, 255 };
			active_cell_color = (Color) { 51, 40, 38, 255 };
			set_font_theme(DARK);
			break;
		case KEY_THREE:
			inactive_cell_color = (Color) { 103, 21, 27, 255 };
			active_cell_color = (Color) { 22, 26, 30, 255 };
			set_font_theme(LIGHT);
			break;
		case KEY_FOUR:
			inactive_cell_color = (Color) { 232, 170, 155, 255 };
			active_cell_color = (Color) { 227, 211, 196, 255 };
			set_font_theme(DARK);
			break;
		case KEY_FIVE:
			inactive_cell_color = (Color) { 1, 46, 64, 255 };
			active_cell_color = (Color) { 242, 227, 213, 255 };
			set_font_theme(LIGHT);
			break;
		case KEY_SIX:
			inactive_cell_color = (Color) { 217, 17, 71, 255 };
			active_cell_color = (Color) { 242, 230, 56, 255 };
			set_font_theme(DARK);
			break;
		case KEY_SEVEN:
			inactive_cell_color = (Color) { 34, 35, 38, 255 };
			active_cell_color = (Color) { 137, 217, 126, 255 };
			set_font_theme(LIGHT);
			break;
		case KEY_EIGHT:
			inactive_cell_color = (Color) { 89, 52, 59, 255 };
			active_cell_color = (Color) { 74, 103, 140, 255 };
			set_font_theme(DARK);
			break;
		case KEY_NINE:
			inactive_cell_color = (Color) { 60, 61, 89, 255 };
			active_cell_color = (Color) { 242, 120, 75, 255 };
			set_font_theme(DARK);
			break;
	}

	if(key_press_timer >= key_press_cooldown) {
		if(IsKeyDown(KEY_MINUS)) universe_tempo += 0.02;
		if(IsKeyDown(KEY_EQUAL)) if(universe_tempo - 0.02 >= 0.02) universe_tempo -= 0.02;
		key_press_timer = 0.0f;
	}
}

int get_neighbors(bool universe[universe_height][universe_width], int x, int y) {
	int neighbors = 0;
	if(y > 0) {
		// top left
		if(x > 0) neighbors += (int) universe[y - 1][x - 1];
		// top middle
		neighbors += (int) universe[y - 1][x];
		// top right
		if(x + 1 < universe_width) neighbors += (int) universe[y - 1][x + 1];
	}
	if(x < universe_width - 1) {
		// middle right
		neighbors += (int) universe[y][x + 1];
		// bottom right
		if(y + 1 < universe_height) neighbors += (int) universe[y + 1][x + 1];
	}
	if(y < universe_height - 1) {
		// bottom middle
		neighbors += (int) universe[y + 1][x];
		// bottom left
		if(x > 0) neighbors += (int) universe[y + 1][x - 1];
	}
	// middle left
	if(x > 0) neighbors += (int) universe[y][x - 1];
	return neighbors;
}

void set_font_theme(FontTheme theme) {
	switch(theme) {
		case DARK:
			strong_text_color = (Color) { 0, 0, 0, 50 };
			weak_text_color = (Color) { 0, 0, 0, 32 };
			break;
		case LIGHT:
			strong_text_color = (Color) { 255, 255, 255, 50 };
			weak_text_color = (Color) { 255, 255, 255, 32 };
	}
}