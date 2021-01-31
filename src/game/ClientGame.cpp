#include <game/ClientGame.h>
#include <game/KeyResolver.h>
#include <objects/Player.h>
#include <objects/Plate.h>
#include <ostream>

#define CLIENT_DELAY 1000

ClientGame::ClientGame(int i) : window(Config::getFloat("Window_Width"), Config::getFloat("Window_Height"))
{
    // Configure keybinds
    glfwSetWindowUserPointer(this->window.glfwViewport, reinterpret_cast<void*> (this));
    glfwSetKeyCallback(this->window.glfwViewport, key_callback_wrapper);

    // Create cursor
    GameObject* cursor = new GameObject(cursorId);
    cursor->setModel(Config::get("Cursor_Model"));
    cursor->setPosition(glm::vec3(0,0,0));
    cursor->applyScale(glm::vec3(1));
    window.addObject(cursorId, cursor);
    this->cursor = cursor;
}

ClientGame::~ClientGame()
{

}

void ClientGame::keyBindsHandler(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods)
{
    // Handles toggling free cam
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        std::cout << "locking / unlocking the camera" << std::endl;
        this->window.camera->toggleFreeCam();
        this->window.toggleCursor();
    }

    // Reading input specifically related to mapbuilding.
    mapbuildingInput(glfwWindow, key, scancode, action, mods);
}

void ClientGame::runGame() 
{
    while(!window.isClosed) 
    {
        // Take local input
        // Send to the server
        processInput();

        // Render world
        window.render();
    }
}

/**
 * Take in input from the viewport window.
 * */
void ClientGame::processInput()
{
	// Exit application.
	if (glfwGetKey(window.glfwViewport, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window.glfwViewport, true);
	}

    glm::vec3 cursorPos = cursor->getPosition();

    // Get key inputs and set direction of message
    if (glfwGetKey(window.glfwViewport, GLFW_KEY_W) == GLFW_PRESS &&
    glfwGetKey(window.glfwViewport, GLFW_KEY_S) == GLFW_PRESS) {
        // Do nothing: The movements should cancel each other out.
    }
    else if (glfwGetKey(window.glfwViewport, GLFW_KEY_W) == GLFW_PRESS)
    {
        cursorPos.z -= Config::getFloat("Cursor_Speed");
    }
	else if (glfwGetKey(window.glfwViewport, GLFW_KEY_S) == GLFW_PRESS) 
    {
        cursorPos.z += Config::getFloat("Cursor_Speed");
    }
    
    if (glfwGetKey(window.glfwViewport, GLFW_KEY_A) == GLFW_PRESS &&
    glfwGetKey(window.glfwViewport, GLFW_KEY_D) == GLFW_PRESS) {
        // Do nothing: The movements should cancel each other out.
    }
	else if (glfwGetKey(window.glfwViewport, GLFW_KEY_A) == GLFW_PRESS)
    {
        cursorPos.x -= Config::getFloat("Cursor_Speed");
    }
	else if (glfwGetKey(window.glfwViewport, GLFW_KEY_D) == GLFW_PRESS)
    {
        cursorPos.x += Config::getFloat("Cursor_Speed");
    }

    // Update cursor position
    cursor->setPosition(cursorPos);

    // Camera movement options (client-side only)
    if (glfwGetKey(window.glfwViewport, GLFW_KEY_UP) == GLFW_PRESS)
		window.camera->processKeyMovement(FORWARD);
    if (glfwGetKey(window.glfwViewport, GLFW_KEY_DOWN) == GLFW_PRESS)
		window.camera->processKeyMovement(BACKWARD);
    if (glfwGetKey(window.glfwViewport, GLFW_KEY_LEFT) == GLFW_PRESS)
		window.camera->processKeyMovement(LEFT);
    if (glfwGetKey(window.glfwViewport, GLFW_KEY_RIGHT) == GLFW_PRESS)
		window.camera->processKeyMovement(RIGHT);
}

