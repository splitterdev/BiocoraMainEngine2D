#pragma once

class ScriptSetQuestInstruction : public ScriptInstruction {
    public:

    class Factory : public ScriptInstructionFactory {
        public:
        Factory() : ScriptInstructionFactory() {}
        virtual ~Factory() {}
        virtual ScriptSetQuestInstruction* create() {
            return new ScriptSetQuestInstruction();
        }
    };

    ScriptSetQuestInstruction() : ScriptInstruction() {}
    virtual ~ScriptSetQuestInstruction() {}

    virtual void perform() {
        double questId = 0;
        ScriptThread* thread = getThread();
        if ( thread && getParameter( 0, &questId ) ) {
            QuestManager::getDefault() -> openQuest( questId );
            thread -> nextInstruction();
        }
    }

};
