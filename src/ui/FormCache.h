#pragma once

#include <stack>

class FormCache {
    private:
    FormCache() {}
    virtual ~FormCache() {}

    static FormBundle* _ACTIVE_BUNDLE;
    static int _REFRESH_FORM;
    static stack < int > _STACKED_FORMS;

    public:

    static void setActiveBundle( FormBundle* bundle ) {
        _ACTIVE_BUNDLE = bundle;
    }

    static void displayActiveBundle() {
        if ( _REFRESH_FORM == -2 ) {
            _ACTIVE_BUNDLE -> rebuildForms();
        } else if ( _REFRESH_FORM >= 0 ) {
            _ACTIVE_BUNDLE -> rebuildForm( _REFRESH_FORM );
        }
        _REFRESH_FORM = -1;
        Form* activeForm = getActiveForm();
        if ( activeForm ) {
            activeForm -> tick();
            activeForm -> mouseTick( double( Keys::getMouseX() ) / double( WINDOW_WIDTH ), double( Keys::getMouseY() ) / double( WINDOW_HEIGHT ) );
            activeForm -> display();
        }
    }

    static Form* getActiveForm() {
        if ( _ACTIVE_BUNDLE ) {
            return _ACTIVE_BUNDLE -> getCurrentForm();
        }
        return NULL;
    }

    static void changeForm( int changeId ) {
        if ( _ACTIVE_BUNDLE ) {
            _ACTIVE_BUNDLE -> setCurrentForm( changeId );
        }
    }

    static void stackCurrentForm() {
        Form* activeForm = getActiveForm();
        if ( activeForm ) {
            _STACKED_FORMS.push( activeForm -> getFormIndex() );
        }
    }

    static bool getLastStackedForm( int* formId ) {
        if ( _ACTIVE_BUNDLE ) {
            if ( !_STACKED_FORMS.empty() ) {
                ( *formId ) = _STACKED_FORMS.top();
                _STACKED_FORMS.pop();
                return true;
            }
        }
        return false;
    }

    static void clearStackedForms() {
        while ( !_STACKED_FORMS.empty() ) {
            _STACKED_FORMS.pop();
        }
    }

    static void refreshPage() {
        Form* form = _ACTIVE_BUNDLE -> getCurrentForm();
        if ( form ) {
            _REFRESH_FORM = form -> getFormIndex();
        }
    }

    static void refreshAll() {
        _REFRESH_FORM = -2;
    }

    static void Initialize();

};

FormBundle* FormCache::_ACTIVE_BUNDLE = NULL;
int FormCache::_REFRESH_FORM = -1;
stack < int > FormCache::_STACKED_FORMS;

