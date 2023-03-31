#pragma once

class ScriptLoadInstruction : public ScriptInstruction {
    public:

    class Factory : public ScriptInstructionFactory {
        public:
        Factory() : ScriptInstructionFactory() {}
        virtual ~Factory() {}
        virtual ScriptLoadInstruction* create() {
            return new ScriptLoadInstruction();
        }
    };

    ScriptLoadInstruction() : ScriptInstruction() {}
    virtual ~ScriptLoadInstruction() {}

    virtual void perform() {
        double scriptId = 0;
        ScriptThread* thread = getThread();
        if ( thread && getParameter( 0, &scriptId ) ) {
            ScriptPool* pool = thread -> getPool();
            if ( pool ) {
                ScriptThread* newThread = Cache::getFileAccessor() -> getScript( scriptId );
                if ( newThread ) {
                    pool -> addScriptThread( newThread );
                }
            }
            thread -> nextInstruction();
        }
    }

};


