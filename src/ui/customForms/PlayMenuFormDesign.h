#pragma once

class PlayMenuFormDesign : public FormDesignFactory {
    public:
    PlayMenuFormDesign() : FormDesignFactory() {
    }
    virtual ~PlayMenuFormDesign() {
    }

    virtual Form* build( FormBuildParameters parameters ) {
        Form* form = new Form( parameters );

        ComponentProperties properties;

        properties.x = 0.1;
        properties.y = 0.1;
        properties.w = 0.3;
        properties.h = 0.1;

        form -> addComponent( new ChangeFormUiButton( FORM_NEW_GAME_MENU, LangStrings::getString( LangStringsID::MENU_NEW_GAME ), properties ) );

        properties.y += properties.h * 1.25;
        form -> addComponent( new ChangeFormUiButton( FORM_SELECT_LEVEL_MENU, LangStrings::getString( LangStringsID::MENU_SELECT_LEVEL ), properties ) );

        properties.y = 0.9 - properties.h;
        form -> addComponent( new BackUiButton( LangStrings::getString( LangStringsID::MENU_BACK ), properties ) );

        return form;
    }
};

