#pragma once

class InventoryInGameForm : public FormDesignFactory {
    public:
    InventoryInGameForm() : FormDesignFactory() {
    }
    virtual ~InventoryInGameForm() {
    }

    static const int DEFAULT_FIELD_BACKGROUND_IMAGE = 7;
    static const int DEFAULT_FIELD_SELECTED_BACKGROUND_IMAGE = 8;
    static const int DEFAULT_FIELD_NAVIGATOR_ARROW = 9;

    static const int INVENTORY_GRID_WIDTH = 4;
    static const int INVENTORY_GRID_HEIGHT = 2;
    static const int INVENTORY_PAGE_SIZE = INVENTORY_GRID_WIDTH * INVENTORY_GRID_HEIGHT;

    static const unsigned int DEFAULT_BUTTON_COLOR = 0x80808080;
    static const unsigned int DEFAULT_ON_BUTTON_COLOR = 0x8080D0F0;

    static const double DEFAULT_INVENTORY_HEIGHT = 0.5;
    static const double DEFAULT_INVENTORY_MARGIN = 0.05;

    virtual Form* build( FormBuildParameters parameters ) {
        Form* form = new Form( parameters );

        ComponentProperties properties;
        properties.x = DEFAULT_INVENTORY_MARGIN;
        properties.y = DEFAULT_INVENTORY_MARGIN;
        double aspect = double( WINDOW_HEIGHT ) / double( WINDOW_WIDTH );
        double selfAspect = double( INVENTORY_GRID_HEIGHT ) / double( INVENTORY_GRID_WIDTH );
        properties.w = DEFAULT_INVENTORY_HEIGHT * aspect;
        properties.h = DEFAULT_INVENTORY_HEIGHT * selfAspect;

        ComponentProperties pageInfoProperties = properties;
        pageInfoProperties.y += pageInfoProperties.h;
        pageInfoProperties.h = ( pageInfoProperties.h / double( INVENTORY_GRID_HEIGHT ) * 0.75 );
        pageInfoProperties.x += pageInfoProperties.h;
        pageInfoProperties.w = ( pageInfoProperties.w / double( INVENTORY_GRID_WIDTH ) * 2.0 );
        pageInfoProperties.enabled = false;
        StaticText* pageInfo = new StaticText( "x/x", pageInfoProperties );

        form -> addComponent( pageInfo );

        _InventoryGridLayout* inventoryGrid = new _InventoryGridLayout( properties, INVENTORY_GRID_WIDTH, INVENTORY_GRID_HEIGHT, pageInfo );
        _resetContent( inventoryGrid );

        form -> addComponent( inventoryGrid );

        ComponentProperties navigatorBackProperties = properties;
        double gridWidth = navigatorBackProperties.w;
        navigatorBackProperties.y += navigatorBackProperties.h;
        navigatorBackProperties.w /= INVENTORY_GRID_WIDTH;
        navigatorBackProperties.h /= INVENTORY_GRID_HEIGHT;
        navigatorBackProperties.backgroundImage = DEFAULT_FIELD_NAVIGATOR_ARROW;
        navigatorBackProperties.rotation = 180.0;
        _PageNavigator* navigatorBack = new _PageNavigator( navigatorBackProperties, inventoryGrid, -1 );

        form -> addComponent( navigatorBack );

        ComponentProperties navigatorForwardProperties = navigatorBackProperties;
        navigatorForwardProperties.x += gridWidth - navigatorForwardProperties.w;
        navigatorForwardProperties.rotation = 0.0;
        _PageNavigator* navigatorForward = new _PageNavigator( navigatorForwardProperties, inventoryGrid, 1 );

        form -> addComponent( navigatorForward );

        // current weapon

        ComponentProperties weaponFieldProperties = properties;
        weaponFieldProperties.x = 1.0 - DEFAULT_INVENTORY_MARGIN - navigatorBackProperties.w;
        weaponFieldProperties.w = navigatorBackProperties.w;
        weaponFieldProperties.h = navigatorBackProperties.h;
        weaponFieldProperties.backgroundImage = DEFAULT_FIELD_BACKGROUND_IMAGE;

        _WeaponField* weaponField = new _WeaponField( weaponFieldProperties );

        form -> addComponent( weaponField );

        // current armor

        ComponentProperties armorFieldProperties = weaponFieldProperties;
        armorFieldProperties.y += weaponFieldProperties.h;

        _ArmorField* armorField = new _ArmorField( armorFieldProperties );

        form -> addComponent( armorField );

        // info box

        ComponentProperties itemInfoBoxProperties = properties;
        itemInfoBoxProperties.y += itemInfoBoxProperties.h + ( navigatorBackProperties.h * 2.0 );
        itemInfoBoxProperties.h = 1.0 - DEFAULT_INVENTORY_MARGIN - itemInfoBoxProperties.y;
        itemInfoBoxProperties.backgroundImage = -1;
        itemInfoBoxProperties.fillColor = 0x80505050;

        _ItemInfoBox* itemInfoBox = new _ItemInfoBox( itemInfoBoxProperties, inventoryGrid );

        form -> addComponent( itemInfoBox );

        // buttons

        ComponentProperties buttonProperties = properties;
        buttonProperties.x += properties.w + DEFAULT_INVENTORY_MARGIN;
        buttonProperties.w = 0.15;
        buttonProperties.h = navigatorBackProperties.h;
        buttonProperties.fillColor = DEFAULT_BUTTON_COLOR;

        _ChangeWeaponArmorButton* changeWAButton = new _ChangeWeaponArmorButton( buttonProperties, inventoryGrid );

        form -> addComponent( changeWAButton );

        buttonProperties.y += buttonProperties.h + 0.05;

        _ConvertToEnergyButton* convertToEnergyButton = new _ConvertToEnergyButton( buttonProperties, inventoryGrid );

        form -> addComponent( convertToEnergyButton );

        buttonProperties.y += buttonProperties.h + 0.05;

        _UpgradeButton* upgradeButton = new _UpgradeButton( buttonProperties, inventoryGrid );

        form -> addComponent( upgradeButton );

        weaponFieldProperties.w = 0.175;
        armorFieldProperties.w = 0.175;
        weaponFieldProperties.h = buttonProperties.h * 0.6;
        armorFieldProperties.h = buttonProperties.h * 0.6;
        weaponFieldProperties.x -= weaponFieldProperties.w + 0.02;
        armorFieldProperties.x -= armorFieldProperties.w + 0.02;
        weaponFieldProperties.y += buttonProperties.h * 0.2;
        armorFieldProperties.y += buttonProperties.h * 0.2;
        weaponFieldProperties.fillColor = DEFAULT_BUTTON_COLOR;
        armorFieldProperties.fillColor = DEFAULT_BUTTON_COLOR;
        weaponFieldProperties.backgroundImage = -1;
        armorFieldProperties.backgroundImage = -1;
        _TakeoffWeaponArmorButton* takeoffWpnButton = new _TakeoffWeaponArmorButton( weaponFieldProperties, inventoryGrid, Inventory::Item::TYPE_WEAPON );
        _TakeoffWeaponArmorButton* takeoffArmorButton = new _TakeoffWeaponArmorButton( armorFieldProperties, inventoryGrid, Inventory::Item::TYPE_ARMOR );
        form -> addComponent( takeoffWpnButton );
        form -> addComponent( takeoffArmorButton );

        return form;
    }

