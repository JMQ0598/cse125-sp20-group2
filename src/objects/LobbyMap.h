#pragma once

#include <objects/Map.h>
#include <objects/Cookware.h>
#include <objects/Table.h>
#include <objects/Plate.h>

class LobbyMap : public Map {
    public:
        LobbyMap();
        void toggleRender();
        void setRender(bool render);
};