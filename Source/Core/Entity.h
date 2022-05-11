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
        Manager &_manager;
        bool alive { true };
        // warranty unique instance
        std::vector<std::unique_ptr<Component>> _components;
    
        // keep a dictionary to quick access component
        ComponentArray _cachedComponents = {};
    
        // keep a flag table of component added -> unique component by entity
        ComponentBitset _componentBitset;
    
        // used to define in which group Entity is registered
        GroupBitset _groupBitset;
    
    public:
        Entity(Manager& mManager) : _manager(mManager) {}
        void update(float mFT);
        void draw();
    
        bool isAlive() const { return alive; }
        void destroy() { alive = false; }
    
        template<typename T> bool hasComponent() const
        {
            return _componentBitset[getComponentTypeID<T>()];
        }
    
        bool hasGroup(Group mGroup) const noexcept
        {
            return _groupBitset[mGroup];
        }
    
        // will register Entity group in manager
        void addGroup(Group mGroup) noexcept;
        void delGroup(Group mGroup) noexcept;
    
        template<typename T, typename... TArgs>
        T& addComponent(TArgs &&... mArgs)
        {
            // only one component type by entity 
            assert(!hasComponent<T>());
    
            // TODO: use DIP injection to automate entity reference
            std::unique_ptr<T> componentPtr = std::make_unique<T>(std::forward<TArgs>(mArgs)...);
    
            // register cache component for fast access
            _cachedComponents[getComponentTypeID<T>()] = componentPtr.get();
            _componentBitset[getComponentTypeID<T>()] = true;
            componentPtr->init();

            // move is mandatory because unique_ptr cannot be copied
            _components.emplace_back(std::move(componentPtr));
            componentPtr = nullptr;
            
            return getComponent<T>();
        }

        template<typename T>
        T& getComponent() const
        {
            assert(hasComponent<T>());
            return static_cast<T&>(*_cachedComponents[getComponentTypeID<T>()]);
        }
    };
}
