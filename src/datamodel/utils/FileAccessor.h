#pragma once

class FileAccessor {
    public:

    class FileStoreException : public Exception {
        public:
        FileStoreException( string s, vector < int > failedIds ) : Exception( _formatException( s, failedIds ) ) {
        }
        FileStoreException( const char* s, vector < int > failedIds ) : Exception( _formatException( string( s ), failedIds ) ) {
        }
        virtual ~FileStoreException() {
        }

        virtual string getClassName() {
            return "FileStoreException";
        }

        private:
        string _formatException( string s, vector < int > failedIds ) {
            string msg = "Failed [at " + s + "] with following ids:";
            for ( vector < int >::iterator i = failedIds.begin(); i != failedIds.end(); i++ ) {
                msg += " " + StringUtils::toString( *i );
            }
            return msg;
        }
    };


    FileAccessor() {
    }
    virtual ~FileAccessor() {
        store();
        clearStorage();
    }

    void load() {
        loadMaps( "data/map/Map.txt" );
        loadTextures( "data/textures/Textures.txt", "data/textures/MapTextures.txt" );
        loadSounds( "data/audio/Sound.txt" );
        loadSoundStreams( "data/audio/Stream.txt" );
        loadClasses( "data/gamedata/PlayerClass.txt" );
        loadCharacters( "data/player/Characters.txt" );
        loadEntities( "data/gamedata/Entity.txt" );
        loadWeapons( "data/gamedata/Weapon.txt" );
        loadArmors( "data/gamedata/Armor.txt" );
        loadPickups( "data/gamedata/Pickup.txt" );
        loadBullets( "data/gamedata/Bullet.txt" );
        loadParticles( "data/gamedata/Particle.txt" );
        loadSkills( "data/gamedata/Skill.txt" );
        loadBlockInfo( "data/gamedata/BlockInfo.txt" );
        loadExplosions( "data/gamedata/Explosion.txt" );
        loadSpawns( "data/gamedata/Spawn.txt" );
        loadChaptersDir( "data/map/chapter", ".dat" );
        loadScripts( "data/gamedata/Script.txt" );
        loadQuests( "data/gamedata/QuestConfig.dat" );
    }

    void store() {
        vector < int > failedIds = storeCharacters( "data/player/Characters.txt" );
        if ( !failedIds.empty() ) {
            throw FileStoreException( "storeCharacters", failedIds );
        }
    }

    void clearStorage() {
        removeTextures( _textures );
        removeSounds( _sounds );
        removeSounds( _soundStreams );
        removeData( _classes );
        removeData( _characters );
        removeData( _entities );
        removeData( _weapons );
        removeData( _armors );
        removeData( _pickups );
        removeData( _bullets );
        removeData( _particles );
        removeData( _explosions );
        removeData( _spawns );
        removeData( _chapters );
        removeData( _skills );
        removeData( _questMap );
    }

    static map < int, string > loadPaths( const char* path ) {
        return InteractivePathLoader::loadPaths( path );
    }

    void loadMaps( const char* path ) {
        _mapPaths = InteractivePathLoader::loadPaths( path );
    }

    void loadTextures( const char* path, const char* mapTexPath ) {
        map < int, string > paths = InteractivePathLoader::loadPaths( path );
        loadTextures( paths );
        map < int, string > mapTexPaths = InteractivePathLoader::loadPaths( mapTexPath );
        loadTextures( mapTexPaths );
        addMapTextureIds( mapTexPaths );
    }

    void loadTextures( map < int, string > paths ) {
        for ( map < int, string >::iterator i = paths.begin(); i != paths.end(); i++ ) {
            SDL_Surface* surface = SDL_LoadBMP( ( i -> second ).c_str() );
            if ( surface ) {
                SDL_SetColorKey( surface, SDL_TRUE, SDL_MapRGB( surface -> format, 255, 255, 255 ) );
                SDL_Texture* texture = SDL_CreateTextureFromSurface( Renderer::getRenderer(), surface );
                if ( texture ) {
                    _textures[ i -> first ] = new Texture( texture, surface -> w, surface -> h );
                }
                SDL_FreeSurface( surface );
            }
        }
    }

    void addMapTextureIds( map < int, string > paths ) {
        for ( map < int, string >::iterator i = paths.begin(); i != paths.end(); i++ ) {
            _mapTextureIds.push_back( i -> first );
        }
    }

    vector < int > getMapTextureIds() {
        return _mapTextureIds;
    }

    void loadSounds( const char* path ) {
        map < int, string > paths = InteractivePathLoader::loadPaths( path );
        for ( map < int, string >::iterator i = paths.begin(); i != paths.end(); i++ ) {
            _sounds[ i -> first ] = new Sound( i -> second, false );
        }
    }

    void loadSoundStreams( const char* path ) {
        map < int, string > paths = InteractivePathLoader::loadPaths( path );
        for ( map < int, string >::iterator i = paths.begin(); i != paths.end(); i++ ) {
            _soundStreams[ i -> first ] = new Sound( i -> second, true );
        }
    }

    void loadClasses( const char* path ) {
        map < int, string > paths = InteractivePathLoader::loadPaths( path );
        InteractiveLoader < EntityType, EntityTypeFileAttributeMapper > * loader = new InteractiveLoader < EntityType, EntityTypeFileAttributeMapper > ();
        _classes = loader -> loadMultipleFiles( paths );
        delete loader;
    }

    void loadCharacters( const char* path ) {
        map < int, string > paths = InteractivePathLoader::loadPaths( path );
        InteractiveLoader < EntityType, EntityTypeFileAttributeMapper > * loader = new InteractiveLoader < EntityType, EntityTypeFileAttributeMapper > ();
        _characters = loader -> loadMultipleFiles( paths );
        _characterPaths = paths;
        delete loader;
    }

    vector < int > storeCharacters( const char* path ) {
        InteractiveLoader < EntityType, EntityTypeFileAttributeMapper > * loader = new InteractiveLoader < EntityType, EntityTypeFileAttributeMapper > ();
        vector < int > failedIds = loader -> store( path, _characters, _characterPaths );
        delete loader;
        removeMarkedFiles();
        return failedIds;
    }

    void loadEntities( const char* path ) {
        map < int, string > paths = InteractivePathLoader::loadPaths( path );
        InteractiveLoader < EntityType, EntityTypeFileAttributeMapper > * loader = new InteractiveLoader < EntityType, EntityTypeFileAttributeMapper > ();
        _entities = loader -> loadMultipleFiles( paths );
        delete loader;
    }

    void loadWeapons( const char* path ) {
        map < int, string > paths = InteractivePathLoader::loadPaths( path );
        InteractiveLoader < WeaponType, WeaponTypeFileAttributeMapper > * loader = new InteractiveLoader < WeaponType, WeaponTypeFileAttributeMapper > ();
        _weapons = loader -> loadMultipleFiles( paths );
        delete loader;
    }

    void loadArmors( const char* path ) {
        map < int, string > paths = InteractivePathLoader::loadPaths( path );
        InteractiveLoader < ArmorType, ArmorTypeFileAttributeMapper > * loader = new InteractiveLoader < ArmorType, ArmorTypeFileAttributeMapper > ();
        _armors = loader -> loadMultipleFiles( paths );
        delete loader;
    }