    private:

    class _GridField;

    class _InventoryGridLayout : public GridLayout {
        public:
        _InventoryGridLayout( ComponentProperties properties, int x, int y, StaticText* pageInfo ) : GridLayout( properties, x, y ) {
            _selectedItem = NULL;
            _pageInfo = pageInfo;
        }
        virtual ~_InventoryGridLayout() {
        }

        virtual void onDisplay();

        inline void setSelection( _GridField* item ) {
            _selectedItem = item;
        }

        inline _GridField* getSelection() {
            return _selectedItem;
        }

        void resetPageInfo( int currentPage, int lastPage ) {
            char buffer[ 128 ];
            sprintf( buffer, "%d/%d", currentPage + 1, lastPage + 1 );
            _pageInfo -> setText( string( buffer ) );
        }

        private:

        _GridField* _selectedItem;
        StaticText* _pageInfo;

    };


    class _GridField : public Button {
        public:
        _GridField( ComponentProperties properties, int x, int y, Inventory::Item assignedItem, _InventoryGridLayout* layout ) : Button( properties ) {
            _x = x;
            _y = y;
            _assignedItem = assignedItem;
            _motherLayout = layout;
        }
        virtual ~_GridField() {
        }

        inline int getX() {
            return _x;
        }

        inline int getY() {
            return _y;
        }

