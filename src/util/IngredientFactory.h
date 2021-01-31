#include <utility>
#include <util/Config.h>
#include <objects/Ingredient.h>

class IngredientFactory {
public:
    static Ingredient* createIngredient(std::string modelPath);
};