#pragma once

class TextButton : public Button {
    public:
    TextButton( string label, ComponentProperties properties ) : Button( properties ) {
        _labelTexture = NULL;
        setText( label );
        _textCenter = true;
    }
    TextButton( const char* label, ComponentProperties properties ) : Button( properties ) {
        _labelTexture = NULL;
        setText( string( label ) );
        _textCenter = true;
    }
    virtual ~TextButton() {
        if ( _labelTexture ) {
            delete _labelTexture;
        }
    }

    virtual string getText() {
        return _label;
    }

    virtual void setText( string label ) {
        _label = label;
        if ( _labelTexture ) {
            delete _labelTexture;
        }
        int prevSize = Text::getSize();
        Text::setSize( Text::SIZE_MENU );
        Text::Color( 254, 254, 254 );
        SDL_Surface* surface = Text::RenderOnly( "%s", label.c_str() );
        Text::setSize( prevSize );
        _labelTexture = new Texture( surface );
        SDL_FreeSurface( surface );
    }

    void textCenter( bool center ) {
        _textCenter = center;
    }

    protected:

    virtual void onDisplay() {
        Button::onDisplay();
        if ( _labelTexture ) {
            double scale = 0.8;
            double horzScale = 0.6;
            ComponentProperties properties = getProperties();
            double textAspect = double( _labelTexture -> getWidth() ) / double( _labelTexture -> getHeight() ) * horzScale;
            double npH = properties.h * scale;
            properties.y += ( properties.h - npH ) / 2.0;
            properties.h = npH;
            double npW = npH * textAspect;
            properties.x += ( properties.w - npW ) / 2.0;
            properties.w = npW;
            if ( !_textCenter ) {
                properties.x = getProperties().x + 0.01;
            }
            Component::displayBackground( _labelTexture, &properties );
        }
    }

    private:

    string _label;
    Texture* _labelTexture;
    bool _textCenter;

};


