#include <objects/DungeonMap.h>

void DungeonMap::setRender(bool b) {
    for(auto terrain : this->terrainList)
        terrain->setRender(b);
}