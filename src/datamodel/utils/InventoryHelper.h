#pragma once

class InventoryHelper {
    private:
    InventoryHelper() {}
    ~InventoryHelper() {}

    public:

    static bool createWeapon( EntityObject* entity, WeaponType* weaponType ) {
        if ( entity && weaponType ) {
            WeaponObject* weaponObject = weaponType -> createObject < WeaponObject > ( entity -> getInventory() -> nextId( Inventory::Item::TYPE_WEAPON ) );
            if ( weaponObject ) {
                Inventory::Item item;
                item.id = weaponObject -> getId();
                item.type = Inventory::Item::TYPE_WEAPON;
                item.itemPointer = weaponObject;
                item.count = 1;
                if ( entity -> getInventory() -> addItemIfAbsent( item ) ) {
                    return item.id;
                }
                delete weaponObject;
            }
        }
        return -1;
    }

    static WeaponObject* currentWeaponOf( EntityObject* entity ) {
        if ( !entity ) {
            return NULL;
        }
        if ( !entity -> getInventory() ) {
            return NULL;
        }
        Inventory::Item item = entity -> getInventory() -> getItem( Inventory::Item::TYPE_WEAPON, entity -> getEquippedWeaponId() );
        if ( item.id >= 0 ) {
            return dynamic_cast < WeaponObject* > ( item.itemPointer );
        }
        return ( WeaponObject* ) entity -> getMeleeWeapon();
    }

    static ArmorObject* currentArmorOf( EntityObject* entity ) {
        if ( !entity ) {
            return NULL;
        }
        if ( !entity -> getInventory() ) {
            return NULL;
        }
        Inventory::Item item = entity -> getInventory() -> getItem( Inventory::Item::TYPE_ARMOR, entity -> getEquippedArmorId() );
        if ( item.id >= 0 ) {
            return dynamic_cast < ArmorObject* > ( item.itemPointer );
        }
        return NULL;
    }

    static WeaponObject* getWeaponOf( EntityObject* entity, int weaponId ) {
        if ( !entity ) {
            return NULL;
        }
        if ( !entity -> getInventory() ) {
            return NULL;
        }
        Inventory::Item item = entity -> getInventory() -> getItem( Inventory::Item::TYPE_WEAPON, weaponId );
        if ( item.id >= 0 ) {
            return dynamic_cast < WeaponObject* > ( item.itemPointer );
        }
        return NULL;
    }

    static void addAmmo( EntityObject* entity, int ammoTypeId, int ammoCount ) {
        if ( entity ) {
            if ( entity -> getInventory() ) {
                Inventory::Item item;
                item.id = ammoTypeId;
                item.type = Inventory::Item::TYPE_AMMO;
                item.count = ammoCount;
                entity -> getInventory() -> addItemMerge( item );
            }
        }
    }

    static int getAmmoState( EntityObject* entity, int ammoTypeId ) {
        if ( entity ) {
            if ( entity -> getInventory() ) {
                Inventory::Item item = entity -> getInventory() -> getItem( Inventory::Item::TYPE_AMMO, ammoTypeId );
                if ( item.type == Inventory::Item::TYPE_AMMO ) {
                    return item.count;
                }
            }
        }
        return -1;
    }

    static void selectNextWeapon( EntityObject* entity, int dir ) {
        if ( entity ) {
            Inventory* inventory = entity -> getInventory();
            if ( inventory ) {
                Inventory::Item equippedItem = inventory -> getItem( Inventory::Item::TYPE_WEAPON, entity -> getEquippedWeaponId() );
                void* equippedObject = equippedItem.itemPointer;
                vector < Inventory::Item > items = inventory -> queryAllItems();
                if ( int( items.size() ) <= 0 ) {
                    return;
                }
                int selectionIndex = -1;
                int currentIndex = 0;
                int firstWeaponIndex = -1;
                vector < Inventory::Item > weapons;
                for ( vector < Inventory::Item >::iterator i = items.begin(); i != items.end(); i++ ) {
                    Inventory::Item item = ( *i );
                    if ( item.type == Inventory::Item::TYPE_WEAPON ) {
                        weapons.push_back( item );
                    }
                }
                if ( int( weapons.size() ) <= 0 ) {
                    return;
                }
                for ( vector < Inventory::Item >::iterator i = weapons.begin(); i != weapons.end(); i++ ) {
                    Inventory::Item item = ( *i );
                    if ( item.itemPointer == equippedObject ) {
                        selectionIndex = currentIndex;
                        break;
                    }
                    currentIndex++;
                }
                if ( selectionIndex < 0 ) {
                    selectionIndex = firstWeaponIndex;
                } else {
                    selectionIndex += dir;
                    if ( selectionIndex < 0 ) {
                        selectionIndex = int( weapons.size() ) - 1;
                    } else if ( selectionIndex >= int( weapons.size() ) ) {
                        selectionIndex = 0;
                    }
                }
                if ( ( selectionIndex >= 0 ) && ( selectionIndex < int( weapons.size() ) ) ) {
                    Inventory::Item selectedItem = weapons[ selectionIndex ];
                    if ( selectedItem.type == Inventory::Item::TYPE_WEAPON ) {
                        WeaponObject* object = dynamic_cast < WeaponObject* > ( selectedItem.itemPointer );
                        if ( object ) {
                            entity -> setEquippedWeaponId( object -> getId() );
                        }
                    }
                }
            }
        }
    }