    void loadPickups( const char* path ) {
        map < int, string > paths = InteractivePathLoader::loadPaths( path );
        InteractiveLoader < PickupType, PickupTypeFileAttributeMapper > * loader = new InteractiveLoader < PickupType, PickupTypeFileAttributeMapper > ();
        _pickups = loader -> loadMultipleFiles( paths );
        delete loader;
    }

    void loadBullets( const char* path ) {
        map < int, string > paths = InteractivePathLoader::loadPaths( path );
        InteractiveLoader < BulletType, BulletTypeFileAttributeMapper > * loader = new InteractiveLoader < BulletType, BulletTypeFileAttributeMapper > ();
        _bullets = loader -> loadMultipleFiles( paths );
        delete loader;
    }

    void loadParticles( const char* path ) {
        map < int, string > paths = InteractivePathLoader::loadPaths( path );
        InteractiveLoader < ParticleType, ParticleTypeFileAttributeMapper > * loader = new InteractiveLoader < ParticleType, ParticleTypeFileAttributeMapper > ();
        _particles = loader -> loadMultipleFiles( paths );
        delete loader;
    }

    void loadSkills( const char* path ) {
        map < int, string > paths = InteractivePathLoader::loadPaths( path );
        InteractiveLoader < SkillType, SkillTypeFileAttributeMapper > * loader = new InteractiveLoader < SkillType, SkillTypeFileAttributeMapper > ();
        _skills = loader -> loadMultipleFiles( paths );
        delete loader;
    }

    void loadBlockInfo( const char* path ) {
        map < int, string > paths = InteractivePathLoader::loadPaths( path );
        for ( map < int, string >::iterator i = paths.begin(); i != paths.end(); i++ ) {
            BlockInfo blockInfo;
            bool solid = false;
            bool transparent_view = false;
            bool transparent_bullet = false;
            bool destructible = false;
            bool glass = false;
            int destroyBlockId = -1;
            int destroyExplosionId = -1;
            double blockHp = NAN;
            vector < string > splits = StringUtils::split( i -> second, '|' );
            int splitSize = splits.size();
            for ( int j = 0; j < splitSize; j++ ) {
                string split = StringUtils::trimCopy( splits[ j ] );
                if ( strcasecmp( split.c_str(), "SOLID" ) == 0 ) {
                    solid = true;
                } else if ( strcasecmp( split.c_str(), "TRANSPARENT_VIEW" ) == 0 ) {
                    transparent_view = true;
                } else if ( strcasecmp( split.c_str(), "TRANSPARENT_BULLET" ) == 0 ) {
                    transparent_bullet = true;
                } else if ( strcasecmp( split.c_str(), "GLASS" ) == 0 ) {
                    glass = true;
                } else {
                    vector < string > subSplits = StringUtils::split( splits[ j ], ';' );
                    if ( subSplits.size() == 6 ) {
                        string subSplitId = StringUtils::trimCopy( subSplits[ 0 ] );
                        if ( strcasecmp( subSplitId.c_str(), "DESTRUCTIBLE" ) == 0 ) {
                            destructible = StringUtils::toInteger( subSplits[ 1 ], &destroyBlockId );
                            destructible &= StringUtils::toDouble( subSplits[ 2 ], &blockHp );
                            vector < string > soundIds = StringUtils::split( subSplits[ 3 ], ',' );
                            for ( int k = 0; k < ( int ) soundIds.size(); k++ ) {
                                int soundId;
                                if ( StringUtils::toInteger( soundIds[ k ], &soundId ) ) {
                                    blockInfo.addDestroySoundId( soundId );
                                }
                            }
                            vector < string > debrisConfigs = StringUtils::split( subSplits[ 4 ], ',' );
                            for ( int k = 0; k < ( int ) debrisConfigs.size(); k++ ) {
                                vector < string > debrisConfigSplits = StringUtils::split( debrisConfigs[ k ], '-' );
                                if ( debrisConfigSplits.size() == 3 ) {
                                    int debrisId;
                                    int minCount;
                                    int maxCount;
                                    if ( !StringUtils::toInteger( debrisConfigSplits[ 0 ], &debrisId ) ) {
                                        continue;
                                    }
                                    if ( !StringUtils::toInteger( debrisConfigSplits[ 1 ], &minCount ) ) {
                                        continue;
                                    }
                                    if ( !StringUtils::toInteger( debrisConfigSplits[ 2 ], &maxCount ) ) {
                                        continue;
                                    }
                                    blockInfo.addDebrisCount( debrisId, minCount, maxCount );
                                }
                            }
                            int explosionId;
                            if ( StringUtils::toInteger( subSplits[ 5 ], &explosionId ) ) {
                                destroyExplosionId = explosionId;
                            }
                        }
                    }
                }
            }
            blockInfo.setSolid( solid );
            blockInfo.setTransparentView( transparent_view );
            blockInfo.setTransparentBullet( transparent_bullet );
            blockInfo.setGlass( glass );
            blockInfo.setDestructible( destructible, destroyBlockId, destroyExplosionId, blockHp );
            _blockInfo[ i -> first ] = blockInfo;
        }
    }

    void loadExplosions( const char* path ) {
        map < int, string > paths = InteractivePathLoader::loadPaths( path );
        InteractiveLoader < ExplosionType, ExplosionTypeFileAttributeMapper > * loader = new InteractiveLoader < ExplosionType, ExplosionTypeFileAttributeMapper > ();
        _explosions = loader -> loadMultipleFiles( paths );
        delete loader;
    }

    void loadSpawns( const char* path ) {
        map < int, string > paths = InteractivePathLoader::loadPaths( path );
        for ( map < int, string >::iterator i = paths.begin(); i != paths.end(); i++ ) {
            try {
                SpawnMachine* machine = SpawnMachine::parse( i -> second );
                if ( machine ) {
                    _spawns[ i -> first ] = machine;
                }
            } catch ( Exception e ) {
                e.log();
            }
        }
    }

    void loadScripts( const char* path ) {
        map < int, string > paths = InteractivePathLoader::loadPaths( path );
        _scriptPaths = paths;
    }

    void loadQuests( const char* path ) {
        try {
            _questMap = Quest::parseFile( path );
        } catch ( ParseException e ) {
            printf( "ParseException during quest load: %s\n", e.cause().c_str() );
        }
    }

    Quest* getQuest( int questId ) {
        map < int, Quest* >::iterator i = _questMap.find( questId );
        if ( i != _questMap.end() ) {
            return i -> second;
        }
        return NULL;
    }

    map < int, Quest* > getQuests() {
        return _questMap;
    }

    void loadChaptersDir( const char* pathDir, const char* scan ) {
        list < string > paths = InteractivePathLoader::loadDir( pathDir, scan, true );
        InteractiveLoader < ChapterType, ChapterTypeFileAttributeMapper > * loader = new InteractiveLoader < ChapterType, ChapterTypeFileAttributeMapper > ();
        for ( list < string >::iterator i = paths.begin(); i != paths.end(); i++ ) {
            ChapterType* chapter = loader -> loadSingleUnassignedFile( ( *i ).c_str() );
            if ( chapter ) {
                chapter -> assignId( chapter -> getChapterId() );
                chapter -> setLoadedPath( *i );
                map < int, ChapterType* >::iterator f = _chapters.find( chapter -> getId() );
                if ( f != _chapters.end() ) {
                    if ( f -> second ) {
                        printf( "[ERROR] duplicate chapter ID: [%s]#%d <- [%s]#%d\n", ( f -> second -> getLoadedPath() ).c_str(), f -> first, ( chapter -> getLoadedPath() ).c_str(), chapter -> getId() );
                    } else {
                        printf( "[ERROR] chapter linking error [%s]#%d\n", ( chapter -> getLoadedPath() ).c_str(), chapter -> getId() );
                    }
                } else {
                    _chapters[ chapter -> getId() ] = chapter;
                }
            }
        }
        delete loader;
    }

