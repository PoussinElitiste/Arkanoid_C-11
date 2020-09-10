#pragma once

#include <cassert>
#include "Event.h"

namespace Event
{
    constexpr std::size_t maxPending { 16 };

    // Double Buffer :
    // ---------------
    // [---xxxxxxx---] <- MAX_PENDING
    //    ^      ^
    //    |      |
    //  _head  _tail

    template <class T>
        struct RingBuffer
    {
        using Delegate = std::function<void(const T &)>;

        std::array<T, maxPending> _pending;
        int _head = 0;
        int _tail = 0;

        void registerEvent(const T &message)
        {
            assert((_tail + 1) % maxPending != _head);

            // Add to the end of the list.
            _pending[_tail] = message;
            _tail = (_tail + 1) % maxPending;
        }

        void forEach(const Delegate &cb)
        {
            for (int i = _head; i != _tail; i = (i + 1) % maxPending)
            {
                cb(_pending[i]);
            }
        }

        T &front()
        {
            return _pending[buffer._head];
        }

        bool isEmpty() const
        {
            return _head == _tail;
        }

        void incrementHead()
        {
            // process to unregister event automatically
            _head = (_head + 1) % maxPending;
        }
    };

    /// TODO: to be update by eventSystem
    // dedicated to trigger a queue of events (FIFO) each frame
    // example: planning action queue (anim, vfx, sfx, etc. )
    class Queue
    {
    public:
        static void registerEvent(const CallBack &cb)
        {
            _buffer.registerEvent(Message { cb });
        }

        static void update()
        {
            if (_buffer.isEmpty()) return;
            _buffer.front().cb();
            _buffer.incrementHead();
        }

    private:
        static RingBuffer<Message> _buffer;
    };

    // dedicated to trigger a batch of events FIFO in 1 frame
    // example: schedule bootstrap
    class Broadcast
    {
    public:
        static void registerEvent(const CallBack &cb)
        {
            _buffer.registerEvent(Message { cb });
        }

        /// TODO: to be update by eventSystem
        static void update()
        {
            _buffer.forEach([&] (const Message & msg) {
                msg.cb();
                _buffer.incrementHead();
            });
        }

    private:
        static RingBuffer<Message> _buffer;
    };
}