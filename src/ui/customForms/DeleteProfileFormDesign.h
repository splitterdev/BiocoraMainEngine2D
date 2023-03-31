#pragma once

class DeleteProfileFormDesign : public FormDesignFactory {
    public:
    DeleteProfileFormDesign() : FormDesignFactory() {
    }
    virtual ~DeleteProfileFormDesign() {
    }

    virtual Form* build( FormBuildParameters parameters ) {
        Form* form = new Form( parameters );

        ComponentProperties properties;
        properties.x = 0.1;
        properties.y = 0.1;
        properties.w = 0.3;
        properties.h = 0.1;

        form -> addComponent( new ConfirmProfileDeleteUiButton( LangStrings::getString( LangStringsID::MENU_CONFIRM ), properties ) );

        properties.y = 0.9 - properties.h;
        form -> addComponent( new BackUiButton( LangStrings::getString( LangStringsID::MENU_BACK ), properties ) );

        return form;
    }
};

