#pragma once

#include <chrono>
#include <functional>
#include <iostream>
#include <bitset>
#include <array>
#include <vector>

namespace ECS
{
    // EC implementations
    struct Component;
    class Entity;
    class System;
    class Manager;
    
    using ComponenID = std::size_t;
    using Group = std::size_t;

    // max bit shift for 32 bit
    constexpr std::size_t maxComponents { 32 };
    using ComponentBitset = std::bitset<maxComponents>;
    using ComponentArray = std::array<std::weak_ptr<Component>, maxComponents>;

    constexpr std::size_t maxGroups { 32 };
    using GroupBitset = std::bitset<maxGroups>;

    using EntityList = std::vector<std::weak_ptr<Entity>>;
    
    namespace Internal
    {
        inline ComponenID getUniqueComponentID() noexcept
        {
            static ComponenID lastID{ 0u };
            return lastID++;
        }
    }
    
    template<typename T> inline ComponenID getComponentTypeID() noexcept
    {
        static_assert(std::is_base_of<Component, T>::value, "T must be inherit from Component");
        
        // will auto generate an unique type ID at compile time
        // note: template will generate "several" getComponentTypeID by Class -> unique static typeID by Class
        static ComponenID typeID{ Internal::getUniqueComponentID() };
        return typeID;
    }
}