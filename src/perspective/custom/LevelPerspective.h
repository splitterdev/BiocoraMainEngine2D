#pragma once

FormBundle* inventoryForms = NULL;

class LevelPerspective : public Perspective {
    public:
    LevelPerspective() : Perspective( PERSPECTIVE_LEVEL_ID ) {
    }
    virtual ~LevelPerspective() {
    }

    virtual void onCreate() {
        GameController::addEventListener( FlagmanController::getController() );
        GameController::setHandleUppercase( false );
        GameController::setHandleKeyRepeat( false );
        GameController::setDisallowedKeys( "" );
    }

    virtual void onDestroy() {
        GameController::clearEventListenerList();
    }

    virtual FormBundle* buildInterface() {
        FormBundleBuilder builder = FormBundleBuilder()
            .with( FORM_MAIN_MENU )
            .startFrom( FORM_MAIN_MENU );
        return NULL;//builder.build();
    }

    virtual bool onTick() {
        int startTick = SDL_GetTicks();
            //Sleep( 100 );
        if ( _levelFinished ) {
            _changePerspectiveOnEndLevel(); // get next level from sequence, or next chapter if last level, or return to menu if last chapter+level
            endLevel(); // save player data
        }
        int controllerResult = GameController::controlEvents();
        if ( controllerResult == GameController::RESULT_RETURN ) {
            FormCache::clearStackedForms();
            Perspective::ChangeTo( PERSPECTIVE_MENU_ID );
            return true;
        }
        if ( controllerResult == GameController::RESULT_EXIT ) {
            return false;
        }
        Renderer::begin();
        if ( Cache::getGameStateHolder() ) {
            if ( Cache::getGameStateHolder() -> getMap() ) {
                Cache::getGameStateProcessor() -> gamePlayTick();
                Vector2D playerPosition;
                EntityObject* player = Cache::getGameStateHolder() -> getPlayer();
                if ( player ) {
                    playerPosition = player -> getPosition();
                    Camera::set( Keys::getMouseX(), Keys::getMouseY(), &playerPosition, Cache::gameTime() );
                } else {
                    Camera::set( Keys::getMouseX(), Keys::getMouseY(), NULL, Cache::gameTime() );
                }
            }
            // draw map
            DisplayUnit::displayMap( Cache::getGameStateHolder() -> getMap() );
            // draw entities
            if ( Cache::getGameStateHolder() -> getMap() ) {
                DisplayUnit::display();
            }

            // draw UI
            //drawInterface();
            EntityObject* player = Cache::getGameStateHolder() -> getPlayer();
            bool inventoryEnabled = isInventoryEnabled( player );
            if ( !inventoryEnabled ) {
                drawPickupTips();
                WeaponObject* weapon = InventoryHelper::currentWeaponOf( player );
                if ( weapon ) {
                    BulletType* usedAmmo = Cache::getFileAccessor() -> getBulletType( weapon -> getBulletId() );
                    if ( usedAmmo ) {
                        if ( usedAmmo -> isAutoAim() ) {
                            AutoAimCursor::display();
                        }
                    }
                }
            }
            if ( Cache::getGameStateHolder() -> isWaypointIndicatorEnabled() && player ) {
                displayWaypointIndicator( Cache::getGameStateHolder() -> getWaypointIndicator(), player -> getPosition() );
            }
            writeHudInformations( player, inventoryEnabled );
            if ( !inventoryEnabled ) {
                Cursor::display();
            }
        }
        Renderer::end();
        double timeDiff = SDL_GetTicks() - startTick;
        Cache::setTimeLastDifferenceMultiplier( timeDiff / ( 1000.0 / Cache::DEFAULT_FRAME_RATE ) );
        //printf( "Last time multiplier: %g\n", Cache::getTimeLastDifferenceMultiplier() );
        Cache::setGameTime( Cache::gameTime() + timeDiff );
        return true;
    }

    void endLevel() {
        EntityObject* player = Cache::getGameStateHolder() -> getPlayer();
        if ( player ) {
            if ( PlayerManager::overWriteCharacter( PlayerManager::getProfileId(), player ) ) {
                printf( "CHARACTER %d OVERWRITTEN\n", PlayerManager::getProfileId() );
            }
        }
    }

    void setLevelFinished() {
        _levelFinished = true;
    }

