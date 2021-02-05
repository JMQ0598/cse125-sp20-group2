#pragma once

#include <util/Config.h>
#include <graphics/render/Shader.h>
#include <graphics/render/Model.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>
#include <collisions/BoundingBox.h>

class BoundingBox;

enum ObjectType
{
    OBJECT = 0,
    PLAYER = 1,
    INGREDIENT = 2,
    COOKWARE = 3,
    WALL = 4,
    TABLE = 5,
    PLATE = 6
};

// is intersecting(GameObject) -> return boundingBox.isIntersecting
class GameObject
{

public:
    // Object counter
    static int counter;

private:
    // Model path
    std::string modelPath;

    // Position in the world
    glm::vec3 pos;

    // Used to determine transformations of object
    glm::vec3 scaleVec;
    float rotation;

    // Identifies the rendering ID of the object
    int ID;

    // Inventory
    std::unordered_map<unsigned int, GameObject *> inventory;

    // Able to pass through object
    bool passable = false;

    // Rendered in world or not
    bool render = true;

protected:
    // base model sizes
    float baseWidth, baseDepth;
    float baseRadius = 1;

    // Sizing - may be needed for collisions, depends on model size and scale
    float width, depth;

    // The bounding box for this game object
    BoundingBox *box = NULL;

    // Object type
    ObjectType objType;

    /**
     * Helper method for constructor. Load collision measurements as needed. Update measurements.
     * */
    void loadCollisionSize();

public:
    GameObject();

    /**
     * This is a constructor for both the server and client.
     * 
     * @param id - This is the id of the game object
     * */
    GameObject(int id);

    ~GameObject();

    // Model
    Model *model = NULL;

    ObjectType getObjectType();

    bool getRender();

    void setRender(bool render);

    void toggleRender();

    std::string getModelPath();

    void loadModel();

    void setObjectType(ObjectType newObjType);

    /**
     * Sets the model of the object based on a given string.
     * 
     * @param path - This is the file path to the model.
     * */
    void setModel(std::string path);

    /**
     * Should be called after setting the model or changing the scale.
     * Should update width/depth accordingly.
     * */
    void updateMeasurements();

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

    void setPosition(float x, float y, float z);

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

    BoundingBox *getBoundingBox();

    void setRotation(float rot);

    GameObject *getItem(int index);

    // Will probably be used on item pickup in dungeon phase
    void setItem(int index, GameObject *item);

    // Will probably be used on item placement in cooking phase
    void removeItem(int index);

    bool isPassable();

    void setPassable(bool passable);

    /**
     * Returns true if there is a collision. 
     * Otherwise, returns false if no collision or either don't have a bounding box.
     * */
    bool isColliding(GameObject *);
};