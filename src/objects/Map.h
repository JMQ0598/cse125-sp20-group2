#pragma once

#include <objects/Wall.h>
#include <objects/Ingredient.h>

class Map {
    public:
        // Walls include tables
        std::vector<Wall*> wallList;
        GameObject* floor;
        std::vector<glm::vec3> spawningLocations;
};