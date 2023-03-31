#pragma once

#include "EntityStatCalculator.h"

class PlayerManager {
    private:
    PlayerManager() {}
    virtual ~PlayerManager() {}

    static int _PROFILE_ID;

    public:

    static bool FORCE_CREATE_NEW;
    static string ROOT_PATH;

    static EntityObject* copyFromCharacter( int characterId, int assignedId ) {
        EntityType* type = Cache::getFileAccessor() -> getCharacter( characterId );
        if ( type ) {
            EntityObject* object = type -> createObject < EntityObject > ( assignedId );
            if ( object ) {
                EntityStatCalculator::calculateAllAttributesForObject( object );
                object -> setParameter( EntityType::ATTRIBUTE_HP_CURRENT, object -> getParameter( EntityType::ATTRIBUTE_HP_CURRENT_MAX ) );
                object -> setParameter( EntityType::ATTRIBUTE_ENERGY_CURRENT, object -> getParameter( EntityType::ATTRIBUTE_ENERGY_MAX ) );
            }
            InventoryHelper::restoreInventoryData( object );
            return object;
        }
        return NULL;
    }

    static bool overWriteCharacter( int characterId, EntityObject* playerObject ) {
        if ( playerObject ) {
            EntityType* type = Cache::getFileAccessor() -> getCharacter( characterId );
            if ( type ) {
                InventoryHelper::compressInventoryData( playerObject );
                type -> pullAttributes( playerObject );
                return true;
            }
        }
        return false;
    }

    static string getNewCharacterFileName() {
        long t = time( NULL );
        string s = StringUtils::toString( t );
        bool fileExists = true;
        string additionalCollisionNumber = "";
        int numInt = 0;
        string path;
        do {
            path = ROOT_PATH + s + additionalCollisionNumber + string( ".dat" );
            FILE* file = fopen( path.c_str(), "r" );
            if ( file ) {
                fclose( file );
                numInt++;
                additionalCollisionNumber = string( "_" ) + StringUtils::toString( numInt );
            } else {
                fileExists = false;
            }
        } while ( fileExists );
        return path;
    }

    static int createCharacter( int playerClassId ) {
        return createCharacter( playerClassId, getNewCharacterFileName() );
    }

    static int createCharacter( int playerClassId, string filePathToStore ) {
        if ( !FORCE_CREATE_NEW ) {
            FILE* check = fopen( filePathToStore.c_str(), "r" );
            if ( check ) {
                fclose( check );
                return -2;
            }
        }
        EntityType* playerClassType = Cache::getFileAccessor() -> getPlayerClass( playerClassId );
        if ( playerClassType ) {
            EntityType* character = new EntityType( -1 );
            if ( character ) {
                character -> pullAttributes( playerClassType );
                int addedId = Cache::getFileAccessor() -> addCharacter( character, filePathToStore );
                if ( addedId < 0 ) {
                    delete character;
                }
                return addedId;
            }
        }
        return -1;
    }

    inline static int getProfilesCount() {
        return Cache::getFileAccessor() -> getCharacterCount();
    }

    inline static int getProfileId() {
        return _PROFILE_ID;
    }

    inline static void setProfileId( int profileId ) {
        _PROFILE_ID = profileId;
    }

    static bool nextProfileId( int direction ) {
        int profileId = _PROFILE_ID + direction;
        EntityType* type = Cache::getFileAccessor() -> getCharacter( profileId );
        if ( type ) {
            _PROFILE_ID = profileId;
            return true;
        }
        return false;
    }

    static bool removeProfile( int profileId ) {
        map < int, EntityType* > characters = Cache::getFileAccessor() -> getAllCharacters();
        if ( characters.size() <= 1 ) {
            return false;
        }
        map < int, EntityType* >::iterator i = characters.find( profileId );
        if ( i != characters.end() ) {
            int toRemove = _PROFILE_ID;
            i++;
            if ( i != characters.end() ) {
                _PROFILE_ID = i -> first;
            } else {
                map < int, EntityType* >::reverse_iterator j = characters.rbegin();
                j--;
                if ( j != characters.rend() ) {
                    _PROFILE_ID = j -> first;
                }
            }
            Cache::getFileAccessor() -> removeCharacter( toRemove );
            return true;
        }
        return false;
    }

};

bool PlayerManager::FORCE_CREATE_NEW = true;
string PlayerManager::ROOT_PATH = string( "data/player/character/" );
int PlayerManager::_PROFILE_ID = 0;