        virtual void onClick( double x, double y ) {
            //printf( "InventoryClick: %d / %d (%d x%d)\n", _x, _y, _assignedItem.id, _assignedItem.count );
            //WeaponObject* weaponObject = dynamic_cast < WeaponObject* > ( _assignedItem.itemPointer );
            /*if ( weaponObject ) {
                printf( " > Weapon ID: %d\n", weaponObject -> getTypeId() );
            }*/
            if ( _motherLayout ) {
                if ( _motherLayout -> getSelection() == this ) {
                    _motherLayout -> setSelection( NULL );
                } else {
                    _motherLayout -> setSelection( this );
                }
            }
        }

        void displayAsSelected() {
            ComponentProperties cProp = getProperties();
            cProp.backgroundImage = DEFAULT_FIELD_SELECTED_BACKGROUND_IMAGE;
            displayBackground( &cProp );
        }

        virtual void onDisplay() {
            Button::onDisplay();
            int textureId = -1;
            int upgradeLevel = 0;
            switch ( _assignedItem.type ) {
                case Inventory::Item::TYPE_WEAPON: {
                    WeaponObject* weaponObject = dynamic_cast < WeaponObject* > ( _assignedItem.itemPointer );
                    if ( weaponObject ) {
                        textureId = weaponObject -> getTextureId();
                        upgradeLevel = weaponObject -> getParameter( WeaponType::ATTRIBUTE_UPGRADE_LEVEL );
                    }
                    break;
                }
                case Inventory::Item::TYPE_ARMOR: {
                    ArmorObject* armorObject = dynamic_cast < ArmorObject* > ( _assignedItem.itemPointer );
                    if ( armorObject ) {
                        textureId = armorObject -> getTextureId();
                    }
                    break;
                }
                case Inventory::Item::TYPE_AMMO: {
                    BulletObject* bulletObject = dynamic_cast < BulletObject* > ( _assignedItem.itemPointer );
                    if ( bulletObject ) {
                        textureId = bulletObject -> getInventoryTextureId();
                    }
                    break;
                }
            }
            ComponentProperties propertiesTextured = getProperties();
            if ( textureId != -1 ) {
                Texture* texture = Cache::getFileAccessor() -> getTexture( textureId );
                if ( texture ) {
                    /*double aspect = double( texture -> getHeight() ) / double( texture -> getWidth() );
                    double nextH = propertiesTextured.h * aspect;
                    propertiesTextured.y += ( propertiesTextured.h - nextH ) / 2.0;
                    propertiesTextured.h = nextH;*/
                    propertiesTextured.fitToTexture( texture );
                    displayBackground( texture, &propertiesTextured );
                }
            }
            Text::Color( 254, 254, 254 );
            ComponentProperties componentProperties = getProperties();
            if ( _assignedItem.count > 1 ) {
                Text::Write( Renderer::getRenderer(), componentProperties.x * WINDOW_WIDTH, componentProperties.y * WINDOW_HEIGHT, "x%d", _assignedItem.count );
            }
            if ( upgradeLevel > 0 ) {
                Text::Color( 254, 254, 254 );
                Text::WriteRight( Renderer::getRenderer(), ( componentProperties.x + componentProperties.w ) * WINDOW_WIDTH, componentProperties.y * WINDOW_HEIGHT, "+%d", upgradeLevel );
            }
        }

        inline Inventory::Item getAssignedItem() {
            return _assignedItem;
        }

        private:
        int _x;
        int _y;
        Inventory::Item _assignedItem;
        _InventoryGridLayout* _motherLayout;
    };

