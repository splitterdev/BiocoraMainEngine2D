#pragma once

const int SOUND_MENU_SELECT = 101;
const int SOUND_MENU_ERROR = 102;
const int SOUND_MENU_CONFIRMATION = 103;

class UiButton : public TextButton {
    public:

    static const int TEXTURE_MOUSE_IN = 4;
    static const int TEXTURE_MOUSE_OUT = 3;

    UiButton( string text, ComponentProperties properties ) : TextButton( text, properties ) {
        _setBaseParams( properties );
    }
    /*UiButton( string text, ComponentProperties properties, int textureActive ) : TextButton( text, properties ) {
        _textureIdMouseIn = textureActive;
        _setBaseParams( properties );
    }*/
    virtual ~UiButton() {
        if ( _selectionTexture ) {
            delete _selectionTexture;
        }
    }

    virtual void onMouseMoveIn( double x, double y ) {
        ComponentProperties properties = getProperties();
        properties.backgroundImage = getButtonTextureEnabled();
        setProperties( properties );
        bringToFront();
        _mouseIn = true;
        soundSelect();
    }

    virtual void onMouseMoveOut( double x, double y ) {
        ComponentProperties properties = getProperties();
        properties.backgroundImage = getButtonTextureDisabled();
        setProperties( properties );
        _mouseIn = false;
    }

    virtual void onTick() {
        scaleComponent( this, _scaleMin, _scaleMax, _mouseIn, &_scale );
    }

    static void scaleComponent( Component* component, double scaleMin, double scaleMax, bool mouseIn, double* scale ) {
        static const double STD_FPS = 60.0;
        static const double STD_TIME = 0.05;
        double scalingSpeed = - ( ( scaleMax - scaleMin ) / ( STD_TIME * STD_FPS ) );
        if ( mouseIn ) {
            scalingSpeed = ( - scalingSpeed );
        }
        double scaleCurrent = ( *scale ) + scalingSpeed;
        if ( scaleCurrent > scaleMax ) {
            scaleCurrent = scaleMax;
        }
        if ( scaleCurrent < scaleMin ) {
            scaleCurrent = scaleMin;
        }
        ( *scale ) = scaleCurrent;
        ComponentProperties properties = component -> getProperties();
        properties.scale = ( *scale );
        component -> setProperties( properties );
    }

    virtual void onClick( double x, double y ) {
        bool clickResult = clickAction();
        if ( clickResult ) {
            soundConfirm();
        } else {
            soundError();
        }
    }

    static void soundSelect() {
        Sound* sound = Cache::getFileAccessor() -> getSound( SOUND_MENU_SELECT );
        if ( sound ) {
            sound -> play();
        }
    }

    static void soundConfirm() {
        Sound* sound = Cache::getFileAccessor() -> getSound( SOUND_MENU_CONFIRMATION );
        if ( sound ) {
            sound -> play();
        }
    }

    static void soundError() {
        Sound* sound = Cache::getFileAccessor() -> getSound( SOUND_MENU_ERROR );
        if ( sound ) {
            sound -> play();
        }
    }

    static inline Texture* createDefaultSelectionTexture() {
        return createSelectionTexture( ComponentProperties(), 0xFF80E0FF, 2 );
    }

    protected:

    virtual void onDisplay() {
        if ( !_initialized ) {
            ComponentProperties properties = getProperties();
            properties.backgroundImage = getButtonTextureDisabled();
            setProperties( properties );
            _initialized = true;
        }
        TextButton::onDisplay();
        if ( ( _selectionTexture ) && ( _mouseIn ) ) {
            ComponentProperties properties = getProperties();
            properties.scale *= 1.2;
            Component::displayBackground( _selectionTexture, &properties );
        }
    }

    virtual bool clickAction() = 0;

    virtual int getButtonTextureEnabled() {
        return TEXTURE_MOUSE_IN;
    }

    virtual int getButtonTextureDisabled() {
        return TEXTURE_MOUSE_OUT;
    }

