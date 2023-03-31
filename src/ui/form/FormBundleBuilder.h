#pragma once

class FormBundleBuilder {
    private:

    class _FormStruc {
        public:
        _FormStruc() {}
        ~_FormStruc() {}
        int formID;
        FormBuildParameters params;
    };

    public:
    FormBundleBuilder() {
        _start = 0;
    }
    virtual ~FormBundleBuilder() {
        _forms.clear();
    }

    FormBundleBuilder with( int formID, FormBuildParameters params ) {
        _FormStruc struc;
        struc.formID = formID;
        struc.params = params;
        _forms.push_back( struc );
        return * this;
    }

    FormBundleBuilder with( int formID ) {
        _FormStruc struc;
        struc.formID = formID;
        struc.params = FormBuildParameters();
        _forms.push_back( struc );
        return * this;
    }

    FormBundleBuilder startFrom( int formID ) {
        _start = formID;
        return * this;
    }

    FormBundle* build() {
        FormBundle* base = new FormBundle();
        for ( list < _FormStruc >::iterator i = _forms.begin(); i != _forms.end(); i++ ) {
            FormDesignFactory* factory = FormDesignFactory::getFactory( i -> formID );
            if ( factory ) {
                //printf( "Building form %d\n", i -> formID );
                Form* form = factory -> build( i -> params );
                if ( form ) {
                    if ( !base -> setForm( i -> formID, form ) ) {
                        delete form;
                    } else {
                        printf( "Constructed form %d\n", i -> formID );
                    }
                }
            }
        }
        base -> setCurrentForm( _start );
        return base;
    }

    private:

    list < _FormStruc > _forms;
    int _start;

};