    static void compressInventoryData( EntityObject* object ) {
        if ( !object ) {
            return;
        }
        Inventory* inventory = object -> getInventory();
        if ( !inventory ) {
            return;
        }
        map < int, string > values;
        vector < Inventory::Item > items = inventory -> queryAllItems();
        for ( int i = 0; i < int( items.size() ); i++ ) {
            string itemStringValue;
            if ( _toString( items[ i ], itemStringValue ) ) {
                //printf( "itemStringValue = \"%s\"\n", itemStringValue.c_str() );
                values[ i ] = itemStringValue;
            }
        }
        object -> setStringMultiValuedParameter( EntityType::ATTRIBUTE_INVENTORY_STORAGE, values );
    }

    static void restoreInventoryData( EntityObject* object ) {
        if ( !object ) {
            return;
        }
        Inventory* inventory = object -> getInventory();
        if ( !inventory ) {
            return;
        }
        //printf( "SMV queried...\n" );
        map < int, string > values;
        if ( !object -> getStringMultiValuedParameter( EntityType::ATTRIBUTE_INVENTORY_STORAGE, values ) ) {
            return;
        }
        //printf( "Resetting inventory...\n" );
        inventory -> clear();
        for ( map < int, string >::iterator i = values.begin(); i != values.end(); i++ ) {
            Inventory::Item item;
            //printf( " > Process %s\n", ( i -> second ).c_str() );
            if ( _fromString( i -> second, item ) ) {
                //printf( " > OK 1\n" );
                if ( !inventory -> addItemIfAbsent( item ) ) {
                    //printf( " > FAIL ADD\n" );
                    if ( item.itemPointer ) {
                        //printf( " > Remove PTR\n" );
                        delete item.itemPointer;
                    }
                } else {
                    //printf( " > OK 2\n" );
                }
            }
        }
    }

    private:

    static const char INVITEM_DELIM = ':';

    static bool _toString( Inventory::Item & item, string & str ) {
        int typeId = item.type;
        int id = item.id;
        int count = item.count;
        InteractiveObject* object = item.itemPointer;
        int customId = -1;
        switch ( typeId ) {
            case Inventory::Item::TYPE_WEAPON: {
                WeaponObject* wpn = dynamic_cast < WeaponObject* > ( object );
                if ( wpn ) {
                    customId = wpn -> getTypeId();
                }
                break;
            }
            case Inventory::Item::TYPE_ARMOR: {
                ArmorObject* arm = dynamic_cast < ArmorObject* > ( object );
                if ( arm ) {
                    customId = arm -> getTypeId();
                }
                break;
            }
            case Inventory::Item::TYPE_AMMO: {
                // not supported since Energy Update
                return false;
            }
        }
        if ( customId != -1 ) {
            string genStr =
                StringUtils::toString( typeId ) + INVITEM_DELIM +
                StringUtils::toString( id ) + INVITEM_DELIM +
                StringUtils::toString( count ) + INVITEM_DELIM +
                StringUtils::toString( customId );
            str = genStr;
            return true;
        }
        return false;
    }

    static bool _fromString( string & str, Inventory::Item & item ) {
        Inventory::Item newItem;
        vector < string > splits = StringUtils::split( str, INVITEM_DELIM );
        if ( splits.size() != 4 ) {
            return false;
        }
        int values[ 4 ];
        for ( int i = 0; i < 4; i++ ) {
            if ( !StringUtils::toInteger( splits[ i ], &( values[ i ] ) ) ) {
                return false;
            }
        }
        newItem.type = values[ 0 ];
        newItem.id = values[ 1 ];
        newItem.count = values[ 2 ];
        int customId = values[ 3 ];
        InteractiveObject* object = NULL;
        switch ( newItem.type ) {
            case Inventory::Item::TYPE_WEAPON: {
                WeaponType* wpnType = Cache::getFileAccessor() -> getWeaponType( customId );
                if ( wpnType ) {
                    object = wpnType -> createObject < WeaponObject > ( newItem.id );
                }
                break;
            }
            case Inventory::Item::TYPE_ARMOR: {
                ArmorType* armType = Cache::getFileAccessor() -> getArmorType( customId );
                if ( armType ) {
                    object = armType -> createObject < ArmorObject > ( newItem.id );
                }
                break;
            }
            case Inventory::Item::TYPE_AMMO: {
                // not supported since Energy Update
                return false;
            }
        }
        if ( object ) {
            newItem.itemPointer = object;
            item = newItem;
            return true;
        }
        return false;
    }
};


