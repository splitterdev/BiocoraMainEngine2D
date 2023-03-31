#pragma once

class ExitConfirmationMenuFormDesign : public FormDesignFactory {
    public:
    ExitConfirmationMenuFormDesign() : FormDesignFactory() {

    }
    virtual ~ExitConfirmationMenuFormDesign() {
    }

    virtual Form* build( FormBuildParameters parameters ) {
        Form* form = new Form( parameters );

        ComponentProperties properties;
        properties.x = 0.1;
        properties.w = 0.3;
        properties.h = 0.1;

        properties.y = 0.9 - properties.h * 2.0 - 0.02;
        form -> addComponent( new ExitUiButton( LangStrings::getString( LangStringsID::MENU_YES ), properties ) );

        properties.y = 0.9 - properties.h;
        form -> addComponent( new ChangeFormUiButton( FORM_MAIN_MENU, LangStrings::getString( LangStringsID::MENU_NO ), properties ) );

        return form;
    }

};


