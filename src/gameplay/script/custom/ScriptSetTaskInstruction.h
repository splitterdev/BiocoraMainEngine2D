#pragma once

class ScriptSetTaskInstruction : public ScriptInstruction {
    public:

    class Factory : public ScriptInstructionFactory {
        public:
        Factory() : ScriptInstructionFactory() {}
        virtual ~Factory() {}
        virtual ScriptSetTaskInstruction* create() {
            return new ScriptSetTaskInstruction();
        }
    };

    ScriptSetTaskInstruction() : ScriptInstruction() {}
    virtual ~ScriptSetTaskInstruction() {}

    virtual void perform() {
        double taskIdParamIndex = 0;
        ScriptThread* thread = getThread();
        if ( getParameter( 0, &taskIdParamIndex ) ) {
            Cache::getGameStateHolder() -> setMainTaskInfo( taskIdParamIndex );
            thread -> nextInstruction();
        }
    }

};