    void enterLevel() {
        _ui_lastHeatValue = 0;
        _ui_overHeat = false;
        _ui_overHeat_strong = false;
        _levelFinished = false;
        CryptChatEngine::getInstance() -> clearMessages();
        CryptChatEngine::getInstance() -> resetUnreadMessages();
    }

    static bool isInventoryEnabled( EntityObject* object ) {
        if ( object ) {
            return object -> inventoryEnabled();
        }
        return false;
    }

    static const double TIP_DISPLAY_RANGE = 2.0;

    void drawPickupTips() {
        GameStateHolder* gameStateHolder = Cache::getGameStateHolder();
        GameStateHolder::Container < PickupObject > pickupContainer = gameStateHolder -> getPickupContainer();
        EntityObject* player = gameStateHolder -> getPlayer();
        if ( player ) {
            Vector2D cursorPos = Vector2D( Camera::X + Keys::getMouseX(), Camera::Y + Keys::getMouseY() );
            Text::Color( 254, 254, 254, 160 );
            while ( pickupContainer.hasMore() ) {
                PickupObject* pickup = pickupContainer.nextObject();
                if ( pickup ) {
                    Vector2D pickupPosition = pickup -> getPosition();
                    if ( cursorPos.dist2D( pickupPosition ) > TIP_DISPLAY_RANGE * double( Map::BLOCK_SIZE ) ) {
                        continue;
                    }
                    string tip = pickup -> getTip();
                    if ( tip.length() > 0 ) {
                        Text::WriteCenterAt( Renderer::getRenderer(), pickupPosition.x - Camera::X, pickupPosition.y - 24 - Camera::Y, 0x80202020, "%s", tip.c_str() );
                    }
                }
            }
        }
    }

    static const double UI_HEAT_SAFE_THRESHOLD_LEVEL = 30.0;
    static const double UI_HEAT_YELLOW_THRESHOLD_LEVEL = 70.0;
    static const double UI_HEAT_RED_THRESHOLD_LEVEL = 110.0;

