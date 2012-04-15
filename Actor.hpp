#ifndef ACTOR_HPP
#define ACTOR_HPP

#include "SceneGraphNode.hpp"

class Actor : public SceneGraphNode
{
public:
    Actor();
    virtual ~Actor();

    virtual void draw();
    virtual void update();

    void addToChain(Actor& inActor);
    void removeFromChain();
    void drawChain();
    void updateChain();

private:
    Actor* mNextActor;
    Actor* mPreviousActor;
};

#endif
