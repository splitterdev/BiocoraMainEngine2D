#pragma once

class FrameHolder {
    public:
    FrameHolder() {
        resetFrameCounter();
    }
    virtual ~FrameHolder() {
        removeFrames();
    }

    Frame* newFrame() {
        Frame* frame = new Frame( _frameIdCounter, this );
        _frames[ _frameIdCounter ] = frame;
        _frameList.push_back( frame );
        _frameIdCounter++;
        return frame;
    }

    Frame* getFrame( int frameId ) {
        map < int, Frame* >::iterator i = _frames.find( frameId );
        if ( i != _frames.end() ) {
            return i -> second;
        }
        return NULL;
    }

    void removeFrames() {
        for ( map < int, Frame* >::iterator i = _frames.begin(); i != _frames.end(); i++ ) {
            if ( i -> second ) {
                delete i -> second;
            }
        }
        _frames.clear();
        _frameList.clear();
        resetFrameCounter();
    }

    inline list < Frame* > getFrames() {
        return _frameList;
    }

    inline void resetFrameCounter() {
        _frameIdCounter = 0;
    }

    inline int getFrameCount() {
        return _frames.size();
    }

    private:

    map < int, Frame* > _frames;
    list < Frame* > _frameList;
    int _frameIdCounter;

};

