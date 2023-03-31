#pragma once

class Sound {
    public:
    Sound( string path, bool streamed ) {
        _streaming = streamed;
        _path = path;
        _stream = 0L;
        _sample = 0L;
        _channel = 0L;
        if ( streamed ) {
            _stream = BASS_StreamCreateFile( FALSE, path.c_str(), 0, 0, 0 );
        } else {
            _sample = BASS_SampleLoad( FALSE, path.c_str(), 0, 0, 1, 0 );
            _channel = BASS_SampleGetChannel( _sample, FALSE );
        }
    }
    virtual ~Sound() {
        stop();
        if ( _streaming ) {
            BASS_StreamFree( _stream );
        } else {
            BASS_SampleFree( _sample );
        }
    }

    static void Init() {
        if ( !BASS_Init( -1, 44100, BASS_DEVICE_16BITS, 0, NULL ) ) {
            printf( "BASS_DRIVER_ERROR %d\n", BASS_ErrorGetCode() );
        }
    }

    void play() {
        if ( _streaming ) {
            BASS_StreamFree( _stream );
            _stream = BASS_StreamCreateFile( FALSE, _path.c_str(), 0, 0, 0 );
            BASS_ChannelPlay( _stream, FALSE );
        } else {
            BASS_ChannelSetPosition( _sample, 0, BASS_POS_BYTE );
            BASS_ChannelPlay( _channel, FALSE );
        }
    }

    bool isPlaying() {
        if ( _streaming ) {
            return ( BASS_ChannelIsActive( _stream ) == BASS_ACTIVE_PLAYING );
        }
        return ( BASS_ChannelIsActive( _channel ) == BASS_ACTIVE_PLAYING );
    }

    void stop() {
        if ( _streaming ) {
            BASS_StreamFree( _stream );
        } else {
            BASS_ChannelPause( _channel );
        }
    }

    private:

    bool _streaming;
    string _path;
    HSAMPLE _sample;
    HSTREAM _stream;
    HCHANNEL _channel;

};

