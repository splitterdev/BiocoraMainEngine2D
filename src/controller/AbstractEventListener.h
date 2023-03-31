#pragma once

class AbstractEventListener {
    public:
    AbstractEventListener() {}
    virtual ~AbstractEventListener() {}

	virtual int catchKeyPushEvent( int key ) = 0;
	virtual int catchKeyReleaseEvent( int key ) = 0;
	virtual int catchMousePushEvent( int button ) = 0;
	virtual int catchMouseReleaseEvent( int button ) = 0;
	virtual int catchMouseScrollEvent( int direction ) = 0;
	virtual int onRegularTick() = 0;
};