    static Texture* createSelectionTexture( ComponentProperties properties, Uint32 color, int border ) {
        double w = 256.0;//properties.w * properties.scale * double( WINDOW_WIDTH );
        double h = 64.0;//properties.h * properties.scale * double( WINDOW_HEIGHT );
        Texture* t = NULL;
        SDL_Surface* surface = SDL_CreateRGBSurface( 0, w, h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000 );
        if ( surface ) {
            SDL_FillRect( surface, NULL, color );
            SDL_Rect r;
            r.x = border;
            r.y = border;
            r.w = w - ( border << 1 );
            r.h = h - ( border << 1 );
            SDL_FillRect( surface, &r, 0x00FFFFFF );
            SDL_Texture* texture = SDL_CreateTextureFromSurface( Renderer::getRenderer(), surface );
            if ( texture ) {
                SDL_SetTextureBlendMode( texture, SDL_BLENDMODE_BLEND );
                t = new Texture( texture, w, h );
            }
            SDL_FreeSurface( surface );
        }
        return t;
    }

    private:

    void _setBaseParams( ComponentProperties properties ) {
        _scale = properties.scale;
        _scaleMin = 1.0;
        _scaleMax = 1.25;
        _mouseIn = false;
        _initialized = false;
        _selectionTexture = createDefaultSelectionTexture();
    }

    bool _mouseIn;
    bool _initialized;
    double _scale;
    double _scaleMin;
    double _scaleMax;

    Texture* _selectionTexture;

};

class ChangeFormUiButton : public UiButton {
    public:
    ChangeFormUiButton( int changeId, string text, ComponentProperties properties ) : UiButton( text, properties ) {
        _changeId = changeId;
    }
    virtual ~ChangeFormUiButton() {
    }
    protected:
    virtual bool clickAction() {
        if ( changeCondition() ) {
            FormCache::stackCurrentForm();
            FormCache::changeForm( _changeId );
            return true;
        }
        return false;
    }
    virtual bool changeCondition() {
        return true;
    }
    private:
    int _changeId;
};

class BackUiButton : public UiButton {
    public:
    BackUiButton( string text, ComponentProperties properties ) : UiButton( text, properties ) {
    }
    virtual ~BackUiButton() {
    }
    protected:
    virtual bool clickAction() {
        int formId;
        if ( FormCache::getLastStackedForm( &formId ) ) {
            FormCache::changeForm( formId );
            return true;
        }
        return false;
    }
};

class ExitUiButton : public UiButton {
    public:
    ExitUiButton( string text, ComponentProperties properties ) : UiButton( text, properties ) {
    }
    virtual ~ExitUiButton() {
    }
    protected:
    virtual bool clickAction() {
        EXIT_GAME_SIGNAL = 1;
        return true;
    }
};

class ResumeGameUiButton : public UiButton {
    public:
    ResumeGameUiButton( string text, ComponentProperties properties ) : UiButton( text, properties ) {
    }
    virtual ~ResumeGameUiButton() {
    }
    protected:
    virtual bool clickAction() {
        Perspective::ChangeTo( PERSPECTIVE_LEVEL_ID );
        return true;
    }
};

class DropGameUiButton : public UiButton {
    public:
    DropGameUiButton( string text, ComponentProperties properties ) : UiButton( text, properties ) {
    }
    virtual ~DropGameUiButton() {
    }
    protected:
    virtual bool clickAction() {
        Cache::dropGameStateHolder();
        FormCache::refreshPage();
        return true;
    }
};

class ChangeProfileUiButton : public UiButton {
    public:
    ChangeProfileUiButton( string text, int direction, ComponentProperties properties ) : UiButton( text, properties ) {
        _direction = direction;
    }
    virtual ~ChangeProfileUiButton() {
    }
    protected:
    virtual bool clickAction() {
        if ( PlayerManager::nextProfileId( _direction ) ) {
            FormCache::refreshAll();
            return true;
        }
        return false;
    }
    private:
    int _direction;
};

class ConfirmProfileDeleteUiButton : public UiButton {
    public:
    ConfirmProfileDeleteUiButton( string text, ComponentProperties properties ) : UiButton( text, properties ) {
    }
    virtual ~ConfirmProfileDeleteUiButton() {
    }
    protected:
    virtual bool clickAction() {
        if ( PlayerManager::removeProfile( PlayerManager::getProfileId() ) ) {
            int formId;
            if ( FormCache::getLastStackedForm( &formId ) ) {
                FormCache::changeForm( formId );
            }
            FormCache::refreshAll();
            return true;
        }
        return false;
    }
    private:
    int _direction;
};

