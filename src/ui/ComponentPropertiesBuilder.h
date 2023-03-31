#pragma once

class ComponentPropertiesBuilder {
    protected:
    ComponentPropertiesBuilder() { }
    public:

    virtual ~ComponentPropertiesBuilder() { }

    ComponentPropertiesBuilder getInstance() {
        return ComponentPropertiesBuilder();
    }

    ComponentPropertiesBuilder withPosition( double x, double y ) {
        _props.x = x;
        _props.y = y;
        return * this;
    }

    ComponentPropertiesBuilder withSize( double w, double h ) {
        _props.w = w;
        _props.h = h;
        return * this;
    }

    ComponentPropertiesBuilder withPositionAndSize( double x, double y, double w, double h ) {
        _props.x = x;
        _props.y = y;
        _props.w = w;
        _props.h = h;
        return * this;
    }

    ComponentPropertiesBuilder withColor( uint32_t color ) {
        _props.fillColor = color;
        return * this;
    }

    ComponentPropertiesBuilder withColor( int r, int g, int b ) {
        _props.fillColor = (
            ( ( r & 0xFF ) << 16 ) +
            ( ( g & 0xFF ) << 8 ) +
            ( ( b & 0xFF ) )
        );
        return * this;
    }

    ComponentPropertiesBuilder withColorDouble( double r, double g, double b ) {
        if ( r < 0.0 ) {
            r = 0.0;
        } else if ( r > 1.0 ) {
            r = 1.0;
        }
        if ( g < 0.0 ) {
            g = 0.0;
        } else if ( g > 1.0 ) {
            g = 1.0;
        }
        if ( b < 0.0 ) {
            b = 0.0;
        } else if ( b > 1.0 ) {
            b = 1.0;
        }
        _props.fillColor = (
            ( ( int( r ) * 0xFF ) << 16 ) +
            ( ( int( g ) * 0xFF ) << 8 ) +
            ( ( int( b ) * 0xFF ) )
        );
        return * this;
    }

    ComponentPropertiesBuilder withImage( int imageID ) {
        _props.backgroundImage = imageID;
        return * this;
    }

    ComponentPropertiesBuilder withAlpha( double alpha ) {
        _props.alpha = alpha;
        return * this;
    }

    ComponentPropertiesBuilder withWeight( double weight ) {
        _props.weight = weight;
        return * this;
    }

    ComponentPropertiesBuilder rotateLeft90( int times ) {
        _props.rotation += times;
        return * this;
    }

    ComponentPropertiesBuilder rotateRight90( int times ) {
        _props.rotation -= times;
        return * this;
    }

    ComponentPropertiesBuilder withRotation( int rotation ) {
        _props.rotation = rotation;
        return * this;
    }

    ComponentPropertiesBuilder show() {
        _props.visible = true;
        return * this;
    }

    ComponentPropertiesBuilder hide() {
        _props.visible = false;
        return * this;
    }

    ComponentPropertiesBuilder enable() {
        _props.enabled = true;
        return * this;
    }

    ComponentPropertiesBuilder disable() {
        _props.enabled = false;
        return * this;
    }

    ComponentPropertiesBuilder withTranslation( double x, double y ) {
        _props.x_translation = x;
        _props.y_translation = y;
        return * this;
    }

    ComponentProperties build() {
        return _props;
    }

    private:

    ComponentProperties _props;

};


