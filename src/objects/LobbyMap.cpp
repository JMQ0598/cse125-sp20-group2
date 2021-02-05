#include <objects/LobbyMap.h>

LobbyMap::LobbyMap() {

}

void LobbyMap::toggleRender() {
    for(auto & terrain : this->terrainList)
        terrain->toggleRender();
}

void LobbyMap::setRender(bool render) {
    for(auto & terrain : this->terrainList)
        terrain->setRender(render);
}