    Map* getMap( int mapId ) {
        map < int, string >::iterator i = _mapPaths.find( mapId );
        if ( i != _mapPaths.end() ) {
            return Map::Loader::load( i -> second, _textures );
        }
        return NULL;
    }

    Texture* getTexture( int textureId ) {
        map < int, Texture* >::iterator i = _textures.find( textureId );
        if ( i != _textures.end() ) {
            return i -> second;
        }
        return NULL;
    }

    map < int, Texture* > getTextureSet() {
        return _textures;
    }

    Sound* getSound( int soundId ) {
        map < int, Sound* >::iterator i = _sounds.find( soundId );
        if ( i != _sounds.end() ) {
            return i -> second;
        }
        return NULL;
    }

    Sound* getSoundStream( int soundId ) {
        map < int, Sound* >::iterator i = _soundStreams.find( soundId );
        if ( i != _soundStreams.end() ) {
            return i -> second;
        }
        return NULL;
    }

    EntityType* getPlayerClass( int playerClassId ) {
        map < int, EntityType* >::iterator i = _classes.find( playerClassId );
        if ( i != _classes.end() ) {
            return i -> second;
        }
        return NULL;
    }

    map < int, EntityType* > getAllPlayerClasses() {
        return _classes;
    }

    EntityType* getCharacter( int characterId ) {
        map < int, EntityType* >::iterator i = _characters.find( characterId );
        if ( i != _characters.end() ) {
            return i -> second;
        }
        return NULL;
    }

    map < int, EntityType* > getAllCharacters() {
        return _characters;
    }

    inline int getCharacterCount() {
        return _characters.size();
    }

    inline bool removeCharacter( int characterId ) {
        map < int, EntityType* >::iterator i = _characters.find( characterId );
        map < int, string >::iterator j = _characterPaths.find( characterId );
        if ( i != _characters.end() && j != _characterPaths.end() ) {
            string filePath = j -> second;
            _characterPathsToRemove.push_back( filePath );
            delete i -> second;
            _characters.erase( i );
            _characterPaths.erase( j );
            return true;
        }
        return false;
    }

    inline void removeMarkedFiles() {
        for ( list < string >::iterator i = _characterPathsToRemove.begin(); i != _characterPathsToRemove.end(); i++ ) {
            removeFile( *i );
        }
        _characterPathsToRemove.clear();
    }

    inline bool removeFile( string path ) {
        return remove( path.c_str() ) == 0;
    }

    EntityType* getEntityType( int typeId ) {
        map < int, EntityType* >::iterator i = _entities.find( typeId );
        if ( i != _entities.end() ) {
            return i -> second;
        }
        return NULL;
    }

    WeaponType* getWeaponType( int typeId ) {
        map < int, WeaponType* >::iterator i = _weapons.find( typeId );
        if ( i != _weapons.end() ) {
            return i -> second;
        }
        return NULL;
    }

    ArmorType* getArmorType( int typeId ) {
        map < int, ArmorType* >::iterator i = _armors.find( typeId );
        if ( i != _armors.end() ) {
            return i -> second;
        }
        return NULL;
    }

    PickupType* getPickupType( int typeId ) {
        map < int, PickupType* >::iterator i = _pickups.find( typeId );
        if ( i != _pickups.end() ) {
            return i -> second;
        }
        return NULL;
    }

    BulletType* getBulletType( int typeId ) {
        map < int, BulletType* >::iterator i = _bullets.find( typeId );
        if ( i != _bullets.end() ) {
            return i -> second;
        }
        return NULL;
    }

    ParticleType* getParticleType( int typeId ) {
        map < int, ParticleType* >::iterator i = _particles.find( typeId );
        if ( i != _particles.end() ) {
            return i -> second;
        }
        return NULL;
    }

    SkillType* getSkillType( int typeId ) {
        map < int, SkillType* >::iterator i = _skills.find( typeId );
        if ( i != _skills.end() ) {
            return i -> second;
        }
        return NULL;
    }

    ScriptThread* getScript( int key ) {
        map < int, string >::iterator i = _scriptPaths.find( key );
        if ( i != _scriptPaths.end() ) {
            return ScriptParser::fromFile( i -> first, i -> second );
        }
        return NULL;
    }

    inline BlockInfo getBlockInfo( int blockId ) {
        static BlockInfo _NULL_INFO;
        map < int, BlockInfo >::iterator i = _blockInfo.find( blockId );
        if ( i != _blockInfo.end() ) {
            return i -> second;
        }
        return _NULL_INFO;
    }

    ExplosionType* getExplosionType( int explosionId ) {
        map < int, ExplosionType* >::iterator i = _explosions.find( explosionId );
        if ( i != _explosions.end() ) {
            return i -> second;
        }
        return NULL;
    }

    SpawnMachine* getSpawnMachine( int spawnId ) {
        map < int, SpawnMachine* >::iterator i = _spawns.find( spawnId );
        if ( i != _spawns.end() ) {
            return i -> second;
        }
        return NULL;
    }

    ChapterType* getChapter( int chapterId ) {
        map < int, ChapterType* >::iterator i = _chapters.find( chapterId );
        if ( i != _chapters.end() ) {
            return i -> second;
        }
        return NULL;
    }

    ChapterType* getChapterAfter( int chapterId ) {
        map < int, ChapterType* >::iterator i = _chapters.find( chapterId );
        if ( i != _chapters.end() ) {
            i++;
            if ( i != _chapters.end() ) {
                return i -> second;
            }
        }
        return NULL;
    }

    vector < ChapterType* > getAllChapters() {
        vector < ChapterType* > types;
        for ( map < int, ChapterType* >::iterator i = _chapters.begin(); i != _chapters.end(); i++ ) {
            types.push_back( i -> second );
        }
        return types;
    }

    int addCharacter( EntityType* character, string pathToStore ) {
        if ( character ) {
            map < int, EntityType* >::reverse_iterator i = _characters.rbegin();
            int characterId = 0;
            if ( i != _characters.rend() ) {
                characterId = ( i -> first ) + 1;
            }
            character -> assignId( characterId );
            _characters[ characterId ] = character;
            _characterPaths[ characterId ] = pathToStore;
            return characterId;
        }
        return -1;
    }

    vector < int > getCharacterIds() {
        vector < int > vec;
        for ( map < int, EntityType* >::iterator i = _classes.begin(); i != _classes.end(); i++ ) {
            vec.push_back( i -> first );
        }
        return vec;
    }

    template < class T > void removeData( map < int, T* > & m ) {
        for ( typename map < int, T* >::iterator i = m.begin(); i != m.end(); i++ ) {
            if ( i -> second ) {
                delete i -> second;
            }
        }
        m.clear();
    }

