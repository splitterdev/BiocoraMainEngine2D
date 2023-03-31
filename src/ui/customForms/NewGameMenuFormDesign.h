#pragma once

class NewGameMenuFormDesign : public FormDesignFactory {
    public:
    NewGameMenuFormDesign() : FormDesignFactory() {
    }
    virtual ~NewGameMenuFormDesign() {
    }

    virtual Form* build( FormBuildParameters parameters ) {
        Form* form = new Form( parameters );

        ComponentProperties properties;
        properties.x = 0.1;
        properties.y = 0.1;
        properties.w = 0.3;
        properties.h = 0.1;
        //properties.backgroundImage = 1023;
        form -> addComponent( new StartGameButton( 0, 0, properties ) );

        properties.y = 0.9 - properties.h;
        form -> addComponent( new BackUiButton( LangStrings::getString( LangStringsID::MENU_BACK ), properties ) );

        return form;
    }
};

