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
        std::vector<std::unique_ptr<Entity>> _entities;

        // allow to register entities by groupID
        std::array<EntityList, maxGroups> _groupedEntities;

    public:
        void update(float mFT);
        void draw();

        void addToGroup(std::weak_ptr<Entity> mEntity, Group mGroup);
        EntityList &getEntitiesByGroup(Group mGroup);

        void refresh();

        Entity &addEntity();
    };
}