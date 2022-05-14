#pragma once
#include <string>
#include <array>

namespace Core
{
    enum class Input
    {
        Space = 0,
        Up,
        Down,
        Left,
        Right,

        NB_KEYS
    };

    class Library
    {
    // Input
    private:
        static std::array<bool, (size_t)Input::NB_KEYS> sPressedKeys;
    public:
        virtual void RefreshInput() = 0;
        static bool KeyPressed(Input key);

    // Render
    public:
        virtual void CreateWindow(int width, int height, const std::string& title) = 0;
        virtual void ReleaseWindow() = 0;
        virtual void StartRender() {}
        virtual void EndRender() {}
        virtual void ClearBackground() = 0;
        virtual void DrawRectangle() = 0;
        virtual void DrawCircle() = 0;
    };
}