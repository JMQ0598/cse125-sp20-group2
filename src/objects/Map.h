#pragma once

#include <objects/Wall.h>
#include <objects/Ingredient.h>

class Map {
    public:
        // Walls include tables
        std::vector<GameObject*> terrainList;
        std::vector<glm::vec3> spawningLocations;
};