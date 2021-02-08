#include <game/ClientGame.h>
#include <game/KeyResolver.h>
#include <objects/Player.h>
#include <objects/Plate.h>

#define CLIENT_DELAY 1000

ClientGame::ClientGame(std::string IP, int port) : client(IP, port)
{
    // Handle bad connect
    if (client.ConnectSocket == INVALID_SOCKET) this->exitCode = ERR_BADHOST;

    // Window setup
    window = new Window(Config::getFloat("Window_Width"), Config::getFloat("Window_Height"));

    // Configure keybinds
    glfwSetWindowUserPointer(this->window->glfwViewport, reinterpret_cast<void*> (this));
    glfwSetKeyCallback(this->window->glfwViewport, key_callback_wrapper);
}

ClientGame::~ClientGame()
{
    delete window;
}

/**
 * Helper method to check if a key is not printable, therefore not
 * mappable
 * */
bool isMappable(int key) {
    return glfwGetKeyName(key, 256) != NULL;
}

/**
 * Callback handles key bindings that involve a single key press (not held down)
 * Differs from processInput in that it does not query the key every frame.
 */
void ClientGame::keyBindsHandler(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods)
{
    // Checking for pending keybinds
    if (action == GLFW_PRESS)
    {
        if (this->window->currkeyBindStatus != Window::KeyBindStatus::none)
        {
            if (this->window->currkeyBindStatus == Window::KeyBindStatus::forwards)
            {
                if (Config::noConflictingKey(key) && isMappable(key))  Config::setInt("Walk_Forward", key);
            }
            else if (this->window->currkeyBindStatus == Window::KeyBindStatus::backwards)
            {
                if (Config::noConflictingKey(key) && isMappable(key))  Config::setInt("Walk_Backward", key);
            }
            else if (this->window->currkeyBindStatus == Window::KeyBindStatus::left)
            {
                if (Config::noConflictingKey(key) && isMappable(key))  Config::setInt("Turn_Left", key);
            }
            else if (this->window->currkeyBindStatus == Window::KeyBindStatus::right)
            {
                if (Config::noConflictingKey(key) && isMappable(key))  Config::setInt("Turn_Right", key);
            }
            else if (this->window->currkeyBindStatus == Window::KeyBindStatus::action)
            {
                if (Config::noConflictingKey(key) && isMappable(key))  Config::setInt("Action", key);
            }
            else if (this->window->currkeyBindStatus == Window::KeyBindStatus::ready)
            {
                if (Config::noConflictingKey(key) && isMappable(key))  Config::setInt("Ready", key);
            }
            else if (this->window->currkeyBindStatus == Window::KeyBindStatus::settings)
            {
                if (Config::noConflictingKey(key) && isMappable(key))  Config::setInt("Open_Settings", key);
            }

            // Reset keybind status - we are no longer changing a keybind
            this->window->currkeyBindStatus = Window::KeyBindStatus::none;
        }
    }
    
    // Handles displaying the Settings Menu
    if (key == Config::getInt("Open_Settings") && action == GLFW_PRESS)
    {
        this->window->showSettings ^= true;
    }

    // No (other) input while settings menu is open
    if (this->window->showSettings) return;

    // Handles ready up
    if (key == Config::getInt("Ready") && action == GLFW_PRESS)
    {
        std::cout << "sending ready up message" << std::endl;
        Game::ClientMessage* readyMsg = MessageBuilder::toReadyMessage(true);
        this->client.send(*readyMsg);
        delete readyMsg;
    }

    // Handles interact event
    if (key == Config::getInt("Action") && action == GLFW_PRESS && this->window->getSelectedIngredient() != NULL && this->window->getRound() == KITCHEN_NUM )
    {
        std::cout << "pressed interact key" << std::endl;

        // Only allow interactions in kitchen round
        if (window->getRound() == KITCHEN_NUM) 
        {
            Game::ClientMessage* cookMsg = MessageBuilder::toCookMessage(this->window->getSelectedIngredient());
            this->client.send(*cookMsg);
            delete cookMsg;

            // Prevent spamming the event key, have to select ingredient again
            this->window->selectedIngredient = NULL;
        }
    }
}

/**
 * Take in input from the viewport window
 */