    void removeTextures( map < int, Texture* > textures ) {
        for ( map < int, Texture* >::iterator i = textures.begin(); i != textures.end(); i++ ) {
            Texture* texture = i -> second;
            if ( texture ) {
                if ( texture -> getTexture() ) {
                    SDL_DestroyTexture( texture -> getTexture() );
                }
                delete texture;
            }
        }
    }

    void removeSounds( map < int, Sound* > sounds ) {
        for ( map < int, Sound* >::iterator i = sounds.begin(); i != sounds.end(); i++ ) {
            Sound* sound = i -> second;
            if ( sound ) {
                delete sound;
            }
        }
    }

    void resetSpawns() {
        for ( map < int, SpawnMachine* >::iterator i = _spawns.begin(); i != _spawns.end(); i++ ) {
            SpawnMachine* machine = i -> second;
            if ( machine ) {
                machine -> reset();
            }
        }
    }

    protected:

    class InteractivePathLoader {
        private:
        InteractivePathLoader() {}
        ~InteractivePathLoader() {}
        public:
        static map < int, string > loadPaths( const char* path ) {
            map < int, string > mappedPaths;
            FILE* handle = fopen( path, "r" );
            if ( handle ) {
                static const int BUFFER_SIZE = 4096;
                char* buffer = ( char* ) malloc( BUFFER_SIZE );
                while ( !feof( handle ) ) {
                    if ( fgets( buffer, BUFFER_SIZE - 1, handle ) ) {
                        string line = string( buffer );
                        vector < string > strings = StringUtils::split( line, ':' );
                        if ( strings.size() == 2 ) {
                            int id;
                            if ( StringUtils::toInteger( strings[ 0 ], &id ) ) {
                                mappedPaths[ id ] = strings[ 1 ];
                                continue;
                            }
                        }
                        free( buffer );
                        throw Exception( "Problem with entities file" );
                    }
                }
                free( buffer );
                fclose( handle );
                return mappedPaths;
            }
            throw Exception( "Cannot open linking file" );
        }

        static list < string > loadDir( const char* pathDir, const char* scan, bool recursive ) {
            list < string > files;
            string pathRegex = string( pathDir ) + '\\' + string( "*" );
            WIN32_FIND_DATA data;
            HANDLE hFind;
            if ( ( hFind = FindFirstFile( pathRegex.c_str(), &data ) ) != INVALID_HANDLE_VALUE ) {
                do {
                    if ( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
                        if ( recursive ) {
                            printf( "Found directory [%s]\n", data.cFileName );
                            if ( ( strcmp( data.cFileName, "." ) == 0 ) || ( strcmp( data.cFileName, ".." ) == 0 ) ) {
                                continue;
                            } else {
                                string recPath = string( pathDir ) + '/' + string( data.cFileName );
                                list < string > loadedSubDir = loadDir( recPath.c_str(), scan, recursive );
                                for ( list < string >::iterator j = loadedSubDir.begin(); j != loadedSubDir.end(); j++ ) {
                                    files.push_back( *j );
                                }
                                continue;
                            }
                        } else {
                            continue;
                        }
                    }
                    string fileName = string( data.cFileName );
                    string scanS = string( scan );
                    if ( StringUtils::endsWith( fileName, scanS ) ) {
                        string fullPath = string( pathDir ) + '/' + fileName;
                        printf( "DEBUG loadDir: [%s]\n", fullPath.c_str() );
                        files.push_back( fullPath );
                    }
                } while ( FindNextFile( hFind, &data ) != 0 );
                FindClose( hFind );
            }
            return files;
        }
    };


    template < class T, class FAM > class InteractiveLoader {
        public:
        InteractiveLoader() {
            _mapper = new FAM();
        }
        virtual ~InteractiveLoader() {
            delete _mapper;
        }

        map < int, T* > loadMultipleFiles( map < int, string > paths ) {
            map < int, T* > mappedInteractives;
            for ( map < int, string >::iterator i = paths.begin(); i != paths.end(); i++ ) {
                T* type = loadSingleFile( i -> first, ( i -> second ).c_str() );
                if ( type ) {
                    mappedInteractives[ i -> first ] = type;
                    continue;
                }
                throw Exception( "Cannot find file" );
            }
            return mappedInteractives;
        }

        vector < int > store( const char* path, map < int, T* > items, map < int, string > paths ) {
            vector < int > failedIds;
            FILE* handle = fopen( path, "w" );
            if ( handle ) {
                for ( typename map < int, T* >::iterator i = items.begin(); i != items.end(); i++ ) {
                    int id = i -> first;
                    T* type = i -> second;
                    map < int, string >::iterator file = paths.find( id );
                    if ( file != paths.end() ) {
                        string filePath = file -> second;
                        if ( writeSingleFile( id, filePath.c_str(), type ) ) {
                            fprintf( handle, "%d:%s\n", id, filePath.c_str() );
                        } else {
                            failedIds.push_back( id );
                        }
                    } else {
                        failedIds.push_back( id );
                    }
                }
                fclose( handle );
                return failedIds;
            }
            throw Exception( "Cannot open linking file" );
        }

        T* loadSingleFile( int id, const char* path ) {
            Properties* properties = Properties::read( path );
            if ( !properties ) {
                return NULL;
            }
            T* type = new T( id );
            mapParams( properties, _mapper, type );
            delete properties;
            return type;
        }

        bool writeSingleFile( int id, const char* path, T* type ) {
            Properties* properties = new Properties();
            mapParams( type, _mapper, properties );
            bool writeResult = properties -> write( path );
            delete properties;
            return writeResult;
        }

        T* loadSingleUnassignedFile( const char* path ) {
            return loadSingleFile( -1, path );
        }

        private:
        FAM* _mapper;
    };

    class FileAttributeMapper {
        public:
        FileAttributeMapper() {
        }
        virtual ~FileAttributeMapper() {
        }

        void addMapping( string key, int id ) {
            _keyToId[ key ] = id;
            _idToKey[ id ] = key;
        }

        void addStringMapping( string key, int id ) {
            _sKeyToId[ key ] = id;
            _sIdToKey[ id ] = key;
        }

        void addMultiMapping( string key, int id ) {
            _mKeyToId[ key ] = id;
            _mIdToKey[ id ] = key;
        }

        void addStringMultiMapping( string key, int id ) {
            _msKeyToId[ key ] = id;
            _msIdToKey[ id ] = key;
        }

        map < string, int > getKeyMapping() {
            return _keyToId;
        }

        map < int, string > getIdMapping() {
            return _idToKey;
        }

        map < string, int > getKeyStringMapping() {
            return _sKeyToId;
        }

        map < int, string > getIdStringMapping() {
            return _sIdToKey;
        }

        map < string, int > getKeyMultiMapping() {
            return _mKeyToId;
        }

        map < int, string > getIdMultiMapping() {
            return _mIdToKey;
        }

        map < string, int > getKeyStringMultiMapping() {
            return _msKeyToId;
        }

        map < int, string > getIdStringMultiMapping() {
            return _msIdToKey;
        }

        bool findAnyMapping( string key ) {
            map < string, int >::iterator i;
            i = _keyToId.find( key );
            if ( i != _keyToId.end() ) {
                return true;
            }
            i = _sKeyToId.find( key );
            if ( i != _sKeyToId.end() ) {
                return true;
            }
            i = _mKeyToId.find( key );
            if ( i != _mKeyToId.end() ) {
                return true;
            }
            i = _msKeyToId.find( key );
            if ( i != _msKeyToId.end() ) {
                return true;
            }
            return false;
        }

