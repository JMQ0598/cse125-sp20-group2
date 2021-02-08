#pragma once

// Standard library
#include <string>
#include <unordered_map>

// ImGui
#include <gui/imgui_impl_glfw.h>
#include <gui/imgui_impl_opengl3.h>

#include <objects/Ingredient.h>
#include <constants/gameplay_vars.h>
#include <graphics/stb_image.h>

// OpenGL function pointers
#include <glad/glad.h>		// Include this...
#include <GLFW/glfw3.h>		// ...before this.

class Window;

class UIScreenFactory {
public:
	ImGuiIO* io = NULL;
	ImVec2 window_pos;
	ImVec2 window_pos_pivot;

	int dungeon_waiting_width;
	int dungeon_waiting_height;
	GLuint dungeon_waiting_texture;

	int kitchen_waiting_width;
	int kitchen_waiting_height;
	GLuint kitchen_waiting_texture;

	int lobby_width;
	int lobby_height;
	GLuint lobby_texture;
	bool lobbyFirst = true;
	bool kitchenFirst = true;
	bool dungeonFirst = true;


	Ingredient* highlighted = NULL;

	UIScreenFactory();
	~UIScreenFactory();

	void setUpFrame();

	void loadImages();

	void UIDungeonInstructions();
	void UIKitchenInstructions();
	void UILobbyScreen();
	void UIGameInfo(int round, int32_t minutes, int32_t seconds);
	void UIScore(int score);
	void UIInventory(std::unordered_map<int, Ingredient*>* map);
	Ingredient* UIButtonInventory(std::unordered_map<int, Ingredient*>* map);
	void UICookingEvent(std::string);
	void UIInstructionSet(std::vector<std::string>, std::string recipeName);
	void UIControls(Window* window);

	void UIGameOver(bool gameWin);

	void UIText(std::string str);
	void setWindowSize(ImVec2 size);

private:

	int corner = 1;
	const float DISTANCE = 10.0f;
};