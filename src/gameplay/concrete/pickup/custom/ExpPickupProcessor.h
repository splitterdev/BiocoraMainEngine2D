#pragma once

class ExpPickupProcessor : public PickupProcessor {
    public:
    ExpPickupProcessor() : PickupProcessor() {
    }
    virtual ~ExpPickupProcessor() {
    }

    virtual bool onPick( EntityObject* picker, PickupObject* pickup ) {
        double gainedExp = pickup -> getCustomValue( 0 );
        if ( picker -> addExperience( gainedExp ) ) {
            if ( picker == Cache::getGameStateHolder() -> getPlayer() ) {
                MessageManager::get() -> addMessage(
                    MessageManager::LEVEL_NOTICE,
                    LangStrings::getString( LangStringsID::MESSAGE_LOG_LEVEL_ADVANCE )
                        + string( ": " )
                        + StringUtils::toString( int( picker -> getLevel() ) )
                );
            }
        }
        return true;
    }

};



