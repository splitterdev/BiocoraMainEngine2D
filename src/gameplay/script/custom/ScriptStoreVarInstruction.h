#pragma once

class ScriptStoreVarInstruction : public ScriptInstruction {
    public:

    class Factory : public ScriptInstructionFactory {
        public:
        Factory() : ScriptInstructionFactory() {}
        virtual ~Factory() {}
        virtual ScriptStoreVarInstruction* create() {
            return new ScriptStoreVarInstruction();
        }
    };

    ScriptStoreVarInstruction() : ScriptInstruction() {}
    virtual ~ScriptStoreVarInstruction() {}

    virtual void perform() {
        double varKey = 0;
        ScriptThread* thread = getThread();
        if ( thread && getParameter( 0, &varKey ) ) {
            thread -> setVariable( varKey, thread -> getAccumulatorValue() );
            thread -> nextInstruction();
        }
    }

};


