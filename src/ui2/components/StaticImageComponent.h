#pragma once

class StaticImageComponent : public Component {
    public:
    StaticImageComponent( int textureId, double x, double y, double w, double h ) : Component( textureId, x, y, w, h ) {
    }
    virtual ~StaticImageComponent() {
    }

    virtual void onDisplay( double x, double y, double w, double h ) {
        displayBackground( x, y, w, h );
    }

    virtual void onClick() {
        printf( "Clicked %p [%d]\n", this, getTextureId() );
    }

};

