#pragma once

class FormDesignFactory {
    public:
    FormDesignFactory() {
    }
    virtual ~FormDesignFactory() {
    }

    virtual Form* build( FormBuildParameters parameters ) = 0;

    static FormDesignFactory* getFactory( int id ) {
        map < int, FormDesignFactory* >::iterator i = _builders.find( id );
        if ( i != _builders.end() ) {
            return i -> second;
        }
        return NULL;
    }

    static bool setFactory( int id, FormDesignFactory* builder ) {
        if ( !builder ) {
            return false;
        }
        if ( getFactory( id ) ) {
            return false;
        }
        _builders[ id ] = builder;
        return true;
    }

    static FormDesignFactory* deleteFactory( int id ) {
        map < int, FormDesignFactory* >::iterator i = _builders.begin();
        if ( i != _builders.end() ) {
            FormDesignFactory* builder = i -> second;
            _builders.erase( i );
            return builder;
        }
        return NULL;
    }

    static void cleanup() {
        for ( map < int, FormDesignFactory* >::iterator i = _builders.begin(); i != _builders.end(); i++ ) {
            if ( i -> second ) {
                delete i -> second;
            }
        }
        _builders.clear();
    }

    static Form* buildForm( int id, FormBuildParameters parameters ) {
        map < int, FormDesignFactory* >::iterator i = _builders.find( id );
        if ( i != _builders.end() ) {
            FormDesignFactory* builder = i -> second;
            return builder -> build( parameters );
        }
        return NULL;
    }

    private:

    static map < int, FormDesignFactory* > _builders;

};

map < int, FormDesignFactory* > FormDesignFactory::_builders;