        virtual string getName() = 0;

        private:

        map < string, int > _keyToId;
        map < int, string > _idToKey;
        map < string, int > _sKeyToId;
        map < int, string > _sIdToKey;
        map < string, int > _mKeyToId;
        map < int, string > _mIdToKey;
        map < string, int > _msKeyToId;
        map < int, string > _msIdToKey;

    };

    class EntityTypeFileAttributeMapper : public FileAttributeMapper {
        public:
        EntityTypeFileAttributeMapper() : FileAttributeMapper() {
            addMapping( "HP", EntityType::ATTRIBUTE_HP );
            addMapping( "SPEED", EntityType::ATTRIBUTE_SPEED );
            addMapping( "SPEED_SPRINT", EntityType::ATTRIBUTE_SPEED_SPRINT );
            addMapping( "DAMAGE", EntityType::ATTRIBUTE_DAMAGE );
            addStringMapping( "NAME", EntityType::ATTRIBUTE_STRING_NAME );
            addMapping( "LOGIC_ID", EntityType::ATTRIBUTE_LOGIC_ID );
            addMapping( "TEXTURE_ID", EntityType::ATTRIBUTE_TEXTURE_ID );
            addMapping( "COLLISION_RADIUS", EntityType::ATTRIBUTE_COLLISION_RADIUS );
            addMapping( "SOLDIER_MIN_DIST", EntityType::ATTRIBUTE_SOLDIER_MIN_DIST );
            addMapping( "SOLDIER_MAX_DIST", EntityType::ATTRIBUTE_SOLDIER_MAX_DIST );
            addMapping( "EXP", EntityType::ATTRIBUTE_EXP );
            addMapping( "EXP_REQUIRED", EntityType::ATTRIBUTE_NEXP );
            addMapping( "LEVEL", EntityType::ATTRIBUTE_LEVEL );
            addMapping( "UNASSIGNED_STAT_POINTS", EntityType::ATTRIBUTE_UNASSIGNED_STAT_POINTS );
            addMultiMapping( "DAMAGE_SOUND", EntityType::ATTRIBUTE_DAMAGE_SOUND );
            addMultiMapping( "DEATH_SOUND", EntityType::ATTRIBUTE_DEATH_SOUND );
            addMultiMapping( "DEATH_SOUND", EntityType::ATTRIBUTE_DEATH_SOUND );
            addMultiMapping( "DEATH_PARTICLE_COUNT_MIN", EntityType::DEATH_PARTICLE_COUNT_MIN );
            addMultiMapping( "DEATH_PARTICLE_COUNT_MAX", EntityType::DEATH_PARTICLE_COUNT_MAX );
            addMultiMapping( "DEATH_PARTICLE", EntityType::ATTRIBUTE_MV_PARTICLE_IDS );
            addMultiMapping( "CORPSE_TEXTURE", EntityType::ATTRIBUTE_MV_CORPSE_IDS );
            addMultiMapping( "BLOOD_TEXTURE", EntityType::ATTRIBUTE_MV_BLOOD_IDS );
            addMultiMapping( "BLOOD_SPLAT_TEXTURE", EntityType::ATTRIBUTE_MV_BLOOD_SPLAT_IDS );
            addMultiMapping( "DROP_ITEM", EntityType::ATTRIBUTE_MV_PICKUP_IDS );
            addMultiMapping( "DROP_ITEM_MIN", EntityType::ATTRIBUTE_MV_PICKUP_ID_MINS );
            addMultiMapping( "DROP_ITEM_MAX", EntityType::ATTRIBUTE_MV_PICKUP_ID_MAXS );
            addMapping( "SHOOT_POSITION_X", EntityType::ATTRIBUTE_SHOOT_POSITION_X );
            addMapping( "SHOOT_POSITION_Y", EntityType::ATTRIBUTE_SHOOT_POSITION_Y );
            addMapping( "INVENTORY_WEAPON_ID", EntityType::ATTRIBUTE_INVENTORY_WEAPON_ID );
            addMapping( "VISIBILITY_DISTANCE", EntityType::ATTRIBUTE_VISIBILITY_DISTANCE );
            addMapping( "EXPLODE_AT_DEATH", EntityType::ATTRIBUTE_EXPLODE_AT_DEATH );
            addMapping( "SOLDIER_FIRE_COUNT_MIN", EntityType::ATTRIBUTE_SOLDIER_FIRE_COUNT_MIN );
            addMapping( "SOLDIER_FIRE_COUNT_MAX", EntityType::ATTRIBUTE_SOLDIER_FIRE_COUNT_MAX );
            addMapping( "SOLDIER_HOLD_FIRE_TIME_MIN", EntityType::ATTRIBUTE_SOLDIER_HOLD_FIRE_TIME_MIN );
            addMapping( "SOLDIER_HOLD_FIRE_TIME_MAX", EntityType::ATTRIBUTE_SOLDIER_HOLD_FIRE_TIME_MAX );
            addMapping( "GAINED_EXP", EntityType::ATTRIBUTE_GAINED_EXP );
            addMapping( "ENERGY", EntityType::ATTRIBUTE_ENERGY );
            addMapping( "ENERGY_RESTORE_LEVEL", EntityType::ATTRIBUTE_ENERGY_RESTORE_LEVEL );
            addMapping( "AUTO_BURST_THRESHOLD", EntityType::ATTRIBUTE_AUTO_BURST_THRESHOLD );
            addMapping( "AUTO_BURST_THRESHOLD_MAX", EntityType::ATTRIBUTE_AUTO_BURST_THRESHOLD_MAX );
            addMapping( "STATIC_ENERGY", EntityType::ATTRIBUTE_STATIC_ENERGY );
            addMapping( "HEALTH_BUFFER_UNIT_ENABLED", EntityType::ATTRIBUTE_HEALTH_BUFFER_UNIT_ENABLED );
            addMapping( "HEALTH_BUFFER_UNIT_CAPACITY", EntityType::ATTRIBUTE_HEALTH_BUFFER_UNIT_CAPACITY );
            addMapping( "HEALTH_BUFFER_UNIT_CONSUMPTION_RATIO", EntityType::ATTRIBUTE_HEALTH_BUFFER_UNIT_CONSUMPTION_RATIO );
            addMapping( "HEALTH_BUFFER_UNIT_CONSUMPTION_SPEED", EntityType::ATTRIBUTE_HEALTH_BUFFER_UNIT_CONSUMPTION_SPEED );
            addMapping( "ARMS_BROKEN_MODEL_ID", EntityType::ATTRIBUTE_ARMS_BROKEN_MODEL_ID );
            addMultiMapping( "ARMS_BROKEN_SOUND_ID", EntityType::ATTRIBUTE_MV_ARMS_BROKEN_SOUND_ID );
            addMultiMapping( "ARMS_BROKEN_DEBRIS_ID", EntityType::ATTRIBUTE_MV_ARMS_BROKEN_DEBRIS_ID );
            addMultiMapping( "ARMS_BROKEN_DEBRIS_MIN", EntityType::ATTRIBUTE_MV_ARMS_BROKEN_DEBRIS_MIN );
            addMultiMapping( "ARMS_BROKEN_DEBRIS_MAX", EntityType::ATTRIBUTE_MV_ARMS_BROKEN_DEBRIS_MAX );
            addMapping( "ARMS_BROKEN_CHANCE", EntityType::ATTRIBUTE_ARMS_BROKEN_CHANCE );
            addMapping( "ARMS_BROKEN_DAMAGE_THRESHOLD", EntityType::ATTRIBUTE_ARMS_BROKEN_DAMAGE_THRESHOLD );
            addMapping( "ACTIVE_SKILL", EntityType::ATTRIBUTE_ACTIVE_SKILL );
            addMultiMapping( "SKILL_LEVEL", EntityType::ATTRIBUTE_SKILL_LEVEL );
            addMultiMapping( "LEVEL_PROGRESS", EntityType::ATTRIBUTE_LEVEL_PROGRESS );
            addStringMultiMapping( "INVENTORY_STORAGE", EntityType::ATTRIBUTE_INVENTORY_STORAGE );
            addMapping( "ARMS_BROKEN_DIE_SOUND_ID", EntityType::ATTRIBUTE_ARMS_BROKEN_DIE_SOUND_ID );
            addMapping( "ARMS_BROKEN_DIE_TEXTURE_ID", EntityType::ATTRIBUTE_ARMS_BROKEN_DIE_TEXTURE_ID );
            addMapping( "MELEE_WEAPON_ID", EntityType::ATTRIBUTE_MELEE_WEAPON_ID );
            // statistics
            addMapping( "BASE_HP", EntityType::ATTRIBUTE_BASE_HP );
            addMapping( "HP_STAT_MULTIPLIER", EntityType::ATTRIBUTE_HP_STAT_MULTIPLIER );
            addMapping( "HP_PER_LEVEL", EntityType::ATTRIBUTE_HP_PER_LEVEL );
            addMapping( "BASE_ENERGY", EntityType::ATTRIBUTE_BASE_ENERGY );
            addMapping( "ENE_STAT_MULTIPLIER", EntityType::ATTRIBUTE_ENE_STAT_MULTIPLIER );
            addMapping( "ENE_PER_LEVEL", EntityType::ATTRIBUTE_ENE_PER_LEVEL );
        }
        virtual ~EntityTypeFileAttributeMapper() {
        }
        virtual string getName() {
            return "Entity";
        }
    };

