#pragma once

class FormBundle;

class Form : public Component {
    public:
    Form( FormBuildParameters parameters ) : Component( ComponentProperties() ) {
        _bundle = NULL;
        _params = parameters;
        _index = 0;
    }
    Form( FormBuildParameters parameters, ComponentProperties properties ) : Component( properties ) {
        _bundle = NULL;
        _params = parameters;
        _index = 0;
    }
    virtual ~Form() {
    }

    inline FormBundle* getFormBundle() {
        return _bundle;
    }

    inline void setFormBundle( FormBundle* bundle ) {
        _bundle = bundle;
    }

    inline int getFormIndex() {
        return _index;
    }

    inline void setFormIndex( int index ) {
        _index = index;
    }

    inline FormBuildParameters getBuildParams() {
        return _params;
    }

    protected:

    virtual void onDisplay() {
    }

    private:

    FormBundle* _bundle;

    FormBuildParameters _params;

    int _index;

};