    void writeHudInformations( EntityObject* player, bool inventoryEnabled ) {
        int lineOffset = 24;
        if ( player ) {
            int yPos = 20;
            double scale = 1.0;
            Text::Color( 254, 254, 254 );
            Text::WriteWithBackground( Renderer::getRenderer(), 16, yPos, scale, 1.0, "%s", ( player -> getName() ).c_str() );
            yPos += lineOffset;
            Text::WriteWithBackground( Renderer::getRenderer(), 16, yPos, scale, 1.0, "Level: %d", ( int ) player -> getLevel() );
            yPos += lineOffset;
            Text::WriteWithBackground( Renderer::getRenderer(), 16, yPos, scale, 1.0, "Exp: %d/%d", ( int ) player -> getExperience(), ( int ) player -> getExperienceRequired() );
            yPos += lineOffset;
            double heatLevel = player -> getHeatLevel();
            _checkHeatLevel( heatLevel );
            if ( heatLevel >= UI_HEAT_RED_THRESHOLD_LEVEL ) {
                Text::Color( 254, 0, 0 );
            } else if ( _ui_overHeat || heatLevel >= UI_HEAT_YELLOW_THRESHOLD_LEVEL ) {
                Text::Color( 254, 254, 0 );
            }
            Text::WriteWithBackground( Renderer::getRenderer(), 16, yPos, scale, 1.0, "Heat level: +%.2lf (C)", heatLevel );
            yPos += lineOffset;
            Text::Color( 254, 254, 254 );
            Text::WriteWithBackground( Renderer::getRenderer(), 16, yPos, scale, 1.0, "Energy level: %.2lf%%", player -> getEnergyPercentageLevel() * 100.0 );
            yPos += lineOffset;
            double staticEnergy = player -> getStaticEnergy();
            const char* _ENERGYQUALIFIER = _getUIEnergyQualifier( staticEnergy );
            Text::WriteWithBackground( Renderer::getRenderer(), 16, yPos, scale, 1.0, "Static energy: %.1lf%s", staticEnergy, _ENERGYQUALIFIER );
            yPos += lineOffset;
            if ( player -> isHealthBufferEnabled() ) {
                Text::Color( 0, 254, 0 );
            } else {
                Text::Color( 254, 254, 254 );
            }
            double hbuLevel = player -> getParameter( EntityType::ATTRIBUTE_HEALTH_BUFFER_UNIT_LEVEL );
            double percentageFillHBU = hbuLevel / player -> getParameter( EntityType::ATTRIBUTE_HEALTH_BUFFER_UNIT_CAPACITY );
            /// hidden information - HP info leak
            //double hpMax = player -> getParameter( EntityType::ATTRIBUTE_HP_CURRENT_MAX );
            //double ratioHBU = player -> getParameter( EntityType::ATTRIBUTE_HEALTH_BUFFER_UNIT_CONSUMPTION_RATIO );
            //double percentageRestoredHealth = hbuLevel / hpMax;
            //  percentageRestoredHealth * 100.0
            Text::WriteWithBackground( Renderer::getRenderer(), 16, yPos, scale, 1.0, "HBU level: %.1lf%%", percentageFillHBU * 100.0 );
            int taskLineId = Cache::getGameStateHolder() -> getMainTaskInfo();
            if ( taskLineId > 0 ) {
                string line = LangStrings::getString( taskLineId );
                yPos += ( lineOffset << 1 );
                Text::Color( 0, 254, 254 );
                Text::WriteWithBackground( Renderer::getRenderer(), 16, yPos, scale, 1.0, "%s", line.c_str() );
            }
            Text::Color( 254, 254, 254 );
        }
        if ( FlagmanController::isUiMessageSystemEnabled() ) {
            MessageManager::get() -> display( 16, WINDOW_HEIGHT - MessageManager::get() -> getMaxSize() * lineOffset - 24, lineOffset );
        }
        CryptChatUIHelper::display( CryptChatEngine::getInstance() );
        // display inventory if enabled
        if ( player ) {
            if ( inventoryEnabled ) {
                Camera::Enabled = false;
                SDL_ShowCursor( 1 );
                if ( !inventoryForms ) {
                    FormBundleBuilder builder = FormBundleBuilder()
                        .with( INGAME_FORM_INVENTORY )
                        .startFrom( INGAME_FORM_INVENTORY );
                    inventoryForms = builder.build();
                }
                FormCache::setActiveBundle( inventoryForms );
                FormCache::displayActiveBundle();
                //FormCache::getActiveForm() -> display();
            } else {
                Camera::Enabled = true;
                SDL_ShowCursor( 0 );
                FormCache::setActiveBundle( NULL );
                if ( inventoryForms ) {
                    delete inventoryForms;
                    inventoryForms = NULL;
                }
                double iconAspect = double( WINDOW_WIDTH ) / double( WINDOW_HEIGHT );
                double iconWidth = 0.04;
                double iconHeight = iconWidth * iconAspect;
                double iconSeqNumber = 1.0;
                // weapon icon
                WeaponObject* weapon = InventoryHelper::currentWeaponOf( player );
                Texture* weaponTexture = NULL;
                if ( weapon ) {
                    weaponTexture = Cache::getFileAccessor() -> getTexture( weapon -> getTextureId() );
                }
                _displayIcon(
                    weaponTexture,
                    1.0 - iconWidth * iconSeqNumber, 1.0 - iconHeight, iconWidth, iconHeight,
                    true, - 1.0
                );
                iconSeqNumber += 1.0;
                // armor icon
                ArmorObject* armor = InventoryHelper::currentArmorOf( player );
                Texture* armorTexture = NULL;
                if ( armor ) {
                    armorTexture = Cache::getFileAccessor() -> getTexture( armor -> getTextureId() );
                }
                _displayIcon(
                    armorTexture,
                    1.0 - iconWidth * iconSeqNumber, 1.0 - iconHeight, iconWidth, iconHeight,
                    true, - 1.0
                );
                iconSeqNumber += 1.0;
                // skill icon
                double skillId = player -> getParameter( EntityType::ATTRIBUTE_ACTIVE_SKILL );
                if ( !isnan( skillId ) ) {
                    if ( player -> getSkillLevel( skillId ) > 0 ) {
                        SkillType* skill = Cache::getFileAccessor() -> getSkillType( skillId );
                        Texture* skillTexture = NULL;
                        if ( skill ) {
                            skillTexture = Cache::getFileAccessor() -> getTexture( skill -> getTextureId() );
                            _displayIcon(
                                skillTexture,
                                1.0 - iconWidth * iconSeqNumber, 1.0 - iconHeight, iconWidth, iconHeight,
                                true, - 1.0
                            );
                            iconSeqNumber += 1.0;
                        }
                    }
                }
                // notification icon
                if ( CryptChatEngine::getInstance() -> hasUnreadMessages() ) {
                    _displayIcon(
                        Cache::getFileAccessor() -> getTexture( 10 ),
                        1.0 - iconWidth * iconSeqNumber, 1.0 - iconHeight, iconWidth, iconHeight,
                        false, 500.0
                    );
                }
            }
        }
    }

