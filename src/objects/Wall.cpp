#include <objects/Wall.h>

Wall::Wall() : GameObject() {
	this->setModel(Config::get("Dirty_Wall_Model"));
    this->applyScale(Config::getVec3("Wall_Scaling"));
    this->baseWidth = Config::getFloat("Wall_Width");
	this->baseDepth = Config::getFloat("Wall_Depth");
    this->objType = WALL;
}

Wall::Wall(int id) : GameObject(id) {
	this->setModel(Config::get("Dirty_Wall_Model"));
    this->applyScale(Config::getVec3("Wall_Scaling"));
    this->baseWidth = Config::getFloat("Wall_Width");
	this->baseDepth = Config::getFloat("Wall_Depth");
    this->objType = WALL;
}