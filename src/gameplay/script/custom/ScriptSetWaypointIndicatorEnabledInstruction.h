#pragma once

class ScriptSetWaypointIndicatorEnabledInstruction : public ScriptInstruction {
    public:

    class Factory : public ScriptInstructionFactory {
        public:
        Factory() : ScriptInstructionFactory() {}
        virtual ~Factory() {}
        virtual ScriptSetWaypointIndicatorEnabledInstruction* create() {
            return new ScriptSetWaypointIndicatorEnabledInstruction();
        }
    };

    ScriptSetWaypointIndicatorEnabledInstruction() : ScriptInstruction() {}
    virtual ~ScriptSetWaypointIndicatorEnabledInstruction() {}

    virtual void perform() {
        double waypointIndicatorEnabled = 0;
        ScriptThread* thread = getThread();
        if ( thread && getParameter( 0, &waypointIndicatorEnabled ) ) {
            Cache::getGameStateHolder() -> setWaypointIndicatorEnabled( waypointIndicatorEnabled >= 0.5 );
            thread -> nextInstruction();
        }
    }

};



