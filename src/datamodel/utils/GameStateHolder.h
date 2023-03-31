#pragma once

#include "SpawnPositionAnalyser.h"

class GameStateHolder {
    public:
    GameStateHolder() {
        _playerId = 0;
        _map = NULL;
        _scriptPool = NULL;
        _mainTaskInfo = -1;
        _waypointIndicator = Vector2D();
        _waypointIndicatorEnabled = false;
    }
    virtual ~GameStateHolder() {
        clearData();
    }

    inline int getPlayerId() {
        return _playerId;
    }

    inline EntityObject* getPlayer() {
        return getEntity( _playerId );
    }

    inline void setPlayer( int playerId ) {
        _playerId = playerId;
    }

    static void deleteMap( Map** mPtr ) {
        Map* m = ( *mPtr );
        if ( m ) {
            SpawnPositionAnalyser* analyser = m -> getAnalyser();
            if ( analyser ) {
                delete analyser;
            }
            delete m;
            ( *mPtr ) = NULL;
        }
    }

    void clearData() {
        deleteMap( &_map );
        setActiveScriptPool( NULL );
        printf( "Cleared map.\n" );
        _clearInteractiveMap( _entities );
        printf( "Cleared entities.\n" );
        _clearInteractiveMap( _pickups );
        printf( "Cleared pickups.\n" );
        _clearInteractiveMap( _bullets );
        printf( "Cleared bullets.\n" );
        _clearInteractiveMap( _particles );
        printf( "Cleared particles.\n" );
    }

    inline Map* getMap() {
        return _map;
    }

    inline ScriptPool* getActiveScriptPool() {
        return _scriptPool;
    }

    inline void setActiveScriptPool( ScriptPool* pool ) {
        if ( _scriptPool ) {
            delete _scriptPool;
            _scriptPool = NULL;
        }
        _scriptPool = pool;
    }

    inline void setMap( Map* currentMap ) {
        deleteMap( &_map );
        _map = currentMap;
    }

    inline bool add( EntityObject* entityObject ) {
        return _addInteractive( entityObject, _entities );
    }

    inline EntityObject* getEntity( int id ) {
        return _findInteractive( id, _entities );
    }

    inline EntityObject* removeEntity( int id ) {
        return _removeInteractive( id, _entities );
    }

    inline int nextEntityId() {
        return _nextId( _entities );
    }

    inline int countEntity() {
        return _storedCount( _entities );
    }

    inline bool add( PickupObject* pickupObject ) {
        return _addInteractive( pickupObject, _pickups );
    }

    inline PickupObject* getPickup( int id ) {
        return _findInteractive( id, _pickups );
    }

    inline PickupObject* removePickup( int id ) {
        return _removeInteractive( id, _pickups );
    }

    inline int nextPickupId() {
        return _nextId( _pickups );
    }

    inline int countPickup() {
        return _storedCount( _pickups );
    }

    inline bool add( BulletObject* bulletObject ) {
        return _addInteractive( bulletObject, _bullets );
    }

    inline BulletObject* getBullet( int id ) {
        return _findInteractive( id, _bullets );
    }

    inline BulletObject* removeBullet( int id ) {
        return _removeInteractive( id, _bullets );
    }

    inline int nextBulletId() {
        return _nextId( _bullets );
    }

    inline int countBullet() {
        return _storedCount( _bullets );
    }

    inline bool add( ParticleObject* particleObject ) {
        return _addInteractive( particleObject, _particles );
    }

    inline ParticleObject* getParticle( int id ) {
        return _findInteractive( id, _particles );
    }

    inline ParticleObject* removeParticle( int id ) {
        return _removeInteractive( id, _particles );
    }

    inline int nextParticleId() {
        return _nextId( _particles );
    }

    inline int countParticle() {
        return _storedCount( _particles );
    }

