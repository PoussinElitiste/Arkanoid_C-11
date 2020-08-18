#pragma once

#include <vector>
#include <bitset> 
#include <array>
#include <memory>
#include <cassert>
#include "ECS.h"
#include "Component.h"

namespace ECS
{
    class Manager;
    struct Component;
    
    class Entity
    {
    private:
        Manager &manager;
        bool alive { true };
        // warranty unique instance
        std::vector<std::unique_ptr<Component>> _components;
    
        // keep a dictionary to quick access component
        ComponentArray componentArray;
    
        // keep a flag table of component added -> unique component by entity
        ComponentBitset componentBitset;
    
        // used to define in which group Entity is registered
        GroupBitset groupBitset;
    
    public:
        Entity(Manager &mManager) : manager(mManager) {}
        void update(float mFT);
        void draw();
    
        bool isAlive() const { return alive; }
        void destroy() { alive = false; }
    
        template<typename T> bool hasComponent() const
        {
            return componentBitset[getComponentTypeID<T>()];
        }
    
        bool hasGroup(Group mGroup) const noexcept
        {
            return groupBitset[mGroup];
        }
    
        // will register Entity group in manager
        // note: defined after manager class
        void addGroup(Group mGroup) noexcept;
        void delGroup(Group mGroup) noexcept
        {
            // will be used by manager during refresh
            groupBitset[mGroup] = false;
        }
    
        template<typename T, typename... TArgs>
        T& addComponent(TArgs &&... mArgs)
        {
            // only one component type by entity 
            assert(!hasComponent<T>());
    
            // TODO: use DIP injection to automate entity reference
            std::unique_ptr<T> component = std::make_unique<T>(std::forward<TArgs>(mArgs)...);
    
            // register component -> could be 
            componentArray[getComponentTypeID<T>()] = component.get();
            componentBitset[getComponentTypeID<T>()] = true;
    
            component->init();
            auto &refComponent = *component;
    
            // move is mandatory because unique_ptr cannot be copied
            _components.emplace_back(std::move(component));
    
            return refComponent;
        }
    
        template<typename T>
        T &getComponent() const
        {
            assert(hasComponent<T>());
            return *static_cast<T*>(componentArray[getComponentTypeID<T>()]);
        }
    };
}
