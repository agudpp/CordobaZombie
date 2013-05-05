/*
 * Backpack.h
 *
 *  Created on: 14/05/2012
 *      Author: agustin
 */

#ifndef BACKPACK_H_
#define BACKPACK_H_

#include <vector>

#include <boost/signal.hpp>
#include <boost/signals/slot.hpp>

#include "tinyxml.h"
#include "BackpackDefines.h"
#include "BackpackItem.h"


class Backpack
{
public:

    // define the differents types of events
    //
    enum Event {
        ITEM_ADDED = 0,
        ITEM_REMOVED,
    };

    // Define the connection and signals type
    //
    typedef boost::signal<void (Backpack*, const BackpackItemPtr&, Event)> Signal;
    typedef boost::signals::scoped_connection Connection;

public:
    Backpack(){};
    ~Backpack(){};

    /**
     * @brief Add a BackpackItem. We have to ensure that the item is not
     *        already in the backpack
     * @param bi	The backpack item to add
     */
    void
    addBackpackItem(BackpackItemPtr& bi);

    /**
     * @brief Remove a backPackItem from a item or the user define element
     * @note That removeBackpackItemUserDef is slow
     */
    void
    removeBackpackItem(const BackpackItemPtr& bi);
    void
    removeBackpackItemUserDef(void *bi);

    /**
     * @brief Check if the backpack have some item
     */
    bool
    hasBackpackItem(const BackpackItemPtr& bi) const;

    /**
     * @brief Check if we have the backPack item associated with a userDef type
     * @note That this method is slower than the above one
     */
    BackpackItemPtr
    hasBackpackItemUserDef(void *ud) const;

    /**
     * @brief Especial methods for checking different kind of elements
     */
    inline bool
    hasWeapon(void* weapon) const;
    inline bool
    hasBomb(void* bomb) const;
    inline bool
    hasItem(void* item) const;

    /**
     * @brief Especial remove methods for different user defined types
     */
    inline void
    removeWeapon(void* weapon);
    inline void
    removeBomb(void* bomb);
    inline void
    removeItem(void* item);

    /**
     * @brief Especial methods to get the associated BackpackItemPtr from a
     *        given user defined object
     */
    inline BackpackItemPtr
    getWeapon(void* ud);
    inline BackpackItemPtr
    getBomb(void* ud);
    inline BackpackItemPtr
    getItem(void* ud);

    /**
     * @brief Add the callback to get events everytime this backpack changes
     * @param subscriber    The one who want to receive the events
     * @returns conn        The associated connection
     */
    Connection
    addCallback(const Signal::slot_type& subscriber);

private:

    // Avoid copy constructor and copying assigning this class
    //
    Backpack(const Backpack&);
    Backpack& operator=(const Backpack&);

    /**
     * @brief Check for the existence of a particular element searching in a vec
     *        and a user define value
     */
    inline bool
    existsUserDefine(const std::vector<BackpackItemPtr>& vec,
                     void* ud) const;

    /**
     * @brief Return the index of the element for a given vector and the user
     *        defined object
     */
    inline bool
    getIndex(const std::vector<BackpackItemPtr>& vec, void* ud, size_t& index) const;

    /**
     * @brief Remove an element from the user define object and a given vector
     * @param vec   The vector from where we want to remove the element
     * @param ud    The user define value we want to match for
     */
    inline void
    removeElementFromUserDef(std::vector<BackpackItemPtr>& vec,
                             void* ud);

private:
    typedef std::vector<BackpackItemPtr> ItemVec;


    ItemVec mItems[BackpackDef::ItemType::COUNT];
    Signal mSignal;
};




// Inline impl
//

inline bool
Backpack::existsUserDefine(const std::vector<BackpackItemPtr>& vec,
                           void* ud) const
{
    for (size_t i = 0, size = vec.size(); i < size; ++i) {
        if (vec[i]->userDef == ud) {
            return true;
        }
    }
    return false;
}

inline bool
Backpack::getIndex(const std::vector<BackpackItemPtr>& vec, void* ud, size_t& index) const
{
    for (size_t i = 0, size = vec.size(); i < size; ++i) {
        if (vec[i]->userDef == ud) {
            index = i;
            return true;
        }
    }
    return false;
}

inline bool
Backpack::hasWeapon(void *weapon) const
{
    return existsUserDefine(mItems[BackpackDef::ItemType::WEAPON], weapon);
}
inline bool
Backpack::hasBomb(void *bomb) const
{
    return existsUserDefine(mItems[BackpackDef::ItemType::BOMB], bomb);
}
inline bool
Backpack::hasItem(void* item) const
{
    return existsUserDefine(mItems[BackpackDef::ItemType::ITEM], item);
}

inline void
Backpack::removeElementFromUserDef(std::vector<BackpackItemPtr>& vec,
                                   void* ud)
{
    for (size_t i = 0, size = vec.size(); i < size; ++i) {
        if (vec[i]->userDef == ud) {
            vec[i] = vec.back();
            vec.pop_back();
            return;
        }
    }
}

void
Backpack::removeWeapon(void* weapon)
{
    ASSERT(hasWeapon(weapon));
    removeElementFromUserDef(mItems[BackpackDef::ItemType::WEAPON], weapon);
}
void
Backpack::removeBomb(void* bomb)
{
    ASSERT(hasBomb(bomb));
    removeElementFromUserDef(mItems[BackpackDef::ItemType::BOMB], bomb);
}
void
Backpack::removeItem(void* item)
{
    ASSERT(hasItem(item));
    removeElementFromUserDef(mItems[BackpackDef::ItemType::ITEM], item);
}

inline BackpackItemPtr
Backpack::getWeapon(void* ud)
{
    size_t index;
    if (!getIndex(mItems[BackpackDef::ItemType::WEAPON], ud, index)) {
        return BackpackItemPtr();
    }
    return (mItems[BackpackDef::ItemType::WEAPON])[index];
}
inline BackpackItemPtr
Backpack::getBomb(void* ud)
{
    size_t index;
    if (!getIndex(mItems[BackpackDef::ItemType::BOMB], ud, index)) {
        return BackpackItemPtr();
    }
    return (mItems[BackpackDef::ItemType::BOMB])[index];
}
inline BackpackItemPtr
Backpack::getItem(void* ud)
{
    size_t index;
    if (!getIndex(mItems[BackpackDef::ItemType::ITEM], ud, index)) {
        return BackpackItemPtr();
    }
    return (mItems[BackpackDef::ItemType::ITEM])[index];
}

#endif /* BACKPACK_H_ */