    static const int WAYPOINT_INDICATOR_TEXTURE_ID = 18;

    void displayWaypointIndicator( Vector2D waypoint, Vector2D playerPosition ) {
        Texture* waypointTexture = Cache::getFileAccessor() -> getTexture( WAYPOINT_INDICATOR_TEXTURE_ID );
        if ( waypointTexture ) {
            double distance = waypoint.dist2D( playerPosition ) / double( Map::BLOCK_SIZE );
            waypoint.x -= Camera::X + ( WINDOW_WIDTH >> 1 );
            waypoint.y -= Camera::Y + ( WINDOW_HEIGHT >> 1 );
            PolarVector2D waypointDir = PolarVector2D( waypoint );
            double max_limit = double( min( WINDOW_WIDTH, WINDOW_HEIGHT ) ) * 0.5 * 0.75;
            if ( waypointDir.r > max_limit ) {
                waypointDir.r = max_limit;
            }
            waypoint = Vector2D( waypointDir );
            waypoint.x += ( WINDOW_WIDTH >> 1 ) - ( waypointTexture -> getWidth() >> 1 );
            waypoint.y += ( WINDOW_HEIGHT >> 1 ) - ( waypointTexture -> getHeight() >> 1 );
            Draw::plain( waypoint.x, waypoint.y, waypointTexture );
            waypoint.y -= 12;
            Text::Color( 0, 254, 254 );
            SDL_Surface* distanceIndicator = Text::RenderOnly( "%.1lfm", distance );
            if ( distanceIndicator ) {
                SDL_Texture* distanceIndicatorTex = SDL_CreateTextureFromSurface( Renderer::getRenderer(), distanceIndicator );
                if ( distanceIndicatorTex ) {
                    Draw::plainCentered( waypoint.x, waypoint.y, distanceIndicator -> w, distanceIndicator -> h, distanceIndicatorTex );
                    SDL_DestroyTexture( distanceIndicatorTex );
                }
                SDL_FreeSurface( distanceIndicator );
            }
            Text::Color( 254, 254, 254 );
        }
    }

    private:

    void _displayIcon( Texture* texture, double x, double y, double w, double h, bool background, double blinkTime ) {
        if ( !texture && !background ) {
            return;
        }
        if ( blinkTime > 0.0 ) {
            double timer = Cache::gameTime() / blinkTime;
            if ( int( timer ) & 0x01 ) {
                return;
            }
        }
        ComponentProperties properties;
        SDL_Rect rectBG = properties.applyDimensions( x, y, w, h ).getSDLRectRawDisplay();
        if ( background ) {
            Uint8 r;
            Uint8 g;
            Uint8 b;
            Uint8 a;
            SDL_GetRenderDrawColor( Renderer::getRenderer(), &r, &g, &b, &a );
            SDL_SetRenderDrawColor( Renderer::getRenderer(), 64, 64, 64, 128 );
            SDL_RenderFillRect( Renderer::getRenderer(), &rectBG );
            SDL_SetRenderDrawColor( Renderer::getRenderer(), r, g, b, a );
        }
        if ( texture ) {
            SDL_Rect rect = properties.fitToTexture( texture ).getSDLRectRawDisplay();
            SDL_RenderCopy( Renderer::getRenderer(), texture -> getTexture(), NULL, &rect );
        }
    }