    class _WeaponField : public Button {
        public:
        _WeaponField( ComponentProperties properties ) : Button( properties ) {}
        virtual ~_WeaponField() {}
        virtual void onClick( double x, double y ) {}
        virtual void onDisplay() {
            Button::onDisplay();
            EntityObject* player = Cache::getGameStateHolder() -> getPlayer();
            if ( player ) {
                WeaponObject* currentWeapon = InventoryHelper::currentWeaponOf( player );
                if ( currentWeapon ) {
                    int textureId = currentWeapon -> getTextureId();
                    if ( textureId != -1 ) {
                        Texture* texture = Cache::getFileAccessor() -> getTexture( textureId );
                        if ( texture ) {
                            ComponentProperties propertiesTextured = getProperties();
                            /*double aspect = double( texture -> getHeight() ) / double( texture -> getWidth() );
                            double nextH = propertiesTextured.h * aspect;
                            propertiesTextured.y += ( propertiesTextured.h - nextH ) / 2.0;
                            propertiesTextured.h = nextH;*/
                            propertiesTextured.fitToTexture( texture );
                            displayBackground( texture, &propertiesTextured );
                        }
                    }
                }
            }
        }
    };

    class _ArmorField : public Button {
        public:
        _ArmorField( ComponentProperties properties ) : Button( properties ) {}
        virtual ~_ArmorField() {}
        virtual void onClick( double x, double y ) {}
        virtual void onDisplay() {
            Button::onDisplay();
            EntityObject* player = Cache::getGameStateHolder() -> getPlayer();
            if ( player ) {
                ArmorObject* currentArmor = InventoryHelper::currentArmorOf( player );
                if ( currentArmor ) {
                    int textureId = currentArmor -> getTextureId();
                    if ( textureId != -1 ) {
                        Texture* texture = Cache::getFileAccessor() -> getTexture( textureId );
                        if ( texture ) {
                            ComponentProperties propertiesTextured = getProperties();
                            /*double aspect = double( texture -> getHeight() ) / double( texture -> getWidth() );
                            double nextH = propertiesTextured.h * aspect;
                            propertiesTextured.y += ( propertiesTextured.h - nextH ) / 2.0;
                            propertiesTextured.h = nextH;*/
                            propertiesTextured.fitToTexture( texture );
                            displayBackground( texture, &propertiesTextured );
                        }
                    }
                }
            }
        }
    };

    class _ItemInfoBox : public Component {
        public:
        _ItemInfoBox( ComponentProperties properties, _InventoryGridLayout* layout ) : Component( properties ) {
            _layout = layout;
        }
        virtual ~_ItemInfoBox() {}

        virtual void onDisplay() {
            displayBackground();
            if ( _layout ) {
                _GridField* field = _layout -> getSelection();
                if ( field ) {
                    Inventory::Item selectedItem = field -> getAssignedItem();
                    printItemInfo( selectedItem );
                }
            }
        }

