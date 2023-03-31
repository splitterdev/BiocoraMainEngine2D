#pragma once

#include "logic/LogicProcessor.h"

class LogicRobot : public GamePlayRobot {
    public:
    LogicRobot() : GamePlayRobot() {
    }
    virtual ~LogicRobot() {}

    virtual int onTick() {
        GameStateHolder* gameStateHolder = Cache::getGameStateHolder();
        GameStateHolder::Container < EntityObject > container = gameStateHolder -> getEntityContainer();
        list < int > removeList = list < int > ();
        while ( container.hasMore() ) {
            EntityObject* object = container.nextObject();
            LogicCallResult result = LogicProcessor::call( object -> getLogicId(), object, gameStateHolder -> getMap(), 0 );
            if ( result.SUICIDE ) {
                // Just die.
                removeList.push_back( object -> getId() );
            }
            if ( object -> getParameter( EntityType::ATTRIBUTE_HP_CURRENT ) < 0.0 ) {
                /// TODO ?
                /*container.startFrom( e -> first );
                container.nextObject();
                container.removeObject();
                if ( mother -> addExperience( entity -> getGainedExp() ) ) {
                    if ( mother == Cache::getGameStateHolder() -> getPlayer() ) {
                        MessageManager::get() -> addMessage(
                            MessageManager::LEVEL_NOTICE,
                            LangStrings::getString( LangStringsID::MESSAGE_LOG_LEVEL_ADVANCE )
                                + string( ": " )
                                + StringUtils::toString( int( mother -> getLevel() ) )
                        );
                    }
                }
                delete entity;*/
            }
            object -> setParameter( EntityType::ATTRIBUTE_LAST_MEAN_DAMAGE, 0 );
        }
        EntityObject* player = gameStateHolder -> getPlayer();
        for ( list < int >::iterator i = removeList.begin(); i != removeList.end(); i++ ) {
            // Remove from container
            container.startFrom( *i );
            EntityObject* entity = container.nextObject();
            if ( entity == player ) {
                continue;
            }
            container.removeObject();
            if ( player -> addExperience( entity -> getGainedExp() ) ) {
                MessageManager::get() -> addMessage(
                    MessageManager::LEVEL_NOTICE,
                    LangStrings::getString( LangStringsID::MESSAGE_LOG_LEVEL_ADVANCE )
                        + string( ": " )
                        + StringUtils::toString( int( player -> getLevel() ) )
                );
            }
            delete entity;
        }
        return GameStateProcessor::EVENT_RESULT_NONE;
    }

};


