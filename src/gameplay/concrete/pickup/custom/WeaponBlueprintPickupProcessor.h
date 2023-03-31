#pragma once

class WeaponBlueprintPickupProcessor : public PickupProcessor {
    public:
    WeaponBlueprintPickupProcessor() : PickupProcessor() {
    }
    virtual ~WeaponBlueprintPickupProcessor() {
    }

    virtual bool onPick( EntityObject* picker, PickupObject* pickup ) {
        int weaponId = ( int ) pickup -> getCustomValue( 0 );
        WeaponType* type = Cache::getFileAccessor() -> getWeaponType( weaponId );
        if ( !type ) {
            return true;
        }
        InventoryHelper::createWeapon( picker, type );
        return true;
    }

};


