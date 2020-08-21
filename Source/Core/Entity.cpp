
#include "Entity.h"
#include "Component.h"
#include "Manager.h"

namespace ECS 
{
    void ECS::Entity::addGroup(Group mGroup) noexcept
    {
        _groupBitset[mGroup] = true;
        _manager.addToGroup(std::make_shared<Entity>(_manager), mGroup);
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

    void Entity::delGroup(Group mGroup) noexcept
    {
        // will be used by manager during refresh
        _groupBitset[mGroup] = false;
    }

} // namespace ECS