#pragma once

#include "custom/SoldierLogic.h"

class LogicProcessorInitializer {
    private:
    LogicProcessorInitializer() {}
    ~LogicProcessorInitializer() {}
    public:
    static void InitProcessor() {
        LogicProcessor::addLogic( LOGIC_SOLDIER, new SoldierLogic() );
    }
};


