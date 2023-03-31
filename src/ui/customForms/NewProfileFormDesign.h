#pragma once

class NewProfileFormDesign : public FormDesignFactory {
    public:
    NewProfileFormDesign() : FormDesignFactory() {
    }
    virtual ~NewProfileFormDesign() {
    }

    virtual Form* build( FormBuildParameters parameters ) {
        Form* form = new Form( parameters );

        ComponentProperties properties;
        properties.x = 0.1;
        properties.y = 0.1;
        properties.w = 0.8;
        properties.h = 0.1;
        properties.fillColor = 0x80808080;

        EditText* editText = new EditText( "New name", 32, properties );
        form -> addComponent( editText );

        properties.w = 0.3;
        properties.y += properties.h + 0.02;
        properties.h = 0.4;

        form -> addComponent( new PlayerClassSelectableList( 3, Cache::getFileAccessor() -> getCharacterIds(), properties ) );

        properties.x = 0.6;
        properties.w = 0.3;

        form -> addComponent( new CharacterClassNameInfoBox( properties ) );

        properties.x = 0.1;
        properties.h = 0.1;

        properties.y = 0.9 - properties.h;
        form -> addComponent( new BackUiButton( LangStrings::getString( LangStringsID::MENU_BACK ), properties ) );

        properties.y -= properties.h + 0.02;
        form -> addComponent( new CreateProfileUiButton( LangStrings::getString( LangStringsID::MENU_CONFIRM ), editText, properties ) );

        return form;
    }
};
