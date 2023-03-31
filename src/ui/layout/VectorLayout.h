#pragma once

class VectorLayout : public Layout {
    public:

    enum Orientation {
        HORIZONTAL, VERTICAL
    };

    VectorLayout( ComponentProperties properties ) : Layout( properties ) {
        _orientation = true;
        _wsum = 0.0;
    }
    virtual ~VectorLayout() {
    }

    Orientation getOrientation() {
        return _orientation ? VERTICAL : HORIZONTAL;
    }

    void setOrientation( Orientation orientation ) {
        if ( orientation == HORIZONTAL ) {
            _orientation = false;
        } else {
            _orientation = true;
        }
        onAddComponent( NULL );
    }

    Component* getComponentInLayout( int index ) {
        if ( ( index >= 0 ) && ( index < int( _components.size() ) ) ) {
            return getComponent( _components[ index ] );
        }
        return NULL;
    }

    int getComponentCount() {
        return _components.size();
    }

    int getComponentIndexAt( double x, double y ) {
        ComponentProperties properties = getProperties();
        if ( _orientation ) {
            y = ( y - properties.y ) / properties.h;
            return y * double( _components.size() );
        } else {
            x = ( x - properties.x ) / properties.w;
            return x * double( _components.size() );
        }
        return -1;
    }

    vector < Component* > getComponents() {
        vector < Component* > vec;
        for ( vector < int64_t >::iterator i = _components.begin(); i != _components.end(); i++ ) {
            Component* c = getComponent( *i );
            vec.push_back( c );
        }
        return vec;
    }

    Component* destroyComponent( int index ) {
        if ( ( index >= 0 ) && ( index < int( _components.size() ) ) ) {
            vector < int64_t >::iterator i = _components.begin() + index;
            Component* c = getComponent( *i );
            if ( c ) {
                c -> unlinkParent();
                onAddComponent( NULL );
            }
            _components.erase( i );
            return c;
        }
        return NULL;
    }

    virtual void rescaleContent() {
        ComponentProperties myProp = getProperties();
        double weightCounter = 0.0;
        int j = 0;
        for ( vector < int64_t >::iterator i = _components.begin(); i != _components.end(); i++ ) {
            Component* c = getComponent( *i );
            if ( c ) {
                ComponentProperties properties = c -> getProperties();
                if ( _orientation ) {
                    properties.h = myProp.h * properties.weight / _wsum;
                    properties.w = myProp.w;
                    properties.y = myProp.y + weightCounter / _wsum * myProp.h;
                    properties.x = myProp.x;
                } else {
                    properties.h = myProp.h;
                    properties.w = myProp.w * properties.weight / _wsum;
                    properties.y = myProp.y;
                    properties.x = myProp.x + weightCounter / _wsum * myProp.w;
                }
                weightCounter += properties.weight;
                c -> setProperties( properties );
                Layout* l = dynamic_cast < Layout* > ( c );
                if ( l ) {
                    l -> rescaleContent();
                }
            }
            j++;
        }
    }

    protected:

    virtual void onAddComponent( Component* component ) {
        ComponentProperties myProp = getProperties();
        if ( component ) {
            _components.push_back( component -> getID() );
            ComponentProperties compProperties = component -> getProperties();
            compProperties.x = myProp.x;
            compProperties.y = myProp.y;
            compProperties.w = myProp.w;
            compProperties.h = myProp.h;
            component -> setProperties( compProperties );
            _wsum += compProperties.weight;
        }
        rescaleContent();
    }

    private:

    vector < int64_t > _components;
    bool _orientation;
    double _wsum;

};


