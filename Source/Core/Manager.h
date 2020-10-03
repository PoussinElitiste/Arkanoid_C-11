#pragma once

#include <vector>
#include <memory>
#include "ECS.h"

namespace ECS
{
    /// TODO: should be renamed Facade or Service locator
    class Manager
    {
    private:
        /// TODO: move to entitySystem
        std::vector<std::unique_ptr<Entity>> _entities;
        // allow to register entities by groupID
        std::array<EntityList, maxGroups> _groupedEntities;

        /// not used fore the moment 
        /// TODO: use injection throw builder to inject system to each components or entities on load and define bootstrap order
        /// - create RenderSystem to draw
        /// - create eventSystem to trigger event/broadcast
        /// - create entityManager to create, changed and remove entity by GROUP during update process
        /// - create componentManager to create, changed and remove component by CATEGORY during update process
        //std::vector<std::unique_ptr<System>> _systems; 

    public:
        ~Manager();
        void update(float mFT);
        void draw();

        void addToGroup(Entity *entity, Group group);
        EntityList &getEntitiesByGroup(Group group);

        void refresh();

        Entity &addEntity();
    };
}