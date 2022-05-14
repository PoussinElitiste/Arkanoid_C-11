#include "Library.h"

namespace Core
{
    std::array<bool, (size_t)Input::NB_KEYS> Library::sPressedKeys{false};

    bool Library::KeyPressed(Input key)
    {
        return Library::sPressedKeys[(int)key];
    }
}