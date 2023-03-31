#pragma once

#include "EventDelegate.h"

class EventDelegateQueue {
    public:
    EventDelegateQueue() {
    }
    virtual ~EventDelegateQueue() {
        for ( map < int, list < EventDelegate* > >::iterator i = _queues.begin(); i != _queues.end(); i++ ) {
            list < EventDelegate* > q = i -> second;
            for ( list < EventDelegate* >::iterator j = q.begin(); j != q.end(); j++ ) {
                delete ( *j );
            }
        }
    }

    bool addEvent( int queueId, EventDelegate* event ) {
        if ( event ) {
            map < int, list < EventDelegate* > >::iterator i = _queues.find( queueId );
            if ( i != _queues.end() ) {
                ( i -> second ).push_back( event );
            } else {
                list < EventDelegate* > newQueue;
                newQueue.push_back( event );
                _queues[ queueId ] = newQueue;
            }
            return true;
        }
        return false;
    }

    EventDelegate* getEvent( int queueId ) {
        map < int, list < EventDelegate* > >::iterator i = _queues.find( queueId );
        if ( i != _queues.end() ) {
            if ( !( i -> second ).empty() ) {
                EventDelegate* event = ( i -> second ).front();
                ( i -> second ).pop_front();
                return event;
            }
        }
        return NULL;
    }

    void clearQueue( int queueId ) {
        map < int, list < EventDelegate* > >::iterator i = _queues.find( queueId );
        if ( i != _queues.end() ) {
            list < EventDelegate* > q = i -> second;
            for ( list < EventDelegate* >::iterator j = q.begin(); j != q.end(); j++ ) {
                delete ( *j );
            }
        }
        _queues[ queueId ] = list < EventDelegate* >();
    }

    private:

    map < int, list < EventDelegate* > > _queues;

};


