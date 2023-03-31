#pragma once

class FormBundle {
    public:
    FormBundle() {
        _current = 0;
    }
    virtual ~FormBundle() {
        clear();
    }

    inline Form* getCurrentForm() {
        return getForm( _current );
    }

    inline void setCurrentForm( int current ) {
        _current = current;
    }

    Form* getForm( int id ) {
        map < int, Form* >::iterator i = _forms.find( id );
        if ( i != _forms.end() ) {
            return i -> second;
        }
        return NULL;
    }

    bool setForm( int id, Form* form ) {
        if ( !form ) {
            return false;
        }
        if ( getForm( id ) ) {
            return false;
        }
        form -> setFormBundle( this );
        form -> setFormIndex( id );
        _forms[ id ] = form;
        return true;
    }

    void rebuildForms() {
        for ( map < int, Form* >::iterator i = _forms.begin(); i != _forms.end(); i++ ) {
            rebuildForm( i -> first );
        }
    }

    bool rebuildForm( int id ) {
        map < int, Form* >::iterator i = _forms.find( id );
        //printf( " > Seek for form = %d\n", id );
        if ( i != _forms.end() ) {
            FormDesignFactory* factory = FormDesignFactory::getFactory( id );
            //printf( "Found form\n" );
            if ( factory ) {
                //printf( "Found factory\n" );
                Form* form = factory -> build( i -> second -> getBuildParams() );
                if ( form ) {
                    //printf( "Form rebuilt\n" );
                    form -> setFormIndex( i -> second -> getFormIndex() );
                    delete i -> second;
                    form -> setFormBundle( this );
                    _forms[ id ] = form;
                    return true;
                }
            }
        }
        return false;
    }

    void clear() {
        for ( map < int, Form* >::iterator i = _forms.begin(); i != _forms.end(); i++ ) {
            if ( i -> second ) {
                delete i -> second;
            }
        }
        _forms.clear();
        _current = 0;
    }

    private:

    map < int, Form* > _forms;
    int _current;

};