void ClientGame::mapbuildingInput(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods)
{
    // Create wall
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        // Create the wall and place it at the cursor.
        GameObject* wall = new Wall();
        wall->setPosition(cursor->getRoundedPosition());

        // Add it to the window.
        window.addObject(wall->getID(), wall);

        // Add it to the stack of map objects.
        mapObjects.push(wall);
    }

    // Create tomato (for ingredient spawn location)
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
        // Create the tomato and place it at the cursor.
        GameObject* tomato = IngredientFactory::createIngredient(TOMATO);
        tomato->setPosition(cursor->getRoundedPosition());

        // Add it to the window.
        window.addObject(tomato->getID(), tomato);

        // Add it to the stack of map objects.
        mapObjects.push(tomato);
    }

    // Create player (for spawn position)
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
    {
        // Create the player spawn point and place it at the cursor.
        GameObject* newPlayer = new Player(playerIndex--);
        newPlayer->setPosition(cursor->getRoundedPosition());

        // Add it to the window.
        window.addObject(newPlayer->getID(), newPlayer);

        // Add it to the stack of map objects.
        mapObjects.push(newPlayer);
    }

    // Create pot
    if (key == GLFW_KEY_4 && action == GLFW_PRESS)
    {
        // Create the pot spawn point and place it at the cursor.
        GameObject* pot = new Cookware(POT);
        pot->setPosition(cursor->getRoundedPosition());

        // Add it to the window.
        window.addObject(pot->getID(), pot);

        // Add it to the stack of map objects.
        mapObjects.push(pot);
    }

    // Create pan
    if (key == GLFW_KEY_5 && action == GLFW_PRESS)
    {
        // Create the pan spawn point and place it at the cursor.
        GameObject* pan = new Cookware(PAN);
        pan->setPosition(cursor->getRoundedPosition());

        // Add it to the window.
        window.addObject(pan->getID(), pan);

        // Add it to the stack of map objects.
        mapObjects.push(pan);
    }

    // Create knife
    if (key == GLFW_KEY_6 && action == GLFW_PRESS)
    {
        // Create the knife spawn point and place it at the cursor.
        GameObject* knife = new Cookware(CUTTING_BOARD);
        knife->setPosition(cursor->getRoundedPosition());

        // Add it to the window.
        window.addObject(knife->getID(), knife);

        // Add it to the stack of map objects.
        mapObjects.push(knife);
    }

    // Create plate
    if (key == GLFW_KEY_7 && action == GLFW_PRESS)
    {
        // Create the plate spawn point and place it at the cursor.
        GameObject* plate = new Plate();
        plate->setPosition(cursor->getRoundedPosition());

        // Add it to the window.
        window.addObject(plate->getID(), plate);

        // Add it to the stack of map objects.
        mapObjects.push(plate);
    }

    // Create floor piece (3x3)
    if (key == GLFW_KEY_8 && action == GLFW_PRESS)
    {
        // Create the 3x3 and place it at the cursor.
        GameObject* floorPiece = new GameObject();
        floorPiece->setPosition(cursor->getRoundedPosition() - glm::vec3(0, 0.5, 0));

        // Set as 3x3 model.
        floorPiece->setModel(Config::get("Tile9_Model"));
        floorPiece->setObjectType(FLOOR9);

        // Add it to the window.
        window.addObject(floorPiece->getID(), floorPiece);

        // Add it to the stack of floor pieces.
        mapObjects.push(floorPiece);
    }

    // Create floor piece (5x5)
    if (key == GLFW_KEY_9 && action == GLFW_PRESS)
    {
        // Create the 5x5 and place it at the cursor.
        GameObject* floorPiece = new GameObject();
        floorPiece->setPosition(cursor->getRoundedPosition() - glm::vec3(0, 0.5, 0));

        // Set as 5x5 model.
        floorPiece->setModel(Config::get("Tile25_Model"));
        floorPiece->setObjectType(FLOOR25);

        // Add it to the window.
        window.addObject(floorPiece->getID(), floorPiece);

        // Add it to the stack of floor pieces.
        mapObjects.push(floorPiece);
    }

    // Create floor piece (7x7)
    if (key == GLFW_KEY_0 && action == GLFW_PRESS)
    {
        // Create the 7x7 and place it at the cursor.
        GameObject* floorPiece = new GameObject();
        floorPiece->setPosition(cursor->getRoundedPosition() - glm::vec3(0, 0.5, 0));

        // Set as 7x7 model.
        floorPiece->setModel(Config::get("Tile49_Model"));
        floorPiece->setObjectType(FLOOR49);

        // Add it to the window.
        window.addObject(floorPiece->getID(), floorPiece);

        // Add it to the stack of floor pieces.
        mapObjects.push(floorPiece);
    }

    // Create floor piece (9x9)
    if (key == GLFW_KEY_MINUS && action == GLFW_PRESS)
    {
        // Create the 9x9 and place it at the cursor.
        GameObject* floorPiece = new GameObject();
        floorPiece->setPosition(cursor->getRoundedPosition() - glm::vec3(0, 0.5, 0));

        // Set as 9x9 model.
        floorPiece->setModel(Config::get("Tile81_Model"));
        floorPiece->setObjectType(FLOOR81);

        // Add it to the window.
        window.addObject(floorPiece->getID(), floorPiece);

        // Add it to the stack of floor pieces.
        mapObjects.push(floorPiece);
    }

    // Restore last removed object
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        if (lastDeleted)
        {
            mapObjects.push(lastDeleted);
            window.addObject(lastDeleted->getID(), lastDeleted);
            lastDeleted = NULL;
        }
    }

    // Operations that require a non-empty stack
    if (!mapObjects.empty())
    {
        GameObject* obj = mapObjects.top();

        // Remove object at top of stack
        if (key == GLFW_KEY_E && action == GLFW_PRESS)
        {
            if (!mapObjects.empty())
            {
                lastDeleted = obj;
                window.removeObject(lastDeleted->getID());
                mapObjects.pop();
            }
        }

        // Nudge object at top of stack
        if (key == GLFW_KEY_Y && action == GLFW_PRESS)
        {
            // Nudge up
            obj->setPosition( obj->getPosition() + glm::vec3(0, 0, -0.5) );
        }
        if (key == GLFW_KEY_H && action == GLFW_PRESS)
        {
            // Nudge down
            obj->setPosition( obj->getPosition() + glm::vec3(0, 0, 0.5) );
        }
        if (key == GLFW_KEY_G && action == GLFW_PRESS)
        {
            // Nudge left
            obj->setPosition( obj->getPosition() + glm::vec3(-0.5, 0, 0) );
        }
        if (key == GLFW_KEY_J && action == GLFW_PRESS)
        {
            // Nudge right
            obj->setPosition( obj->getPosition() + glm::vec3(0.5, 0, 0) );
        }

        // Rotate object at top of stack 90 degrees
        if (key == GLFW_KEY_T && action == GLFW_PRESS)
        {
            // Ignore player spawn points
            if (obj->getObjectType() != PLAYER) obj->setRotation( obj->getRotation() + 90 );
        }

        // Scale up object at top of stack (x)
        if (key == GLFW_KEY_Z && action == GLFW_PRESS)
        {
            // Ignore player spawn points
            if (obj->getObjectType() != PLAYER) obj->applyScale( obj->getScaleVec() + glm::vec3(0.5, 0, 0) );
        }

        // Scale up object at top of stack (y)
        if (key == GLFW_KEY_X && action == GLFW_PRESS)
        {
            // Ignore player spawn points
            if (obj->getObjectType() != PLAYER) obj->applyScale( obj->getScaleVec() + glm::vec3(0, 0.5, 0) );
        }

        // Scale up object at top of stack (z)
        if (key == GLFW_KEY_C && action == GLFW_PRESS)
        {
            // Ignore player spawn points
            if (obj->getObjectType() != PLAYER) obj->applyScale( obj->getScaleVec() + glm::vec3(0, 0, 0.5) );
        }

        // Scale down object at top of stack (x)
        if (key == GLFW_KEY_V && action == GLFW_PRESS)
        {
            // Ignore player spawn points
            if (obj->getObjectType() != PLAYER) obj->applyScale( obj->getScaleVec() + glm::vec3(-0.5, 0, 0) );
        }

        // Scale down object at top of stack (y)
        if (key == GLFW_KEY_B && action == GLFW_PRESS)
        {
            // Ignore player spawn points
            if (obj->getObjectType() != PLAYER) obj->applyScale( obj->getScaleVec() + glm::vec3(0, -0.5, 0) );
        }

        // Scale down object at top of stack (z)
        if (key == GLFW_KEY_N && action == GLFW_PRESS)
        {
            // Ignore player spawn points
            if (obj->getObjectType() != PLAYER) obj->applyScale( obj->getScaleVec() + glm::vec3(0, 0, -0.5) );
        }

        // Export map objects to file
        if (key == GLFW_KEY_M && action == GLFW_PRESS)
        {
            exportMapTxt();
        }
    }
}

