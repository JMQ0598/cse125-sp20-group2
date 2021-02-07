#include <objects/GameObject.h>

// Hardcoded counter allows us to create IDs
int GameObject::counter = 1000000;

GameObject::GameObject() {
	this->ID = GameObject::counter;
	GameObject::counter++;
	this->setPosition(glm::vec3(0, 0, 0));
	this->setRotation(0);
	this->applyScale(glm::vec3(1));
	this->model = NULL;
	this->box = new BoundingBox(this); 
	this->updateMeasurements();
	this->setPassable(false);
	this->setObjectType(OBJECT);
}

/**
 * Initialize a new gameobject with a unique ID that should be provided by the server.
 * 
 * @param id - The unique unsigned int identifier of this object.
 * */
GameObject::GameObject(int id) {

	// Set id
	this->ID = id;

	// Set transformations, assuming origin with no rotation
	this->setPosition(glm::vec3(0, 0, 0));
	this->setRotation(0);
	this->applyScale(glm::vec3(1));

	// Model should be set afterward
	this->model = NULL;

	// For collisions
	this->box = new BoundingBox(this); 
	this->updateMeasurements();
	this->setPassable(false);

	// Generic object type
	this->setObjectType(OBJECT);
}

GameObject::~GameObject() 
{
	if (model) delete model;
	if (box) delete box;
}

void GameObject::setObjectType(ObjectType newObjType) {
	this->objType = newObjType;
}

ObjectType GameObject::getObjectType() {
	return this->objType;
}

void GameObject::draw(Shader shader) {
	this->model->draw(shader);
}

int GameObject::getID() {
	return this->ID;
}

BoundingBox* GameObject::getBoundingBox() {
	return box;
}

void GameObject::setModel(std::string path) {
	this->modelPath = path;
}

void GameObject::updateMeasurements()
{
	// This code serves to update collisions. If either of these are missing, stop
	if (!box) return;

	// Update collision box, if applicable
	if (!this->box->isCircleBoundingBox())
	{
		this->box->setWidth(baseWidth * scaleVec.x);
		this->box->setDepth(baseDepth * scaleVec.z);
		this->box->updateCorners();
	} 

	// Update collision circle, if applicable
	else 
	{
		this->box->setRadius(baseRadius * scaleVec.x);
	}
}

std::string GameObject::getModelPath()
{
	return this->modelPath;
}

void GameObject::loadModel()
{
	this->model = new Model(modelPath);
	this->updateMeasurements();
}

// Update the world position
void GameObject::setPosition(glm::vec3 loc) {
	this->pos = loc;
}

void GameObject::setPosition(float x, float y, float z) {
	this->pos = glm::vec3(x, y, z);
}

void GameObject::renderInvisible() {
	glm::vec3 originalPos = this->getPosition();
	glm::vec3 newPos = glm::vec3(originalPos.x, -100, originalPos.z);
	this->setPosition(newPos);
	this->render = false;
	this->box = NULL;
}

bool GameObject::getRender() {
	return this->render;
}

// toggle render
void GameObject::toggleRender() {
	this->render = !this->render;
}

// set render
void GameObject::setRender(bool render) {
	this->render = render;
}

// Apply scaling to the model.
void GameObject::applyScale(glm::vec3 scale) {
	this->scaleVec = scale;
	this->updateMeasurements();
}

glm::vec3 GameObject::getPosition() {
	return this->pos;
}

float GameObject::getRotation() 
{
	return this->rotation;
}

// Set rotation and rotate the model to the new angle.
void GameObject::setRotation(float rot) 
{
	// Set rotation
	this->rotation = rot;

	// Objects
	if ((int)rot % 90 != 0 && objType == WALL)
	{
		std::cout << "Warning: Trying to rotate wall on a non-right angle." << std::endl;
	}

	// Bounding box rotation
	if (rot == 90 || rot == 270)
	{
		float oldWidth = baseWidth;
		baseWidth = baseDepth;
		baseDepth = oldWidth;
		this->updateMeasurements();
	}
}

bool GameObject::isPassable() {
	return this->passable;
}

void GameObject::setPassable(bool passable) {
	this->passable = passable;
}

glm::vec3 GameObject::getScaleVec() {
    return this->scaleVec;
}

bool GameObject::isColliding(GameObject* obj) {
	if (this->box == NULL || obj->box == NULL || !obj->render || !this->render ) {
		return false;
	}
	return this->box->isIntersecting(obj->box);
}