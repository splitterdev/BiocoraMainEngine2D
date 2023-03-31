#pragma once

class ScriptGenerateExplosionInstruction : public ScriptInstruction {
    public:

    class Factory : public ScriptInstructionFactory {
        public:
        Factory() : ScriptInstructionFactory() {}
        virtual ~Factory() {}
        virtual ScriptGenerateExplosionInstruction* create() {
            return new ScriptGenerateExplosionInstruction();
        }
    };

    ScriptGenerateExplosionInstruction() : ScriptInstruction() {}
    virtual ~ScriptGenerateExplosionInstruction() {}

    virtual void perform() {
        Vector2D position;
        double explosionId;
        ScriptThread* thread = getThread();
        if ( thread && getParameter( 0, &position.x ) && getParameter( 1, &position.y ) && getParameter( 2, &explosionId ) ) {
            AttackHelper::createExplosion( ( int ) explosionId, position );
            thread -> nextInstruction();
        }
    }

};
