#pragma once

class ScriptPushAccumValueInstruction : public ScriptInstruction {
    public:

    class Factory : public ScriptInstructionFactory {
        public:
        Factory() : ScriptInstructionFactory() {}
        virtual ~Factory() {}
        virtual ScriptPushAccumValueInstruction* create() {
            return new ScriptPushAccumValueInstruction();
        }
    };

    ScriptPushAccumValueInstruction() : ScriptInstruction() {}
    virtual ~ScriptPushAccumValueInstruction() {}

    virtual void perform() {
        ScriptThread* thread = getThread();
        if ( thread ) {
            thread -> pushToStack( thread -> getAccumulatorValue() );
            thread -> nextInstruction();
        }
    }

};