void ClientGame::exportMapTxt()
{
    // Index that we will reuse often.
    int i = 1;

    // Default map name
    std::string mapName = "Dungeon";

    // Map name prompt
    std::cout << "Please type in the map type. Use Dungeon/Kitchen/Lobby. Rename this file as preferred after it is generated." << std::endl;
    std::cin >> mapName;

    // Output stream
    std::ofstream ofs;
    ofs.open ("maps/" + mapName + ".txt");

    if (!ofs.is_open()) 
    {
        std::cerr << "An error occurred when trying to open this file for writing." << std::endl;
        return;
    }

    // Stacks to take in all the game objects
    std::stack<GameObject*> walls;
    std::stack<GameObject*> ingredientLocations;
    std::stack<GameObject*> playerSpawns;
    std::stack<GameObject*> pots;
    std::stack<GameObject*> pans;
    std::stack<GameObject*> knives;
    std::stack<GameObject*> plates;
    std::stack<GameObject*> floorPieces9;
    std::stack<GameObject*> floorPieces25;
    std::stack<GameObject*> floorPieces49;
    std::stack<GameObject*> floorPieces81;

    // Iterate over map objects to distribute
    while(!mapObjects.empty())
    {
        // Remove object from window and stack
        GameObject* obj = mapObjects.top();
        mapObjects.pop();
        window.removeObject(obj->getID());

        // Place in respective stack
        switch (obj->getObjectType())
        {
            case WALL:
                walls.push(obj); 
                break;

            case INGREDIENT:    
                ingredientLocations.push(obj);
                break;

            case PLAYER:        
                playerSpawns.push(obj);
                break;

            case COOKWARE:
                {
                    std::string cookwareName = ((Cookware*)obj)->getName();
                    if (cookwareName.compare(POT) == 0)                 pots.push(obj);
                    else if (cookwareName.compare(PAN) == 0)            pans.push(obj);
                    else if (cookwareName.compare(CUTTING_BOARD) == 0)  knives.push(obj);
                }

            case PLATE:         
                plates.push(obj);
                break;

            case FLOOR9:         
                floorPieces9.push(obj);
                break;

            case FLOOR25:
                floorPieces25.push(obj);
                break;

            case FLOOR49:
                floorPieces49.push(obj);
                break;

            case FLOOR81:
                floorPieces81.push(obj);
                break;

            default:
                std::cerr << "Undefined object appeared in map." << std::endl;
                break;
        }
    }

    std::cout << "Processing walls." << std::endl;

    // Comments
    ofs << "// Wall data" << std::endl << std::endl;
    ofs.flush();

    // Add wall count.
    ofs << mapName + "_Wall_Count=" << walls.size() << std::endl;
    ofs.flush();

    // Iterate over walls and add to file
    while (!walls.empty())
    {
        // Remove from stack
        GameObject* wall = walls.top();
        glm::vec3 pos = wall->getPosition();
        float rot = wall->getRotation();
        glm::vec3 scale = wall->getScaleVec();
        walls.pop();

        // Add next wall strings
        ofs << mapName + "_Wall_Pos_" << i << "=";
        ofs << pos.x << "," << pos.y << "," << pos.z << std::endl;
        ofs.flush();
        ofs << mapName + "_Wall_Rot_" << i << "=";
        ofs << rot << std::endl;
        ofs.flush();
        ofs << mapName + "_Wall_Scale_" << i << "=";
        ofs << scale.x << "," << scale.y << "," << scale.z << std::endl;
        ofs.flush();

        // NEXT
        i++;
    }
    // Reset
    i = 0;

    std::cout << "Processing ingredient spawn points." << std::endl;

    // Comments
    ofs << std::endl;
    ofs << "// Ingredient spawning data" << std::endl << std::endl;
    ofs.flush();


    // Add ingredient spawn count.
    ofs << mapName + "_Ingredient_Count=" << ingredientLocations.size() << std::endl;
    ofs.flush();

    // Iterate over ingredient locations and add to file
    while (!ingredientLocations.empty())
    {
        // Remove from stack
        GameObject* loc = ingredientLocations.top();
        glm::vec3 pos = loc->getPosition();
        ingredientLocations.pop();

        // Add next wall strings
        ofs << mapName + "_Ingredient_Pos_" << i << "=";
        ofs << pos.x << "," << pos.y << "," << pos.z << std::endl;
        ofs.flush();

        // NEXT
        i++;
    }
    // Reset
    i = 0;

    std::cout << "Processing player spawn points." << std::endl;

    // Comments
    ofs << std::endl;
    ofs << "// Player spawning data" << std::endl << std::endl;
    ofs.flush();

    // Add ingredient spawn count.
    ofs << mapName + "_Spawn_Count=" << playerSpawns.size() << std::endl;
    ofs.flush();

    // Iterate over ingredient locations and add to file
    while (!playerSpawns.empty())
    {
        // Remove from stack
        GameObject* loc = playerSpawns.top();
        glm::vec3 pos = loc->getPosition();
        playerSpawns.pop();

        // Add next wall strings
        ofs << mapName + "_Spawn_" << i << "=";
        ofs << pos.x << "," << pos.y << "," << pos.z << std::endl;
        ofs.flush();

        // NEXT
        i++;
    }
    // Reset
    i = 0;

    std::cout << "Processing cookware locations." << std::endl;

    // Comments
    ofs << std::endl;
    ofs << "// Pot spawning data" << std::endl << std::endl;
    ofs.flush();

    // Add pot spawn count. 
    ofs << mapName + "_Pot_Count=" << pots.size() << std::endl;
    ofs.flush();

    // Iterate over ingredient locations and add to file
    while (!pots.empty())
    {
        // Remove from stack
        GameObject* loc = pots.top();
        glm::vec3 pos = loc->getPosition();
        pots.pop();

        // Add next wall strings
        ofs << mapName + "_Pot_" << i << "=";
        ofs << pos.x << "," << pos.y << "," << pos.z << std::endl;
        ofs.flush();

        // NEXT
        i++;
    }
    // Reset
    i = 0;

    // Comments
    ofs << std::endl;
    ofs << "// Pan spawning data" << std::endl << std::endl;
    ofs.flush();

    // Add pan spawn count.
    ofs << mapName + "_Pan_Count=" << pans.size() << std::endl;
    ofs.flush();

    // Iterate over ingredient locations and add to file
    while (!pans.empty())
    {
        // Remove from stack
        GameObject* loc = pans.top();
        glm::vec3 pos = loc->getPosition();
        pans.pop();

        // Add next wall strings
        ofs << mapName + "_Pan_" << i << "=";
        ofs << pos.x << "," << pos.y << "," << pos.z << std::endl;
        ofs.flush();

        // NEXT
        i++;
    }
    // Reset
    i = 0;

    // Comments
    ofs << std::endl;
    ofs << "// Knives spawning data" << std::endl << std::endl;
    ofs.flush();

    // Add knives spawn count.
    ofs << mapName + "_Knife_Count=" << knives.size() << std::endl;
    ofs.flush();

    // Iterate over ingredient locations and add to file
    while (!knives.empty())
    {
        // Remove from stack
        GameObject* loc = knives.top();
        glm::vec3 pos = loc->getPosition();
        knives.pop();

        // Add next wall strings
        ofs << mapName + "_Knife_" << i << "=";
        ofs << pos.x << "," << pos.y << "," << pos.z << std::endl;
        ofs.flush();

        // NEXT
        i++;
    }
    // Reset
    i = 0;

    // Comments
    ofs << std::endl;
    ofs << "// Plate spawning data" << std::endl << std::endl;
    ofs.flush();

    // Add plate spawn count. 
    ofs << mapName + "_Plate_Count=" << plates.size() << std::endl;
    ofs.flush();

    // Iterate over ingredient locations and add to file
    while (!plates.empty())
    {
        // Remove from stack
        GameObject* loc = plates.top();
        glm::vec3 pos = loc->getPosition();
        plates.pop();

        // Add next wall strings
        ofs << mapName + "_Plate_" << i << "=";
        ofs << pos.x << "," << pos.y << "," << pos.z << std::endl;
        ofs.flush();

        // NEXT
        i++;
    }

    std::cout << "Processing floor pieces." << std::endl;

    // Comments
    ofs << std::endl;
    ofs << "// Floor spawning data" << std::endl << std::endl;
    ofs.flush();

    // Add floor9 count
    ofs << mapName + "_Floor_Piece9_Count=" << floorPieces9.size() << std::endl;
    ofs.flush();

    // Iterate over ingredient locations and add to file
    while (!floorPieces9.empty())
    {
        // Remove from stack
        GameObject* loc = floorPieces9.top();
        glm::vec3 pos = loc->getPosition();
        floorPieces9.pop();

        // Add next wall strings
        ofs << mapName + "_Floor9_Piece_" << i << "=";
        ofs << pos.x << "," << pos.y << "," << pos.z << std::endl;
        ofs.flush();

        // NEXT
        i++;
    }

    // Comments
    ofs << std::endl;
    ofs.flush();

    // Add floor25 count
    ofs << mapName + "_Floor_Piece25_Count=" << floorPieces25.size() << std::endl;
    ofs.flush();

    // Iterate over ingredient locations and add to file
    while (!floorPieces25.empty())
    {
        // Remove from stack
        GameObject* loc = floorPieces25.top();
        glm::vec3 pos = loc->getPosition();
        floorPieces25.pop();

        // Add next wall strings
        ofs << mapName + "_Floor25_Piece_" << i << "=";
        ofs << pos.x << "," << pos.y << "," << pos.z << std::endl;
        ofs.flush();

        // NEXT
        i++;
    }

    // Comments
    ofs << std::endl;
    ofs.flush();

    // Add floor49 count
    ofs << mapName + "_Floor_Piece49_Count=" << floorPieces49.size() << std::endl;
    ofs.flush();

    // Iterate over ingredient locations and add to file
    while (!floorPieces49.empty())
    {
        // Remove from stack
        GameObject* loc = floorPieces49.top();
        glm::vec3 pos = loc->getPosition();
        floorPieces49.pop();

        // Add next wall strings
        ofs << mapName + "_Floor9_Piece_" << i << "=";
        ofs << pos.x << "," << pos.y << "," << pos.z << std::endl;
        ofs.flush();

        // NEXT
        i++;
    }

    // Comments
    ofs << std::endl;
    ofs.flush();

    // Add floor count
    ofs << mapName + "_Floor_Piece81_Count=" << floorPieces81.size() << std::endl;
    ofs.flush();

    // Iterate over ingredient locations and add to file
    while (!floorPieces81.empty())
    {
        // Remove from stack
        GameObject* loc = floorPieces81.top();
        glm::vec3 pos = loc->getPosition();
        floorPieces81.pop();

        // Add next wall strings
        ofs << mapName + "_Floor81_Piece_" << i << "=";
        ofs << pos.x << "," << pos.y << "," << pos.z << std::endl;
        ofs.flush();

        // NEXT
        i++;
    }

    std::cout << "Finished." << std::endl;

    // Close output stream
    ofs.close();
}