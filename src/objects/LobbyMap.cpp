#include <objects/LobbyMap.h>

LobbyMap::LobbyMap() {

}

void LobbyMap::toggleRender() {
    for(auto & wall : this->wallList)
        wall->toggleRender();
}

void LobbyMap::setRender(bool render) {
    for(auto & wall : this->wallList)
        wall->setRender(render);
}