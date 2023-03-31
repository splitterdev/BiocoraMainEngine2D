#pragma once

class GamePlayRobot {
    public:
    GamePlayRobot() {}
    virtual ~GamePlayRobot() {}

    virtual int onTick() = 0;

    private:
};


