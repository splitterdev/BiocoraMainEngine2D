#pragma once

#include "../TimedTask.h"

class TimedTaskRobot : public GamePlayRobot {
    public:
    TimedTaskRobot() : GamePlayRobot() {
    }
    virtual ~TimedTaskRobot() {}

    virtual int onTick() {
        TimedTask::performReadyTasks();
        return GameStateProcessor::EVENT_RESULT_NONE;
    }

};