        void printItemInfo( Inventory::Item & item ) {
            Text::Color( 254, 254, 254 );
            int x = getProperties().x * WINDOW_WIDTH + 8;
            int y = getProperties().y * WINDOW_HEIGHT + 8;
            const int SPACING = 24;
            switch ( item.type ) {
                case Inventory::Item::TYPE_WEAPON: {
                    WeaponObject* weaponObject = dynamic_cast < WeaponObject* > ( item.itemPointer );
                    if ( weaponObject ) {
                        int upgradeLevel = int( weaponObject -> getParameter( WeaponType::ATTRIBUTE_UPGRADE_LEVEL ) );
                        if ( upgradeLevel > 0 ) {
                            Text::Write( Renderer::getRenderer(), x, y, "Schema name: %s (+%d)", ( weaponObject -> getTypeName() ).c_str(), upgradeLevel );
                        } else {
                            Text::Write( Renderer::getRenderer(), x, y, "Schema name: %s", ( weaponObject -> getTypeName() ).c_str() );
                        }
                        y += SPACING;
                        Text::Write( Renderer::getRenderer(), x, y, "Damage: %d - %d",
                                    int( weaponObject -> getParameter( WeaponType::ATTRIBUTE_DAMAGE_MIN ) ),
                                    int( weaponObject -> getParameter( WeaponType::ATTRIBUTE_DAMAGE_MAX ) )
                        );
                        y += SPACING;
                        Text::Write( Renderer::getRenderer(), x, y, "RPM: %.1lf", 60.0 / double( weaponObject -> getShootDelay() ) );
                        y += SPACING;
                        double precision = weaponObject -> getParameter( WeaponType::ATTRIBUTE_PRECISION );
                        Text::Write( Renderer::getRenderer(), x, y, "Base precision: %.2lf deg",
                                     atan2( precision, WeaponObject::PRECISION_CALC_RADIUS_DEFAULT ) * 180.0 / M_PI
                        );
                        y += SPACING;
                        Text::Write( Renderer::getRenderer(), x, y, "Clip size: %d", weaponObject -> getClipSize() );
                        y += SPACING;
                        Text::Write( Renderer::getRenderer(), x, y, "BPS: %d", int( weaponObject -> getParameter( WeaponType::ATTRIBUTE_BUCK ) ) );
                        y += SPACING;
                        /// TODO write
                    }
                    break;
                }
                case Inventory::Item::TYPE_ARMOR: {
                    ArmorObject* armorObject = dynamic_cast < ArmorObject* > ( item.itemPointer );
                    if ( armorObject ) {
                        Text::Write( Renderer::getRenderer(), x, y, "Schema name: %s", ( armorObject -> getTypeName() ).c_str() );
                        /// TODO write
                    }
                    break;
                }
                case Inventory::Item::TYPE_AMMO: {
                    BulletObject* bulletObject = dynamic_cast < BulletObject* > ( item.itemPointer );
                    if ( bulletObject ) {
                        Text::Write( Renderer::getRenderer(), x, y, "Schema name: %s", ( bulletObject -> getTypeName() ).c_str() );
                        /// TODO write
                    }
                    break;
                }
            }
        }

        private:

        _InventoryGridLayout* _layout;

    };

    class _GridFieldFactory {
        public:
        _GridFieldFactory( _InventoryGridLayout* motherLayout ) {
            _motherLayout = motherLayout;
        }
        virtual ~_GridFieldFactory() {
        }
        Component* createNewComponent( int x, int y, Inventory::Item assignedItem ) {
            ComponentProperties properties;
            properties.backgroundImage = DEFAULT_FIELD_BACKGROUND_IMAGE;
            return new _GridField( properties, x, y, assignedItem, _motherLayout );
        }
        private:
        _InventoryGridLayout* _motherLayout;
    };

    class _PageNavigator : public Button {
        public:
        _PageNavigator( ComponentProperties properties, _InventoryGridLayout* layout, int dir ) : Button( properties ) {
            _layout = layout;
            _dir = dir;
        }
        virtual ~_PageNavigator() {
        }
        virtual void onClick( double x, double y ) {
            EntityObject* player = Cache::getGameStateHolder() -> getPlayer();
            if ( player ) {
                int inventoryPage = player -> getParameter( EntityType::ATTRIBUTE_INVENTORY_PAGE_NUMBER );
                inventoryPage += _dir;
                player -> setParameter( EntityType::ATTRIBUTE_INVENTORY_PAGE_NUMBER, inventoryPage );
                _resetContent( _layout );
            }
        }

        private:
        _InventoryGridLayout* _layout;
        int _dir;
    };

    class _InventoryButton : public TextButton {
        public:
        _InventoryButton( string name, ComponentProperties properties ) : TextButton( name, properties ) {}
        virtual ~_InventoryButton() {}
        virtual void onMouseMoveIn( double x, double y ) {
            ComponentProperties prop = getProperties();
            prop.fillColor = DEFAULT_ON_BUTTON_COLOR;
            setProperties( prop );
        }
        virtual void onMouseMoveOut( double x, double y ) {
            ComponentProperties prop = getProperties();
            prop.fillColor = DEFAULT_BUTTON_COLOR;
            setProperties( prop );
        }
    };

