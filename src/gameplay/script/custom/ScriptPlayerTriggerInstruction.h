#pragma once

class ScriptPlayerTriggerInstruction : public ScriptInstruction {
    public:

    class Factory : public ScriptInstructionFactory {
        public:
        Factory() : ScriptInstructionFactory() {}
        virtual ~Factory() {}
        virtual ScriptPlayerTriggerInstruction* create() {
            return new ScriptPlayerTriggerInstruction();
        }
    };

    ScriptPlayerTriggerInstruction() : ScriptInstruction() {}
    virtual ~ScriptPlayerTriggerInstruction() {}

    virtual void perform() {
        double xPosition = 0;
        double yPosition = 0;
        double maxRange = 0;
        EntityObject* player = Cache::getGameStateHolder() -> getPlayer();
        ScriptThread* thread = getThread();
        if ( player && thread && getParameter( 0, &xPosition ) && getParameter( 1, &yPosition ) && getParameter( 2, &maxRange ) ) {
            if ( !isnan( xPosition ) && !isnan( yPosition ) ) {
                Vector2D trigger = Vector2D( xPosition, yPosition );
                if ( trigger.dist2D( player -> getPosition() ) <= maxRange ) {
                    thread -> nextInstruction();
                }
            }
        }
    }

};
