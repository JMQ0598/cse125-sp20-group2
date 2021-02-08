#pragma once

#include <objects/GameObject.h>

class GameObject;

class BoundingBox
{
protected:
    GameObject *obj;
    float radius;
    float minX, minZ, maxX, maxZ;
    float width, depth;
    bool isCircle;

    bool isAABBIntersecting(BoundingBox *rbb);

    bool isCircleRectangleIntersecting(BoundingBox *cbb);

    bool isCircleIntersecting(BoundingBox *bb);

public:
    BoundingBox(GameObject *obj);
    ~BoundingBox();

    void setRadius(float radius);

    float getRadius();

    bool isIntersecting(BoundingBox *bb);

    void updateCorners();

    bool isCircleBoundingBox();

    void setCircleBoundingBox();

    void setWidth(float width);

    void setDepth(float depth);
};
