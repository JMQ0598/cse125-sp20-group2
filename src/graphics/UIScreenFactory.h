#pragma once

#include <gui/imgui_impl_glfw.h>
#include <gui/imgui_impl_opengl3.h>
#include <string>
#include <unordered_map>
#include <objects/Ingredient.h>
#include <constants/gameplay_vars.h>
#include <graphics/stb_image.h>

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


	Ingredient* highlighted = NULL;

	UIScreenFactory();
	~UIScreenFactory();

	void setUpFrame();

	void loadImages();

	void UIText(std::string str);
	void setWindowSize(ImVec2 size);

private:

	int corner = 1;
	const float DISTANCE = 10.0f;
};