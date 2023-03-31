#pragma once

class TimedTask {
    public:
    TimedTask( double time ) {
        if ( isnan( time ) ) {
            time = 0.0; // instant
        }
        _performTime = Cache::gameTime() + time;
    }
    virtual ~TimedTask() {
    }

    virtual void performTask() = 0;

    inline double getPerformTime() {
        return _performTime;
    }

    static void add( TimedTask* task ) {
        if ( task == NULL ) {
            return;
        }
        list < TimedTask* > samePlannedTasks;
        map < double, list < TimedTask* > >::iterator i = _timedTaskMap.find( task -> _performTime );
        if ( i != _timedTaskMap.end() ) {
            samePlannedTasks = i -> second;
        }
        samePlannedTasks.push_back( task );
        _timedTaskMap[ task -> _performTime ] = samePlannedTasks;
    }

    static void removeAll() {
        for ( map < double, list < TimedTask* > >::iterator i = _timedTaskMap.begin(); i != _timedTaskMap.end(); i++ ) {
            list < TimedTask* > tasks = i -> second;
            for ( list < TimedTask* >::iterator k = tasks.begin(); k != tasks.end(); k++ ) {
                delete ( *k );
            }
        }
        _timedTaskMap.clear();
    }

    static void performReadyTasks() {
        if ( _timedTaskMap.empty() ) {
            return;
        }
        double currentTime = Cache::gameTime();
        //printf( "CurrentTime = %lf\n", currentTime );
        map < double, list < TimedTask* > >::iterator i = _timedTaskMap.begin();
        while ( i -> first <= currentTime ) {
            map < double, list < TimedTask* > >::iterator j = i;
            i++;
            list < TimedTask* > tasks = j -> second;
            for ( list < TimedTask* >::iterator k = tasks.begin(); k != tasks.end(); k++ ) {
                TimedTask* task = ( *k );
                task -> performTask();
                delete task;
            }
            _timedTaskMap.erase( j );
            if ( i == _timedTaskMap.end() ) {
                break;
            }
        }
    }

    private:

    double _performTime;

    static map < double, list < TimedTask* > > _timedTaskMap;

};

map < double, list < TimedTask* > > TimedTask::_timedTaskMap;

#include "timedtask/MessageTimedTask.h"