void ClientGame::processInput()
{
    // No input while settings menu is open
    if (this->window->showSettings) return;

    // Message to send
    Game::ClientMessage movementMessage;
    Game::ClientMessage rotationMessage;

    // Get key inputs and set direction of message
    if (glfwGetKey(window->glfwViewport, Config::getInt("Walk_Forward")) == GLFW_PRESS &&
    glfwGetKey(window->glfwViewport, Config::getInt("Walk_Backward")) == GLFW_PRESS) {
        // Do nothing: The movements should cancel each other out.
    }
    else if (glfwGetKey(window->glfwViewport, Config::getInt("Walk_Forward")) == GLFW_PRESS)
    {
        movementMessage.set_direction(Game::Direction::UP);  
    }
	else if (glfwGetKey(window->glfwViewport, Config::getInt("Walk_Backward")) == GLFW_PRESS) 
    {
        movementMessage.set_direction(Game::Direction::DOWN); 
    }

    
    if (glfwGetKey(window->glfwViewport, Config::getInt("Turn_Left")) == GLFW_PRESS &&
    glfwGetKey(window->glfwViewport, Config::getInt("Turn_Right")) == GLFW_PRESS) {
        // Do nothing: The movements should cancel each other out.
    }
	else if (glfwGetKey(window->glfwViewport, Config::getInt("Turn_Left")) == GLFW_PRESS)
    {
        rotationMessage.set_direction(Game::Direction::LEFT); 
    }
	else if (glfwGetKey(window->glfwViewport, Config::getInt("Turn_Right")) == GLFW_PRESS)
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
}

/**
 * Main game loop.
 */
int ClientGame::runGame() 
{
    // Background music
    music.openFromFile(Config::get("8bit_Paradise"));
    music.setLoop(true);
    music.play();

    // Game loop runs while window is open
    while(!window->isClosed && !exitCode) 
    {
        // Update volume
        music.setVolume(Config::getInt("Background_Music_Volume"));

        // Take local input
        // Send to the server
        processInput();

        // Receive updated state from server
        receiveUpdates();

        // Update local game state
        updateGameState();

        // Render world
        window->render();
    }

    // Nonzero if an error has occurred - otherwise exited normally
    return exitCode;
}

/**
 * Read update messages from server
 */
void ClientGame::receiveUpdates()
{
    client.read();
}

