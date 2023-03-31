#pragma once

class ScriptRunAndWaitDialogInstruction : public ScriptInstruction {
    public:

    class Factory : public ScriptInstructionFactory {
        public:
        Factory() : ScriptInstructionFactory() {}
        virtual ~Factory() {}
        virtual ScriptRunAndWaitDialogInstruction* create() {
            return new ScriptRunAndWaitDialogInstruction();
        }
    };

    ScriptRunAndWaitDialogInstruction() : ScriptInstruction() {
        _started = false;
    }
    virtual ~ScriptRunAndWaitDialogInstruction() {}

    virtual void perform() {
        double dialogId;
        ScriptThread* thread = getThread();
        if ( thread && getParameter( 0, &dialogId ) ) {
            if ( !isnan( dialogId ) ) {
                DialogReader::Dialog* dialog = DialogReader::getDialog( int( dialogId ) );
                if ( dialog ) {
                    if ( !_started ) {
                        dialog -> resetDialog();
                        _started = true;
                    }
                    if ( !dialog -> processDialog() ) {
                        return;
                    }
                }
            }
            _started = false;
            thread -> nextInstruction();
        }
    }

    private:
    bool _started;

};