    class _ChangeWeaponArmorButton : public _InventoryButton {
        public:
        _ChangeWeaponArmorButton( ComponentProperties properties, _InventoryGridLayout* layout ) : _InventoryButton( LangStrings::getString( LangStringsID::INVENTORY_CHANGE ), properties ) {
            _layout = layout;
        }
        virtual ~_ChangeWeaponArmorButton() {
        }
        virtual void onClick( double x, double y ) {
            EntityObject* player = Cache::getGameStateHolder() -> getPlayer();
            if ( !player ) {
                return;
            }
            if ( _layout ) {
                _GridField* field = _layout -> getSelection();
                if ( field ) {
                    Inventory::Item selectedItem = field -> getAssignedItem();
                    switch ( selectedItem.type ) {
                        case Inventory::Item::TYPE_WEAPON: {
                            WeaponObject* weaponObject = dynamic_cast < WeaponObject* > ( selectedItem.itemPointer );
                            if ( weaponObject ) {
                                player -> setEquippedWeaponId( weaponObject -> getId() );
                            }
                            break;
                        }
                        case Inventory::Item::TYPE_ARMOR: {
                            ArmorObject* armorObject = dynamic_cast < ArmorObject* > ( selectedItem.itemPointer );
                            if ( armorObject ) {
                                player -> setEquippedArmorId( armorObject -> getId() );
                            }
                            break;
                        }
                    }
                }
            }
        }
        private:
        _InventoryGridLayout* _layout;
    };

    class _TakeoffWeaponArmorButton : public _InventoryButton {
        public:
        _TakeoffWeaponArmorButton( ComponentProperties properties, _InventoryGridLayout* layout, int type ) : _InventoryButton( LangStrings::getString( LangStringsID::INVENTORY_TAKEOFF ), properties ) {
            _layout = layout;
            _type = type;
        }
        virtual ~_TakeoffWeaponArmorButton() {
        }
        virtual void onClick( double x, double y ) {
            EntityObject* player = Cache::getGameStateHolder() -> getPlayer();
            if ( !player ) {
                return;
            }
            if ( _layout ) {
                switch ( _type ) {
                    case Inventory::Item::TYPE_WEAPON: {
                        player -> setEquippedWeaponId( -1 );
                        break;
                    }
                    case Inventory::Item::TYPE_ARMOR: {
                        player -> setEquippedArmorId( -1 );
                        break;
                    }
                }
            }
        }
        private:
        _InventoryGridLayout* _layout;
        int _type;
    };

    class _ConvertToEnergyButton : public _InventoryButton {
        public:
        _ConvertToEnergyButton( ComponentProperties properties, _InventoryGridLayout* layout ) : _InventoryButton( LangStrings::getString( LangStringsID::INVENTORY_CONVERT ), properties ) {
            _layout = layout;
        }
        virtual ~_ConvertToEnergyButton() {
        }
        virtual void onClick( double x, double y ) {
            EntityObject* player = Cache::getGameStateHolder() -> getPlayer();
            if ( !player ) {
                return;
            }
            Inventory* inventory = player -> getInventory();
            if ( !inventory ) {
                return;
            }
            if ( _layout ) {
                _GridField* field = _layout -> getSelection();
                if ( field ) {
                    Inventory::Item selectedItem = field -> getAssignedItem();
                    switch ( selectedItem.type ) {
                        case Inventory::Item::TYPE_WEAPON: {
                            WeaponObject* weaponObject = dynamic_cast < WeaponObject* > ( selectedItem.itemPointer );
                            if ( weaponObject ) {
                                if ( player -> getEquippedWeaponId() == weaponObject -> getId() ) {
                                    int otherWeapon = _searchOtherWeapon( weaponObject -> getId() );
                                    /*if ( otherWeapon < 0 ) {
                                        break;
                                    }*/
                                    player -> setEquippedWeaponId( otherWeapon );
                                }
                                double energyRestored = weaponObject -> getParameter( WeaponType::ATTRIBUTE_ENERGY_CONVERSION_VALUE );
                                player -> addStaticEnergy( energyRestored );
                                inventory -> removeFromInventory( Inventory::Item::TYPE_WEAPON, weaponObject -> getId() );
                                delete weaponObject;
                                INVENTORY_FORCE_REFRESH_SIGNAL = true;
                            }
                            break;
                        }
                        case Inventory::Item::TYPE_ARMOR: {
                            ArmorObject* armorObject = dynamic_cast < ArmorObject* > ( selectedItem.itemPointer );
                            if ( armorObject ) {
                                if ( player -> getEquippedArmorId() == armorObject -> getId() ) {
                                    player -> setEquippedArmorId( -1 );
                                }
                                double energyRestored = armorObject -> getParameter( WeaponType::ATTRIBUTE_ENERGY_CONVERSION_VALUE );
                                player -> addStaticEnergy( energyRestored );
                                inventory -> removeFromInventory( Inventory::Item::TYPE_ARMOR, armorObject -> getId() );
                                delete armorObject;
                                INVENTORY_FORCE_REFRESH_SIGNAL = true;
                            }
                            break;
                        }
                    }
                }
            }
        }
        private:
        _InventoryGridLayout* _layout;
    };

