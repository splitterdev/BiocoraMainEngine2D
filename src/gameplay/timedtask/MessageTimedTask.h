#pragma once

class MessageTimedTask : public TimedTask {
    public:
    MessageTimedTask( int priorityLevel, string message, double time ) : TimedTask( time ) {
        _priorityLevel = priorityLevel;
        _message = message;
    }
    virtual ~MessageTimedTask() {}

    virtual void performTask() {
        if ( Cache::getGameStateHolder() -> getPlayer() ) {
            MessageManager::get() -> addMessage( _priorityLevel, _message );
        }
    }

    private:

    int _priorityLevel;
    string _message;

};

