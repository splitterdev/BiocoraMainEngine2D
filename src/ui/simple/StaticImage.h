#pragma once

class StaticImage : public Component {
    public:
    StaticImage( ComponentProperties properties ) : Component( properties ) {
    }
    virtual ~StaticImage() {
    }

    inline static StaticImage* create( int textureID, double x, double y, double w, double h ) {
        return create( textureID, x, y, w, h, 1.0 );
    }

    static StaticImage* create( int textureID, double x, double y, double w, double h, double alpha ) {
        ComponentProperties properties;
        properties.backgroundImage = textureID;
        properties.x = x;
        properties.y = y;
        properties.w = w;
        properties.h = h;
        properties.alpha = alpha;
        return new StaticImage( properties );
    }

    virtual void onDisplay() {
        displayBackground();
    }

};



