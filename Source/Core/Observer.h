#pragma once
#include <memory>
#include <unordered_map>
#include "Event.h"

namespace Event
{
    class Subject;

    struct SubjectHash
    {
        size_t operator()(const std::reference_wrapper<Subject>& key) const;
    };

    struct SubjectEqual
    {
        bool operator()(const std::reference_wrapper<Subject>& key,
            const std::reference_wrapper<Subject>& key2) const;
    };

    // dedicated to be a linked list
    class Observer
    {
        using SubjectRef = std::reference_wrapper<Subject>;
        using SubjectMap = std::unordered_map<SubjectRef, CallBack, SubjectHash, SubjectEqual>;

        friend class Subject;
    public:
        virtual ~Observer();
        
    private:
        
        SubjectMap _callbackMap;
        std::weak_ptr<Observer> _next;

        void process(Subject &reference)
        {
            auto it = _callbackMap.find(std::ref(reference));
            if (it != _callbackMap.end())
            {
                it->second();
            }
        }

        void clear(Subject &reference)
        {
            _callbackMap.erase(std::ref(reference));
            _next.reset();
        }
    };

    class Subject
    {
    public:
        virtual ~Subject()
        {
            clear();
        }
   
        void subscribe(Observer &observer, CallBack cb)
        {
            observer._callbackMap.emplace(std::make_pair(std::ref(*this), std::move(cb)));
            observer._next = std::move(_head);
            _head = std::make_shared<Observer>(observer);
        }

        void unsubscribe(Observer &observer)
        {
            if (auto pHead = _head.lock())
            {
                if (pHead.get() == &observer)
                {
                    _head = std::move(observer._next);
                    observer.clear(*this);
                    return;
                }

                auto it = pHead;
                while (it)
                {
                    auto pNext = it->_next.lock();
                    if (pNext)
                    {
                        if (pNext.get() == &observer)
                        {
                            it->_next = std::move(observer._next);
                            observer.clear(*this);
                            return;
                        }
                    }

                    it = pNext;
                }
            }
        }

        void operator()()
        {
            if (auto pHead = _head.lock())
            {
                auto it = pHead;
                while (it)
                {
                    it->process(*this);

                    it = it->_next.lock();
                }
            }
        }

        void clear()
        {
            if (auto pHead = _head.lock())
            {
                auto it = pHead;
                while (it)
                {
                    auto pNext = it->_next.lock();
                    if (pNext)
                        it->clear(*this);

                    it = pNext;
                }
            }

            _head.reset();
        }

    private:
        std::weak_ptr<Observer> _head;
    };
}