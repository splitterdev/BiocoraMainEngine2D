#pragma once

#include "AbstractEventListener.h"

class MouseUIController : public AbstractEventListener {
    public:
    MouseUIController() : AbstractEventListener() {
    }
    virtual ~MouseUIController() {
    }

	virtual int catchKeyPushEvent( int key ) {
	    if ( key == 27 ) {
            int formId;
            if ( FormCache::getLastStackedForm( &formId ) ) {
                FormCache::changeForm( formId );
            } else {
                FormCache::stackCurrentForm();
                FormCache::changeForm( FORM_EXIT_CONFIRMATION_MENU );
            }
	    }
	    Form* active = FormCache::getActiveForm();
	    if ( active ) {
            active -> keyInput( key );
	    }
	    return GameController::RESULT_EMPTY;
	}

	virtual int catchKeyReleaseEvent( int key ) {
	    return GameController::RESULT_EMPTY;
	}

	virtual int catchMousePushEvent( int button ) {
	    if ( button == SDL_BUTTON_LEFT ) {
            Form* activeForm = FormCache::getActiveForm();
            if ( activeForm ) {
                activeForm -> press( double( Keys::getMouseX() ) / double( WINDOW_WIDTH ), double( Keys::getMouseY() ) / double( WINDOW_HEIGHT ) );
            }
	    }
	    return GameController::RESULT_EMPTY;
	}

	virtual int catchMouseReleaseEvent( int button ) {
	    if ( button == SDL_BUTTON_LEFT ) {
            Form* activeForm = FormCache::getActiveForm();
            if ( activeForm ) {
                activeForm -> release( double( Keys::getMouseX() ) / double( WINDOW_WIDTH ), double( Keys::getMouseY() ) / double( WINDOW_HEIGHT ) );
            }
	    }
	    return GameController::RESULT_EMPTY;
	}

	virtual int catchMouseScrollEvent( int direction ) {
	    return GameController::RESULT_EMPTY;
	}

	static void Init() {
	    dispose();
	    _DEFAULT_CONTROLLER = new MouseUIController();
	}

	static MouseUIController* getController() {
        return _DEFAULT_CONTROLLER;
	}

	static void dispose() {
	    if ( _DEFAULT_CONTROLLER ) {
            delete _DEFAULT_CONTROLLER;
            _DEFAULT_CONTROLLER = NULL;
	    }
	}

	virtual int onRegularTick() {
	    return GameController::RESULT_EMPTY;
	}

    private:

    static MouseUIController* _DEFAULT_CONTROLLER;

};

MouseUIController* MouseUIController::_DEFAULT_CONTROLLER = NULL;