    class _UpgradeButton : public _InventoryButton {
        public:
        _UpgradeButton( ComponentProperties properties, _InventoryGridLayout* layout ) : _InventoryButton( LangStrings::getString( LangStringsID::INVENTORY_UPGRADE ), properties ) {
            _layout = layout;
        }
        virtual ~_UpgradeButton() {
        }
        virtual void onClick( double x, double y ) {
            /// TODO
            EntityObject* player = Cache::getGameStateHolder() -> getPlayer();
            if ( !player ) {
                return;
            }
            Inventory* inventory = player -> getInventory();
            if ( !inventory ) {
                return;
            }
            if ( _layout ) {
                _GridField* field = _layout -> getSelection();
                if ( field ) {
                    Inventory::Item selectedItem = field -> getAssignedItem();
                    switch ( selectedItem.type ) {
                        case Inventory::Item::TYPE_WEAPON: {
                            WeaponObject* weaponObject = dynamic_cast < WeaponObject* > ( selectedItem.itemPointer );
                            if ( weaponObject ) {
                                int otherWeapon = _searchOtherWeapon( weaponObject -> getId() );
                                if ( otherWeapon < 0 ) {
                                    MessageManager::get() -> addMessage( MessageManager::LEVEL_WARN, LangStrings::getString( LangStringsID::MESSAGE_LOG_CANNOT_UPGRADE ) );
                                    break;
                                }
                                double energyForUpgrade = weaponObject -> getParameter( WeaponType::ATTRIBUTE_ENERGY_UPGRADE_VALUE );
                                if ( player -> payStaticEnergy( energyForUpgrade ) ) {
                                    if ( weaponObject -> upgrade() ) {
                                        MessageManager::get() -> addMessage( MessageManager::LEVEL_NOTICE, LangStrings::getString( LangStringsID::MESSAGE_LOG_UPGRADE_SUCCESS ) );
                                    } else {
                                        MessageManager::get() -> addMessage( MessageManager::LEVEL_ERROR, LangStrings::getString( LangStringsID::MESSAGE_LOG_UPGRADE_FAIL ) );
                                        if ( player -> getEquippedWeaponId() == weaponObject -> getId() ) {
                                            player -> setEquippedWeaponId( otherWeapon );
                                        }
                                        inventory -> removeFromInventory( Inventory::Item::TYPE_WEAPON, weaponObject -> getId() );
                                        delete weaponObject;
                                    }
                                }
                                INVENTORY_FORCE_REFRESH_SIGNAL = true;
                            }
                            break;
                        }
                        case Inventory::Item::TYPE_ARMOR: {
                            ArmorObject* armorObject = dynamic_cast < ArmorObject* > ( selectedItem.itemPointer );
                            if ( armorObject ) {
                                /*
                                if ( player -> getEquippedArmorId() == armorObject -> getId() ) {
                                    player -> setEquippedArmorId( -1 );
                                }
                                double energyRestored = armorObject -> getParameter( WeaponType::ATTRIBUTE_ENERGY_CONVERSION_VALUE );
                                player -> addStaticEnergy( energyRestored );
                                inventory -> removeFromInventory( Inventory::Item::TYPE_ARMOR, armorObject -> getId() );
                                delete armorObject;
                                */
                                INVENTORY_FORCE_REFRESH_SIGNAL = true;
                            }
                            break;
                        }
                    }
                }
            }
        }
        private:
        _InventoryGridLayout* _layout;

    };

