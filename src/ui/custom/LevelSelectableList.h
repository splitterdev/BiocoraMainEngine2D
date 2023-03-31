#pragma once

#include "StartGameButton.h"

class SelectorItem {
    public:
    SelectorItem() {
    }
    virtual ~SelectorItem() {
    }
    int chapterId;
    int levelId;
    ComponentProperties properties;
};

class LevelSelectableList : public SelectableList {
    public:
    LevelSelectableList( int visibleRows, vector < SelectorItem > items, ComponentProperties properties ) : SelectableList( visibleRows, items.size(), properties ) {
        _items = items;
    }
    virtual ~LevelSelectableList() {
    }

    static const int TEXTURE_UP_ENABLED = 11;
    static const int TEXTURE_DOWN_ENABLED = 12;
    static const int TEXTURE_UP_DISABLED = 13;
    static const int TEXTURE_DOWN_DISABLED = 14;

    private:
    vector < SelectorItem > _items;
    protected:
    virtual Component* buildData( int rowId ) {
        if ( ( rowId < 0 ) || ( rowId >= int( _items.size() ) ) ) {
            return NULL;
        }
        SelectorItem item = _items[ rowId ];
        return new StartGameButton( item.chapterId, item.levelId, item.properties );
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

