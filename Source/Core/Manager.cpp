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

    void Manager::addToGroup(std::weak_ptr<Entity> mEntity, Group mGroup)
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
                    [i] (std::weak_ptr<Entity> entity) 
                {
                    if (auto ptr = entity.lock())
                    {
                        return !ptr->isAlive() || !ptr->hasGroup(i);
                    }
                    // cleanup null entry
					// TODO: add warning
                    return true;
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
        _entities.emplace_back(std::make_unique<Entity>(*this));

        return *_entities.back();
    }
} // namespace ECS