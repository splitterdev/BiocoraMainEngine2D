#pragma once

class ScriptSetAccumValueInstruction : public ScriptInstruction {
    public:

    class Factory : public ScriptInstructionFactory {
        public:
        Factory() : ScriptInstructionFactory() {}
        virtual ~Factory() {}
        virtual ScriptSetAccumValueInstruction* create() {
            return new ScriptSetAccumValueInstruction();
        }
    };

    ScriptSetAccumValueInstruction() : ScriptInstruction() {}
    virtual ~ScriptSetAccumValueInstruction() {}

    virtual void perform() {
        double value = 0;
        ScriptThread* thread = getThread();
        if ( thread && getParameter( 0, &value ) ) {
            thread -> setAccumulatorValue( value );
            thread -> nextInstruction();
        }
    }

};


