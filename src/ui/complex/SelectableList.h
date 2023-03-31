#pragma once

class SelectableList : public ComplexComponent {
    public:
    SelectableList( int visibleRows, int totalRows, ComponentProperties properties ) : ComplexComponent( properties ) {
        _selectedRow = 0;
        _visibleRows = visibleRows;
        _totalRows = totalRows;
        _orientation = VectorLayout::VERTICAL;
        _arrowUp = NULL;
        _arrowDown = NULL;
    }
    virtual ~SelectableList() {
        if ( _arrowUp ) {
            deleteChildComponent( _arrowUp -> getID() );
            delete _arrowUp;
        }
        if ( _arrowDown ) {
            deleteChildComponent( _arrowDown -> getID() );
            delete _arrowDown;
        }
    }

    void clear() {
        reset();
        _totalRows = 0;
    }

    void setTotalRows( int totalRows ) {
        reset();
        _totalRows = totalRows;
    }

    void selectRow( int rowID ) {
        _selectedRow = rowID;
    }

    protected:

    list < Component* > buildStructure() {
        VectorLayout* layout = new VectorLayout( getProperties() );
        layout -> setOrientation( _orientation );
        if ( !_arrowUp ) {
            _arrowUp = getArrowUp( -1 );
            setNotCleared( _arrowUp );
        }
        if ( !_arrowDown ) {
            _arrowDown = getArrowDown( 1 );
            setNotCleared( _arrowDown );
        }
        layout -> addComponent( _arrowUp );
        int visibleRowLimit = min( _visibleRows, _totalRows );
        for ( int i = 0; i < visibleRowLimit; i++ ) {
            Component* component = buildData( _selectedRow + i );
            if ( !component ) {
                component = getBlank();
            }
            if ( !layout -> addComponent( component ) ) {
                if ( component ) {
                    delete component;
                }
            }
        }
        for ( int i = visibleRowLimit; i < _visibleRows; i++ ) {
            Component* component = getBlank();
            if ( !layout -> addComponent( component ) ) {
                if ( component ) {
                    delete component;
                }
            }
        }
        layout -> addComponent( _arrowDown );
        list < Component* > components = unwrap( layout );
        delete layout;
        return components;
    }

    virtual Component* buildData( int rowID ) = 0;

    virtual Component* getBlank() {
        ComponentProperties props;
        props.fillColor = 0x00000000;
        return new StaticImage( props );
    }

    class Arrow : public Button {
        public:
        Arrow( int dir, SelectableList* selList, ComponentProperties properties, int texIdEnabled, int texIdDisabled ) : Button( properties ) {
            _dir = dir;
            _selList = selList;
            _texIdEnabled = texIdEnabled;
            _texIdDisabled = texIdDisabled;
            properties.backgroundImage = _texIdDisabled;
            setProperties( properties );
        }
        virtual ~Arrow() {
        }
        virtual void onClick( double x, double y ) {
            bool switched = false;
            if ( _dir > 0 && _selList -> _selectedRow < _selList -> _totalRows - _dir ) {
                _selList -> _selectedRow += _dir;
                switched = true;
            } else if ( _dir < 0 && _selList -> _selectedRow >= ( - _dir ) ) {
                _selList -> _selectedRow += _dir;
                switched = true;
            }
            clickAction( switched );
            _selList -> reset();
        }

        virtual void onMouseMoveIn( double x, double y ) {
            ComponentProperties properties = getProperties();
            properties.backgroundImage = _texIdEnabled;
            setProperties( properties );
        }

        virtual void onMouseMoveOut( double x, double y ) {
            ComponentProperties properties = getProperties();
            properties.backgroundImage = _texIdDisabled;
            setProperties( properties );
        }

        virtual void clickAction( bool switched ) {
        }

        inline int getDirection() {
            return _dir;
        }

        private:
        int _dir;
        SelectableList* _selList;
        int _texIdEnabled;
        int _texIdDisabled;
    };

    virtual Component* getArrowUp( int direction ) = 0;
    virtual Component* getArrowDown( int direction ) = 0;

    VectorLayout::Orientation getOrientation() {
        return _orientation;
    }

    void setOrientation( VectorLayout::Orientation orientation ) {
        _orientation = orientation;
    }

    private:

    int _selectedRow;
    int _visibleRows;
    int _totalRows;
    VectorLayout::Orientation _orientation;

    Component* _arrowUp;
    Component* _arrowDown;

};


