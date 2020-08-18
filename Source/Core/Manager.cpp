#include "Manager.h"
#include "Entity.h"

namespace ECS
{
    void Manager::update(float mFT)
    {
        for (auto &e : entities) 
            e->update(mFT);
    }

    void Manager::draw()
    {
        for (auto &e : entities) e->draw();
    }

    void Manager::refresh()
    {
        // remove from group first
        for (auto i { 0u }; i < maxGroups; ++i)
        {
            auto &v(groupedEntities[i]);
            v.erase(
                remove_if(begin(v), end(v),
                    [i] (Entity *mEntity) {
                return !mEntity->isAlive() || !mEntity->hasGroup(i);
            }),
                end(v));
        }

        // Note: remove_if sort list and push all destroyed brick at the end of the list
        entities.erase(
            remove_if(begin(entities), end(entities),
                [] (const std::unique_ptr<Entity> &mEntity) {
            return !mEntity->isAlive();
        }),
            end(entities));
    }

    ECS::Entity & Manager::addEntity()
    {
        //Entity *e{ new Entity{*this} };
        std::unique_ptr<Entity> uPtr = std::make_unique<Entity>(*this);
        entities.emplace_back(move(uPtr));

        //return *uPtr;
        return *entities.back();
    }
} // namespace ECS