class CharacterClassCache {
    private:
    CharacterClassCache() {}
    virtual ~CharacterClassCache() {}
    public:
    static int createdPlayerId;
};

int CharacterClassCache::createdPlayerId = -1;

class CharacterClassNameInfoBox : public Component {
    public:
    CharacterClassNameInfoBox( ComponentProperties properties ) : Component( properties ) {
    }
    virtual ~CharacterClassNameInfoBox() {
    }
    protected:
    virtual void onDisplay() {
        displayBackground();
        EntityType* characterType = Cache::getFileAccessor() -> getPlayerClass( CharacterClassCache::createdPlayerId );
        if ( characterType ) {
            Text::Color( 254, 254, 254 );
            int x = getProperties().x * WINDOW_WIDTH + 8;
            int y = getProperties().y * WINDOW_HEIGHT + 8;
            const int SPACING = 24;

            string characterName = string( "???" );
            if ( characterType ) {
                characterType -> getStringParameter( EntityType::ATTRIBUTE_STRING_NAME, characterName );
                Text::Write( Renderer::getRenderer(), x, y, "Character name: %s", characterName.c_str() );
                y += SPACING;

                Text::Write( Renderer::getRenderer(), x, y, "Base HP: %d", ( int ) characterType -> getParameter( EntityType::ATTRIBUTE_HP ) );
                y += SPACING;

                Text::Write( Renderer::getRenderer(), x, y, "Base energy: %d", ( int ) characterType -> getParameter( EntityType::ATTRIBUTE_ENERGY_MAX ) );
                y += SPACING;
            }

        }
    }
};

class CreateProfileUiButton : public UiButton {
    public:
    CreateProfileUiButton( string text, EditText* editText, ComponentProperties properties ) : UiButton( text, properties ) {
        _characterNameEditText = editText;
    }
    virtual ~CreateProfileUiButton() {
        CharacterClassCache::createdPlayerId = -1; // do not delete - for sake of forms refreshing
    }
    protected:
    virtual bool clickAction() {
        string name = _characterNameEditText -> getText();
        StringUtils::trim( name );
        if ( name.empty() ) {
            return false;
        }
        int classId = CharacterClassCache::createdPlayerId;
        if ( classId < 0 ) {
            return false;
        }
        int characterId = PlayerManager::createCharacter( classId );
        if ( characterId >= 0 ) {
            EntityType* characterType = Cache::getFileAccessor() -> getCharacter( characterId );
            characterType -> setStringParameter( EntityType::ATTRIBUTE_STRING_NAME, name );
            PlayerManager::setProfileId( characterId );
            int formId;
            if ( FormCache::getLastStackedForm( &formId ) ) {
                FormCache::changeForm( formId );
            }
            Cache::getFileAccessor() -> store();
            FormCache::refreshAll();
            CharacterClassCache::createdPlayerId = -1;
            return true;
        }
        return false;
    }
    private:
    EditText* _characterNameEditText;
};

class CharacterClassSettingUiButton : public UiButton {
    public:
    CharacterClassSettingUiButton( int characterClassId, string text, ComponentProperties properties ) : UiButton( text, properties ) {
        _characterClassId = characterClassId;
    }
    virtual ~CharacterClassSettingUiButton() {
    }
    protected:
    virtual bool clickAction() {
        CharacterClassCache::createdPlayerId = _characterClassId;
        return true;
    }
    private:
    int _characterClassId;
};

class PlayerClassSelectableList : public SelectableList {
    public:
    PlayerClassSelectableList( int visibleRows, vector < int > items, ComponentProperties properties ) : SelectableList( visibleRows, items.size(), properties ) {
        _items = items;
    }
    virtual ~PlayerClassSelectableList() {
    }

    static const int TEXTURE_UP_ENABLED = 11;
    static const int TEXTURE_DOWN_ENABLED = 12;
    static const int TEXTURE_UP_DISABLED = 13;
    static const int TEXTURE_DOWN_DISABLED = 14;