/**
 * Apply changes to game state based on server messages
 */
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
                if (window->objectsToRender.count(id) > 0) 
                {
                    obj = window->objectsToRender[id];
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
                    // Add object to window
                    window->addObject(id, obj);
                }

                /// Set model based on the model path provided by the server
                std::string modelPath = currMessage.object().modelpath();

                // We got an updated model, set old model to null
                if (modelPath.compare(obj->getModelPath()) != 0)
                {
                    delete obj->model;
                    obj->model = NULL;
                }

                // Set new model path so we can reload
                obj->setModel(modelPath);

                // Set object parameters
                obj->setRotation(rotation);

                // The player is being scaled up which means the player picked up the vodka
                if (currMessage.object().has_scale() && currMessage.object().id() == this->objectId)
                {
                    glm::vec3 originalScale = obj->getScaleVec();
                    glm::vec3 newScale (scale.x(), scale.y(), scale.z());

                    // Bigger player
                    if (originalScale.x < newScale.x) {
                        // Play soviet national anthem
                        music.stop();
                        music.openFromFile(Config::get("Soviet_Anthem"));
                        music.setVolume(Config::getInt("Background_Music_Volume"));
                        music.setLoop(true);
                        music.play();

                        // Turn screen red
                        this->window->vodkaActive = true;
                    }

                    // Smaller player
                    else if (originalScale.x > newScale.x)
                    {
                        if (this->window->getRound() == DUNGEON_NUM) 
                        {
                            // Background music
                            music.stop();
                            music.openFromFile(Config::get("8bit_Paradise"));
                            music.setVolume(Config::getInt("Background_Music_Volume"));
                            music.setLoop(true);
                            music.play();
                        }
                        // Revert screen color
                        this->window->vodkaActive = false; 
                    }
                }

                if (currMessage.object().has_scale()) obj->applyScale(glm::vec3(scale.x(), scale.y(), scale.z()));
                obj->setRender(render);
                obj->setPosition(glm::vec3(location.x(), location.y(), location.z()));
                window->removeCookingEventMessage();
                break;
            }

            case Game::ServerMessage::EventCase::kScore:
            {
                window->setScore(currMessage.score().currscore());
                break;
            }

            // Inventory update messages
            case Game::ServerMessage::EventCase::kInventory: 
            {
                // Get player associated with this client
                Player* player = (Player*)window->objectsToRender[objectId];
                auto currInventory = player->getInventory();

                // Get id of the object to be picked up, set the conditions
                Ingredient* pickup = (Ingredient*)window->objectsToRender[currMessage.inventory().id()];
                pickup->setName(currMessage.inventory().name());
                pickup->setQualityIndex(currMessage.inventory().qualityindex());
                pickup->setStatus(Ingredient::stringToIngredientStatus[currMessage.inventory().ingredientstatus()]);
                
                if(soundEffect.getStatus() == sf::SoundSource::Playing) {
                    soundEffect.stop();
                }

                if (soundBuffer.loadFromFile(Config::get("Inventory_Pickup"))) {
                    soundEffect.setBuffer(soundBuffer);
                    soundEffect.setVolume(Config::getFloat("Sound_Effect_Volume"));
                    soundEffect.play();
                }

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
                std::cout << "got valid cooking event from server" << std::endl;
                std::cout<<currMessage.validcook().message().at(0)<<std::endl;
                switch(currMessage.validcook().message().at(0)) {
                    case 'C': { 
                        soundBuffer.loadFromFile(Config::get("Cutting_Sound")); 
                        soundEffect.setBuffer(soundBuffer);
                        soundEffect.setVolume(Config::getFloat("Sound_Effect_Volume"));
                        break;
                    }
                    case 'F': {
                        soundBuffer.loadFromFile(Config::get("Frying_Sound")); 
                        soundEffect.setBuffer(soundBuffer);
                        soundEffect.setVolume(Config::getFloat("Sound_Effect_Volume"));
                        break;
                    }
                    case 'B': {
                        soundBuffer.loadFromFile(Config::get("Pot_Boiling_Sound")); 
                        soundEffect.setBuffer(soundBuffer);
                        soundEffect.setVolume(Config::getFloat("Sound_Effect_Volume"));
                        break; 
                    }
                    case 'P': {
                        soundBuffer.loadFromFile(Config::get("Dish_Sound")); 
                        soundEffect.setBuffer(soundBuffer);
                        soundEffect.setVolume(Config::getFloat("Sound_Effect_Volume"));
                        break; 
                    }
                    default: {
                        switch(rand()%6) {
                            case 0: soundBuffer.loadFromFile(Config::get("Nyet1")); break;
                            case 1: soundBuffer.loadFromFile(Config::get("Nyet2")); break;
                            case 2: soundBuffer.loadFromFile(Config::get("Nyet3")); break;
                            case 3: soundBuffer.loadFromFile(Config::get("Nyet4")); break;
                            case 4: soundBuffer.loadFromFile(Config::get("Nyet5")); break;
                            case 5: soundBuffer.loadFromFile(Config::get("Nyet6")); break;
                        }
                        soundEffect.setBuffer(soundBuffer);
                        soundEffect.setVolume(Config::getFloat("Nyet_Effect_Volume"));
                    }
                }
                soundEffect.play();
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
                window->camera->setTarget(window->objectsToRender[this->objectId]);
                Player* p = (Player*) window->objectsToRender[this->objectId];
                window->addInventory(p->getInventory());
                break;
            }

            case Game::ServerMessage::EventCase::kInstruction:
            {
                int index = currMessage.instruction().index();
                std::string msg = currMessage.instruction().instructionmsg();
                if(currMessage.instruction().has_recipename() )
                    window->recipeName = currMessage.instruction().recipename();
                window->instructionStrings.insert(window->instructionStrings.begin()+index, msg);
                break;
            }

            case Game::ServerMessage::EventCase::kTime:
            {
                uint32_t seconds = currMessage.time().seconds();
                window->setTimer(seconds);
                break;
            }

            case Game::ServerMessage::EventCase::kRound:
            {
                window->updateRound(currMessage.round().type());         
                if (currMessage.round().type() == Game::RoundInfo_RoundState_KITCHEN_WAITING) {
                    music.stop();
                    music.openFromFile(Config::get("Cooking_Theme"));
                    music.setVolume(Config::getInt("Background_Music_Volume"));
                    music.setLoop(true);
                    music.play();
                }
                std::cout << " received round update message " << currMessage.DebugString() << std::endl;
                break;
            }

            /// TODO: NEED TO CHANGE LATER
            case Game::ServerMessage::EventCase::kWin:
            {
                std::cout << " received win event " << std::endl;
                window->gameOver = true;

                // Win or lose
                if (currMessage.win().clientid() == clientId) {
                    window->gameWin = true;
                    music.stop();
                    music.openFromFile(Config::get("Soviet_Anthem"));
                    music.setLoop(true);
                    music.setVolume(Config::getInt("Background_Music_Volume"));
                    music.play();
                }
                else {
                    window->gameWin = false;
                    music.stop();
                    music.openFromFile(Config::get("The_Dusty_Attic"));
                    music.setLoop(true);
                    music.setVolume(Config::getInt("Background_Music_Volume"));
                    music.play();
                }

                break;
            }

            case Game::ServerMessage::EventCase::kDisconnect:
            {
                /// NOTE: Currently only handles full server.
                std::cout << " got a disconnect message " << std::endl;
                std::cout << "reason: " + currMessage.disconnect().message() << std::endl;
                exitCode = ERR_FULLSERVER;
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