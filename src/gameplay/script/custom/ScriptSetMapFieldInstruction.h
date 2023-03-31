#pragma once

class ScriptSetMapFieldInstruction : public ScriptInstruction {
    public:

    class Factory : public ScriptInstructionFactory {
        public:
        Factory() : ScriptInstructionFactory() {}
        virtual ~Factory() {}
        virtual ScriptSetMapFieldInstruction* create() {
            return new ScriptSetMapFieldInstruction();
        }
    };

    ScriptSetMapFieldInstruction() : ScriptInstruction() {}
    virtual ~ScriptSetMapFieldInstruction() {}

    virtual void perform() {
        Vector2D position;
        double fieldId;
        ScriptThread* thread = getThread();
        if ( thread && getParameter( 0, &position.x ) && getParameter( 1, &position.y ) && getParameter( 2, &fieldId ) ) {
            MAP_FIELD_TYPE field = ( MAP_FIELD_TYPE ) fieldId;
            Map* currentMap = Cache::getGameStateHolder() -> getMap();
            if ( !currentMap ) {
                return;
            }
            currentMap -> beginUpdate();
            if ( currentMap -> setField( position.x, position.y, field ) ) {
                currentMap -> updateDisplayField( position.x, position.y, Cache::getFileAccessor() -> getTexture( ( int ) field ) -> getTexture() );
            }
            currentMap -> endUpdate();
            thread -> nextInstruction();
        }
    }

};

