#include <collisions/BoundingBox.h>
#include <iostream>

BoundingBox::BoundingBox(GameObject *gameObject)
{
    this->obj = gameObject;
}

BoundingBox::~BoundingBox()
{
}

bool BoundingBox::isAABBIntersecting(BoundingBox *rbb)
{
    updateCorners();
    rbb->updateCorners();
    return (this->minX <= rbb->maxX && this->maxX >= rbb->minX) &&
           (this->minZ <= rbb->maxZ && this->maxZ >= rbb->minZ);
}

bool BoundingBox::isCircleRectangleIntersecting(BoundingBox *cbb)
{
    updateCorners();
    glm::vec3 circlePos = cbb->obj->getPosition();
    float x = std::max(this->minX, std::min(circlePos.x, this->maxX));
    float z = std::max(this->minZ, std::min(circlePos.z, this->maxZ));

    float distance = std::sqrt((x - circlePos.x) * (x - circlePos.x) +
                               (z - circlePos.z) * (z - circlePos.z));

    return distance < cbb->radius;
}

bool BoundingBox::isCircleIntersecting(BoundingBox *bb)
{
    ///std::cout<<"Radiuses of colliding spheres:"<<bb->radius<<"  "<<this->radius<<std::endl;
    glm::vec3 circlePos1 = this->obj->getPosition();
    glm::vec3 circlePos2 = bb->obj->getPosition();
    float distance = sqrt((circlePos1.x - circlePos2.x) * (circlePos1.x - circlePos2.x) +
                          (circlePos1.z - circlePos2.z) * (circlePos1.z - circlePos2.z));
    return distance < (this->radius + bb->radius);
}

void BoundingBox::setCircleBoundingBox() { this->isCircle = true; }

void BoundingBox::setWidth(float width)
{
    this->isCircle = false;
    this->width = width;
}

void BoundingBox::setDepth(float depth)
{
    this->isCircle = false;
    this->depth = depth;
}

void BoundingBox::setHeight(float height)
{
    this->isCircle = false;
    this->height = height;
}

bool BoundingBox::isCircleBoundingBox()
{
    return this->isCircle;
}

void BoundingBox::setRadius(float radius)
{
    setCircleBoundingBox();
    this->radius = radius;
}

float BoundingBox::getRadius()
{
    return this->radius;
}

bool BoundingBox::isIntersecting(BoundingBox *bb)
{
    if (bb == NULL) {
        return false;
    }
    if (this->isCircle && bb->isCircle)
    {
        return this->isCircleIntersecting(bb);
    }
    else if (this->isCircle)
    {
        return bb->isCircleRectangleIntersecting(this);
    }
    else if (bb->isCircle)
    {
        return this->isCircleRectangleIntersecting(bb);
    }
    else
    {
        return this->isAABBIntersecting(bb);
    }
}

void BoundingBox::updateCorners()
{
    glm::vec3 position = obj->getPosition();
    maxX = position.x + width / 2.0;
    minX = position.x - width / 2.0;
    maxZ = position.z + depth / 2.0;
    minZ = position.z - depth / 2.0;
}