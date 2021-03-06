#include <util/MapBuilder.h>

DungeonMap* MapBuilder::getDungeonMap() {

    // Create the dungeon map
    DungeonMap *mp = new DungeonMap();

    // Process walls for dungeon
    processWalls(mp, "Dungeon");

    // Process floors for dungeon
    processFloor(mp, "Dungeon");

    // Process spawns for dungeon
    processSpawns(mp, "Dungeon");
    
    // Set box for ingredient spawning position
    mp->lowerX = Config::getFloat("Dungeon_Lower_X");
    mp->upperX = Config::getFloat("Dungeon_Upper_X");
    mp->lowerZ = Config::getFloat("Dungeon_Lower_Z");
    mp->upperZ = Config::getFloat("Dungeon_Upper_Z");

    // Set time difference between spawning ingredients
    mp->timeDelta = std::chrono::seconds(Config::getInt("Dungeon_Ingredient_Spawn_Rate"));

    // Set initial time to spawn ingredient
    mp->ingredientSpawnTime = std::chrono::high_resolution_clock::now();

    return mp;
}

KitchenMap* MapBuilder::getKitchenMap() {
    
    // Create kitchenmap container
    KitchenMap* mp = new KitchenMap();

    // Process walls for kitchen
    processWalls(mp, "Kitchen");

    // Process floors for kitchen
    processFloor(mp, "Kitchen");

    // Process spawns for kitchen
    processSpawns(mp, "Kitchen");

    // Create the prison cell :)))
    int cellWallCount = Config::getInt("Cell_Wall_Count");
    for (int i = 0; i < cellWallCount; i++)
    {
        // Adjust bounding box and model and position
        Wall* cellWall = new Wall();
        cellWall->setPosition(Config::getVec3("Cell_Wall_" + std::to_string(i)));
        cellWall->setModel(Config::get("Cage_Bar_Model"));
        cellWall->applyScale(glm::vec3(Config::getInt("Cell_Scale")));

        BoundingBox* wallBoundBox = cellWall->getBoundingBox();

        // Top and bottom walls
        if (i >= 2)
        {
            cellWall->setRotation(90);
            wallBoundBox->setDepth(0.1);
            wallBoundBox->updateCorners();
        }
        // Left and right walls
        else
        {
            wallBoundBox->setWidth(0.1);
            wallBoundBox->updateCorners();
        }

        mp->terrainList.push_back(cellWall);
    }

    // Create the base for the cell
    Wall* cellBase = new Wall();
    cellBase->setPosition(Config::getVec3("Cell_Base"));
    cellBase->setModel(Config::get("Cage_Topbot_Model"));
    cellBase->applyScale(glm::vec3(Config::getInt("Cell_Scale")));
    cellBase->setPassable(true);
    mp->terrainList.push_back(cellBase);

    // Add prison spawn positions
    int prisonSpawnCount = Config::getInt("Cell_Spawn_Count");
    for (int i = 0; i < prisonSpawnCount; i++)
    {
        mp->prisonLocations.push_back(Config::getVec3("Cell_Wall_Spawn_" + std::to_string(i)));
    }

    // Get plate count
    int count = Config::getInt("Plate_Count");

    // Add plates 
    for (int i = 0; i < count; i++)
    {
        glm::vec3 currPos = Config::getVec3("Plate_" + std::to_string(i));
        Plate* plate = new Plate();
        plate->assignId(i);
        plate->setPosition(currPos);

        // assign model based on id
        plate->setModel(Config::get("Plate_" + std::to_string(i) + "_Model"));
        mp->plateObjects.push_back(plate);
    }

    // Get cutting board count
    count = Config::getInt("Cutting_Board_Count");

    // Add cutting boards
    for (int i = 0; i < count; i++)
    {
        glm::vec3 currPos = Config::getVec3("Cutting_Board_" + std::to_string(i));
        Cookware* c = new Cookware(CUTTING_BOARD);
        c->setPosition(currPos);
        mp->cookwareObjects.push_back(c);
    }

    // Get pot count
    count = Config::getInt("Pot_Count");

    // Add pots 
    for (int i = 0; i < count; i++)
    {
        glm::vec3 currPos = Config::getVec3("Pot_" + std::to_string(i));
        Cookware* c = new Cookware(POT);
        c->setPosition(currPos);
        mp->cookwareObjects.push_back(c);
    }

    // Get pan count
    count = Config::getInt("Pan_Count");

    // Add pans 
    for (int i = 0; i < count; i++)
    {
        glm::vec3 currPos = Config::getVec3("Pan_" + std::to_string(i));
        Cookware* c = new Cookware(PAN);
        c->setPosition(currPos);
        mp->cookwareObjects.push_back(c);
    }

    // Add winner model
    GameObject* winner = new GameObject();
    winner->setPosition(Config::getVec3("Winner_Pos"));
    winner->setModel(Config::get("Winner_Model"));
    winner->applyScale(glm::vec3(Config::getInt("Winner_Scale")));
    winner->setRotation(Config::getFloat("Winner_Rot"));
    winner->setPassable(true);
    mp->winner = winner;
    mp->terrainList.push_back(winner);

    // Return
    return mp;
}

