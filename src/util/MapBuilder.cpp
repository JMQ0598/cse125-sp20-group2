#include <util/MapBuilder.h>

DungeonMap* MapBuilder::getDungeonMap() {

    // Create the dungeon map
    DungeonMap *mp = new DungeonMap();

    // Get wall count
    int dungeonWallCount = Config::getInt("Dungeon_Wall_Count");
    
    // Iterate over walls and add them to the map
    for (int i = 0; i < dungeonWallCount; i++)
    {
        Wall* wall = new Wall();
        wall->setPosition(Config::getVec3("Dungeon_Wall_Pos_" + std::to_string(i)));
        
        // Get scale and rotation
        std::string scaleVecString = Config::get("Dungeon_Wall_Scale_" + std::to_string(i));
        std::string rotString = Config::get("Dungeon_Wall_Rot_" + std::to_string(i));

        // Apply scale and rotation, if applicable
        if (scaleVecString.size() != 0)
        {
            glm::vec3 scaleVec = Config::getVec3("Dungeon_Wall_Scale_" + std::to_string(i));
            wall->applyScale(scaleVec);
        }
        if (rotString.size() != 0)
        {
            float rot = Config::getFloat("Dungeon_Wall_Rot_" + std::to_string(i));
            wall->setRotation(rot);
        }

        // Add wall to list
        mp->wallList.push_back(wall);
    }

    // Get spawn count
    int spawnCount = Config::getInt("Dungeon_Spawn_Count");

    // Iterate over spawns and add to map
    for (int i = 0; i < spawnCount; i++)
    {
        mp->spawningLocations.push_back(Config::getVec3("Dungeon_Spawn_" + std::to_string(i)));
    }

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

        mp->wallList.push_back(cellWall);
    }

    // Create the base for the cell
    Wall* cellBase = new Wall();
    cellBase->setPosition(Config::getVec3("Cell_Base"));
    cellBase->setModel(Config::get("Cage_Topbot_Model"));
    cellBase->applyScale(glm::vec3(Config::getInt("Cell_Scale")));
    cellBase->setPassable(true);
    mp->wallList.push_back(cellBase);

    // Get spawn count
    int spawnCount = Config::getInt("Kitchen_Spawn_Count");

    // For testing, spawn player in prison
    int prisonSpawnCount = Config::getInt("Cell_Spawn_Count");
    for (int i = 0; i < prisonSpawnCount; i++)
    {
        mp->prisonLocations.push_back(Config::getVec3("Cell_Wall_Spawn_" + std::to_string(i)));
    }

    // Iterate over spawns and add to map
    for (int i = 0; i < spawnCount; i++)
    {
        mp->spawningLocations.push_back(Config::getVec3("Kitchen_Spawn_" + std::to_string(i)));
    }

    // Add plates (vary by x)
    for (int i = 0; i < spawnCount; i++)
    {
        glm::vec3 currPos = Config::getVec3("Plate_Init_Spawn");
        float variation = Config::getFloat("Plate_Spacing");
        currPos.x += i * variation;
        Plate* plate = new Plate();
        plate->assignId(i);
        plate->setPosition(currPos);

        // assign model based on id
        plate->setModel(Config::get("Plate_" + std::to_string(i) + "_Model"));
        mp->plateObjects.push_back(plate);
    }

    // Add cutting boards (vary by x)
    for (int i = 0; i < spawnCount; i++)
    {
        glm::vec3 currPos = Config::getVec3("Cutting_Board_Init_Spawn");
        float variation = Config::getFloat("Cutting_Board_Spacing");
        currPos.x += i * variation;
        Cookware* c = new Cookware(CUTTING_BOARD);
        c->setPosition(currPos);
        mp->cookwareObjects.push_back(c);
    }

    // Add pots (vary by z)
    for (int i = 0; i < spawnCount; i++)
    {
        glm::vec3 currPos = Config::getVec3("Pot_Init_Spawn");
        float variation = Config::getFloat("Pot_Spacing");
        currPos.z += i * variation;
        Cookware* c = new Cookware(POT);
        c->setPosition(currPos);
        mp->cookwareObjects.push_back(c);
    }

    // Add pans (vary by z)
    for (int i = 0; i < spawnCount; i++)
    {
        glm::vec3 currPos = Config::getVec3("Pan_Init_Spawn");
        float variation = Config::getFloat("Pan_Spacing");
        currPos.z += i * variation;
        Cookware* c = new Cookware(PAN);
        c->setPosition(currPos);
        mp->cookwareObjects.push_back(c);
    }

    return mp;
}

LobbyMap* MapBuilder::getLobbyMap() {
    
    // Create lobbymap container
    LobbyMap* mp = new LobbyMap();

    ///TODO: Complete lobby map setup (read wall locations, scales, rots, etc.)

    return mp;
}

void MapBuilder::assignIngredientPositions( Recipe* recipe, DungeonMap* mp ) {
    /*int i = 0;
    for( auto ingredient : recipe->ingredientList ) {
        ingredient->setPosition(mp->ingredientPositions[i]);
        i++;
    }*/
    std::cout << "WARNING, INGREDIENT POSITIONS ARE NOT BEING ASSIGNED" << std::endl;
    // DungeonMap* dMapPtr = static_cast<DungeonMap*>(mp);
    // for (int i = 0; i < recipe->ingredientList.size(); i++) {
    //     Ingredient* ing = recipe->ingredientList.front();
    //     recipe->ingredientList.erase(recipe->ingredientList.begin());
    //     ing->setPosition(dMapPtr->ingredientPositions[i]);
    //     recipe->ingredientList.push_back(ing);
    // }
}