    private:
    vector < int > _items;
    protected:
    virtual Component* buildData( int rowId ) {
        if ( ( rowId < 0 ) || ( rowId >= int( _items.size() ) ) ) {
            return NULL;
        }
        int classId = _items[ rowId ];
        EntityType* character = Cache::getFileAccessor() -> getPlayerClass( classId );
        string name = "???";
        if ( character ) {
            character -> getStringParameter( EntityType::ATTRIBUTE_STRING_NAME, name );
            name = string( "Class: " ) + name;
        }
        ComponentProperties properties;
        return new CharacterClassSettingUiButton( classId, name, properties );
    }

    virtual Component* getArrowUp( int direction ) {
        return new CustomArrow( direction, this, TEXTURE_UP_ENABLED, TEXTURE_UP_DISABLED );
    }

    virtual Component* getArrowDown( int direction ) {
        return new CustomArrow( direction, this, TEXTURE_DOWN_ENABLED, TEXTURE_DOWN_DISABLED );
    }

    class CustomArrow : public SelectableList::Arrow {
        public:
        CustomArrow( int dir, SelectableList* selList, int texEnabled, int texDisabled ) : SelectableList::Arrow( dir, selList, ComponentProperties(), texEnabled, texDisabled ) {
            _selectionTexture = UiButton::createDefaultSelectionTexture();
            _mouseIn = false;
        }
        virtual ~CustomArrow() {
            if ( _selectionTexture ) {
                delete _selectionTexture;
            }
        }
        virtual void clickAction( bool switched ) {
            if ( switched ) {
                UiButton::soundConfirm();
            } else {
                UiButton::soundError();
            }
        }
        virtual void onMouseMoveIn( double x, double y ) {
            Arrow::onMouseMoveIn( x, y );
            UiButton::soundSelect();
            _mouseIn = true;
            bringToFront();
        }
        virtual void onMouseMoveOut( double x, double y ) {
            Arrow::onMouseMoveOut( x, y );
            _mouseIn = false;
        }
        virtual void onDisplay() {
            SelectableList::Arrow::onDisplay();
            if ( ( _selectionTexture ) && ( _mouseIn ) ) {
                ComponentProperties properties = getProperties();
                properties.scale *= 1.2;
                Component::displayBackground( _selectionTexture, &properties );
            }
        }
        private:
        Texture* _selectionTexture;
        bool _mouseIn;
    };

};

bool isValidLevelPending() {
    if ( Cache::getGameStateHolder() ) {
        return Cache::getGameStateHolder() -> getPlayer() != NULL;
    }
    return false;
}

#include "../custom/LevelSelectableList.h"

#include "MainMenuFormDesign.h"
#include "PlayMenuFormDesign.h"
#include "NewGameMenuFormDesign.h"
#include "SelectLevelMenuFormDesign.h"
#include "ExitConfirmationMenuFormDesign.h"
#include "InventoryInGameForm.h"
#include "SelectChapterMenuFormDesign.h"

#include "ProfileOverviewFormDesign.h"
#include "NewProfileFormDesign.h"
#include "DeleteProfileFormDesign.h"
#include "UpgradeCharacterFormDesign.h"

void FormCache::Initialize() {
        FormDesignFactory::setFactory( FORM_MAIN_MENU, new MainMenuFormDesign() );
        FormDesignFactory::setFactory( FORM_PLAY_MENU, new PlayMenuFormDesign() );
        FormDesignFactory::setFactory( FORM_NEW_GAME_MENU, new NewGameMenuFormDesign() );
        FormDesignFactory::setFactory( FORM_SELECT_LEVEL_MENU, new SelectLevelMenuFormDesign() );
        FormDesignFactory::setFactory( FORM_SELECT_CHAPTER_MENU, new SelectChapterMenuFormDesign() );
        FormDesignFactory::setFactory( FORM_EXIT_CONFIRMATION_MENU, new ExitConfirmationMenuFormDesign() );
        FormDesignFactory::setFactory( INGAME_FORM_INVENTORY, new InventoryInGameForm() );
        FormDesignFactory::setFactory( FORM_PROFILES_MENU, new ProfileOverviewFormDesign() );
        FormDesignFactory::setFactory( FORM_NEW_PROFILE_MENU, new NewProfileFormDesign() );
        FormDesignFactory::setFactory( FORM_DELETE_PROFILE_MENU, new DeleteProfileFormDesign() );
        FormDesignFactory::setFactory( FORM_STATS_MENU, new UpgradeCharacterFormDesign() );
    }
