#include <objects/KitchenMap.h>

KitchenMap::KitchenMap() {

}

void KitchenMap::toggleRender() {
    for(auto & terrain : this->terrainList)
        terrain->toggleRender();

    for(auto & table : this->tableList)
        table->toggleRender();

    for(auto & c : this->cookwareObjects)
        c->toggleRender();

    for (auto & p : this->plateObjects)
        p->toggleRender();
}

void KitchenMap::setRender(bool render) {
    for(auto & terrain : this->terrainList)
        terrain->setRender(render);

    for(auto & table : this->tableList)
        table->setRender(render);

    for(auto & c : this->cookwareObjects)
        c->setRender(render);
    
    for(auto & p : this->plateObjects)
        p->setRender(render);
}