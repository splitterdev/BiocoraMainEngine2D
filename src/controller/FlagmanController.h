#pragma once

#include "AbstractEventListener.h"

bool PLAYING_LOCK = false;

class FlagmanController : public AbstractEventListener {
    public:
    FlagmanController() : AbstractEventListener() {
    }
    virtual ~FlagmanController() {
    }

    static const int HEAL_SOUND_ID = 915;

    inline static void toggleUiMessageSystemEnabled() {
        _UI_MESSAGE_SYSTEM_ENABLED = !_UI_MESSAGE_SYSTEM_ENABLED;
    }

    inline static bool isUiMessageSystemEnabled() {
        return _UI_MESSAGE_SYSTEM_ENABLED;
    }

	virtual int catchKeyPushEvent( int key ) {
	    if ( key == 27 ) {
	        if ( Cache::getGameStateHolder() ) {
                EntityObject* player = Cache::getGameStateHolder() -> getPlayer();
                if ( player ) {
                    double invOpen = player -> getParameter( EntityType::ATTRIBUTE_INVENTORY_OPEN );
                    if ( !isnan( invOpen ) && ( invOpen >= 0.5 ) ) {
                        invOpen = 0.0;
                        player -> setParameter( EntityType::ATTRIBUTE_INVENTORY_OPEN, invOpen );
                        return GameController::RESULT_EMPTY;
                    }
                }
	        }
            return GameController::RESULT_RETURN;
	    }
	    /*if ( key == 13 ) {
            try {
                int characterId = PlayerManager::createCharacter( PlayerManager::getProfileId(), string( "data/player/character/Tester.txt" ) );
                printf( "Created player: %d\n", characterId );
                Cache::getFileAccessor() -> store();
            } catch ( Exception e ) {
                e.log();
            }
	    }*/
	    if ( Cache::getGameStateHolder() ) {
            EntityObject* player = Cache::getGameStateHolder() -> getPlayer();
            if ( player ) {
                if ( key == 'r' ) {
                    WeaponObject* weapon = InventoryHelper::currentWeaponOf( player );
                    if ( weapon ) {
                        int clipSize = weapon -> getClipSize();
                        int clipSizeCurrent = weapon -> getCurrentClip();
                        if ( ( clipSizeCurrent < clipSize ) && ( isnan( weapon -> getReloadingCurrentTime() ) ) ) {
                            weapon -> setReloadingCurrentTime( Cache::gameTime() );
                        }
                    }
                }
                if ( key == 'g' ) {
                    Vector2D explosionPosition = Vector2D( Camera::X + Keys::getMouseX(), Camera::Y + Keys::getMouseY() );
                    AttackHelper::createExplosion( 1, explosionPosition );
                }
                if ( key == 'i' ) {
                    double invOpen = player -> getParameter( EntityType::ATTRIBUTE_INVENTORY_OPEN );
                    if ( isnan( invOpen ) || ( invOpen < 0.5 ) ) {
                        invOpen = 1.0;
                    } else {
                        invOpen = 0.0;
                    }
                    player -> setParameter( EntityType::ATTRIBUTE_INVENTORY_OPEN, invOpen );
                }
                if ( key == ' ' ) {
                    player -> toggleHealthBuffer();
                    if ( player -> isHealthBufferEnabled() ) {
                        MessageManager::get() -> addMessage( MessageManager::LEVEL_WARN, LangStrings::getString( LangStringsID::MESSAGE_LOG_HBU_ENABLED ) );
                    } else {
                        MessageManager::get() -> addMessage( MessageManager::LEVEL_WARN, LangStrings::getString( LangStringsID::MESSAGE_LOG_HBU_DISABLED ) );
                    }
                }
                if ( key == SDLK_F3 ) {
                    toggleUiMessageSystemEnabled();
                }
                if ( key == '1' ) { /// TODO
                    int result = player -> convertEnergyStaticToDynamic( 0.2 );
                    if ( result > 0 ) {
                        // refilled
                    } else if ( result < 0 ) {
                        // can't refill
                    } else {
                        // full energy
                    }
                }
                if ( key == '0' ) {
                    if ( AutoAimCursor::isEnabled() ) {
                        AutoAimCursor::setEnabled( false );
                        MessageManager::get() -> addMessage( MessageManager::LEVEL_WARN, LangStrings::getString( LangStringsID::MESSAGE_LOG_AAS_DISABLED ) );
                    } else {
                        AutoAimCursor::setEnabled( true );
                        MessageManager::get() -> addMessage( MessageManager::LEVEL_WARN, LangStrings::getString( LangStringsID::MESSAGE_LOG_AAS_ENABLED ) );
                    }
                }
                if ( key == 'q' ) {
                    InventoryHelper::selectNextWeapon( player, -1 );
                }
                if ( key == 'e' ) {
                    InventoryHelper::selectNextWeapon( player, 1 );
                }
                if ( key == 'f' ) {
                    /// TODO remove this test spawn
                    EntityObject* entity = EntitySpawner::spawnAt( 101, Camera::X + Keys::getMouseX(), Camera::Y + Keys::getMouseY() );
                    if ( entity ) {
                        Cache::getGameStateHolder() -> add( entity );
                        MessageManager::get() -> addMessage( 1, "Spawned test enemy" );
                    } else {
                        MessageManager::get() -> addMessage( 3, "Cannot spawn test enemy" );
                    }
                }
                if ( key == 'm' ) {
                    CryptChatEngine::getInstance() -> setActive( !CryptChatEngine::getInstance() -> isActive() );
                }
                if ( key == 'n' ) {
                    CryptChatEngine::getInstance() -> sendMessage( 100020, 10001, 300001, 0xFF80C0C0, CryptChatEngine::GUEST );
                }
                if ( key == 'b' ) {
                    CryptChatEngine::getInstance() -> sendMessage( 100020, 10001, 300001, 0xFF80C0C0, CryptChatEngine::OWNER );
                }
            }
	    }
	    if ( key == 'p' ) {
            Perspective::ChangeTo( PERSPECTIVE_WARP_ID );
	    }
	    return GameController::RESULT_EMPTY;
	}

