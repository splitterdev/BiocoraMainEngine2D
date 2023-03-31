#pragma once

class CryptChatEngine {
    public:
    CryptChatEngine() {
        _active = false;
        _unreadMessages = false;
        _messageSurface = NULL;
        _messageTexture = NULL;
        _lastAvatarId = -1;
        _lastSender = -1;
        _lastAvatarPositionY = -1;
        _opacity = 255;//192;
        setRenderedSize( 200, 200 );
    }
    virtual ~CryptChatEngine() {
        clearMessages();
    }

    static const int MESSAGE_SOUND_ID = 916;
    static const int MESSAGE_SOUND_ID_OWNER = 917;

    static CryptChatEngine* getInstance() {
        if ( !_INSTANCE ) {
            _INSTANCE = new CryptChatEngine();
        }
        return _INSTANCE;
    }

    inline bool isActive() {
        return _active;
    }

    inline void setActive( bool active ) {
        _active = active;
    }

    inline bool hasUnreadMessages() {
        return _unreadMessages;
    }

    inline void resetUnreadMessages() {
        _unreadMessages = false;
    }

    enum RenderMode {
        OWNER, GUEST
    };

    void sendMessage( int senderLabelId, int senderAvatarId, int textLabelId, Uint32 textColor, RenderMode renderMode ) {
        string sender = LangStrings::getString( senderLabelId );
        string message = LangStrings::getString( textLabelId );
        Texture* avatar = Cache::getFileAccessor() -> getTexture( senderAvatarId );
        _addMessageRender( sender, message, avatar, senderLabelId, senderAvatarId, textColor, renderMode );
        _lastAvatarId = senderAvatarId;
        _lastSender = senderLabelId;
        int messageSoundId = -1;
        if ( renderMode == GUEST ) {
            messageSoundId = MESSAGE_SOUND_ID;
            _unreadMessages = true;
        } else {
            messageSoundId = MESSAGE_SOUND_ID_OWNER;
        }
        Sound* sound = Cache::getFileAccessor() -> getSound( messageSoundId );
        if ( sound ) {
            sound -> play();
        }
    }

    void setRenderedSize( int w, int h ) {
        _lineXMax = w;
        _lineYMax = h;
    }

    inline SDL_Texture* getDisplayTexture() {
        return _messageTexture;
    }

    void clearMessages() {
        if ( _messageSurface ) {
            SDL_FreeSurface( _messageSurface );
            _messageSurface = NULL;
        }
        if ( _messageTexture ) {
            SDL_DestroyTexture( _messageTexture );
            _messageTexture = NULL;
        }
        _lastAvatarId = -1;
        _lastSender = -1;
        _lastAvatarPositionY = -1;
    }

    private:

