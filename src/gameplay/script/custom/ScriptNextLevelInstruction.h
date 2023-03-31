#pragma once

class ScriptNextLevelInstruction : public ScriptInstruction {
    public:

    class Factory : public ScriptInstructionFactory {
        public:
        Factory() : ScriptInstructionFactory() {}
        virtual ~Factory() {}
        virtual ScriptNextLevelInstruction* create() {
            return new ScriptNextLevelInstruction();
        }
    };

    ScriptNextLevelInstruction() : ScriptInstruction() {}
    virtual ~ScriptNextLevelInstruction() {}

    virtual void perform();

};
