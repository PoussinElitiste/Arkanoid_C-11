
#include "Entity.h"
#include "Component.h"
#include "Manager.h"

namespace ECS 
{
    void ECS::Entity::addGroup(Group mGroup) noexcept
    {
        groupBitset[mGroup] = true;
        manager.addToGroup(this, mGroup);
    }

    void Entity::update(float mFT)
    {
        for (auto &c : _components)
            c->update(mFT);
    }

    void Entity::draw()
    {
        for (auto &c : _components)
            c->draw();
    }
} // namespace ECS