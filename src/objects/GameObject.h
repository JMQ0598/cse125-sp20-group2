#pragma once

#include <util/Config.h>
#include <graphics/render/Shader.h>
#include <graphics/render/Model.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>

enum ObjectType
{
    OBJECT = 0,
    PLAYER = 1,
    INGREDIENT = 2,
    COOKWARE = 3,
    WALL = 4,
    TABLE = 5,
    PLATE = 6,
    FLOOR9 = 7,
    FLOOR25 = 8,
    FLOOR49 = 9,
    FLOOR81 = 10,
    FLOOR729 = 11
};

// is intersecting(GameObject) -> return boundingBox.isIntersecting
class GameObject
{

    /// TODO: remove this later - hardcoded id
public:
    static int counter;
    bool selected = false;
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

    // Rendered in world or not
    bool render = true;

protected:
    // base model sizes
    float baseWidth, baseDepth;
    float baseRadius;

    // Sizing - may be needed for collisions, depends on model size and scale
    float width, height, depth;

    // Object type
    ObjectType objType;

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

    void setRotation(float rot);

    GameObject *getItem(int index);

    // Will probably be used on item pickup in dungeon phase
    void setItem(int index, GameObject *item);

    // Will probably be used on item placement in cooking phase
    void removeItem(int index);
    
    glm::vec3 getRoundedPosition();
};