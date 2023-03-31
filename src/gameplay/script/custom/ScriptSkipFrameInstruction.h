#pragma once

class ScriptSkipFrameInstruction : public ScriptInstruction {
    public:

    class Factory : public ScriptInstructionFactory {
        public:
        Factory() : ScriptInstructionFactory() {}
        virtual ~Factory() {}
        virtual ScriptSkipFrameInstruction* create() {
            return new ScriptSkipFrameInstruction();
        }
    };

    ScriptSkipFrameInstruction() : ScriptInstruction() {
        _frameSkipped = false;
    }
    virtual ~ScriptSkipFrameInstruction() {}

    virtual void perform() {
        ScriptThread* thread = getThread();
        if ( thread ) {
            if ( _frameSkipped ) {
                _frameSkipped = false;
                thread -> nextInstruction();
            } else {
                _frameSkipped = true;
            }
        }
    }

    private:
    bool _frameSkipped;

};