    template < class T > class Container {
        public:
        Container( map < int, T* > * sourceMap ) {
            _sourceMap = sourceMap;
            _iterator = _sourceMap -> begin();
        }
        virtual ~Container() {
        }

        inline bool hasMore() {
            return _iterator != _sourceMap -> end();
        }

        inline T* nextObject() {
            if ( _iterator != _sourceMap -> end() ) {
                _iteratorShadow = _iterator;
                T* object = _iterator -> second;
                _iterator++;
                return object;
            }
            return NULL;
        }

        inline void reset() {
            _iterator = _sourceMap -> begin();
            _iteratorShadow = _sourceMap -> end();
        }

        inline bool startFrom( int id ) {
            _iterator = _sourceMap -> find( id );
            _iteratorShadow = _sourceMap -> end();
            return _iterator != _sourceMap -> end();
        }

        inline bool startAfter( int id ) {
            _iterator = _sourceMap -> find( id );
            _iteratorShadow = _sourceMap -> end();
            if ( _iterator != _sourceMap -> end() ) {
                _iterator++;
            }
            return _iterator != _sourceMap -> end();
        }

        inline void over() {
            _iterator = _sourceMap -> end();
            _iteratorShadow = _sourceMap -> end();
        }

        inline T* removeObject() {
            if ( _iteratorShadow != _sourceMap -> end() ) {
                typename map < int, T* >::iterator memory = _iteratorShadow;
                T* object = memory -> second;
                _sourceMap -> erase( memory );
                return object;
            }
            return NULL;
        }

        inline bool hasObject( int id ) {
            typename map < int, T* >::iterator memory = _sourceMap -> find( id );
            if ( memory != _sourceMap -> end() ) {
                return true;
            }
            return false;
        }

        private:
        typename map < int, T* >::iterator _iterator;
        typename map < int, T* >::iterator _iteratorShadow;
        map < int, T* > * _sourceMap;

    };

    Container < EntityObject > getEntityContainer() {
        return Container < EntityObject > ( &_entities );
    }

    Container < PickupObject > getPickupContainer() {
        return Container < PickupObject > ( &_pickups );
    }

    Container < BulletObject > getBulletContainer() {
        return Container < BulletObject > ( &_bullets );
    }

    Container < ParticleObject > getParticleContainer() {
        return Container < ParticleObject > ( &_particles );
    }

    inline int getMainTaskInfo() {
        return _mainTaskInfo;
    }

    inline void setMainTaskInfo( int taskInfoId ) {
        _mainTaskInfo = taskInfoId;
    }

    inline Vector2D getWaypointIndicator() {
        return _waypointIndicator;
    }

    inline void setWaypointIndicator( Vector2D waypoint ) {
        _waypointIndicator = waypoint;
        _waypointIndicatorEnabled = true;
    }

    inline bool isWaypointIndicatorEnabled() {
        return _waypointIndicatorEnabled;
    }

    inline void setWaypointIndicatorEnabled( bool waypointEnabled ) {
        _waypointIndicatorEnabled = waypointEnabled;
    }

    protected:

    template < class T > bool _addInteractive( T* added, map < int, T* > & interactiveMap ) {
        if ( added ) {
            typename map < int, T* >::iterator i = interactiveMap.find( added -> getId() );
            if ( i == interactiveMap.end() ) {
                interactiveMap[ added -> getId() ] = added;
                return true;
            }
        }
        return false;
    }

    template < class T > T* _findInteractive( int id, map < int, T* > & interactiveMap ) {
        typename map < int, T* >::iterator i = interactiveMap.find( id );
        if ( i != interactiveMap.end() ) {
            return i -> second;
        }
        return NULL;
    }

    template < class T > T* _removeInteractive( int id, map < int, T* > & interactiveMap ) {
        typename map < int, T* >::iterator i = interactiveMap.find( id );
        if ( i != interactiveMap.end() ) {
            T* removed = i -> second;
            interactiveMap.erase( i );
            return removed;
        }
        return NULL;
    }

    template < class T > void _clearInteractiveMap( map < int, T* > & interactiveMap ) {
        for ( typename map < int, T* >::iterator i = interactiveMap.begin(); i != interactiveMap.end(); i++ ) {
            T* removed = i -> second;
            if ( removed ) {
                delete removed;
            }
        }
        interactiveMap.clear();
    }

    template < class T > int _storedCount( map < int, T* > & interactiveMap ) {
        return interactiveMap.size();
    }

    template < class T > int _nextId( map < int, T* > & interactiveMap ) {
        typename map < int, T* >::reverse_iterator i = interactiveMap.rbegin();
        if ( i != interactiveMap.rend() ) {
            return ( i -> first ) + 1;
        }
        return 0;
    }

    private:

    Map* _map;
    map < int, EntityObject* > _entities;
    map < int, PickupObject* > _pickups;
    map < int, BulletObject* > _bullets;
    map < int, ParticleObject* > _particles;
    int _playerId;

    int _mainTaskInfo;
    bool _waypointIndicatorEnabled;
    Vector2D _waypointIndicator;

    ScriptPool* _scriptPool;

};