    class WeaponTypeFileAttributeMapper : public FileAttributeMapper {
        public:
        WeaponTypeFileAttributeMapper() : FileAttributeMapper() {
            addMapping( "INVENTORY_TEXTURE_ID", WeaponType::ATTRIBUTE_TEXTURE_ID );
            addMapping( "BULLET_ID", WeaponType::ATTRIBUTE_BULLET_ID );
            addMapping( "SHOOT_SOUND_ID", WeaponType::ATTRIBUTE_SHOOT_SOUND_ID );
            addMapping( "DAMAGE_MIN", WeaponType::ATTRIBUTE_DAMAGE_MIN );
            addMapping( "DAMAGE_MAX", WeaponType::ATTRIBUTE_DAMAGE_MAX );
            addMapping( "PRECISION", WeaponType::ATTRIBUTE_PRECISION );
            addMapping( "DELAY", WeaponType::ATTRIBUTE_DELAY );
            addMapping( "BUCK", WeaponType::ATTRIBUTE_BUCK );
            addMapping( "CLIP_SIZE", WeaponType::ATTRIBUTE_CLIP_SIZE );
            addMapping( "RELOAD_TIME", WeaponType::ATTRIBUTE_RELOAD_TIME );
            addMapping( "BULLET_SPEED", WeaponType::ATTRIBUTE_BULLET_SPEED );
            addMapping( "SHOOT_QUAKE_POWER", WeaponType::ATTRIBUTE_SHOOT_QUAKE_POWER );
            addMapping( "SHOOT_QUAKE_DURATION", WeaponType::ATTRIBUTE_SHOOT_QUAKE_DURATION );
            addMapping( "DEATH_PUSH", WeaponType::ATTRIBUTE_DEATH_PUSH );
            addMapping( "FIRE_TEXTURE_ID", WeaponType::ATTRIBUTE_FIRE_TEXTURE_ID );
            addMapping( "INFINITY_AMMO", WeaponType::ATTRIBUTE_INFINITY_AMMO );
            addMapping( "CURSOR_AFFECTION", WeaponType::ATTRIBUTE_CURSOR_AFFECTION );
            addMapping( "NAME", WeaponType::ATTRIBUTE_NAME );
            addMapping( "ENERGY_CONSUMPTION", WeaponType::ATTRIBUTE_ENERGY_CONSUMPTION );
            addMapping( "HEAT_MULTIPLIER", WeaponType::ATTRIBUTE_HEAT_MULTIPLIER );
            addMapping( "ENERGY_CONVERSION_VALUE", WeaponType::ATTRIBUTE_ENERGY_CONVERSION_VALUE );
            addMapping( "ENERGY_UPGRADE_VALUE", WeaponType::ATTRIBUTE_ENERGY_UPGRADE_VALUE );
            addMapping( "BULLET_SPREAD", WeaponType::ATTRIBUTE_BULLET_SPREAD );
        }
        virtual ~WeaponTypeFileAttributeMapper() {
        }
        virtual string getName() {
            return "Weapon";
        }
    };

    class ArmorTypeFileAttributeMapper : public FileAttributeMapper {
        public:
        ArmorTypeFileAttributeMapper() : FileAttributeMapper() {
            addMapping( "HP", ArmorType::ATTRIBUTE_HP );
            addMapping( "INVENTORY_TEXTURE_ID", ArmorType::ATTRIBUTE_TEXTURE_ID );
            addMapping( "NAME", ArmorType::ATTRIBUTE_NAME );
            addMapping( "ENERGY_CONVERSION_VALUE", ArmorType::ATTRIBUTE_ENERGY_CONVERSION_VALUE );
            addMapping( "ENERGY_UPGRADE_VALUE", ArmorType::ATTRIBUTE_ENERGY_UPGRADE_VALUE );
        }
        virtual ~ArmorTypeFileAttributeMapper() {
        }
        virtual string getName() {
            return "Armor";
        }
    };

    class PickupTypeFileAttributeMapper : public FileAttributeMapper {
        public:
        PickupTypeFileAttributeMapper() : FileAttributeMapper() {
            addMapping( "TEXTURE_ID", PickupType::ATTRIBUTE_TEXTURE_ID );
            addMapping( "PICK_TYPE", PickupType::ATTRIBUTE_PICKUP_TYPE );
            addMapping( "PICK_RANGE", PickupType::ATTRIBUTE_PICK_RANGE );
            addMapping( "COLLECT_SOUND", PickupType::ATTRIBUTE_COLLECT_SOUND );
            addMultiMapping( "CUSTOM_VALUE", PickupType::ATTRIBUTE_CUSTOM_VALUE );
            addMapping( "TIP_HOVER", PickupType::ATTRIBUTE_TIP_HOVER );
        }
        virtual ~PickupTypeFileAttributeMapper() {
        }
        virtual string getName() {
            return "Pickup";
        }
    };