    void _addMessageRender( string sender, string message, Texture* avatar, int senderLabelId, int avatarId, Uint32 textColor, RenderMode renderMode ) {
        int lineBegin = 72;
        SDL_Rect cursor = Draw::getSDLRect( lineBegin, 0 );
        int lineCounter = 0;
        int lineXTextMax = double( _lineXMax ) * 0.9;
        int lineYStd = Text::SIZE_GAME;
        int messageYMinSize = avatar -> getHeight() + lineYStd;
        SDL_Surface* renderedText = _createSurface( lineXTextMax, lineYStd + 2, 0x00000000 );
        if ( !_messageSurface ) {
            _messageSurface = _createSurface( _lineXMax, _lineYMax, 0x00000000 );
        }
        if ( !renderedText || !_messageSurface ) {
            return;
        }
        Text::setSize( Text::SIZE_GAME );
        Text::Color( textColor );
        vector < string > splits = StringUtils::split( message, ' ' );
        SDL_Surface* delimiterSurface = Text::RenderOnly( " " );
        if ( delimiterSurface ) {
            int delimiterWidth = delimiterSurface -> w;
            int i = 0;
            int wordCount = int( splits.size() );
            while ( i < wordCount ) {
                string word = splits[ i ];
                SDL_Surface* wordSurface = Text::RenderOnly( "%s", word.c_str() );
                if ( !wordSurface ) {
                    // one (and so far the only) case is empty string, which comes from repetitive delimiter
                    i++;
                    cursor.x += delimiterWidth;
                    continue;
                }
                bool endWriting = false;
                if ( ( i >= wordCount - 1 ) && !( ( cursor.x + wordSurface -> w > lineXTextMax ) && ( cursor.x > lineBegin ) ) ) {
                    _switchLineUp( &_messageSurface, lineYStd );
                    SDL_BlitSurface( wordSurface, NULL, renderedText, &cursor );
                    SDL_Rect dstRect = Draw::getSDLRect( 0, _messageSurface -> h - lineYStd );
                    SDL_BlitSurface( renderedText, NULL, _messageSurface, &dstRect );
                    lineCounter++;
                    endWriting = true;
                } else if ( ( cursor.x + wordSurface -> w > lineXTextMax ) && ( cursor.x > lineBegin ) ) {
                    _switchLineUp( &_messageSurface, lineYStd );
                    SDL_Rect dstRect = Draw::getSDLRect( 0, _messageSurface -> h - lineYStd );
                    SDL_BlitSurface( renderedText, NULL, _messageSurface, &dstRect );
                    cursor.x = lineBegin;
                    lineCounter++;
                    SDL_FillRect( renderedText, NULL, 0x00000000 );
                } else {
                    SDL_BlitSurface( wordSurface, NULL, renderedText, &cursor );
                    cursor.x += wordSurface -> w + delimiterWidth;
                    i++;
                }
                SDL_FreeSurface( wordSurface );
                if ( endWriting ) {
                    break;
                }
            }
            SDL_FreeSurface( delimiterSurface );
        }
        SDL_FreeSurface( renderedText );
        int switchUpDiff = messageYMinSize - lineCounter * lineYStd;
        if ( switchUpDiff > 0 ) {
            _switchLineUp( &_messageSurface, switchUpDiff );
        }
        int messageYSize = max( lineCounter * lineYStd, messageYMinSize );
        SDL_Rect avatarPos = Draw::getSDLRect(
            4, _messageSurface -> h - messageYSize,
            avatar -> getWidth(), avatar -> getHeight()
        );
        _updateTexture( messageYSize, renderMode );
        _lastAvatarPositionY += messageYSize;
        if ( renderMode == GUEST ) {
            if ( ( _lastAvatarId != avatarId ) || ( _lastSender != senderLabelId ) || ( _lastAvatarPositionY >= 0.9 * _lineYMax ) ) {
                _lastAvatarPositionY = messageYSize;
                SDL_Texture* renderTarget = SDL_GetRenderTarget( Renderer::getRenderer() );
                SDL_SetRenderTarget( Renderer::getRenderer(), _messageTexture );
                SDL_RenderCopy( Renderer::getRenderer(), avatar -> getTexture(), NULL, &avatarPos );
                SDL_Rect senderLabelPos = avatarPos;
                senderLabelPos.y += avatar -> getHeight();
                senderLabelPos.h = lineYStd;
                Text::Color( 120, 160, 200 );
                Text::Write( Renderer::getRenderer(), senderLabelPos, 255, "%s", sender.c_str() );
                Text::Color( 254, 254, 254 );
                SDL_SetRenderTarget( Renderer::getRenderer(), renderTarget );
            }
        }
    }

    inline SDL_Surface* _createSurface( int x, int y, Uint32 fillColor ) {
        SDL_Surface* surface = SDL_CreateRGBSurface( 0, x, y, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000 );
        SDL_FillRect( surface, NULL, fillColor );
        return surface;
    }

