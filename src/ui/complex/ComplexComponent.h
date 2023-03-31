#pragma once

class ComplexComponent : public Component {
    public:
    ComplexComponent( ComponentProperties properties ) : Component( properties ) {
        _dirty = true;
    }
    virtual ~ComplexComponent() {
    }

    protected:

    virtual void onDisplay() {
    }

    virtual void onTick() {
        if ( _dirty ) {
            _dirty = false;
            vector < int64_t > oldComponents = getChildComponents();
            for ( vector < int64_t >::iterator i = oldComponents.begin(); i != oldComponents.end(); i++ ) {
                Component* oldComponent = getComponent( *i );
                if ( oldComponent ) {
                    if ( deleteChildComponent( oldComponent -> getID() ) ) {
                        if ( !_isNotCleared( oldComponent ) ) {
                            delete oldComponent;
                        }
                    }
                }
            }
            list < Component* > components = buildStructure();
            for ( list < Component* >::iterator i = components.begin(); i != components.end(); i++ ) {
                Component* component = *i;
                if ( !addComponent( component ) ) {
                    if ( component != NULL && !_isNotCleared( component ) ) {
                        delete component;
                    }
                }
            }
            afterRebuild();
        }
    }

    virtual void reset() {
        _dirty = true;
    }

    virtual void afterRebuild() {
    }

    protected:

    virtual list < Component* > buildStructure() = 0;

    void setNotCleared( Component* component ) {
        if ( component ) {
            _notClearedOnDirty[ component -> getID() ] = component;
            printf( "Component not cleared: %d\n", int( component -> getID() ) );
        }
    }

    private:

    map < int64_t, Component* > _notClearedOnDirty;

    inline bool _isNotCleared( Component* component ) {
        if ( component ) {
            map < int64_t, Component* >::iterator i = _notClearedOnDirty.find( component -> getID() );
            if ( i != _notClearedOnDirty.end() ) {
                if ( component == i -> second ) {
                    return true;
                }
                _notClearedOnDirty.erase( i );
            }
        }
        return false;
    }

    bool _dirty;

};