    class BulletTypeFileAttributeMapper : public FileAttributeMapper {
        public:
        BulletTypeFileAttributeMapper() : FileAttributeMapper() {
            addMapping( "TEXTURE_ID", BulletType::ATTRIBUTE_TEXTURE_ID );
            addMapping( "MAX_CAPACITY", BulletType::ATTRIBUTE_MAX_CAPACITY );
            addMapping( "COST", BulletType::ATTRIBUTE_COST );
            addMapping( "BOX_SIZE", BulletType::ATTRIBUTE_BOX_SIZE );
            addMultiMapping( "CORPSE_TEXTURE_ID", BulletType::ATTRIBUTE_CORPSE_TEXTURE_ID );
            addMapping( "INVENTORY_TEXTURE_ID", BulletType::ATTRIBUTE_INVENTORY_TEXTURE_ID );
            addMapping( "NAME", BulletType::ATTRIBUTE_NAME );
            addMapping( "COLOR.R", BulletType::ATTRIBUTE_COLOR_R );
            addMapping( "COLOR.G", BulletType::ATTRIBUTE_COLOR_G );
            addMapping( "COLOR.B", BulletType::ATTRIBUTE_COLOR_B );
            addMapping( "COLOR.A", BulletType::ATTRIBUTE_COLOR_A );
            addMapping( "TYPE", BulletType::ATTRIBUTE_TYPE );
            addMapping( "EXPLOSION_ON_DEATH_ID", BulletType::ATTRIBUTE_EXPLOSION_ON_DEATH_ID );
            addMapping( "AUTO_AIM", BulletType::ATTRIBUTE_AUTO_AIM );
            addMapping( "IGNORE_WALLS", BulletType::ATTRIBUTE_IGNORE_WALLS );
            addMapping( "IS_MATERIAL", BulletType::ATTRIBUTE_IS_MATERIAL );
            addMapping( "RANGE", BulletType::ATTRIBUTE_RANGE );
        }
        virtual ~BulletTypeFileAttributeMapper() {
        }
        virtual string getName() {
            return "Bullet";
        }
    };

    class ParticleTypeFileAttributeMapper : public FileAttributeMapper {
        public:
        ParticleTypeFileAttributeMapper() : FileAttributeMapper() {
            addMapping( "TEXTURE_ID", ParticleType::ATTRIBUTE_TEXTURE_ID );
            addMapping( "MASS", ParticleType::ATTRIBUTE_MASS );
            addMultiMapping( "COLLISION_TEXTURE_ID", ParticleType::ATTRIBUTE_COLLISION_TEXTURE_ID );
            addMultiMapping( "COLLISION_SOUND_ID", ParticleType::ATTRIBUTE_COLLISION_SOUND_ID );
        }
        virtual ~ParticleTypeFileAttributeMapper() {
        }
        virtual string getName() {
            return "Particle";
        }
    };

    class ExplosionTypeFileAttributeMapper : public FileAttributeMapper {
        public:
        ExplosionTypeFileAttributeMapper() : FileAttributeMapper() {
            addMultiMapping( "TEXTURE_ID", ExplosionType::ATTRIBUTE_TEXTURE_ID );
            addMapping( "RANGE_MIN", ExplosionType::ATTRIBUTE_RANGE_MIN );
            addMapping( "RANGE_MAX", ExplosionType::ATTRIBUTE_RANGE_MAX );
            addMapping( "DAMAGE_MIN", ExplosionType::ATTRIBUTE_DAMAGE_MIN );
            addMapping( "DAMAGE_MAX", ExplosionType::ATTRIBUTE_DAMAGE_MAX );
            addMapping( "DEATH_PUSH", ExplosionType::ATTRIBUTE_DEATH_PUSH );
            addMapping( "DURATION", ExplosionType::ATTRIBUTE_DURATION );
            addMultiMapping( "SOUND_ID", ExplosionType::ATTRIBUTE_SOUND_ID );
            addMapping( "QUAKE_POWER", ExplosionType::ATTRIBUTE_QUAKE_POWER );
            addMapping( "LOAD_TYPE", ExplosionType::ATTRIBUTE_LOAD_TYPE );
        }
        virtual ~ExplosionTypeFileAttributeMapper() {
        }
        virtual string getName() {
            return "Explosion";
        }
    };

    class ChapterTypeFileAttributeMapper : public FileAttributeMapper {
        public:
        ChapterTypeFileAttributeMapper() : FileAttributeMapper() {
            addMapping( "ID", ChapterType::ATTRIBUTE_CHAPTER_ID );
            addMapping( "NAME", ChapterType::ATTRIBUTE_NAME );
            addMultiMapping( "LEVEL", ChapterType::ATTRIBUTE_LEVEL );
            addMultiMapping( "LEVEL_NAME", ChapterType::ATTRIBUTE_LEVEL_NAME );
        }
        virtual ~ChapterTypeFileAttributeMapper() {
        }
        virtual string getName() {
            return "Chapter";
        }
    };

    class SkillTypeFileAttributeMapper : public FileAttributeMapper {
        public:
        SkillTypeFileAttributeMapper() : FileAttributeMapper() {
            addMapping( "NAME", SkillType::ATTRIBUTE_NAME );
            addMapping( "PLACEMENT_TYPE", SkillType::ATTRIBUTE_PLACEMENT_TYPE );
            addMapping( "SKILL_TYPE", SkillType::ATTRIBUTE_SKILL_TYPE );
            addMapping( "BASE_DAMAGE_MIN", SkillType::ATTRIBUTE_BASE_DAMAGE_MIN );
            addMapping( "BASE_DAMAGE_MAX", SkillType::ATTRIBUTE_BASE_DAMAGE_MAX );
            addMapping( "BASE_ENERGY_COST", SkillType::ATTRIBUTE_BASE_ENERGY_COST );
            addMapping( "BASE_ENERGY_USAGE", SkillType::ATTRIBUTE_BASE_ENERGY_USAGE );
            addMapping( "BASE_DAMAGE_LEVEL_MUL", SkillType::ATTRIBUTE_BASE_DAMAGE_LEVEL_MUL );
            addMapping( "BASE_DAMAGE_LEVEL_MUL_TYPE", SkillType::ATTRIBUTE_BASE_DAMAGE_LEVEL_MUL_TYPE );
            addMapping( "BASE_ENERGY_COST_CURVE", SkillType::ATTRIBUTE_BASE_ENERGY_COST_CURVE );
            addMapping( "BASE_ENERGY_USAGE_MUL", SkillType::ATTRIBUTE_BASE_ENERGY_USAGE_MUL );
            addMapping( "EXPLOSION_TYPE_ID", SkillType::ATTRIBUTE_EXPLOSION_TYPE_ID );
            addMapping( "DURATION", SkillType::ATTRIBUTE_DURATION );
            addMapping( "TEXTURE_UI_ID", SkillType::ATTRIBUTE_TEXTURE_UI_ID );
        }
        virtual ~SkillTypeFileAttributeMapper() {
        }
        virtual string getName() {
            return "Skill";
        }
    };



