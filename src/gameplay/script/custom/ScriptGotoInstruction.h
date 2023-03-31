#pragma once

class ScriptGotoInstruction : public ScriptInstruction {
    public:

    class Factory : public ScriptInstructionFactory {
        public:
        Factory() : ScriptInstructionFactory() {}
        virtual ~Factory() {}
        virtual ScriptGotoInstruction* create() {
            return new ScriptGotoInstruction();
        }
    };

    ScriptGotoInstruction() : ScriptInstruction() {}
    virtual ~ScriptGotoInstruction() {}

    virtual void perform() {
        double jumpIndex = 0;
        ScriptThread* thread = getThread();
        if ( thread && getParameter( 0, &jumpIndex ) ) {
            thread -> jumpToInstruction( jumpIndex );
        }
    }

};

