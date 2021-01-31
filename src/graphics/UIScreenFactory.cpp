#include <graphics/UIScreenFactory.h>

// Simple helper function to load an image into a OpenGL texture with common settings
bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload pixels into texture
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

UIScreenFactory::UIScreenFactory() {
	
}

UIScreenFactory::~UIScreenFactory() {

}

void UIScreenFactory::setUpFrame() {
	io = &ImGui::GetIO();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void UIScreenFactory::loadImages() {
	this->dungeon_waiting_width = 0;
	this->dungeon_waiting_height = 0;
	this->dungeon_waiting_texture = 0;
	bool ret = LoadTextureFromFile("assets/images/DungeonWaitingScreen.png", &dungeon_waiting_texture, &dungeon_waiting_width, &dungeon_waiting_height);
	IM_ASSERT(ret);

	this->kitchen_waiting_width = 0;
	this->kitchen_waiting_height = 0;
	this->kitchen_waiting_texture = 0;
	ret = LoadTextureFromFile("assets/images/KitchenWaitingScreen.png", &kitchen_waiting_texture, &kitchen_waiting_width, &kitchen_waiting_height);
	IM_ASSERT(ret);

	this->lobby_width = 0;
	this->lobby_height = 0;
	this->lobby_texture = 0;
	ret = LoadTextureFromFile("assets/images/LobbyScreen.png", &lobby_texture, &lobby_width, &lobby_height);
	IM_ASSERT(ret);
}

void UIScreenFactory::UIText(std::string str) {
	ImGui::Text(str.c_str());
}

void UIScreenFactory::setWindowSize(ImVec2 size) {
	ImGui::SetNextWindowSize(size);
}