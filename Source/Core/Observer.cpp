#pragma once

#include "Observer.h"

namespace Event
{
    Observer::~Observer()
    {
        for (const auto& entry : _callbackMap)
        {
            entry.first.get().unsubscribe(*this);
        }

        _callbackMap.clear();
    }

    size_t SubjectHash::operator()(const std::reference_wrapper<Subject>& key) const
    {
        return std::hash<Subject*>()(&key.get());
    }

    /// We check here the equality of the reference and not the object's value itself
    bool SubjectEqual::operator()(const std::reference_wrapper<Subject>& key, const std::reference_wrapper<Subject>& key2) const
    {
        return &key.get() == &key2.get();
    }
}