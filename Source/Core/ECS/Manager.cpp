
#include "Manager.h"

#include "System.h"
#include "Entity.h"

namespace ECS
{
    Manager::~Manager()
    = default;

    void Manager::Update(float mFT)
    {
        for (auto &e : _entities) 
            e->Update(mFT);
    }

    void Manager::Draw()
    {
        for (auto &e : _entities) e->Draw();
    }

    void Manager::addToGroup(Entity * entity, Group group)
    {
        _groupedEntities[group].emplace_back(entity);
    }

    ECS::EntityList& Manager::getEntitiesByGroup(Group group)
    {
        return _groupedEntities[group];
    }

    void Manager::refresh()
    {
        // remove from group first
        for (auto i { 0u }; i < maxGroups; ++i)
        {
            auto &v(_groupedEntities[i]);
            v.erase(
                remove_if(begin(v), end(v),
                    [i] (Entity *entity) 
                {
                    //if (auto ptr = entity.lock())
                    {
                        return !entity->isAlive() || !entity->hasGroup(i);
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
        std::unique_ptr<Entity> entityPtr = std::make_unique<Entity>(*this);
        ECS::Entity& entity = *entityPtr.get();
        _entities.emplace_back(std::move(entityPtr));

        return entity;
    }
} // namespace ECS