    static int _searchOtherWeapon( int weaponId ) {
        EntityObject* player = Cache::getGameStateHolder() -> getPlayer();
        if ( player ) {
            Inventory* inventory = player -> getInventory();
            if ( inventory ) {
                vector < Inventory::Item > items = inventory -> queryAllItems();
                for ( vector < Inventory::Item >::iterator i = items.begin(); i != items.end(); i++ ) {
                    Inventory::Item item = ( *i );
                    if ( item.type == Inventory::Item::TYPE_WEAPON ) {
                        WeaponObject* weaponObject = dynamic_cast < WeaponObject* > ( item.itemPointer );
                        if ( weaponObject ) {
                            int observedId = weaponObject -> getId();
                            if ( ( observedId != weaponId ) && ( observedId >= 0 ) ) {
                                return observedId;
                            }
                        }
                    }
                }
            }
        }
        return -1;
    }

    static void _fillInvGrid( _InventoryGridLayout* grid, int w, int h, int queryStartPage, vector < Inventory::Item > & items ) {
        if ( !grid ) {
            return;
        }
        vector < Inventory::Item >::iterator itemIterator = items.begin();
        itemIterator += queryStartPage * INVENTORY_PAGE_SIZE;
        _GridFieldFactory gff = _GridFieldFactory( grid );
        for ( int j = 0; j < h; j++ ) {
            for ( int i = 0; i < w; i++ ) {
                Component* component = NULL;
                if ( itemIterator != items.end() ) {
                    component = gff.createNewComponent( i, j, *itemIterator );
                    itemIterator++;
                } else {
                    component = gff.createNewComponent( i, j, Inventory::Item() );
                }
                if ( component ) {
                    grid -> deleteComponent( i, j );
                    grid -> addComponent( component, i, j );
                }
            }
        }
    }

    static void _fixPage( vector < Inventory::Item > & items, int & inventoryPage, int lastPage ) {
        if ( inventoryPage < 0 ) {
            inventoryPage = lastPage;
        }
        inventoryPage = ( inventoryPage % ( lastPage + 1 ) );
    }

    static void _resetContent( _InventoryGridLayout* inventoryGrid ) {
        if ( !inventoryGrid ) {
            return;
        }
        EntityObject* player = Cache::getGameStateHolder() -> getPlayer();
        if ( player ) {
            Inventory* inventory = player -> getInventory();
            if ( inventory ) {
                vector < Inventory::Item > items = inventory -> queryAllItems();
                int itemCount = items.size();
                int lastPage = ( itemCount - 1 ) / INVENTORY_PAGE_SIZE;
                if ( lastPage < 0 ) {
                    lastPage = 0;
                }
                inventoryGrid -> setSelection( NULL );
                int inventoryPage = player -> getParameter( EntityType::ATTRIBUTE_INVENTORY_PAGE_NUMBER );
                _fixPage( items, inventoryPage, lastPage );
                inventoryGrid -> resetPageInfo( inventoryPage, lastPage );
                _fillInvGrid( inventoryGrid, INVENTORY_GRID_WIDTH, INVENTORY_GRID_HEIGHT, inventoryPage, items );
                player -> setParameter( EntityType::ATTRIBUTE_INVENTORY_PAGE_NUMBER, inventoryPage );
            }
        }
    }

};

void InventoryInGameForm::_InventoryGridLayout::onDisplay() {
    if ( INVENTORY_FORCE_REFRESH_SIGNAL ) {
        int x = -1;
        int y = -1;
        if ( _selectedItem ) {
            x = _selectedItem -> getX();
            y = _selectedItem -> getY();
        }
        INVENTORY_FORCE_REFRESH_SIGNAL = false;
        InventoryInGameForm::_resetContent( this );
        if ( ( x >= 0 ) && ( y >= 0 ) ) {
            _GridField* field = dynamic_cast < _GridField* > ( getComponent( x, y ) );
            _selectedItem = field;
        }
        printf( "Inventory reset performed\n" );
    }
    GridLayout::onDisplay();
    if ( _selectedItem ) {
        _selectedItem -> displayAsSelected();
    }
}
