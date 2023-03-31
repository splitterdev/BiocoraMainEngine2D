#pragma once

class ProfileOverviewFormDesign : public FormDesignFactory {
    protected:

    class ProfileInformation : public Component {
        public:
        ProfileInformation( ComponentProperties properties ) : Component( properties ) {
        }
        virtual ~ProfileInformation() {
        }
        virtual void onDisplay() {
            displayBackground();
            EntityType* character = Cache::getFileAccessor() -> getCharacter( PlayerManager::getProfileId() );
            if ( character ) {
                _printProfileInfo( character );
            }
        }
        private:
        void _printProfileInfo( EntityType* character ) {
            Text::Color( 254, 254, 254 );
            int x = getProperties().x * WINDOW_WIDTH + 8;
            int y = getProperties().y * WINDOW_HEIGHT + 8;
            const int SPACING = 24;

            string characterName = "???";
            character -> getStringParameter( EntityType::ATTRIBUTE_STRING_NAME, characterName );
            Text::Write( Renderer::getRenderer(), x, y, "Name: %s", characterName.c_str() );
            y += SPACING;

            Text::Write( Renderer::getRenderer(), x, y, "Character level: %d", ( int ) character -> getParameter( EntityType::ATTRIBUTE_LEVEL ) );
            y += SPACING;

            Text::Write( Renderer::getRenderer(), x, y, "Exp: %d/%d",
                ( int ) character -> getParameter( EntityType::ATTRIBUTE_EXP ),
                ( int ) character -> getParameter( EntityType::ATTRIBUTE_NEXP )
            );
            y += SPACING << 1;

            Text::Write( Renderer::getRenderer(), x, y, "HP: %d", ( int ) character -> getParameter( EntityType::ATTRIBUTE_HP ) );
            y += SPACING;

            Text::Write( Renderer::getRenderer(), x, y, "Energy: %d (%g)",
                ( int ) character -> getParameter( EntityType::ATTRIBUTE_ENERGY_MAX ),
                ( double ) character -> getParameter( EntityType::ATTRIBUTE_ENERGY_RESTORE_LEVEL )
            );
            y += SPACING << 1;

            Text::Write( Renderer::getRenderer(), x, y, "Health buffer: %g/%g/%g",
                ( double ) character -> getParameter( EntityType::ATTRIBUTE_HEALTH_BUFFER_UNIT_CAPACITY ),
                ( double ) character -> getParameter( EntityType::ATTRIBUTE_HEALTH_BUFFER_UNIT_CONSUMPTION_RATIO ),
                ( double ) character -> getParameter( EntityType::ATTRIBUTE_HEALTH_BUFFER_UNIT_CONSUMPTION_SPEED )
            );
            y += SPACING;

            y = ( getProperties().y + getProperties().h ) * WINDOW_HEIGHT - 8 - SPACING;
            Text::Write( Renderer::getRenderer(), x, y, "Unassigned points: %d", ( int ) character -> getParameter( EntityType::ATTRIBUTE_UNASSIGNED_STAT_POINTS ) );
        }
    };

    public:

    static int getInitialValue( int statId ) {
        map < int, int >::iterator i = _cachedInitialValues.find( statId );
        if ( i != _cachedInitialValues.end() ) {
            return i -> second;
        }
        return 0;
    }

    ProfileOverviewFormDesign() : FormDesignFactory() {
    }
    virtual ~ProfileOverviewFormDesign() {
    }

    virtual Form* build( FormBuildParameters parameters ) {
        Form* form = new Form( parameters );

        ComponentProperties properties;
        // profile overview
        properties.x = 0.6;
        properties.y = 0.1;
        properties.w = 0.3;
        properties.h = 0.8;
        properties.fillColor = 0x80202020;
        form -> addComponent( new ProfileInformation( properties ) );

        // menu
        properties.x = 0.1;
        properties.y = 0.1;
        properties.w = 0.3;
        properties.h = 0.1;

        form -> addComponent( new ChangeFormUiButton( FORM_NEW_PROFILE_MENU, LangStrings::getString( LangStringsID::MENU_NEW_PROFILE ), properties ) );

        properties.y += properties.h + 0.02;
        form -> addComponent( new _ChangeToDeleteFormUiButton( FORM_DELETE_PROFILE_MENU, LangStrings::getString( LangStringsID::MENU_DELETE_PROFILE ), properties ) );

        properties.y += properties.h + 0.02;
        form -> addComponent( new ChangeProfileUiButton( LangStrings::getString( LangStringsID::MENU_PREV_PROFILE ), -1, properties ) );

        properties.y += properties.h + 0.02;
        form -> addComponent( new ChangeProfileUiButton( LangStrings::getString( LangStringsID::MENU_NEXT_PROFILE ), 1, properties ) );

        properties.y += properties.h + 0.02;
        form -> addComponent( new _ChangeToOverviewFormUiButton( FORM_STATS_MENU, LangStrings::getString( LangStringsID::MENU_STATS ), properties ) );

        properties.y = 0.9 - properties.h;
        form -> addComponent( new BackUiButton( LangStrings::getString( LangStringsID::MENU_BACK ), properties ) );

        return form;
    }

    private:
    class _ChangeToDeleteFormUiButton : public ChangeFormUiButton {
        public:
        _ChangeToDeleteFormUiButton( int changeId, string text, ComponentProperties properties ) : ChangeFormUiButton( changeId, text, properties ) {}
        virtual ~_ChangeToDeleteFormUiButton() {}
        protected:
        virtual bool changeCondition() {
            return PlayerManager::getProfilesCount() > 1;
        }
    };

    class _ChangeToOverviewFormUiButton : public ChangeFormUiButton {
        public:
        _ChangeToOverviewFormUiButton( int changeId, string text, ComponentProperties properties ) : ChangeFormUiButton( changeId, text, properties ) {}
        virtual ~_ChangeToOverviewFormUiButton() {}
        protected:
        virtual bool clickAction() {
            _setCachedInitialValues();
            return ChangeFormUiButton::clickAction();
        }
    };

    static void _setCachedInitialValues() {
        map < int, int > newCached;
        int hpAttr = EntityType::ATTRIBUTE_HP;
        int eneAttr = EntityType::ATTRIBUTE_ENERGY_MAX;
        EntityType* character = Cache::getFileAccessor() -> getCharacter( PlayerManager::getProfileId() );
        if ( character ) {
            newCached[ hpAttr ] = MathUtils::roundToInt( character -> getParameter( hpAttr ) );
            newCached[ eneAttr ] = MathUtils::roundToInt( character -> getParameter( eneAttr ) );
        }
        _cachedInitialValues = newCached;
    }

    static map < int, int > _cachedInitialValues;

};

map < int, int > ProfileOverviewFormDesign::_cachedInitialValues;



