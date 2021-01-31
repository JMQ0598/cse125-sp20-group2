#pragma once

#include <objects/GameObject.h>
#include <string>
#include <unordered_map>
#include <constants/gameplay_vars.h>

enum class IngredientStatus
{
    Raw,
    Cut,
    Boiled, 
    Fried
};

/**
raw -> boiled
raw -> fried
raw -> cut -> boiled
raw -> cut -> fried
raw -> cut**/

struct VertexCoord {
    glm::vec3 position;
    glm::vec2 texcoord;
};

class Ingredient : public GameObject
{
protected:
    std::string name;

    IngredientStatus status;

    GLuint textureID;

    GLuint VAO, VBO, EBO;


public:
    static std::unordered_map<IngredientStatus, std::string> IngredientStatusToString;

    static std::unordered_map<std::string, IngredientStatus> stringToIngredientStatus;

    Ingredient(int ID);

    Ingredient();

    ~Ingredient();

    /**
     * Set the ingredient name, which is used by the UI.
     * */
    void setName(std::string name);

    /**
     * Set the ingredient preparation status.
     * */
    void setStatus(IngredientStatus status);

    /**
     * Get the ingredient's status as a string.
     * */
    std::string getStringStatus();

    /**
     * Set the ingredient's status from a string input.
     * */
    void setStatusFromString(std::string);

    std::string getDetailedName();

    std::string getName();

    /**
     * Get the ingredient's preparation status.
     * */
    IngredientStatus getStatus();

    void draw();

    GLuint getTextureID();

};