    static void mapParams( Properties* properties, FileAttributeMapper* mapper, InteractiveType* type ) {
        map < string, int > keyToId = mapper -> getKeyMapping();
        for ( map < string, int >::iterator i = keyToId.begin(); i != keyToId.end(); i++ ) {
            double value;
            if ( properties -> getDouble( i -> first, &value ) ) {
                type -> setParameter( i -> second, value );
            } else {
                printf( "ERROR: no valid parameter for %s Mapper: %s\n", ( mapper -> getName() ).c_str(), ( i -> first ).c_str() );
            }
        }
        map < string, int > sKeyToId = mapper -> getKeyStringMapping();
        for ( map < string, int >::iterator i = sKeyToId.begin(); i != sKeyToId.end(); i++ ) {
            string value;
            if ( properties -> get( i -> first, &value ) ) {
                type -> setStringParameter( i -> second, value );
            } else {
                printf( "ERROR: no valid string parameter for %s Mapper: %s\n", ( mapper -> getName() ).c_str(), ( i -> first ).c_str() );
            }
        }
        map < string, int > mKeyToId = mapper -> getKeyMultiMapping();
        for ( map < string, int >::iterator i = mKeyToId.begin(); i != mKeyToId.end(); i++ ) {
            map < string, string > value;
            if ( properties -> getMultiValued( i -> first, value ) ) {
                map < int, double > convertedValue;
                for ( map < string, string >::iterator j = value.begin(); j != value.end(); j++ ) {
                    int id;
                    double doubleValue;
                    if ( StringUtils::toInteger( j -> first, &id ) ) {
                        if ( StringUtils::toDouble( j -> second, &doubleValue ) ) {
                            convertedValue[ id ] = doubleValue;
                        } else {
                            printf( "ERROR: cannot convert to Double for %s Mapper: %s\n", ( mapper -> getName() ).c_str(), ( j -> second ).c_str() );
                        }
                    } else {
                        printf( "ERROR: cannot convert to Int for %s Mapper: %s\n", ( mapper -> getName() ).c_str(), ( j -> first ).c_str() );
                    }
                }
                type -> setMultiValuedParameter( i -> second, convertedValue );
            } else {
                printf( "ERROR: no valid multivalued parameter for %s Mapper: %s\n", ( mapper -> getName() ).c_str(), ( i -> first ).c_str() );
            }
        }
        map < string, int > msKeyToId = mapper -> getKeyStringMultiMapping();
        for ( map < string, int >::iterator i = msKeyToId.begin(); i != msKeyToId.end(); i++ ) {
            map < string, string > value;
            if ( properties -> getMultiValued( i -> first, value ) ) {
                map < int, string > convertedValue;
                for ( map < string, string >::iterator j = value.begin(); j != value.end(); j++ ) {
                    int id;
                    if ( StringUtils::toInteger( j -> first, &id ) ) {
                        convertedValue[ id ] = j -> second;
                    } else {
                        printf( "ERROR: cannot convert to Int for %s Mapper: %s\n", ( mapper -> getName() ).c_str(), ( j -> first ).c_str() );
                    }
                }
                type -> setStringMultiValuedParameter( i -> second, convertedValue );
            } else {
                printf( "ERROR: no valid multivalued string parameter for %s Mapper: %s\n", ( mapper -> getName() ).c_str(), ( i -> first ).c_str() );
            }
        }
        // check redundant attributes
        map < string, string > parsed = properties -> getAll();
        map < string, map < string, string > > parsedMultiValued = properties -> getAllMultiValued();
        for ( map < string, string >::iterator p = parsed.begin(); p != parsed.end(); p++ ) {
            if ( !mapper -> findAnyMapping( p -> first ) ) {
                printf( "Warn: redundant attribute for %s Mapper: [%s]\n", ( mapper -> getName() ).c_str(), ( p -> first ).c_str() );
            }
        }
        for ( map < string, map < string, string > >::iterator mv = parsedMultiValued.begin(); mv != parsedMultiValued.end(); mv++ ) {
            if ( !mapper -> findAnyMapping( mv -> first ) ) {
                printf( "Warn: redundant multivalued attribute for %s Mapper: [%s]\n", ( mapper -> getName() ).c_str(), ( mv -> first ).c_str() );
            }
        }
    }

    static void mapParams( InteractiveType* type, FileAttributeMapper* mapper, Properties* properties ) {
        map < int, string > idToKey = mapper -> getIdMapping();
        for ( map < int, string >::iterator i = idToKey.begin(); i != idToKey.end(); i++ ) {
            double value = type -> getParameter( i -> first );
            if ( value != NAN ) {
                properties -> set( i -> second, StringUtils::toString( value ) );
            } else {
                printf( "ERROR: no valid parameter for %s Mapper: %d\n", ( mapper -> getName() ).c_str(), i -> first );
            }
        }
        map < int, string > sIdToKey = mapper -> getIdStringMapping();
        for ( map < int, string >::iterator i = sIdToKey.begin(); i != sIdToKey.end(); i++ ) {
            string value;
            if ( type -> getStringParameter( i -> first, value ) ) {
                properties -> set( i -> second, value );
            } else {
                printf( "ERROR: no valid parameter for %s Mapper: %d\n", ( mapper -> getName() ).c_str(), i -> first );
            }
        }
        map < int, string > mIdToKey = mapper -> getIdMultiMapping();
        for ( map < int, string >::iterator i = mIdToKey.begin(); i != mIdToKey.end(); i++ ) {
            map < int, double > value;
            if ( type -> getMultiValuedParameter( i -> first, value ) ) {
                map < string, string > convertedValue;
                for ( map < int, double >::iterator j = value.begin(); j != value.end(); j++ ) {
                    string id = StringUtils::toString( j -> first );
                    string doubleValue = StringUtils::toString( j -> second );
                    convertedValue[ id ] = doubleValue;
                }
                properties -> setMultiValued( i -> second, convertedValue );
            } else {
                printf( "ERROR: no valid multivalued parameter for %s Mapper: %d\n", ( mapper -> getName() ).c_str(), i -> first );
            }
        }
        map < int, string > msIdToKey = mapper -> getIdStringMultiMapping();
        for ( map < int, string >::iterator i = msIdToKey.begin(); i != msIdToKey.end(); i++ ) {
            map < int, string > value;
            if ( type -> getStringMultiValuedParameter( i -> first, value ) ) {
                map < string, string > convertedValue;
                for ( map < int, string >::iterator j = value.begin(); j != value.end(); j++ ) {
                    string id = StringUtils::toString( j -> first );
                    string stringValue = j -> second;
                    convertedValue[ id ] = stringValue;
                }
                properties -> setMultiValued( i -> second, convertedValue );
            } else {
                printf( "ERROR: no valid string multivalued parameter for %s Mapper: %d\n", ( mapper -> getName() ).c_str(), i -> first );
            }
        }
    }

    private:

    map < int, string > _mapPaths;
    map < int, Texture* > _textures;
    map < int, Sound* > _sounds;
    map < int, Sound* > _soundStreams;

    map < int, EntityType* > _classes;
    map < int, EntityType* > _characters;
    map < int, EntityType* > _entities;
    map < int, WeaponType* > _weapons;
    map < int, ArmorType* > _armors;
    map < int, PickupType* > _pickups;
    map < int, BulletType* > _bullets;
    map < int, ParticleType* > _particles;
    map < int, ExplosionType* > _explosions;
    map < int, SpawnMachine* > _spawns;
    map < int, ChapterType* > _chapters;
    map < int, SkillType* > _skills;

    map < int, BlockInfo > _blockInfo;
    map < int, string > _scriptPaths; // dynamic load

    map < int, Quest* > _questMap;

    map < int, string > _characterPaths;
    list < string > _characterPathsToRemove;

    // for map editor
    vector < int > _mapTextureIds;

};


