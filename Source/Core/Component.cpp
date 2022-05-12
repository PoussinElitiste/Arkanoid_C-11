
#include "Component.h"

namespace ECS
{
    Component::Component(Entity& entity) 
        : _entity(entity) {}
}