// Arkanoid_C++11.cpp : Ce fichier contient la fonction 'main'. 
// program execution start here
//
#include "pch.h"

#include "ArkanoidConfig.h"
#include "Arkanoid_ECS.h"
//#include "Arkanoid_Classic.h"
#include <vector>
#include <iostream>
#include <cassert>
#include <algorithm>

constexpr int MAX_PENDING = 16;

using EventId = unsigned int;

struct EventMessage
{
	EventId id;
	int value;
};

class EventBroadcast
{
public:
	static void init()
	{
		_pending.reserve(MAX_PENDING);
		_numPending = 0;
	}

	void queueEvent(EventId id, int volume)
	{
		assert(_numPending < MAX_PENDING);

		_pending[_numPending].id = id;
		_pending[_numPending].value = volume;
		_numPending++;
	}

	// broadcast events
	static void update()
	{
		for (auto &entry : _pending)
		{
			processEvent(entry);
		}

		_numPending = 0;
	}

private:
	static std::vector<EventMessage> _pending;
	static int _numPending;

private:
	static void processEvent(const EventMessage &evtInfos)
	{
		std::cout << "id :" << evtInfos.id << std::endl;
	}
};

// use ring buffer
// [---xxxxxxxxx---] <- MAX_PENDING
//     ^        ^
//     |        |
//   _head    _tail
class EventQueue
{
public:
	static void init()
	{
		_pending.reserve(MAX_PENDING);
		_head = 0;
		_tail = 0;
	}

	static void queueEvent(EventId id, int value, bool unique = true)
	{
		assert((_tail + 1) % MAX_PENDING != _head);

		if (unique)
		{
			// prevent double events
			for (int i = _head; i != _tail; i = (i + 1) % MAX_PENDING)
			{
				if (_pending[i].id == id)
				{
					_pending[_tail].value = std::max(value, _pending[i].value);
					return;
				}
			}
		}

		_pending[_tail].id = id;
		_pending[_tail].value = value;
		_tail = (_tail + 1) % MAX_PENDING;
	}

	// process event by update
	static void update()
	{
		if (_head == _tail) return;
		
		processEvent(_pending[_head]);

		_head = (_head + 1) % MAX_PENDING;
	}

private:
	static std::vector<EventMessage> _pending;
	static int _head;
	static int _tail;

private:
	static void processEvent(const EventMessage &evtInfos)
	{
		std::cout << "id :" << evtInfos.id << std::endl;
	}
};

// Main
//------
using namespace Arkanoid;

int main(int argc, char *argv[])
{
	if (argc < 2) 
	{
		// report version
		std::cout << argv[0] << " Version " << Arkanoid_VERSION_MAJOR << "."
              << Arkanoid_VERSION_MINOR << std::endl;
		std::cout << "Usage: " << argv[0] << " number" << std::endl;

		Game_v2{}.run();
		//Game{}.run();

		return 1;
	}

	return 0;
}