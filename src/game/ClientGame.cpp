#include <game/ClientGame.h>
#include <game/KeyResolver.h>
#include <objects/Player.h>
#include <objects/Plate.h>

#define CLIENT_DELAY 1000

ClientGame::ClientGame(std::string IP, int port) : client(IP, port), window(Config::getFloat("Window_Width"), Config::getFloat("Window_Height"))
{
    // Configure keybinds
    glfwSetWindowUserPointer(this->window.glfwViewport, reinterpret_cast<void*> (this));
    glfwSetKeyCallback(this->window.glfwViewport, key_callback_wrapper);

    // Create floor lol
    GameObject* floor = new GameObject(-1);
    floor->setModel(Config::get("Floor_Model"));
    floor->setPosition(glm::vec3(0,-0.5,0));
    floor->applyScale(glm::vec3(1));
    window.addObject(-1, floor);

    runGame();
}

ClientGame::~ClientGame()
{

}

void ClientGame::keyBindsHandler(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods)
{
    /*// Handles ready up
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        std::cout << "sending ready up message" << std::endl;
        Game::ClientMessage* readyMsg = MessageBuilder::toReadyMessage(true);
        this->client.send(*readyMsg);
        delete readyMsg;
    }*/

    // Handles toggling free cam
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        std::cout << "locking / unlocking the camera" << std::endl;
        this->window.camera->toggleFreeCam();
        this->window.toggleCursor();
    }

    /*// Handles interact event
    if (key == GLFW_KEY_E && action == GLFW_PRESS && this->window.getSelectedIngredient() != NULL && this->window.getRound() == KITCHEN_NUM )
    {
        std::cout << "pressed interact key" << std::endl;
        Game::ClientMessage* cookMsg = MessageBuilder::toCookMessage(this->window.getSelectedIngredient());
        this->client.send(*cookMsg);
        delete cookMsg;
    }*/

    ///TODO: DEV TOOL- SHOULD NOT BE IN THE FINAL RELEASE
    mapbuildingInput(glfwWindow, key, scancode, action, mods);
}

void ClientGame::runGame() 
{
    while(!window.isClosed) 
    {
        // Take local input
        // Send to the server
        processInput();

        // Receive updated state from server
        receiveUpdates();

        // Update local game state
        updateGameState();

        // Render world
        window.render();
    }
}

void ClientGame::receiveUpdates()
{
    client.read();
}

