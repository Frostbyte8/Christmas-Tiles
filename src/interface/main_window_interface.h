#pragma once

class MainWindowInterface {
    public:
        virtual void implDisplayTestMessage() = 0;
        virtual void implGameStateChanged(const int& gameState) = 0;
};