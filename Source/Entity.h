#ifndef _ENTITY_H
#define _ENTITY_H


#include <vector>
using namespace std;
#include <array>
using namespace std;

class Component;

class Entity {
  private:
    vector<unique_ptr<Component>> components;

    array<Component *> componentArray;

    bitset<bool> componentBitset;

    constexpr size_t maxComponent;

};
#endif
