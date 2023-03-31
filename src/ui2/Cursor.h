#pragma once

class Cursor {
    private:
    Cursor() {}
    ~Cursor() {}
    public:

    static const int TEXTURE_ID = 1;
    static const int RELOADING_TEXTURE_ID = 2;
    static const double AFFECTION_INERTIA = 0.9825;
    static const double AFFECTION_MAX_VALUE = 2.25;

    static void display() {
        Texture* cursorTexture = Cache::getFileAccessor() -> getTexture( TEXTURE_ID );
        if ( cursorTexture ) {
            double scale = 0.3 + _affection * 0.75;
            EntityObject* player = Cache::getGameStateHolder() -> getPlayer();
            if ( player ) {
                WeaponObject* weapon = InventoryHelper::currentWeaponOf( player );
                if ( weapon ) {
                    double percentageReloadedTime = weapon -> getPercentageReloadingTime( Cache::gameTime() );
                    if ( !isnan( percentageReloadedTime ) ) {
                        Texture* cursorReloadingTexture = Cache::getFileAccessor() -> getTexture( RELOADING_TEXTURE_ID );
                        if ( cursorReloadingTexture ) {
                            Draw::progressBar( Keys::getMouseX(), Keys::getMouseY(), cursorReloadingTexture, 0.0, scale, percentageReloadedTime );
                        }
                    }
                }
            }
            Draw::spriteStatic( Keys::getMouseX(), Keys::getMouseY(), cursorTexture, 0.0, scale );
        }
        recalculateAffection();
    }

    static void addAffection( double affection ) {
        _affection += affection;
        if ( _affection > AFFECTION_MAX_VALUE ) {
            _affection = AFFECTION_MAX_VALUE;
        } else if ( _affection < 0.0 ) {
            _affection = 0.0;
        }
    }

    static void recalculateAffection() {
        _affection *= AFFECTION_INERTIA;
    }

    static double getAffection() {
        return _affection;
    }

    private:

    static double _affection;

};

double Cursor::_affection = 0.0;

