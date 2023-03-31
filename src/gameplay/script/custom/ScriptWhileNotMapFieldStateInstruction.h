#pragma once

class ScriptWhileNotMapFieldStateInstruction : public ScriptInstruction {
    public:

    class Factory : public ScriptInstructionFactory {
        public:
        Factory() : ScriptInstructionFactory() {}
        virtual ~Factory() {}
        virtual ScriptWhileNotMapFieldStateInstruction* create() {
            return new ScriptWhileNotMapFieldStateInstruction();
        }
    };

    ScriptWhileNotMapFieldStateInstruction() : ScriptInstruction() {}
    virtual ~ScriptWhileNotMapFieldStateInstruction() {}

    virtual void perform() {
        Vector2D position;
        double fieldId;
        ScriptThread* thread = getThread();
        if ( thread && getParameter( 0, &position.x ) && getParameter( 1, &position.y ) && getParameter( 2, &fieldId ) ) {
            MAP_FIELD_TYPE field;
            Map* currentMap = Cache::getGameStateHolder() -> getMap();
            if ( !currentMap ) {
                return;
            }
            if ( currentMap -> getField( position.x, position.y, field ) ) {
                if ( field == ( MAP_FIELD_TYPE ) fieldId ) {
                    thread -> nextInstruction();
                }
            }
        }
    }

};
