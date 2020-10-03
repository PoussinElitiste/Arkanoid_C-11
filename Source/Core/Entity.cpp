
#include "Entity.h"
#include "Component.h"
#include "Manager.h"

namespace ECS 
{
    void ECS::Entity::addGroup(Group mGroup) noexcept
    {
        _groupBitset[mGroup] = true;

        //std::weak_ptr<Entity> entityRef = std::make_shared<Entity>(this);
        // note: for the moment I can't pass weak ptr of Entity because entity contain list of unique_ptr component
        _manager.addToGroup(this, mGroup);
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