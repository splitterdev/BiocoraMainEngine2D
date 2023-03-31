#pragma once

class Inventory {
    public:
    Inventory() {}
    virtual ~Inventory() {
        clear();
    }

    class Item {
        public:
        Item() {
            type = TYPE_NONE;
            id = -1;
            count = 1;
            itemPointer = NULL;
        }
        ~Item() {
        }

        static const int TYPE_NONE = 0;
        static const int TYPE_WEAPON = 1;
        static const int TYPE_ARMOR = 2;
        static const int TYPE_AMMO = 3;

        int type;
        int id;
        int count;
        InteractiveObject* itemPointer;

        bool compare( Item & item ) {
            return (
                type == item.type &&
                id == item.id
            );
        }

    };

    Item getItem( int type, int itemId ) {
        map < int, map < int, Item >* >::iterator typed = _items.find( type );
        if ( typed != _items.end() ) {
            map < int, Item >* typedItems = typed -> second;
            map < int, Item >::iterator item = typedItems -> find( itemId );
            if ( item != typedItems -> end() ) {
                return item -> second;
            }
        }
        return Item();
    }

    vector < Item > queryAllItems() {
        vector < Item > qItems;
        for ( map < int, map < int, Item >* >::iterator i = _items.begin(); i != _items.end(); i++ ) {
            map < int, Item >* typedItems = i -> second;
            for ( map < int, Item >::iterator j = typedItems -> begin(); j != typedItems -> end(); j++ ) {
                qItems.push_back( j -> second );
            }
        }
        return qItems;
    }

    int nextId( int type ) {
        map < int, map < int, Item >* >::iterator typed = _items.find( type );
        if ( typed != _items.end() ) {
            map < int, Item >* typedItems = typed -> second;
            map < int, Item >::reverse_iterator item = typedItems -> rbegin();
            if ( item != typedItems -> rend() ) {
                return ( item -> first ) + 1;
            }
        }
        return 0;
    }

    bool addItemIfAbsent( Item item ) {
        if ( item.type == Item::TYPE_NONE ) {
            return false;
        }
        Item check = getItem( item.type, item.id );
        if ( item.compare( check ) ) {
            return false;
        }
        _addForce( item );
        return true;
    }

    void addItemMerge( Item item ) {
        if ( item.type == Item::TYPE_NONE ) {
            return;
        }
        Item previousItem = getItem( item.type, item.id );
        if ( previousItem.type == item.type ) {
            item.count += previousItem.count;
        }
        _addForce( item );
    }

    void clear() {
        for ( map < int, map < int, Item >* >::iterator i = _items.begin(); i != _items.end(); i++ ) {
            map < int, Item >* items = i -> second;
            if ( items ) {
                for ( map < int, Item >::iterator j = items -> begin(); j != items -> end(); j++ ) {
                    Item item = j -> second;
                    if ( item.itemPointer ) {
                        delete item.itemPointer;
                    }
                }
                delete items;
            }
        }
        _items.clear();
    }

    void removeFromInventory( int type, int itemId ) {
        map < int, map < int, Item >* >::iterator typed = _items.find( type );
        if ( typed != _items.end() ) {
            map < int, Item >* typedItems = typed -> second;
            map < int, Item >::iterator item = typedItems -> find( itemId );
            if ( item != typedItems -> end() ) {
                typedItems -> erase( item );
            }
        }
    }

    private:

    void _addForce( Item & item ) {
        map < int, map < int, Item >* >::iterator typed = _items.find( item.type );
        map < int, Item >* typedItems;
        if ( typed != _items.end() ) {
            typedItems = typed -> second;
        } else {
            typedItems = new map < int, Item > ();
            _items[ item.type ] = typedItems;
        }
        map < int, Item >::iterator prev = typedItems -> find( item.id );
        if ( prev != typedItems -> end() ) {
            typedItems -> erase( prev );
        }
        pair < int, Item > p;
        p.first = item.id;
        p.second = item;
        typedItems -> insert( p );
    }

    map < int, map < int, Item >* > _items;

};