void ClientGame::updateGameState()
{
    // Process messages to update client game state
    for (Game::ServerMessage currMessage : client.messages)
    {
        // Process different types of messages
        switch(currMessage.event_case()) 
        {
            // Object-related messages
            case Game::ServerMessage::EventCase::kObject:
            {
                Game::Vector3 location = currMessage.object().worldposition();
                float rotation = currMessage.object().rotation();
                uint32_t id = currMessage.object().id();
                bool render = currMessage.object().render();
                Game::Vector3 scale = currMessage.object().scale();

                GameObject* obj = NULL;

                // Update existing object state
                if (window.objectsToRender.count(id) > 0) 
                {
                    obj = window.objectsToRender[id];
                }

                // Instantiate new object 
                else 
                {
                    // Different object types
                    switch(currMessage.object().type())
                    {
                        // Player object.
                        case Game::PLAYER: obj = new Player(id); break;

                        // Ingredient object.
                        case Game::INGREDIENT: obj = new Ingredient(id); 
                        ((Ingredient*)obj)->setQualityIndex(currMessage.object().quality());
                        break;

                        // Cookware object.
                        case Game::COOKWARE: obj = new Cookware(id); break;
                        
                        // Wall object.
                        case Game::WALL: obj = new Wall(id); break;

                        // Table object..
                        case Game::TABLE: obj = new Table(id); break;

                        // Plate object.
                        case Game::PLATE: obj = new Plate(id); break;

                        // Basic gameobject.
                        default: obj = new GameObject(id); break;
                    }

                    /// Set model based on the model path provided by the server TODO: Might not be necessary???
                    obj->setModel(currMessage.object().modelpath());

                    // Add object to window
                    window.addObject(id, obj);
                }

                // Set object parameters
                obj->setRotation(rotation);
                if (currMessage.object().has_scale()) obj->applyScale(glm::vec3(scale.x(), scale.y(), scale.z()));
                obj->setRender(render);
                obj->setPosition(glm::vec3(location.x(), location.y(), location.z()));
                window.removeCookingEventMessage();
                break;
            }

            case Game::ServerMessage::EventCase::kScore:
            {
                window.setScore(currMessage.score().currscore());
                break;
            }

            // Inventory update messages
            case Game::ServerMessage::EventCase::kInventory: 
            {
                // Get player associated with this client
                Player* player = (Player*)window.objectsToRender[objectId];
                auto currInventory = player->getInventory();

                // Get id of the object to be picked up, set the conditions
                Ingredient* pickup = (Ingredient*)window.objectsToRender[currMessage.inventory().id()];
                pickup->setName(currMessage.inventory().name());
                pickup->setQualityIndex(currMessage.inventory().qualityindex());
                pickup->setStatus(Ingredient::stringToIngredientStatus[currMessage.inventory().ingredientstatus()]);
                window.removeCookingEventMessage();

                // Already exists
                if (currInventory->count(currMessage.inventory().id()) > 0)
                {
                    if (!currMessage.inventory().add()) 
                        player->removeFromInventory(pickup);
                }
                // Doesn't exist in inventory
                else
                {
                    player->addToInventory(pickup);
                }
                
                std::cout << "Picked up: " << pickup->getName() << std::endl;
                break;
            }

            case Game::ServerMessage::EventCase::kValidCook:
            {
                std::cout << "got valid event from server" << std::endl;
                window.addCookingEventMessage(currMessage.validcook().message());
                break;   
            }

            // Client info messages
            case Game::ServerMessage::EventCase::kClientInfo:
            {
                // Set client id
                this->clientId = currMessage.clientinfo().clientid();
                std::cout << "Connected. Client ID: " << clientId << std::endl;

                // Set object id
                this->objectId = currMessage.clientinfo().objectid();

                // Set camera target
                window.camera->setTarget(window.objectsToRender[this->objectId]);
                Player* p = (Player*) window.objectsToRender[this->objectId];
                window.addInventory(p->getInventory());
                break;
            }

            case Game::ServerMessage::EventCase::kInstruction:
            {
                int index = currMessage.instruction().index();
                std::string msg = currMessage.instruction().instructionmsg();
                window.instructionStrings.insert(window.instructionStrings.begin()+index, msg);
                break;
            }

            case Game::ServerMessage::EventCase::kTime:
            {
                uint32_t seconds = currMessage.time().seconds();
                window.setTimer(seconds);
                break;
            }

            case Game::ServerMessage::EventCase::kRound:
            {
                window.updateRound(currMessage.round().type());         
                std::cout << " received round update message " << currMessage.DebugString() << std::endl;
                break;
            }

            /// TODO: NEED TO CHANGE LATER
            case Game::ServerMessage::EventCase::kWin:
            {
                std::cout << " received win event " << std::endl;
                window.gameOver = true;

                // Win or lose
                if (currMessage.win().clientid() == clientId)
                    window.gameWin = true;
                else
                    window.gameWin = false;

                break;
            }

            // All other messages
            default:
            {
                std::cerr << "Unidentified message type." << std::endl; 
            }
        }
        //PrintUtil::print(currMessage);
    }
    client.messages.clear();
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

    // Message to send
    Game::ClientMessage movementMessage;
    Game::ClientMessage rotationMessage;

    // Get key inputs and set direction of message
    if (glfwGetKey(window.glfwViewport, GLFW_KEY_W) == GLFW_PRESS &&
    glfwGetKey(window.glfwViewport, GLFW_KEY_S) == GLFW_PRESS) {
        // Do nothing: The movements should cancel each other out.
    }
    else if (glfwGetKey(window.glfwViewport, GLFW_KEY_W) == GLFW_PRESS)
    {
        movementMessage.set_direction(Game::Direction::UP);  
    }
	else if (glfwGetKey(window.glfwViewport, GLFW_KEY_S) == GLFW_PRESS) 
    {
        movementMessage.set_direction(Game::Direction::DOWN); 
    }

    
    if (glfwGetKey(window.glfwViewport, GLFW_KEY_A) == GLFW_PRESS &&
    glfwGetKey(window.glfwViewport, GLFW_KEY_D) == GLFW_PRESS) {
        // Do nothing: The movements should cancel each other out.
    }
	else if (glfwGetKey(window.glfwViewport, GLFW_KEY_A) == GLFW_PRESS)
    {
        rotationMessage.set_direction(Game::Direction::LEFT); 
    }
	else if (glfwGetKey(window.glfwViewport, GLFW_KEY_D) == GLFW_PRESS)
    {
        rotationMessage.set_direction(Game::Direction::RIGHT); 
    }

    // Send message only if it has a direction associated with it
    if (movementMessage.has_direction()) {
        this->client.send(movementMessage);
    }
    if (rotationMessage.has_direction()) {
        this->client.send(rotationMessage);
    }

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
        // Get the player - we're placing a wall at the player.
        GameObject* player = window.objectsToRender[objectId];

        // Create the wall and place it at the player.
        GameObject* wall = new Wall();
        wall->setPosition(player->getRoundedPosition());

        // Add it to the window.
        window.addObject(wall->getID(), wall);

        // Add it to the stack of map objects.
        mapObjects.push(wall);
    }

    // Remove last created object
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
    {
        if (!mapObjects.empty())
        {
            lastDeleted = mapObjects.top();
            window.removeObject(lastDeleted->getID());
            mapObjects.pop();
        }
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
}