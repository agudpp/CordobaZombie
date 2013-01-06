/*
 * SoundHandler.cpp
 *
 *  This is the highest level class of the whole SoundSystem.
 *
 *  It's in charge of manipulating the SoundManager to implement playlists,
 *  random ambience sounds (e.g. distant howls and screams, crow's cries, etc),
 *  equally spaced repeating sounds (e.g. an ambulance's siren), and so on.
 *
 *	This is a SINGLETON CLASS.
 *  User access to the game's Sound System should be through the SoundHandler.
 *
 *  Created on: Jan 5, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban.
 */

#include "SoundHandler.h"

// SoundHandler's static member initialization.
SoundManager& SoundHandler::sSoundManager = SoundManager::getInstance();