    void _checkHeatLevel( double currentLevel ) {
        if ( ( currentLevel >= UI_HEAT_RED_THRESHOLD_LEVEL ) && ( _ui_lastHeatValue < UI_HEAT_RED_THRESHOLD_LEVEL ) && ( !_ui_overHeat_strong ) ) {
            MessageManager::get() -> addMessage( MessageManager::LEVEL_WARN, LangStrings::getString( LangStringsID::MESSAGE_LOG_OVERHEAT_WARN_2 ) );
            _ui_overHeat = true;
            _ui_overHeat_strong = true;
        } else if ( ( currentLevel >= UI_HEAT_YELLOW_THRESHOLD_LEVEL ) && ( _ui_lastHeatValue < UI_HEAT_YELLOW_THRESHOLD_LEVEL ) && ( !_ui_overHeat ) ) {
            MessageManager::get() -> addMessage( MessageManager::LEVEL_WARN, LangStrings::getString( LangStringsID::MESSAGE_LOG_OVERHEAT_WARN_1 ) );
            _ui_overHeat = true;
        } else if ( ( currentLevel <= UI_HEAT_SAFE_THRESHOLD_LEVEL ) && ( _ui_overHeat ) ) {
            MessageManager::get() -> addMessage( MessageManager::LEVEL_NOTICE, LangStrings::getString( LangStringsID::MESSAGE_LOG_OVERHEAT_GONE ) );
            _ui_overHeat = false;
            _ui_overHeat_strong = false;
        }
        _ui_lastHeatValue = currentLevel;
    }

    const char* _getUIEnergyQualifier( double & energy ) {
        int qualifierLevel = 0;
        while ( energy >= 500.0 ) {
            if ( qualifierLevel >= _ENERGY_QUALIFIER_ARRAY_SIZE ) {
                return _ENERGY_QUALIFIER_ARRAY[ _ENERGY_QUALIFIER_ARRAY_SIZE - 1 ];
            }
            energy /= 1000.0;
            qualifierLevel++;
        }
        return _ENERGY_QUALIFIER_ARRAY[ qualifierLevel ];
    }

    void _changePerspectiveOnEndLevel() {
        LoadingPerspective* loading = dynamic_cast < LoadingPerspective* > ( Perspective::GetPerspective( PERSPECTIVE_LOADING_ID ) );
        if ( !loading ) {
            return;
        }
        int chapterId = loading -> getLoadedChapterId();
        int levelId = loading -> getLoadedLevelId();
        printf( "Loading DATA PRESERVED C/L: %d/%d\n", chapterId, levelId );
        ChapterType* chapter = Cache::getFileAccessor() -> getChapter( chapterId );
        if ( !chapter ) {
            return;
        }
        if ( chapter -> isLastLevelInChapter( levelId ) ) {
            // auto next chapter
            printf( " > Next chapter\n" );
            chapter = Cache::getFileAccessor() -> getChapterAfter( chapter -> getChapterId() );
            if ( !chapter ) {
                // game finished?
                Perspective::ChangeTo( PERSPECTIVE_MENU_ID );
                return;
            }
            chapterId = chapter -> getChapterId();
            levelId = chapter -> getFirstLevelId();
        } else {
            printf( " > Next level\n" );
            // auto next level
            levelId = chapter -> getLevelAfter( levelId );
        }
        printf( "Loading C/L: %d/%d\n", chapterId, levelId );
        loading -> setLoadingData( chapterId, levelId );
        EntityObject* player = Cache::getGameStateHolder() -> getPlayer();
        if ( player ) {
            player -> setNextLevelProgress( chapterId, levelId );
        }
        Perspective::ChangeTo( PERSPECTIVE_WARP_ID );
    }

    double _ui_lastHeatValue;
    bool _ui_overHeat;
    bool _ui_overHeat_strong;
    bool _levelFinished;

    static const int _ENERGY_QUALIFIER_ARRAY_SIZE = 8;
    static const char* _ENERGY_QUALIFIER_ARRAY[];

};
const char* LevelPerspective::_ENERGY_QUALIFIER_ARRAY[ _ENERGY_QUALIFIER_ARRAY_SIZE ] = {
    "kJ",
    "MJ",
    "GJ",
    "TJ",
    "PJ",
    "EJ",
    "ZJ",
    "YJ"
};


// here because I don't want to mess with include order anymore -.-
void ScriptNextLevelInstruction::perform() {
        LevelPerspective* level = dynamic_cast < LevelPerspective* > ( Perspective::GetPerspective( PERSPECTIVE_LEVEL_ID ) );
        if ( level ) {
            level -> setLevelFinished();
        }
    }

void LoadingPerspective::restartLevel() {
        LevelPerspective* level = dynamic_cast < LevelPerspective* > ( Perspective::GetPerspective( PERSPECTIVE_LEVEL_ID ) );
        if ( level ) {
            level -> enterLevel();
        }
    }
