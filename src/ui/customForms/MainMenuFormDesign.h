#pragma once

class MainMenuFormDesign : public FormDesignFactory {
    public:
    MainMenuFormDesign() : FormDesignFactory() {
    }
    virtual ~MainMenuFormDesign() {
    }

    virtual Form* build( FormBuildParameters parameters ) {
        Form* form = new Form( parameters );

        ComponentProperties properties;
        properties.x = 0.1;
        properties.y = 0.1;
        properties.w = 0.3;
        properties.h = 0.1;

        if ( isValidLevelPending() ) {
            form -> addComponent( new ResumeGameUiButton( LangStrings::getString( LangStringsID::MENU_RESUME ), properties ) );
            properties.y += properties.h + 0.02;
        }

        form -> addComponent( new ChangeFormUiButton( FORM_PLAY_MENU, LangStrings::getString( LangStringsID::MENU_PLAY ), properties ) );
        properties.y += properties.h + 0.02;

        if ( !isValidLevelPending() ) {
            form -> addComponent( new ChangeFormUiButton( FORM_PROFILES_MENU, LangStrings::getString( LangStringsID::MENU_SELECT_PROFILE ), properties ) );
            properties.y += properties.h + 0.02;
        }

        properties.y = 0.9 - properties.h;

        if ( isValidLevelPending() ) {
            form -> addComponent( new DropGameUiButton( LangStrings::getString( LangStringsID::MENU_DROP_GAME ), properties ) );
        } else {
            form -> addComponent( new ChangeFormUiButton( FORM_EXIT_CONFIRMATION_MENU, LangStrings::getString( LangStringsID::MENU_EXIT ), properties ) );
        }

        return form;
    }
};

