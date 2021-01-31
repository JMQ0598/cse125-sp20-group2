#include <objects/Ingredient.h>

Ingredient::Ingredient(int ID) : GameObject(ID) {
    this->objType = INGREDIENT;
	this->setModel(Config::get("Wall_Model"));
	this->setPosition(glm::vec3(1,0,1));
}

Ingredient::Ingredient() : GameObject() {
    this->objType = INGREDIENT;
	this->setModel(Config::get("Wall_Model"));
}
    
Ingredient::~Ingredient() {
    if (this->textureID != 0) {
        this->textureID = 0;
    }
}

void Ingredient::setName(std::string name) {
    this->name = name;
}

std::string Ingredient::getName() {
    return this->name;
}

IngredientStatus Ingredient::getStatus() {
    return this->status;
}

std::string Ingredient::getStringStatus() {
    return IngredientStatusToString[this->status];
}

void Ingredient::setStatus(IngredientStatus status) {
    this->status = status;
}

void Ingredient::setStatusFromString(std::string status) {
    this->status = stringToIngredientStatus[status];
}

void Ingredient::draw() {
}

GLuint Ingredient::getTextureID() {
    return this->textureID;
}

std::string Ingredient::getDetailedName() {
    return IngredientStatusToString[getStatus()] + this->name;
}

std::unordered_map<IngredientStatus, std::string> Ingredient::IngredientStatusToString = {
    {IngredientStatus::Raw, "Raw"},
    {IngredientStatus::Cut, "Cut"},
    {IngredientStatus::Boiled, "Boiled"},
    {IngredientStatus::Fried, "Fried"}
};

std::unordered_map<std::string, IngredientStatus> Ingredient::stringToIngredientStatus = {
    {"Raw", IngredientStatus::Raw},
    {"Cut", IngredientStatus::Cut},
    {"Boiled", IngredientStatus::Boiled},
    {"Fried", IngredientStatus::Fried}
};