#pragma once

class ScriptMessageManagerInstruction : public ScriptInstruction {
    public:

    class Factory : public ScriptInstructionFactory {
        public:
        Factory() : ScriptInstructionFactory() {}
        virtual ~Factory() {}
        virtual ScriptMessageManagerInstruction* create() {
            return new ScriptMessageManagerInstruction();
        }
    };

    ScriptMessageManagerInstruction() : ScriptInstruction() {}
    virtual ~ScriptMessageManagerInstruction() {}

    virtual void perform() {
        double logLevel = 0;
        double messageId = 0;
        ScriptThread* thread = getThread();
        if ( thread && getParameter( 0, &logLevel ) && getParameter( 1, &messageId ) ) {
            MessageManager::get() -> addMessage( logLevel, LangStrings::getString( messageId ) );
            thread -> nextInstruction();
        }
    }

};