    void _updateTexture( int switchOffsetPrevious, RenderMode renderMode ) {
        SDL_Texture* renderTarget = SDL_GetRenderTarget( Renderer::getRenderer() );
        SDL_SetRenderDrawColor( Renderer::getRenderer(), 0, 0, 0, 0 );
        SDL_SetRenderDrawBlendMode( Renderer::getRenderer(), SDL_BLENDMODE_NONE );
        if ( !_messageTexture ) {
            _messageTexture = SDL_CreateTexture( Renderer::getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, _messageSurface -> w, _messageSurface -> h );
            SDL_SetRenderTarget( Renderer::getRenderer(), _messageTexture );
            SDL_RenderFillRect( Renderer::getRenderer(), NULL );
        }
        SDL_Texture* finalRender = SDL_CreateTexture( Renderer::getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, _messageSurface -> w, _messageSurface -> h );
        SDL_SetRenderTarget( Renderer::getRenderer(), finalRender );
        SDL_RenderFillRect( Renderer::getRenderer(), NULL );
        SDL_Texture* sfcTexture = SDL_CreateTextureFromSurface( Renderer::getRenderer(), _messageSurface );
        SDL_SetRenderDrawBlendMode( Renderer::getRenderer(), SDL_BLENDMODE_BLEND );
        SDL_SetTextureBlendMode( finalRender, SDL_BLENDMODE_BLEND );
        SDL_SetTextureBlendMode( sfcTexture, SDL_BLENDMODE_BLEND );
        SDL_SetTextureBlendMode( _messageTexture, SDL_BLENDMODE_BLEND );
        SDL_Rect srcRect = Draw::getSDLRect( 0, _messageSurface -> h - switchOffsetPrevious, _messageSurface -> w, switchOffsetPrevious );
        SDL_Rect dstRect = Draw::getSDLRect( 0, _messageSurface -> h - switchOffsetPrevious, _messageSurface -> w, switchOffsetPrevious );
        if ( renderMode == OWNER ) {
            dstRect.x = _messageSurface -> w - ( double( _lineXMax ) * 0.9 );
        }
        if ( renderMode == OWNER ) {
            SDL_SetRenderDrawColor( Renderer::getRenderer(), 20, 20, 20, 255 );
        } else {
            SDL_SetRenderDrawColor( Renderer::getRenderer(), 80, 80, 80, 255 );
        }
        SDL_RenderFillRect( Renderer::getRenderer(), &dstRect );
        SDL_RenderCopy( Renderer::getRenderer(), sfcTexture, &srcRect, &dstRect );
        srcRect = Draw::getSDLRect( 0, switchOffsetPrevious, _messageSurface -> w, _messageSurface -> h - switchOffsetPrevious );
        dstRect = Draw::getSDLRect( 0, 0, _messageSurface -> w, _messageSurface -> h - switchOffsetPrevious );
        SDL_RenderCopy( Renderer::getRenderer(), _messageTexture, &srcRect, &dstRect );
        SDL_DestroyTexture( sfcTexture );
        SDL_SetRenderTarget( Renderer::getRenderer(), renderTarget );
        if ( _messageTexture ) {
            SDL_DestroyTexture( _messageTexture );
        }
        _messageTexture = finalRender;
    }

    void _switchLineUp( SDL_Surface** surface, int height ) {
        SDL_Surface* cpy = _createSurface( ( *surface ) -> w, ( *surface ) -> h, 0x00000000 );
        if ( cpy ) {
            SDL_Rect srcRect = Draw::getSDLRect( 0, height, ( *surface ) -> w, ( *surface ) -> h - height );
            SDL_BlitSurface( ( *surface ), &srcRect, cpy, NULL );
            SDL_FreeSurface( ( *surface ) );
            ( *surface ) = cpy;
        }
    }

    static CryptChatEngine* _INSTANCE;

    bool _active;
    bool _unreadMessages;
    int _lineXMax;
    int _lineYMax;
    int _lastAvatarId;
    int _lastSender;
    int _lastAvatarPositionY;
    int _opacity;
    SDL_Surface* _messageSurface;
    SDL_Texture* _messageTexture;

};

CryptChatEngine* CryptChatEngine::_INSTANCE = NULL;

