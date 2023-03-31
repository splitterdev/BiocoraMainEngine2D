#pragma once

class ScriptInstructionFactory {
    public:
    ScriptInstructionFactory() {
    }
    virtual ~ScriptInstructionFactory() {
    }

    static void setFactory( int key, ScriptInstructionFactory* factory, int parameterRequiredCount ) {
        if ( factory ) {
            if ( !getFactory( key ) ) {
                _factories[ key ] = factory;
                _createRequiredParamValidation[ key ] = parameterRequiredCount;
                return;
            }
            throw Exception( string( "Duplicate ScriptInstructionFactory key: " ) + StringUtils::toString( key ) );
        }
        throw Exception( string( "Cannot add null ScriptInstructionFactory with key: " ) + StringUtils::toString( key ) );
    }

    static ScriptInstructionFactory* getFactory( int key ) {
        map < int, ScriptInstructionFactory* >::iterator i = _factories.find( key );
        if ( i != _factories.end() ) {
            return i -> second;
        }
        return NULL;
    }

    static ScriptInstruction* createInstruction( int key, vector < double > & params ) {
        ScriptInstructionFactory* factory = getFactory( key );
        if ( factory ) {
            int paramCountReq = getRequiredParameterCount( key );
            if ( int( params.size() ) != paramCountReq ) {
                throw Exception(
                    string( "Error: invalid parameter count in instruction " )
                    + StringUtils::toString( key )
                    + string( ": required " )
                    + StringUtils::toString( paramCountReq )
                    + string( ", found " )
                    + StringUtils::toString( int( params.size() ) )
                );
            }
            ScriptInstruction* instruction = factory -> create();
            if ( instruction ) {
                instruction -> _parameters = params;
            }
            return instruction;
        }
        return NULL;
    }

    static int getRequiredParameterCount( int key ) {
        map < int, int >::iterator i = _createRequiredParamValidation.find( key );
        if ( i != _createRequiredParamValidation.end() ) {
            return i -> second;
        }
        return -1;
    }

    virtual ScriptInstruction* create() = 0;

    private:

    static map < int, ScriptInstructionFactory* > _factories;
    static map < int, int > _createRequiredParamValidation;

};

map < int, ScriptInstructionFactory* > ScriptInstructionFactory::_factories;
map < int, int > ScriptInstructionFactory::_createRequiredParamValidation;

