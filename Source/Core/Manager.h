#pragma once

#include <vector>
#include <memory>
#include "ECS.h"

namespace ECS
{
    class Entity;

    class Manager
    {
    private:
        std::vector<std::unique_ptr<Entity>> entities;

        // allow to register entities by groupID
        std::array<EntityList, maxGroups> groupedEntities;

    public:
        void update(float mFT);
        void draw();

        void addToGroup(Entity *mEntity, Group mGroup)
        {
            groupedEntities[mGroup].emplace_back(mEntity);
        }

        EntityList &getEntitiesByGroup(Group mGroup)
        {
            return groupedEntities[mGroup];
        }

        void refresh();

        Entity &addEntity();
    };
}