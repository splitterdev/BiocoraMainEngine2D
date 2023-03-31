#pragma once

class ScriptLoadVarInstruction : public ScriptInstruction {
    public:

    class Factory : public ScriptInstructionFactory {
        public:
        Factory() : ScriptInstructionFactory() {}
        virtual ~Factory() {}
        virtual ScriptLoadVarInstruction* create() {
            return new ScriptLoadVarInstruction();
        }
    };

    ScriptLoadVarInstruction() : ScriptInstruction() {}
    virtual ~ScriptLoadVarInstruction() {}

    virtual void perform() {
        double varKey = 0;
        ScriptThread* thread = getThread();
        if ( thread && getParameter( 0, &varKey ) ) {
            thread -> setAccumulatorValue( thread -> getVariable( varKey ) );
            thread -> nextInstruction();
        }
    }

};


