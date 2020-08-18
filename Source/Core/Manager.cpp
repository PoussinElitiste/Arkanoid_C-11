#include "Manager.h"
#include "Entity.h"

namespace ECS
{
    void Manager::update(float mFT)
    {
        for (auto &e : _entities) 
            e->update(mFT);
    }

    void Manager::draw()
    {
        for (auto &e : _entities) e->draw();
    }

    void Manager::addToGroup(Entity *mEntity, Group mGroup)
    {
        _groupedEntities[mGroup].emplace_back(mEntity);
    }

    ECS::EntityList & Manager::getEntitiesByGroup(Group mGroup)
    {
        return _groupedEntities[mGroup];
    }

    void Manager::refresh()
    {
        // remove from group first
        for (auto i { 0u }; i < maxGroups; ++i)
        {
            auto &v(_groupedEntities[i]);
            v.erase(
                remove_if(begin(v), end(v),
                    [i] (Entity *mEntity) {
                return !mEntity->isAlive() || !mEntity->hasGroup(i);
            }),
                end(v));
        }

        // Note: remove_if sort list and push all destroyed brick at the end of the list
        _entities.erase(
            remove_if(begin(_entities), end(_entities),
                [] (const std::unique_ptr<Entity> &mEntity) {
            return !mEntity->isAlive();
        }),
            end(_entities));
    }

    ECS::Entity & Manager::addEntity()
    {
        std::unique_ptr<Entity> uPtr = std::make_unique<Entity>(*this);
        _entities.emplace_back(move(uPtr));

        return *_entities.back();
    }
} // namespace ECS