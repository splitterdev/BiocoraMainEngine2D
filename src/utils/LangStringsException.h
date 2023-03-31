#pragma once

class LangStringsException : public Exception {
    public:
    LangStringsException( string s ) : Exception( s ) {
    }
    LangStringsException( const char* s ) : Exception( s ) {
    }
    virtual ~LangStringsException() {
    }

    virtual string getClassName() {
        return "LangStringsException";
    }

    private:
};

