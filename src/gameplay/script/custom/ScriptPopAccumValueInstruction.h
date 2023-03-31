#pragma once

class ScriptPopAccumValueInstruction : public ScriptInstruction {
    public:

    class Factory : public ScriptInstructionFactory {
        public:
        Factory() : ScriptInstructionFactory() {}
        virtual ~Factory() {}
        virtual ScriptPopAccumValueInstruction* create() {
            return new ScriptPopAccumValueInstruction();
        }
    };

    ScriptPopAccumValueInstruction() : ScriptInstruction() {}
    virtual ~ScriptPopAccumValueInstruction() {}

    virtual void perform() {
        ScriptThread* thread = getThread();
        if ( thread ) {
            thread -> setAccumulatorValue( thread -> popFromStack() );
            thread -> nextInstruction();
        }
    }

};

