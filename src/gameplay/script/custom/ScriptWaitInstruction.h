#pragma once

class ScriptWaitInstruction : public ScriptInstruction {
    public:

    class Factory : public ScriptInstructionFactory {
        public:
        Factory() : ScriptInstructionFactory() {}
        virtual ~Factory() {}
        virtual ScriptWaitInstruction* create() {
            return new ScriptWaitInstruction();
        }
    };

    ScriptWaitInstruction() : ScriptInstruction() {
        _startTime = NAN;
    }
    virtual ~ScriptWaitInstruction() {}

    virtual void perform() {
        double waitTime = 0;
        ScriptThread* thread = getThread();
        if ( thread && getParameter( 0, &waitTime ) ) {
            if ( isnan( _startTime ) ) {
                _startTime = Cache::gameTime();
            } else {
                if ( _startTime + waitTime <= Cache::gameTime() ) {
                    _startTime = NAN;
                    thread -> nextInstruction();
                }
            }
        }
    }

    private:

    double _startTime;

};