	virtual int catchKeyReleaseEvent( int key ) {
	    return GameController::RESULT_EMPTY;
	}

	virtual int catchMousePushEvent( int button ) {
        Form* activeForm = FormCache::getActiveForm();
	    if ( button == SDL_BUTTON_LEFT ) {
            if ( activeForm ) {
                activeForm -> press( double( Keys::getMouseX() ) / double( WINDOW_WIDTH ), double( Keys::getMouseY() ) / double( WINDOW_HEIGHT ) );
            }
	    }
	    return GameController::RESULT_EMPTY;
	}

	virtual int catchMouseReleaseEvent( int button ) {
        Form* activeForm = FormCache::getActiveForm();
	    if ( button == SDL_BUTTON_LEFT ) {
            if ( activeForm ) {
                activeForm -> release( double( Keys::getMouseX() ) / double( WINDOW_WIDTH ), double( Keys::getMouseY() ) / double( WINDOW_HEIGHT ) );
            }
	    } else if ( button == SDL_BUTTON_RIGHT ) {
	        if ( !activeForm ) {
	            EntityObject* player = Cache::getGameStateHolder() -> getPlayer();
	            if ( player ) {
                    double hpRestored = player -> tryUseHealthBuffer();
                    if ( hpRestored > 0.0 ) {
                        Sound* healSound = Cache::getFileAccessor() -> getSound( HEAL_SOUND_ID );
                        if ( healSound ) {
                            healSound -> play();
                        }
                        MessageManager::get() -> addMessage( MessageManager::LEVEL_NOTICE, LangStrings::getString( LangStringsID::MESSAGE_LOG_HBU_USED ) );
                    } else if ( hpRestored < 0.0 ) {
                        MessageManager::get() -> addMessage( MessageManager::LEVEL_ERROR, LangStrings::getString( LangStringsID::MESSAGE_LOG_HBU_USED_FAIL ) );
                    } else {
                        MessageManager::get() -> addMessage( MessageManager::LEVEL_NOTICE, LangStrings::getString( LangStringsID::MESSAGE_LOG_HBU_USED_NOT_NECESSARY ) );
                    }
	            }
	        }
	    } else if ( button == SDL_BUTTON_MIDDLE ) {
            if ( !activeForm ) {
                SkillHelper::useCurrentSkill( Cache::getGameStateHolder() -> getPlayer(), Vector2D( Camera::X + Keys::getMouseX(), Camera::Y + Keys::getMouseY() ) );
            }
	    }
	    return GameController::RESULT_EMPTY;
	}

