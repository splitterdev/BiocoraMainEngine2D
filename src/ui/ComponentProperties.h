#pragma once

class ComponentProperties {
    public:
    ComponentProperties() {
        x = 0.0;
        y = 0.0;
        w = 1.0;
        h = 1.0;
        x_translation = 0.0;
        y_translation = 0.0;
        //center_x = 0.5;
        //center_y = 0.5;
        rotation = 0;
        backgroundImage = -1;
        fillColor = 0xFF000000;
        weight = 1.0;
        scale = 1.0;
        alpha = 1.0;
        visible = true;
        enabled = true;
    }
    ~ComponentProperties() {
    }

    /*void center() {
        center_x = ( x + w / 2.0 );
        center_y = ( y + h / 2.0 );
    }*/

    ComponentProperties* allocate() {
        ComponentProperties* p = new ComponentProperties();
        ( *p ) = ( *this );
        return p;
    }

    double x;
    double y;
    double w;
    double h;

    double alpha;

    double x_translation;
    double y_translation;

    //double center_x;
    //double center_y;

    double rotation;

    double weight;

    double scale;

    int backgroundImage;
    unsigned int fillColor;

    bool visible;
    bool enabled;

    ComponentProperties & fitToTexture( Texture* texture ) {
        int tWidth = texture -> getWidth();
        int tHeight = texture -> getHeight();
        double aspect = double( tHeight ) / double( tWidth );
        if ( tWidth > tHeight ) {
            double nextH = h * aspect;
            y += ( h - nextH ) / 2.0;
            h = nextH;
        } else {
            double nextW = w / aspect;
            x += ( w - nextW ) / 2.0;
            w = nextW;
        }
        return * this;
    }

    inline SDL_Rect getSDLRectRawDisplay() {
        return Draw::getSDLRect( x * double( WINDOW_WIDTH ), y * double( WINDOW_HEIGHT ), w * double( WINDOW_WIDTH ), h * double( WINDOW_HEIGHT ) );
    }

    inline ComponentProperties & applyDimensions( double iX, double iY, double iW, double iH ) {
        x = iX;
        y = iY;
        w = iW;
        h = iH;
        return * this;
    }

};


