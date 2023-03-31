#pragma once

class LogicCallResult {
    public:
    LogicCallResult() {
        DELETE_CALLER = false;
        NOT_FOUND = false;
        SUICIDE = false;
        DAMAGEEFFECT = false;
        SHOOT = false;
        SPAWN = false;
        DAMAGESIREN = false;
        KILLPLAYER = false;
    }
    ~LogicCallResult() {
    }

    static LogicCallResult Empty() {
        return LogicCallResult();
    }

    static LogicCallResult NotFound() {
        LogicCallResult result;
        result.NOT_FOUND = true;
        return result;
    }

    static LogicCallResult suicide() {
        LogicCallResult result;
        result.SUICIDE = true;
        result.DELETE_CALLER = true;
        result.DAMAGEEFFECT = true;
        return result;
    }

    LogicCallResult killPlayer() {
        KILLPLAYER = true;
        return * this;
    }

    LogicCallResult shoot() {
        SHOOT = true;
        return * this;
    }

    LogicCallResult spawn() {
        SPAWN = true;
        return * this;
    }

    LogicCallResult removeCaller() {
        DELETE_CALLER = true;
        return * this;
    }

    LogicCallResult damageEffect() {
        DAMAGEEFFECT = true;
        return * this;
    }

    LogicCallResult damageSiren() {
        DAMAGESIREN = true;
        return * this;
    }

    bool DELETE_CALLER;
    bool NOT_FOUND;
    bool SUICIDE;
    bool DAMAGEEFFECT;
    bool SHOOT;
    bool SPAWN;
    bool DAMAGESIREN;
    bool KILLPLAYER;

};


