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

    // Create player (for spawn position)
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
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
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
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
    if (key == GLFW_KEY_4 && action == GLFW_PRESS)
    {
        // Create the pan spawn point and place it at the cursor.
        GameObject* pan = new Cookware(PAN);
        pan->setPosition(cursor->getRoundedPosition());

        // Add it to the window.
        window.addObject(pan->getID(), pan);

        // Add it to the stack of map objects.
        mapObjects.push(pan);
    }

    // Create cutting board
    if (key == GLFW_KEY_5 && action == GLFW_PRESS)
    {
        // Create the cutting board spawn point and place it at the cursor.
        GameObject* cboard = new Cookware(CUTTING_BOARD);
        cboard->setPosition(cursor->getRoundedPosition());

        // Add it to the window.
        window.addObject(cboard->getID(), cboard);

        // Add it to the stack of map objects.
        mapObjects.push(cboard);
    }

    // Create plate
    if (key == GLFW_KEY_6 && action == GLFW_PRESS)
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
    if (key == GLFW_KEY_7 && action == GLFW_PRESS)
    {
        // Create the 3x3 and place it at the cursor.
        GameObject* floorPiece = new GameObject();
        floorPiece->setPosition(cursor->getRoundedPosition() - glm::vec3(0, 0.5, 0));

        // Set as 3x3 model.
        floorPiece->setModel(Config::get("Tile9_Model"));
        floorPiece->setObjectType(FLOOR9);

        // Add it to the window.
        window.addObject(floorPiece->getID(), floorPiece);

        // Add it to the stack of map objects.
        mapObjects.push(floorPiece);
    }

    // Create floor piece (5x5)
    if (key == GLFW_KEY_8 && action == GLFW_PRESS)
    {
        // Create the 5x5 and place it at the cursor.
        GameObject* floorPiece = new GameObject();
        floorPiece->setPosition(cursor->getRoundedPosition() - glm::vec3(0, 0.5, 0));

        // Set as 5x5 model.
        floorPiece->setModel(Config::get("Tile25_Model"));
        floorPiece->setObjectType(FLOOR25);

        // Add it to the window.
        window.addObject(floorPiece->getID(), floorPiece);

        // Add it to the stack of map objects.
        mapObjects.push(floorPiece);
    }

    // Create floor piece (7x7)
    if (key == GLFW_KEY_9 && action == GLFW_PRESS)
    {
        // Create the 7x7 and place it at the cursor.
        GameObject* floorPiece = new GameObject();
        floorPiece->setPosition(cursor->getRoundedPosition() - glm::vec3(0, 0.5, 0));

        // Set as 7x7 model.
        floorPiece->setModel(Config::get("Tile49_Model"));
        floorPiece->setObjectType(FLOOR49);

        // Add it to the window.
        window.addObject(floorPiece->getID(), floorPiece);

        // Add it to the stack of map objects.
        mapObjects.push(floorPiece);
    }

    // Create floor piece (9x9)
    if (key == GLFW_KEY_0 && action == GLFW_PRESS)
    {
        // Create the 9x9 and place it at the cursor.
        GameObject* floorPiece = new GameObject();
        floorPiece->setPosition(cursor->getRoundedPosition() - glm::vec3(0, 0.5, 0));

        // Set as 9x9 model.
        floorPiece->setModel(Config::get("Tile81_Model"));
        floorPiece->setObjectType(FLOOR81);

        // Add it to the window.
        window.addObject(floorPiece->getID(), floorPiece);

        // Add it to the stack of map objects.
        mapObjects.push(floorPiece);
    }

    // Create floor piece (27x27)
    if (key == GLFW_KEY_MINUS && action == GLFW_PRESS)
    {
        // Create the 27x27 and place it at the cursor.
        GameObject* floorPiece = new GameObject();
        floorPiece->setPosition(cursor->getRoundedPosition() - glm::vec3(0, 0.5, 0));

        // Set as 27x27 model.
        floorPiece->setModel(Config::get("Tile729_Model"));
        floorPiece->setObjectType(FLOOR729);

        // Add it to the window.
        window.addObject(floorPiece->getID(), floorPiece);

        // Add it to the stack of map objects.
        mapObjects.push(floorPiece);
    }

    // Create indicator for upper-left ingredient spawning boundary
    if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS)
    {
        // Do not allow more than one. Replace the old one.
        if (this->upperLeft)
        {
            std::cout << "Upper-left marker already exists. Replacing with new one." << std::endl;
            window.removeObject(this->upperLeft->getID());
        }

        // Create the upper left marker at the cursor.
        GameObject* marker = new GameObject();
        marker->setPosition(cursor->getRoundedPosition() - glm::vec3(0, 0.5, 0));

        // Set as upper left model.
        marker->setModel(Config::get("UpperLeft_Marker_Model"));
        this->upperLeft = marker;

        // Add it to the window.
        window.addObject(marker->getID(), marker);
    }

    // Create indicator for lower-right ingredient spawning boundary
    if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS)
    {
        // Do not allow more than one. Replace the old one.
        if (this->lowerRight)
        {
            std::cout << "Lower-right marker already exists. Replacing with new one." << std::endl;
            window.removeObject(this->lowerRight->getID());
        }

        // Create the lower right marker at the cursor.
        GameObject* marker = new GameObject();
        marker->setPosition(cursor->getRoundedPosition() - glm::vec3(0, 0.5, 0));

        // Set as lower right model.
        marker->setModel(Config::get("LowerRight_Marker_Model"));
        this->lowerRight = marker;

        // Add it to the window.
        window.addObject(marker->getID(), marker);
    }

    // Create base for prison cell
    if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS)
    {
        // Do not allow more than one. Replace the old one.
        if (this->cell)
        {
            std::cout << "Prison cell already exists. Replacing with new one." << std::endl;
            window.removeObject(this->cell->getID());
        }

        // Create the prison cell base at the cursor.
        GameObject* cell = new GameObject();
        cell->setPosition(cursor->getRoundedPosition() - glm::vec3(0, 0.5, 0));

        // Set as prison cell base model.
        cell->setModel(Config::get("Cage_Topbot_Model"));
        this->cell = cell;

        // Add it to the window.
        window.addObject(cell->getID(), cell);
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
            if (mods == GLFW_MOD_SHIFT) obj->setPosition( obj->getPosition() + glm::vec3(0, 0, -Config::getFloat("Nudge_Fast_Speed")) );
            else                        obj->setPosition( obj->getPosition() + glm::vec3(0, 0, -Config::getFloat("Nudge_Speed")) );

            // Debug
            glm::vec3 pos = obj->getPosition();
            std::cout << "New position: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
        }
        if (key == GLFW_KEY_H && action == GLFW_PRESS)
        {
            // Nudge down
            if (mods == GLFW_MOD_SHIFT) obj->setPosition( obj->getPosition() + glm::vec3(0, 0, Config::getFloat("Nudge_Fast_Speed")) );
            else                        obj->setPosition( obj->getPosition() + glm::vec3(0, 0, Config::getFloat("Nudge_Speed")) );

            // Debug
            glm::vec3 pos = obj->getPosition();
            std::cout << "New position: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
        }
        if (key == GLFW_KEY_G && action == GLFW_PRESS)
        {
            // Nudge left
            if (mods == GLFW_MOD_SHIFT) obj->setPosition( obj->getPosition() + glm::vec3(-Config::getFloat("Nudge_Fast_Speed"), 0, 0) );
            else                        obj->setPosition( obj->getPosition() + glm::vec3(-Config::getFloat("Nudge_Speed"), 0, 0) );

            // Debug
            glm::vec3 pos = obj->getPosition();
            std::cout << "New position: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
        }
        if (key == GLFW_KEY_J && action == GLFW_PRESS)
        {
            // Nudge right
            if (mods == GLFW_MOD_SHIFT) obj->setPosition( obj->getPosition() + glm::vec3(Config::getFloat("Nudge_Fast_Speed"), 0, 0) );
            else                        obj->setPosition( obj->getPosition() + glm::vec3(Config::getFloat("Nudge_Speed"), 0, 0) );

            // Debug
            glm::vec3 pos = obj->getPosition();
            std::cout << "New position: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
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

    // Default map type
    std::string mapType = "";

    // Map type prompt
    std::cout << "Please type in the map type. Use Dungeon/Kitchen/Lobby. This will create a file of that name (i.e. Dungeon.dm). Rename this file as preferred after it is generated." << std::endl;
    std::cin >> mapType;

    // Determine file extension
    std::string ext = "";
    if (mapType.compare("Dungeon") == 0)        ext = ".dm";
    else if (mapType.compare("Kitchen") == 0)   ext = ".km";
    else if (mapType.compare("Lobby") == 0)     ext = ".lm";

    // Output stream
    std::ofstream ofs;
    ofs.open ("maps/" + mapType + ext);

    if (!ofs.is_open()) 
    {
        std::cerr << "An error occurred when trying to open this file for writing." << std::endl;
        return;
    }

    // Stacks to take in all the game objects
    std::stack<GameObject*> walls;
    std::stack<GameObject*> playerSpawns;
    std::stack<GameObject*> pots;
    std::stack<GameObject*> pans;
    std::stack<GameObject*> cboards;
    std::stack<GameObject*> plates;
    std::stack<GameObject*> floor9Pieces;
    std::stack<GameObject*> floor25Pieces;
    std::stack<GameObject*> floor49Pieces;
    std::stack<GameObject*> floor81Pieces;
    std::stack<GameObject*> floor729Pieces;

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

            case PLAYER:        
                playerSpawns.push(obj);
                break;

            case COOKWARE:
                {
                    std::string cookwareName = ((Cookware*)obj)->getName();
                    std::cerr << "cookware found: " << cookwareName << std::endl;
                    if (cookwareName.compare(POT) == 0)                 pots.push(obj);
                    else if (cookwareName.compare(PAN) == 0)            pans.push(obj);
                    else if (cookwareName.compare(CUTTING_BOARD) == 0)  cboards.push(obj);
                    break;
                }

            case PLATE:         
                plates.push(obj);
                break;

            case FLOOR9:         
                floor9Pieces.push(obj);
                break;

            case FLOOR25:
                floor25Pieces.push(obj);
                break;

            case FLOOR49:
                floor49Pieces.push(obj);
                break;

            case FLOOR81:
                floor81Pieces.push(obj);
                break;

            case FLOOR729:
                floor729Pieces.push(obj);
                break;

            default:
                std::cerr << "Undefined object appeared in map." << std::endl;
                break;
        }
    }

    std::cout << "Processing walls." << std::endl;

    // Comments
    ofs << "// Wall data" << std::endl;
    ofs.flush();

    // Add wall count.
    ofs << mapType + "_Wall_Count=" << walls.size() << std::endl;
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
        ofs << mapType + "_Wall_Pos_" << i << "=";
        ofs << pos.x << "," << pos.y << "," << pos.z << std::endl;
        ofs.flush();
        ofs << mapType + "_Wall_Rot_" << i << "=";
        ofs << rot << std::endl;
        ofs.flush();
        ofs << mapType + "_Wall_Scale_" << i << "=";
        ofs << scale.x << "," << scale.y << "," << scale.z << std::endl;
        ofs.flush();

        // NEXT
        i++;
    }
    // Reset
    i = 0;

    std::cout << "Processing player spawn points." << std::endl;

    // Comments
    ofs << std::endl;
    ofs << "// Player spawning data" << std::endl;
    ofs.flush();

    // Add player spawn count.
    ofs << mapType + "_Spawn_Count=" << playerSpawns.size() << std::endl;
    ofs.flush();

    // Iterate over player locations and add to file
    while (!playerSpawns.empty())
    {
        // Remove from stack
        GameObject* obj = playerSpawns.top();
        glm::vec3 pos = obj->getPosition();
        playerSpawns.pop();

        // Add next wall strings
        ofs << mapType + "_Spawn_" << i << "=";
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
    ofs << "// Pot spawning data" << std::endl;
    ofs.flush();

    // Add pot spawn count. 
    ofs << "Pot_Count=" << pots.size() << std::endl;
    ofs.flush();

    // Iterate over pot locations and add to file
    while (!pots.empty())
    {
        // Remove from stack
        GameObject* obj = pots.top();
        glm::vec3 pos = obj->getPosition();
        pots.pop();

        // Add next wall strings
        ofs << "Pot_" << i << "=";
        ofs << pos.x << "," << pos.y << "," << pos.z << std::endl;
        ofs.flush();

        // NEXT
        i++;
    }
    // Reset
    i = 0;

    // Comments
    ofs << std::endl;
    ofs << "// Pan spawning data" << std::endl;
    ofs.flush();

    // Add pan spawn count.
    ofs << "Pan_Count=" << pans.size() << std::endl;
    ofs.flush();

    // Iterate over pan locations and add to file
    while (!pans.empty())
    {
        // Remove from stack
        GameObject* obj = pans.top();
        glm::vec3 pos = obj->getPosition();
        pans.pop();

        // Add next wall strings
        ofs << "Pan_" << i << "=";
        ofs << pos.x << "," << pos.y << "," << pos.z << std::endl;
        ofs.flush();

        // NEXT
        i++;
    }
    // Reset
    i = 0;

    // Comments
    ofs << std::endl;
    ofs << "// Cutting board spawning data" << std::endl;
    ofs.flush();

    // Add cutting board spawn count.
    ofs << "Cutting_Board_Count=" << cboards.size() << std::endl;
    ofs.flush();

    // Iterate over cutting board locations and add to file
    while (!cboards.empty())
    {
        // Remove from stack
        GameObject* obj = cboards.top();
        glm::vec3 pos = obj->getPosition();
        cboards.pop();

        // Add next wall strings
        ofs << "Cutting_Board_" << i << "=";
        ofs << pos.x << "," << pos.y << "," << pos.z << std::endl;
        ofs.flush();

        // NEXT
        i++;
    }
    // Reset
    i = 0;

    // Comments
    ofs << std::endl;
    ofs << "// Plate spawning data" << std::endl;
    ofs.flush();

    // Add plate spawn count. 
    ofs << "Plate_Count=" << plates.size() << std::endl;
    ofs.flush();

    // Iterate over plate locations and add to file
    while (!plates.empty())
    {
        // Remove from stack
        GameObject* obj = plates.top();
        glm::vec3 pos = obj->getPosition();
        plates.pop();

        // Add plate strings
        ofs << "Plate_" << i << "=";
        ofs << pos.x << "," << pos.y << "," << pos.z << std::endl;
        ofs.flush();

        // NEXT
        i++;
    }
    // Reset
    i = 0;

    std::cout << "Processing floor pieces." << std::endl;

    // Comments
    ofs << std::endl;
    ofs << "// Floor spawning data" << std::endl;
    ofs.flush();

    // Add floor9 count
    ofs << mapType + "_Floor9_Piece_Count=" << floor9Pieces.size() << std::endl;
    ofs.flush();

    // Iterate over floor piece (3x3) locations and add to file
    while (!floor9Pieces.empty())
    {
        // Remove from stack
        GameObject* obj = floor9Pieces.top();
        glm::vec3 pos = obj->getPosition();
        floor9Pieces.pop();

        // Add next wall strings
        ofs << mapType + "_Floor9_Piece_" << i << "=";
        ofs << pos.x << "," << pos.y << "," << pos.z << std::endl;
        ofs.flush();

        // NEXT
        i++;
    }
    // Reset
    i = 0;

    // Comments
    ofs << std::endl;
    ofs.flush();

    // Add floor25 count
    ofs << mapType + "_Floor25_Piece_Count=" << floor25Pieces.size() << std::endl;
    ofs.flush();

    // Iterate over floor piece (5x5) locations and add to file
    while (!floor25Pieces.empty())
    {
        // Remove from stack
        GameObject* obj = floor25Pieces.top();
        glm::vec3 pos = obj->getPosition();
        floor25Pieces.pop();

        // Add next wall strings
        ofs << mapType + "_Floor25_Piece_" << i << "=";
        ofs << pos.x << "," << pos.y << "," << pos.z << std::endl;
        ofs.flush();

        // NEXT
        i++;
    }
    // Reset
    i = 0;

    // Comments
    ofs << std::endl;
    ofs.flush();

    // Add floor49 count
    ofs << mapType + "_Floor49_Piece_Count=" << floor49Pieces.size() << std::endl;
    ofs.flush();

    // Iterate over floor piece (7x7) locations and add to file
    while (!floor49Pieces.empty())
    {
        // Remove from stack
        GameObject* obj = floor49Pieces.top();
        glm::vec3 pos = obj->getPosition();
        floor49Pieces.pop();

        // Add next wall strings
        ofs << mapType + "_Floor9_Piece_" << i << "=";
        ofs << pos.x << "," << pos.y << "," << pos.z << std::endl;
        ofs.flush();

        // NEXT
        i++;
    }
    // Reset
    i = 0;

    // Comments
    ofs << std::endl;
    ofs.flush();

    // Add floor count
    ofs << mapType + "_Floor81_Piece_Count=" << floor81Pieces.size() << std::endl;
    ofs.flush();

    // Iterate over floor piece (9x9) locations and add to file
    while (!floor81Pieces.empty())
    {
        // Remove from stack
        GameObject* obj = floor81Pieces.top();
        glm::vec3 pos = obj->getPosition();
        floor81Pieces.pop();

        // Add next wall strings
        ofs << mapType + "_Floor81_Piece_" << i << "=";
        ofs << pos.x << "," << pos.y << "," << pos.z << std::endl;
        ofs.flush();

        // NEXT
        i++;
    }
    // Reset
    i = 0;

    // Comments
    ofs << std::endl;
    ofs.flush();

    // Add floor count
    ofs << mapType + "_Floor729_Piece_Count=" << floor729Pieces.size() << std::endl;
    ofs.flush();

    // Iterate over floor piece (27x27) locations and add to file
    while (!floor729Pieces.empty())
    {
        // Remove from stack
        GameObject* obj = floor729Pieces.top();
        glm::vec3 pos = obj->getPosition();
        floor729Pieces.pop();

        // Add next wall strings
        ofs << mapType + "_Floor729_Piece_" << i << "=";
        ofs << pos.x << "," << pos.y << "," << pos.z << std::endl;
        ofs.flush();

        // NEXT
        i++;
    }

    // Info
    std::cout << "Processing gameplay aspects." << std::endl;

    // Handling dungeon unique settings
    if (mapType.compare("Dungeon") == 0)
    {
        // Comments
        ofs << std::endl;
        ofs << "// Ingredient spawning properties" << std::endl;
        ofs.flush();

        if (this->upperLeft && this->lowerRight)
        {
            // Ingredient spawning boundaries
            ofs << mapType + "_Upper_X=" << std::to_string((int)this->upperLeft->getPosition().x) << std::endl;
            ofs << mapType + "_Upper_Z=" << std::to_string((int)this->upperLeft->getPosition().z) << std::endl;
            ofs << mapType + "_Lower_X=" << std::to_string((int)this->lowerRight->getPosition().x) << std::endl;
            ofs << mapType + "_Lower_Z=" << std::to_string((int)this->lowerRight->getPosition().z) << std::endl;
        }

            // Response string
            std::string response = "";

            // Ingredient spawn rates
            std::cout << "Please enter the ingredient spawn rate (as a float). That is, indicate how many seconds of delay there will be between each ingredient spawn." << std::endl;
            std::cin >> response;
            ofs << mapType + "_Ingredient_Spawn_Rate=" << response << std::endl;

            // Vodka spawn chance
            std::cout << "Please enter the chance of a vodka spawning (as an int). It should value from 0-100." << std::endl;
            std::cin >> response;
            ofs << "Vodka_Chance=" << response << std::endl;
        
    }

    // Handling kitchen unique settings
    else if (mapType.compare("Kitchen") == 0)
    {
        if (this->cell)
        {
            // Comments
            ofs << std::endl;
            ofs << "// Prison cell properties" << std::endl;
            ofs.flush();

            // Cell properties
            glm::vec3 pos = cell->getPosition();
            ofs << "Cell_Wall_Count=4" << std::endl;
            ofs << "Cell_Scale=3" << std::endl;
            ofs << "Cell_Wall_0=" << (pos.x - 6) << "," << (pos.y + 0.5) << "," << pos.z << std::endl;
            ofs << "Cell_Wall_1=" << (pos.x + 6) << "," << (pos.y + 0.5) << "," << pos.z << std::endl;
            ofs << "Cell_Wall_2=" << pos.x << "," << (pos.y + 0.5) << "," << (pos.z - 6) << std::endl;
            ofs << "Cell_Wall_3=" << pos.x << "," << (pos.y + 0.5) << "," << (pos.z + 6) << std::endl;
            ofs << "Cell_Base=" << pos.x << "," << pos.y << "," << pos.z << std::endl;

            // Comments
            ofs << std::endl;
            ofs << "// Prison cell spawns" << std::endl;
            ofs.flush();

            // Cell spawning properties
            ofs << "Cell_Spawn_Count=3" << std::endl;
            ofs << "Cell_Wall_Spawn_0=" << (pos.x - 1) << "," << pos.y << "," << (pos.z - 1) << std::endl;
            ofs << "Cell_Wall_Spawn_1=" << (pos.x - 1) << "," << pos.y << "," << (pos.z + 1) << std::endl;
            ofs << "Cell_Wall_Spawn_0=" << (pos.x + 1) << "," << pos.y << "," << (pos.z + 1) << std::endl;
        }

        // Comments
        ofs << std::endl;
        ofs << "// Plate scoring" << std::endl;
        ofs.flush();

        // Plate scoring
        ofs << "Plate_Score=5" << std::endl;
    }

    // Handling lobby unique settings
    else if (mapType.compare("Lobby") == 0)
    {
        // Do nothing for now.
        // This can be used for something useful in the future - maybe some settings for lobby-specific activities
        // Maybe something to pass the time?
    }

    // Finished
    std::cout << "Finished." << std::endl;

    // Cleanup of object trackers
    lastDeleted = NULL;
    if (this->upperLeft) 
    {
        window.removeObject(this->upperLeft->getID());
        this->upperLeft = NULL;
    }
    if (this->lowerRight)
    {
        window.removeObject(this->lowerRight->getID());
        this->lowerRight = NULL;
    }
    if (this->cell)
    {
        window.removeObject(this->cell->getID());
        this->cell = NULL;
    }

    // Close output stream
    ofs.close();
}