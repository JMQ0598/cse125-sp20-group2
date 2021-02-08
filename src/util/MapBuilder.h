#pragma once

#include <objects/Wall.h>
#include <util/Recipe.h>
#include <objects/Map.h>
#include <objects/KitchenMap.h>
#include <objects/DungeonMap.h>
#include <objects/LobbyMap.h>
#include <objects/Table.h>
#include <objects/Plate.h>
#include <game/GameState.h>

class MapBuilder {
public:
    static DungeonMap* getDungeonMap();
    static KitchenMap* getKitchenMap();
    static LobbyMap* getLobbyMap();
    static void processWalls(Map* mp, std::string prefix);
    static void processFloor(Map* mp, std::string prefix);
    static void processSpawns(Map* mp, std::string prefix);
};