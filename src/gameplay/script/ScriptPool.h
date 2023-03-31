#pragma once

class ScriptPool {
    public:
    ScriptPool() {
    }
    virtual ~ScriptPool() {
        clearData();
    }

    void addScriptThread( ScriptThread* thread ) {
        if ( thread ) {
            if ( !thread -> _pool ) {
                thread -> _pool = this;
                _threads.push_back( thread );
            }
        }
    }

    void executeTick() {
        list < ScriptThread* > activeThreads;
        for ( list < ScriptThread* >::iterator i = _threads.begin(); i != _threads.end(); i++ ) {
            ScriptThread* thread = ( *i );
            if ( thread ) {
                if ( thread -> isDone() ) {
                    _recentlyEndedThreadIds.push_back( thread -> getId() );
                    delete thread;
                    continue;
                }
                activeThreads.push_back( thread );
                int index = 0;
                do {
                    ScriptInstruction* instruction = thread -> getCurrentInstruction();
                    if ( !instruction ) {
                        break;
                    }
                    index = thread -> getCurrentIndex();
                    instruction -> perform();
                } while ( index != thread -> getCurrentIndex() );
            }
        }
        _threads = activeThreads;
    }

    void clearData() {
        for ( list < ScriptThread* >::iterator i = _threads.begin(); i != _threads.end(); i++ ) {
            ScriptThread* thread = ( *i );
            if ( thread ) {
                delete thread;
            }
        }
        _threads.clear();
    }

    inline list < ScriptThread* > getThreadList() {
        return _threads;
    }

    inline list < int > getRecentlyEndedThreads() {
        return _recentlyEndedThreadIds;
    }

    inline void clearRecentlyEndedThreads() {
        _recentlyEndedThreadIds.clear();
    }

    private:

    list < ScriptThread* > _threads;
    list < int > _recentlyEndedThreadIds;

};