LobbyMap* MapBuilder::getLobbyMap() {
    
    // Create lobbymap container
    LobbyMap* mp = new LobbyMap();

    // Process walls for lobby
    processWalls(mp, "Lobby");

    // Process floor for lobby
    processFloor(mp, "Lobby");

    return mp;
}

/**
 * Helper function designed to process walls for all map types.
 */
void MapBuilder::processWalls(Map* mp, std::string prefix)
{
    // Get wall count
    int wallCount = Config::getInt(prefix + "_Wall_Count");
    
    // Iterate over walls and add them to the map
    for (int i = 0; i < wallCount; i++)
    {
        Wall* wall = new Wall();
        wall->setPosition(Config::getVec3(prefix + "_Wall_Pos_" + std::to_string(i)));
        
        // Get scale and rotation
        std::string scaleVecString = Config::get(prefix + "_Wall_Scale_" + std::to_string(i));
        std::string rotString = Config::get(prefix + "_Wall_Rot_" + std::to_string(i));

        // Apply scale and rotation, if applicable
        if (scaleVecString.size() != 0)
        {
            glm::vec3 scaleVec = Config::getVec3(prefix + "_Wall_Scale_" + std::to_string(i));
            wall->applyScale(scaleVec);
        }
        if (rotString.size() != 0)
        {
            float rot = Config::getFloat(prefix + "_Wall_Rot_" + std::to_string(i));
            wall->setRotation(rot);
        }

        // Add wall to list
        mp->terrainList.push_back(wall);
    }
}

/**
 * Helper function designed to process floors for all map types.
 */
void MapBuilder::processFloor(Map* mp, std::string prefix)
{
    // Create floor object with map-specific model
    GameObject* floor = new GameObject();
    floor->setModel(Config::get(prefix + "_Floor_Model"));

    // Apply scale and rotation
    glm::vec3 scaleVec = Config::getVec3(prefix + "_Floor_Scale");
    floor->applyScale(scaleVec);
    float rot = Config::getFloat(prefix + "_Floor_Rot");
    floor->setRotation(rot);

    // Make floor passable
    floor->setPassable(true);

    // Set position
    floor->setPosition(Config::getVec3(prefix + "_Floor_Pos"));

    // Add to terrain
    mp->terrainList.push_back(floor);
}
/**
 * Helper function designed to process spawn points for all map types.
 */
void MapBuilder::processSpawns(Map* mp, std::string prefix)
{
    // Get spawn count
    int spawnCount = Config::getInt(prefix + "_Spawn_Count");

    // Iterate over spawns and add to map
    for (int i = 0; i < spawnCount; i++)
    {
        mp->spawningLocations.push_back(Config::getVec3(prefix + "_Spawn_" + std::to_string(i)));
    }
}