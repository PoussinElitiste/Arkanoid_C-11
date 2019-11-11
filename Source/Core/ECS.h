#pragma once

#include <chrono>
#include <functional>
#include <iostream>
#include <bitset>
#include <array>
#include <assert.h>

using namespace std;

namespace ECS
{
	// EC implementations
	struct Component;
	class Entity;
	class Manager;

	using ComponenID = std::size_t;
	using Group = std::size_t;

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

	// max bit shift for 32 bit
	constexpr std::size_t maxComponents{ 32 };
	using ComponentBitset = std::bitset<maxComponents>;
	using ComponentArray = std::array<Component*, maxComponents>;

	constexpr std::size_t maxGroups{ 32 };
	using GroupBitset = std::bitset<maxGroups>;

	using EntityList = std::vector<Entity*>;

	struct Component
	{
		Entity *entityPtr{ nullptr };

		virtual void init() {}
		virtual void update(float) {}
		virtual void draw() {}
		virtual ~Component() {}
	};

	class Entity
	{
	private:
		Manager &manager;
		bool alive{ true };
		// warranty unique instance
		vector<unique_ptr<Component>> components;

		// keep a dictinnary to quick access component
		ComponentArray componentArray;

		// keep a flag table of component added -> unique component by entity
		ComponentBitset componentBitset;

		// used to define in shich group Entity is registered
		GroupBitset groupBitset;

	public:
		Entity(Manager &mManager) : manager(mManager) {}
		void update(float mFT) { for (auto &c : components) c->update(mFT); }
		void draw() { for (auto &c : components) c->draw(); }

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

			T* c(new T(forward<TArgs>(mArgs)...));

			c->entityPtr = this;

			// wrap into unique ptr to avoid memory leak with vector list
			unique_ptr<Component> uPtr{ c };

			// move is mandatory because unique_ptr cannot be copied
			components.emplace_back(move(uPtr));

			// register component -> could be 
			componentArray[getComponentTypeID<T>()] = c;
			componentBitset[getComponentTypeID<T>()] = true;

			c->init();
			return *c;
		}

		template<typename T>
		T &getComponent() const
		{
			assert(hasComponent<T>());
			auto ptr(componentArray[getComponentTypeID<T>()]);
			return *static_cast<T*>(ptr);
		}
	};

	class Manager
	{
	private:
		vector<unique_ptr<Entity>> entities;

		// allow to register entities by groupID
		array<EntityList, maxGroups> groupedEntities;

	public:
		void update(float mFT) { for (auto &e : entities) e->update(mFT); }
		void draw() { for (auto &e : entities) e->draw(); }

		void addToGroup(Entity *mEntity, Group mGroup)
		{
			groupedEntities[mGroup].emplace_back(mEntity);
		}

		EntityList &getEntitiesByGroup(Group mGroup)
		{
			return groupedEntities[mGroup];
		}

		void refresh()
		{
			// remove from group first
			for (auto i{ 0u }; i < maxGroups; ++i)
			{
				auto &v(groupedEntities[i]);
				v.erase(
					remove_if(begin(v), end(v),
						[i](Entity *mEntity)
				{
					return !mEntity->isAlive() || !mEntity->hasGroup(i);
				}),
					end(v));
			}

			// Note: remove_if sort list and push all destroyed brick at the end of the list
			entities.erase(
				remove_if(begin(entities), end(entities),
					[](const unique_ptr<Entity> &mEntity)
			{
				return !mEntity->isAlive();
			}),
				end(entities));
		}

		Entity &addEntity()
		{
			Entity *e{ new Entity{*this} };
			// TODO: use DIP injection
			unique_ptr<Entity> uPtr{ e };
			entities.emplace_back(move(uPtr));

			return *e;
		}
	};

	// need to be defined after Manager
	void Entity::addGroup(Group mGroup) noexcept
	{
		groupBitset[mGroup] = true;
		manager.addToGroup(this, mGroup);
	}
}