#include <util/IngredientFactory.h>

Ingredient* IngredientFactory::createIngredient( std::string model ) {
    Ingredient* obj = new Ingredient();
    obj->setName(model);
    obj->setModel(Config::get(model + "_Model"));
    obj->applyScale(Config::getVec3("Ingredient_Scaling"));
    return obj;
}