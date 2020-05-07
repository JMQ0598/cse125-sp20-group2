#pragma once

#include <graphics/render/Shader.h>
#include <graphics/render/Model.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <constants/graphics_vars.h>
#include <unordered_map>
#include <collisions/BoundingBox.h>

class BoundingBox;

enum ObjectType
{
    OBJECT = 0, // GameObject
    PLAYER = 1, // Player
    INGREDIENT = 2, // IngredientObject
    COOKWARE = 3  // CookwareObject
};

// is intersecting(GameObject) -> return boundingBox.isIntersecting
class GameObject
{
private:
    // Rendered model
    Model* model;

    // Position in the world
	glm::vec3 pos;

    // Used to determine transformations of object
	glm::vec3 scaleVec;
    float rotation;

    // Identifies the rendering ID of the object
	int ID;

    // Inventory
    std::unordered_map<unsigned int, GameObject*> inventory;

    // Passing through object
    bool passable = false;

protected:
    // The bounding box for this game object
    BoundingBox* box;

    ObjectType objType;

public:
    /**
     * This is a constructor for both the server and client.
     * 
     * @param id - This is the id of the game object
     * */
    GameObject(int id);

    ObjectType getObjectType();

    void setObjectType(ObjectType newObjType);

    /**
     * Sets the model of the object based on a given string.
     * 
     * @param path - This is the file path to the model.
     * */
    void setModel(std::string path);

    /**
     * Draw the model using the given shader.
     * 
     * @param shader - The shader to use to draw the model.
     * */
    void draw(Shader shader);

    int getID();

    /**
     * Get the world position of the object
     * */
    glm::vec3 getPosition();

    /**
     * Moves the object and sets the world position.
     * 
     * @param loc - The new location of the object.
     * */
    void setPosition(glm::vec3 loc);

    /**
     * Moves the object below the floor, rendering it invisible to players
     * */
    void renderInvisible();

    /**
     * The scale vector (x, y, z scaling) to be applied to the object.
     * 
     * @param scale - The scale vector to be applied.
     * */
    void applyScale(glm::vec3 scale);

    glm::vec3 getScaleVec();

    float getRotation();

    BoundingBox* getBoundingBox();

    void setRotation(float rot);
    
    GameObject* getItem(int index);
    
    // Will probably be used on item pickup in dungeon phase
    void setItem(int index, GameObject* item);

    // Will probably be used on item placement in cooking phase
    void removeItem(int index);

    bool isPassable();

    void setPassable(bool passable);
    
    bool isColliding(GameObject*);
};