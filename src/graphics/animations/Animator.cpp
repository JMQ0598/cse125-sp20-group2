#include <graphics/animations/Animator.h>

Animator::Animator()
{

}

bool Animator::isAnimsLoadedFor(GameObject* object)
{
    if (animsLoadedFor.find(object) == animsLoadedFor.end()) return false;
    return animsLoadedFor[object];
}

void Animator::loadAnimations(GameObject* object)
{
    // Iterate over all the animations
    for (auto it = animations[object].begin(); it != animations[object].end(); ++it)
    {
        // Load the frames of the animation only if needed.
        Animation* anim = it->second;
        if (!anim->isLoaded())
        {
            anim->loadFrames();
        }
    }
    this->animsLoadedFor[object] = true;
}

void Animator::play(GameObject* object)
{
    // Edge case: No current animation
    if (currentAnimation.find(object) == currentAnimation.end())
    {
        std::cerr << "Animation error: Animation state not set for this object." << std::endl;
        return;
    }

    // Previous state
    PlayerAnimState prevState = currentState[object];

    // Set animation state based on object's action
    switch (object->getObjectType())
    {
        // Handle various player animation states
        case PLAYER:
            if      (((Player*)object)->moving)     currentState[object] = PlayerAnimState::WADDLE;
            else if (((Player*)object)->cooking)    currentState[object] = PlayerAnimState::COOK;
            else                                    currentState[object] = PlayerAnimState::IDLE;
            break;

        // Default case - really should not happen
        default:
            std::cerr << "Bruh how did u get here" << std::endl;
            return;
    }

    // Transition animations, if necessary
    if (currentState[object] != prevState)
    {
        setCurrentAnimation(object, currentState[object]);
    }

    ///NOTE: Debug statement
    //std::cout << "Playing animation" << currentState[object] << std:: endl;

    // Play the next frame of the animation
    currentAnimation[object]->playNextFrame(object);
}

void Animator::addAnimation(GameObject* object, PlayerAnimState animState, Animation* anim)
{
    // Insert animation with the given name
    (animations[object])[animState] = anim;
}

void Animator::setCurrentAnimation(GameObject* object, PlayerAnimState animState)
{
    // Edge case: Cannot find animation
    if (animations[object].find(animState) == animations[object].end())
    {
        std::cerr << "Animation error: animation does not exist." << std::endl;
        return;
    }

    // Reset progress on current animation, if applicable
    if (currentAnimation.find(object) != currentAnimation.end()) currentAnimation[object]->reset();

    // Set current animation based on a given name
    currentAnimation[object] = (animations[object])[animState];
}