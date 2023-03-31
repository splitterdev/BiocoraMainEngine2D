#pragma once

class CryptChatUIHelper {
    private:
    CryptChatUIHelper() {}
    virtual ~CryptChatUIHelper() {}
    public:

    static void display( CryptChatEngine* engine ) {
        if ( !engine ) {
            return;
        }
        if ( !Cache::getGameStateHolder() -> getPlayer() ) {
            CryptChatEngine::getInstance() -> setActive( false );
            CryptChatEngine::getInstance() -> resetUnreadMessages();
        }
        double x = 0.5;
        double y = 0.55;
        double w = 0.45;
        double h = 0.4;
        engine -> setRenderedSize( WINDOW_WIDTH * w, WINDOW_HEIGHT * h );
        if ( !engine -> isActive() ) {
            return;
        }
        engine -> resetUnreadMessages();
        SDL_SetRenderDrawColor( Renderer::getRenderer(), 32, 32, 32, 80 );
        SDL_Rect rect = Draw::getSDLRect( WINDOW_WIDTH * x, WINDOW_HEIGHT * y, WINDOW_WIDTH * w, WINDOW_HEIGHT * h );
        SDL_RenderFillRect( Renderer::getRenderer(), &rect );
        //Text::Write( Renderer::getRenderer(), rect.x + 8, rect.y + 8, "CryptChat" );
        SDL_SetTextureAlphaMod( engine -> getDisplayTexture(), 128 );
        SDL_RenderCopy( Renderer::getRenderer(), engine -> getDisplayTexture(), NULL, &rect );
        SDL_SetTextureAlphaMod( engine -> getDisplayTexture(), 255 );
    }

};


