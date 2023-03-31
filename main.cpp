int WINDOW_WIDTH = 1280;//1024;
int WINDOW_HEIGHT = 768;
int WINDOW_FULLSCREEN = 0;

int EXIT_GAME_SIGNAL = 0;

#include "src/Core.h"

#include "src/Utils.h"
#include "src/Drawing.h"
#include "src/Script.h"
#include "src/DataModel.h"
#include "src/Cache.h"
#include "src/Ui.h"
#include "src/GamePlay.h"
#include "src/Controller.h"

#include "src/drawing/DisplayUnit.h"
#include "src/PerspectiveCustom.h"

#include "src/MainSetupCoreLoader.h"

int main( int argc, char** argv ) {
    MainSetupCoreLoader::loadCore();

    Perspective::Setup( new LevelPerspective() );
    Perspective::Setup( new LoadingPerspective() );
    Perspective::Setup( new MenuPerspective() );
    Perspective::Setup( new WarpPerspective() );
    Perspective::ChangeTo( PERSPECTIVE_MENU_ID );

    try {
        bool done = false;
        while ( !done ) {
            done = !Perspective::tick();
            if ( EXIT_GAME_SIGNAL != 0 ) {
                done = true;
            }
        }
    } catch ( Exception e ) {
        printf( "[EXCEPTION] %s\n", e.fullMessage().c_str() );
    }

    return MainSetupCoreLoader::exitGame();
}