	virtual int catchMouseScrollEvent( int direction ) {
	    Form* activeForm = FormCache::getActiveForm();
	    if ( !activeForm ) {
            SkillHelper::setNextActiveSkill( Cache::getGameStateHolder() -> getPlayer(), direction );
	    }
	    return GameController::RESULT_EMPTY;
	}

	virtual int onRegularTick() {
	    SDL_PumpEvents();
	    GameStateHolder* holder = Cache::getGameStateHolder();
	    if ( holder ) {
            EntityObject* player = holder -> getPlayer();
            if ( player ) {
                AttackHelper::weaponIdleTick( player );
                bool isSprint = Keys::isPressed( Keys::SHIFT_LEFT ) || Keys::isPressed( Keys::SHIFT_RIGHT );
                double motionSpeed = player -> getMotionSpeed( isSprint );
                double axisX = 0.0;
                double axisY = 0.0;
                if ( Keys::isPressed( 'w' ) ) {
                    axisY -= motionSpeed;
                }
                if ( Keys::isPressed( 's' ) ) {
                    axisY += motionSpeed;
                }
                if ( Keys::isPressed( 'a' ) ) {
                    axisX -= motionSpeed;
                }
                if ( Keys::isPressed( 'd' ) ) {
                    axisX += motionSpeed;
                }
                if ( axisX != 0.0 && axisY != 0.0 ) {
                    axisX /= sqrt( 2.0 );
                    axisY /= sqrt( 2.0 );
                }
                axisX *= Cache::getTimeLastDifferenceMultiplier();
                axisY *= Cache::getTimeLastDifferenceMultiplier();
                Vector2D position = MovementHelper::calcPositionMapAndEntityCollision( player, axisX, axisY );
                if ( Keys::isMousePressed( SDL_BUTTON_LEFT ) && !( player -> inventoryEnabled() ) ) {
                    AttackHelper::tryShoot( player, Vector2D( Camera::X + Keys::getMouseX(), Camera::Y + Keys::getMouseY() ) );
                }
                player -> setPosition( position );
                player -> setTargetPosition( Camera::X + Keys::getMouseX(), Camera::Y + Keys::getMouseY() );
            }
	    }
	    return GameController::RESULT_EMPTY;
	}

	static void Init() {
	    dispose();
	    _DEFAULT_CONTROLLER = new FlagmanController();
	}

	static FlagmanController* getController() {
        return _DEFAULT_CONTROLLER;
	}

	static void dispose() {
	    if ( _DEFAULT_CONTROLLER ) {
            delete _DEFAULT_CONTROLLER;
            _DEFAULT_CONTROLLER = NULL;
	    }
	}

    private:

    static bool _UI_MESSAGE_SYSTEM_ENABLED;

    static FlagmanController* _DEFAULT_CONTROLLER;

};

bool FlagmanController::_UI_MESSAGE_SYSTEM_ENABLED = true;

FlagmanController* FlagmanController::_DEFAULT_CONTROLLER = NULL;
