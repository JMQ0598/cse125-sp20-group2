#include <objects/Cookware.h>

Cookware::Cookware(int ID) : GameObject(ID) {
    this->setModel(Config::get("Pan_Model"));
    this->objType = COOKWARE;
    this->applyScale(Config::getVec3("Cookware_Scaling"));
}

Cookware::Cookware() : GameObject() {
    this->setModel(Config::get("Pan_Model"));
    this->objType = COOKWARE;
    this->applyScale(Config::getVec3("Cookware_Scaling"));
}

Cookware::Cookware(std::string model): GameObject() {
    this->setModel(Config::get(model + "_Model"));
    this->setName(model);
    this->objType = COOKWARE;
    this->applyScale(Config::getVec3("Cookware_Scaling"));
}

void Cookware::setBusy(bool busy) {
    this->busy = busy;
}

bool Cookware::getBusy() {
    return this->busy;
}
    
void Cookware::setName(std::string name) {
    this->name = name;
}

std::string Cookware::getName() {
    return this->name;
}

bool Cookware::cookIngredient(Ingredient* ing) {
    if( this->name == PAN ) {
        if( ing->getStatus() == IngredientStatus::Boiled || ing->getStatus() == IngredientStatus::Fried )
            return false;
        else
            ing->setStatus(IngredientStatus::Fried);
    } else if ( this->name == POT ) {
        if( ing->getStatus() == IngredientStatus::Boiled || ing->getStatus() == IngredientStatus::Fried )
            return false;
        else
            ing->setStatus(IngredientStatus::Boiled);
    } else if ( this->name == CUTTING_BOARD ) {
        if( ing->getStatus() == IngredientStatus::Boiled || ing->getStatus() == IngredientStatus::Fried || ing->getStatus() == IngredientStatus::Cut )
            return false;
        else
            ing->setStatus(IngredientStatus::Cut);
    }
    return true;
}
