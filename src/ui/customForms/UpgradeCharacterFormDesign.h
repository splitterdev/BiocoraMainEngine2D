#pragma once

class UpgradeCharacterFormDesign : public FormDesignFactory {
    public:
    UpgradeCharacterFormDesign() : FormDesignFactory() {
    }
    virtual ~UpgradeCharacterFormDesign() {
    }

    virtual Form* build( FormBuildParameters parameters ) {
        EntityType* character = Cache::getFileAccessor() -> getCharacter( PlayerManager::getProfileId() );
        Form* form = new Form( parameters );

        ComponentProperties properties;
        properties.x = 0.1;
        properties.y = 0.1;
        properties.w = 0.4;
        properties.h = 0.1;
        properties.fillColor = 0x00FFFFFF;

        if ( character ) {
            int availablePoints = ( int ) character -> getParameter( EntityType::ATTRIBUTE_UNASSIGNED_STAT_POINTS );
            form -> addComponent( new StaticText( LangStrings::getString( LangStringsID::MENU_AVAILABLE_POINTS ) + StringUtils::toString( availablePoints ), properties ) );

            ComponentProperties statButtonProperties;
            statButtonProperties.x = properties.x + 0.02;
            statButtonProperties.y = properties.y + properties.h + 0.02;
            statButtonProperties.w = 0.35;
            statButtonProperties.h = 0.075;
            statButtonProperties.fillColor = 0x00FFFFFF;

            double windowAspect = double( WINDOW_HEIGHT ) / double( WINDOW_WIDTH );

            map < int, int > statParams = _getStatParams();
            for ( map < int, int >::iterator i = statParams.begin(); i != statParams.end(); i++ ) {
                int statValue = ( int ) character -> getParameter( i -> first );
                form -> addComponent( new StaticText( LangStrings::getString( i -> second ) + string( ": " ) + StringUtils::toString( statValue ), statButtonProperties ) );

                ComponentProperties addButtonProperties = statButtonProperties;
                addButtonProperties.x += statButtonProperties.w + 0.05;
                addButtonProperties.w = addButtonProperties.h * windowAspect;
                addButtonProperties.backgroundImage = 19;
                form -> addComponent( new _AddStatButton( i -> first, character, addButtonProperties ) );

                addButtonProperties.x += addButtonProperties.w + 0.02;
                addButtonProperties.backgroundImage = 20;
                form -> addComponent( new _SubStatButton( i -> first, character, addButtonProperties ) );

                statButtonProperties.y += statButtonProperties.h + 0.015;
            }
        }

        properties.w = 0.3;
        properties.y = 0.9 - properties.h;
        form -> addComponent( new BackUiButton( LangStrings::getString( LangStringsID::MENU_BACK ), properties ) );

        return form;
    }

    static const int TEXTURE_MOUSE_IN_BUTTON_ADD = 19;
    static const int TEXTURE_MOUSE_OUT_BUTTON_ADD = 20;
    static const int TEXTURE_MOUSE_IN_BUTTON_SUB = 21;
    static const int TEXTURE_MOUSE_OUT_BUTTON_SUB = 22;

    private:

    static bool _payStatPoint( EntityType* character, int pointsNumber, bool checkOnly ) {
        int points = MathUtils::roundToInt( character -> getParameter( EntityType::ATTRIBUTE_UNASSIGNED_STAT_POINTS ) );
        if ( points >= pointsNumber ) {
            if ( !checkOnly ) {
                points -= pointsNumber;
                character -> setParameter( EntityType::ATTRIBUTE_UNASSIGNED_STAT_POINTS, points );
            }
            return true;
        }
        return false;
    }

    static void _upgradeStat( EntityType* character, int statId, int upgradeByCount ) {
        int statValue = MathUtils::roundToInt( character -> getParameter( statId ) );
        statValue += upgradeByCount;
        character -> setParameter( statId, statValue );
    }

    class _AddStatButton : public UiButton {
        public:
        _AddStatButton( int statId, EntityType* character, ComponentProperties properties ) : UiButton( string( "" ), properties ) {
            _statId = statId;
            _character = character;
        }
        virtual ~_AddStatButton() {}
        protected:
        virtual bool clickAction() {
            if ( _payStatPoint( _character, 1, false ) ) {
                _upgradeStat( _character, _statId, 1 );
                FormCache::refreshAll();
                return true;
            }
            return false;
        }

        virtual int getButtonTextureEnabled() {
            return TEXTURE_MOUSE_IN_BUTTON_ADD;
        }

        virtual int getButtonTextureDisabled() {
            return TEXTURE_MOUSE_OUT_BUTTON_ADD;
        }

        private:
        int _statId;
        EntityType* _character;
    };

    class _SubStatButton : public UiButton {
        public:
        _SubStatButton( int statId, EntityType* character, ComponentProperties properties ) : UiButton( string( "" ), properties ) {
            _statId = statId;
            _character = character;
        }
        virtual ~_SubStatButton() {}
        protected:
        virtual bool clickAction() {
            int currentLevel = MathUtils::roundToInt( _character -> getParameter( _statId ) );
            if ( currentLevel <= ProfileOverviewFormDesign::getInitialValue( _statId ) ) {
                return false;
            }
            if ( _payStatPoint( _character, -1, false ) ) {
                _upgradeStat( _character, _statId, -1 );
                FormCache::refreshAll();
                return true;
            }
            return false;
        }

        virtual int getButtonTextureEnabled() {
            return TEXTURE_MOUSE_IN_BUTTON_SUB;
        }

        virtual int getButtonTextureDisabled() {
            return TEXTURE_MOUSE_OUT_BUTTON_SUB;
        }

        private:
        int _statId;
        EntityType* _character;
    };

    map < int, int > _getStatParams() {
        map < int, int > paramIds;
        int hpAttr = EntityType::ATTRIBUTE_HP;
        int eneAttr = EntityType::ATTRIBUTE_ENERGY_MAX;
        paramIds[ hpAttr ] = LangStringsID::CHARACTER_STAT_HP;
        paramIds[ eneAttr ] = LangStringsID::CHARACTER_STAT_ENERGY;
        return paramIds;
    }

};

