#include "window.h"

// Initial mouse positions
float lastX, lastY;

// Function prototypes
void framebuffer_size_callback(GLFWwindow* glfwViewport, int width, int height);
void processInput(GLFWwindow* glfwViewport);

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

Window::Window(int width = Config::getFloat("Window_Width"), int height = Config::getFloat("Window_Height")) : objNum (0) {
	this->isClosed = false;
	this->width = width;
	this->height = height;
	this->setupWindow();
	this->shader = new Shader(Config::get("Vertex_Shader"), Config::get("Fragment_Shader"));
	this->UIshader = new Shader("src//graphics//shaders//ui_vert_shader.glsl", "src//graphics//shaders//ui_frag_shader.glsl"); 
	///TODO: Need to add to Config?
	this->camera = new Camera(Config::getVec3("Camera_Location"));
	this->inventory = NULL;
}

void Window::addObject(int id, GameObject* object) {
	this->objectsToRender.insert({id, object});
	objNum++;
}

void Window::removeObject(int index) {
	this->objectsToRender.erase(index);
	objNum--;
}

void Window::addInventory(std::unordered_map<int, Ingredient*>* inventoryPtr) {
	this->inventory = inventoryPtr;
}

void Window::toggleCursor()
{
	// Disable cursor
	if (glfwGetInputMode(glfwViewport, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
	{
		glfwSetInputMode(glfwViewport, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	// Enable cursor
	else
	{
		glfwSetInputMode(glfwViewport, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void Window::setupWindow() {

	// inital mouse positions
	lastX = Config::getFloat("Window_Width") / 2, lastY = Config::getFloat("Window_Height") / 2;

	// // //
	// Setup

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	GLFWwindow* glfwViewport = glfwCreateWindow((int)Config::getFloat("Window_Width"), (int)Config::getFloat("Window_Height"), Config::get("Window_Title").c_str(), NULL, NULL);
	
	// Mouse centering
	glfwSetCursorPos(glfwViewport, lastX, lastY);

	if (glfwViewport == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}

	// Current context is window
	glfwMakeContextCurrent(glfwViewport);

	// Register callback functions
	glfwSetFramebufferSizeCallback(glfwViewport, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to load GLAD" << std::endl;
	}

	// Enable depth testing
	// This is used to make sure we occlude textures when a
	// surface is obscured by another surface (i.e. in 3D objects).
	glEnable(GL_DEPTH_TEST);

	// Set window var
	this->glfwViewport = glfwViewport;
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfw_InitForOpenGL(glfwViewport, true);

	ui.loadImages();
	//impl = GlfwRenderer(glfwViewport);

	ImGui_ImplOpenGL3_Init("#version 130");
	// ImFont* font1 = io.Fonts->AddFontFromFileTTF("assets/fonts/NikkyouSans-B6aV.ttf", Config::getInt("Font_Size_Pixels"));

	//glfwViewport.refresh_font_texture();
	ImGuiStyle * style = &ImGui::GetStyle();
 
	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;
 
	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.50f, 0.00f, 0.00f, 0.95f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
}

void Window::addCookingEventMessage(std::string msg) {
	this->cookingEventMsg = msg;
}

void Window::removeCookingEventMessage() {
	this->cookingEventMsg = "";
}

void Window::close() {
	
	// Cleanup IMGUI and GLFW resources.
	if (glfwWindowShouldClose(glfwViewport))
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		isClosed = true; 
		glfwTerminate();
	}

	// For some reason, close was called before the window should close.
	else std::cerr << "ERROR: Unable to close window!" << std::endl;
}

void Window::setTimer(int64_t timer) {
	this->timer = timer;
}

Ingredient* Window::getSelectedIngredient() {
	return this->selectedIngredient;
}

void Window::setScore(int score) {
	this->score = score;
}

void Window::render()
{
	if (glfwViewport == NULL) {
		std::cerr << "ERROR: No window!" << std::endl;
	}


	// // //
	// User shader program
	shader->use();

	// // //
	// per-frame time logic
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// Make BG light gray
	glClearColor(0.8f, 0.8f, 0.8f, 0.8f);

	// Clear color + depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Apply lighting options
	shader->setVec3("lightColor", glm::vec3(1.0, 1.0, 1.0f));
	shader->setFloat("ambientStrength", Config::getFloat("ambientStrength"));
	shader->setVec3("lightPos", Config::getVec3("lightPos"));
	shader->setVec3("defaultObjColor", Config::getVec3("defaultObjColor"));
	shader->setFloat("noColorPrecision", Config::getFloat("noColorPrecision"));
	shader->setFloat("specularStrength", Config::getFloat("specularStrength"));
	shader->setVec3("viewPos", this->camera->pos);
	
	// Target following
    if (!camera->freeCam) camera->warpToTarget();

	// // //
	// View + perspective projection matrices

	// camera view transformation (related to xy axis rotations)
	glm::mat4 view = this->camera->getViewMatrix();

	// camera projection transformation
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(this->camera->fov), Config::getFloat("Window_Width") / Config::getFloat("Window_Height"), 0.1f, 100.0f);

	// pass transformation matrices to the shader
	shader->setMat4("projection", projection); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
	shader->setMat4("view", view);

	// Update camera view appropriately
	cameraViewUpdate();

	// // //
	// Render each GameObject

	for (auto it = objectsToRender.begin(); it != objectsToRender.end(); ++it) {
		
		// Get the next object to render.
		GameObject* obj = it->second;

		// If we don't render the object, ignore it.
		if (!obj->getRender()) continue;

		// Get the color scaling
		glm::vec4 colorScale = Config::getVec4("colorScale");

		// Rotate ingredient because why not
		if (obj->getObjectType() == INGREDIENT)
		{
			obj->setRotation(obj->getRotation() + Config::getFloat("Ingredient_Rotation_Speed"));
		}

		// load model if needed
		if (!obj->model) obj->loadModel();

		// Set respective model matrix for each object and send it to the shader.
		glm::mat4 mat = glm::mat4(1.0);
		mat = glm::translate(mat, obj->getPosition());
		mat = glm::scale(mat, obj->getScaleVec());	
		/// TODO: Make sure this works
		mat = glm::rotate(mat, (float)((obj->getRotation() * 3.14)/180.0), glm::vec3(0.0f, 1.0f, 0.0f));
		shader->setMat4("model", mat);

		// Used to convert normal vectors to world space coordinates, without applying translations to them
		shader->setMat4("normalMatrix", glm::transpose(glm::inverse(mat)));

		// Apply color scaling
		shader->setVec4("colorScale", colorScale);

		// Draw the model
		obj->draw(*shader);
	}
	ui.setUpFrame();
	ImGui::SetWindowFontScale(Config::getFloat("Font_Scale"));
	
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	// ImGui::PopFont();

	// // //
	// GLFW stuff
	glfwSwapBuffers(glfwViewport);
	glfwPollEvents();

	// Close the window when appropriate.
	if (glfwWindowShouldClose(glfwViewport)) close();
}

// A callback function to resize the rendering window when the window is resized
void framebuffer_size_callback(GLFWwindow* glfwViewport, int width, int height)
{
	glViewport(0, 0, width, height);
}

void Window::cameraViewUpdate() {
	double xpos, ypos;
	glfwGetCursorPos(glfwViewport, &xpos, &ypos);

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	camera->processMouseMovement(xoffset, yoffset);
}