#pragma once

class ScriptSetWaypointIndicatorInstruction : public ScriptInstruction {
    public:

    class Factory : public ScriptInstructionFactory {
        public:
        Factory() : ScriptInstructionFactory() {}
        virtual ~Factory() {}
        virtual ScriptSetWaypointIndicatorInstruction* create() {
            return new ScriptSetWaypointIndicatorInstruction();
        }
    };

    ScriptSetWaypointIndicatorInstruction() : ScriptInstruction() {}
    virtual ~ScriptSetWaypointIndicatorInstruction() {}

    virtual void perform() {
        double waypointIndicatorX = 0;
        double waypointIndicatorY = 0;
        ScriptThread* thread = getThread();
        if ( thread && getParameter( 0, &waypointIndicatorX ), getParameter( 1, &waypointIndicatorY ) ) {
            Cache::getGameStateHolder() -> setWaypointIndicator( Vector2D( waypointIndicatorX, waypointIndicatorY ) );
            thread -> nextInstruction();
        }
    }

};



