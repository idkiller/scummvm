/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * MIT License:
 *
 * Copyright (c) 2009 Alexei Svitkine, Eugene Sandulenko
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "common/file.h"

#include "wage/wage.h"
#include "wage/entities.h"
#include "wage/script.h"
#include "wage/sound.h"
#include "wage/world.h"

namespace Wage {

World::World(WageEngine *engine) {
	_storageScene = new Scene;
	_storageScene->_name = STORAGESCENE;

	_orderedScenes.push_back(_storageScene);
	_scenes[STORAGESCENE] = _storageScene;

	_gameOverMessage = nullptr;
	_saveBeforeQuitMessage = nullptr;
	_saveBeforeCloseMessage = nullptr;
	_revertMessage = nullptr;

	_globalScript = nullptr;
	_player = nullptr;

	_weaponMenuDisabled = true;

	_engine = engine;

	_patterns = new Patterns;
}

World::~World() {
	for (uint i = 0; i < _orderedObjs.size(); i++)
		delete _orderedObjs[i];

	for (uint i = 0; i < _orderedChrs.size(); i++)
		delete _orderedChrs[i];

	for (uint i = 0; i < _orderedSounds.size(); i++)
		delete _orderedSounds[i];

	for (uint i = 0; i < _orderedScenes.size(); i++)
		delete _orderedScenes[i];

	for (uint i = 0; i < _patterns->size(); i++)
		free(_patterns->operator[](i));

	delete _patterns;

	delete _globalScript;

	delete _gameOverMessage;
	delete _saveBeforeQuitMessage;
	delete _saveBeforeCloseMessage;
	delete _revertMessage;

}

bool World::loadWorld(Common::MacResManager *resMan) {
	Common::MacResIDArray resArray;
	Common::SeekableReadStream *res;
	Common::MacResIDArray::const_iterator iter;

	// Dumping interpreter code
#if 1
	res = resMan->getResource(MKTAG('C','O','D','E'), 1);
	warning("code size: %d", res->size());
	byte *buf = (byte *)malloc(res->size());
	res->read(buf, res->size());
	Common::DumpFile out;
	out.open("code.bin");
	out.write(buf, res->size());
	out.close();
	free(buf);
	delete res;
#endif

	if ((resArray = resMan->getResIDArray(MKTAG('G','C','O','D'))).size() == 0)
		return false;

	// Load global script
	res = resMan->getResource(MKTAG('G','C','O','D'), resArray[0]);
	_globalScript = new Script(res);

	// TODO: read creator

	// Load main configuration
	if ((resArray = resMan->getResIDArray(MKTAG('V','E','R','S'))).size() == 0)
		return false;

	_name = resMan->getBaseFileName();

	if (resArray.size() > 1)
		warning("Too many VERS resources");

	if (!resArray.empty()) {
		debug(3, "Loading version info");

		res = resMan->getResource(MKTAG('V','E','R','S'), resArray[0]);

		res->skip(10);
		byte b = res->readByte();
		_weaponMenuDisabled = (b != 0);
		if (b != 0 && b != 1)
			error("Unexpected value for weapons menu");

		res->skip(3);
		_aboutMessage = readPascalString(res);

		if (!scumm_stricmp(resMan->getBaseFileName().c_str(), "Scepters"))
			res->skip(1); // ????

		_soundLibrary1 = readPascalString(res);
		_soundLibrary2 = readPascalString(res);

		delete res;
	}

	Common::String *message;
	if ((message = loadStringFromDITL(resMan, 2910, 1)) != NULL) {
		message->trim();
		debug(2, "_gameOverMessage: %s", message->c_str());
		_gameOverMessage = message;
	}
	if ((message = loadStringFromDITL(resMan, 2480, 3)) != NULL) {
		message->trim();
		debug(2, "_saveBeforeQuitMessage: %s", message->c_str());
		_saveBeforeQuitMessage = message;
	}
	if ((message = loadStringFromDITL(resMan, 2490, 3)) != NULL) {
		message->trim();
		debug(2, "_saveBeforeCloseMessage: %s", message->c_str());
		_saveBeforeCloseMessage = message;
	}
	if ((message = loadStringFromDITL(resMan, 2940, 2)) != NULL) {
		message->trim();
		debug(2, "_revertMessage: %s", message->c_str());
		_revertMessage = message;
	}

	// Load scenes
	resArray = resMan->getResIDArray(MKTAG('A','S','C','N'));
	debug(3, "Loading %d scenes", resArray.size());

	for (iter = resArray.begin(); iter != resArray.end(); ++iter) {
		res = resMan->getResource(MKTAG('A','S','C','N'), *iter);
		Scene *scene = new Scene(resMan->getResName(MKTAG('A','S','C','N'), *iter), res);

		res = resMan->getResource(MKTAG('A','C','O','D'), *iter);
		if (res != NULL)
			scene->_script = new Script(res);

		res = resMan->getResource(MKTAG('A','T','X','T'), *iter);
		if (res != NULL) {
			scene->_textBounds = readRect(res);
			scene->_fontType = res->readUint16BE();
			scene->_fontSize = res->readUint16BE();

			Common::String text;
			while (res->pos() < res->size()) {
				char c = res->readByte();
				if (c == 0x0d)
					c = '\n';
				text += c;
			}
			scene->_text = text;

			delete res;
		}
		addScene(scene);
	}

	// Load Objects
	resArray = resMan->getResIDArray(MKTAG('A','O','B','J'));
	debug(3, "Loading %d objects", resArray.size());

	for (iter = resArray.begin(); iter != resArray.end(); ++iter) {
		res = resMan->getResource(MKTAG('A','O','B','J'), *iter);
		addObj(new Obj(resMan->getResName(MKTAG('A','O','B','J'), *iter), res));
	}

	// Load Characters
	resArray = resMan->getResIDArray(MKTAG('A','C','H','R'));
	debug(3, "Loading %d characters", resArray.size());

	for (iter = resArray.begin(); iter != resArray.end(); ++iter) {
		res = resMan->getResource(MKTAG('A','C','H','R'), *iter);
		Chr *chr = new Chr(resMan->getResName(MKTAG('A','C','H','R'), *iter), res);

		addChr(chr);
		// TODO: What if there's more than one player character?
		if (chr->_playerCharacter)
			_player = chr;
	}

	// Load Sounds
	resArray = resMan->getResIDArray(MKTAG('A','S','N','D'));
	debug(3, "Loading %d sounds", resArray.size());

	for (iter = resArray.begin(); iter != resArray.end(); ++iter) {
		res = resMan->getResource(MKTAG('A','S','N','D'), *iter);
		addSound(new Sound(resMan->getResName(MKTAG('A','S','N','D'), *iter), res));
	}

	if (!_soundLibrary1.empty()) {
		loadExternalSounds(_soundLibrary1);
	}
	if (!_soundLibrary2.empty()) {
		loadExternalSounds(_soundLibrary2);
	}

	// Load Patterns
	res = resMan->getResource(MKTAG('P','A','T','#'), 900);
	if (res != NULL) {
		int count = res->readUint16BE();
		debug(3, "Loading %d patterns", count);

		for (int i = 0; i < count; i++) {
			byte *pattern = (byte *)malloc(8);

			res->read(pattern, 8);
			_patterns->push_back(pattern);
		}

		delete res;
	} else {
		/* Enchanted Scepters did not use the PAT# resource for the textures. */
		res = resMan->getResource(MKTAG('C','O','D','E'), 1);
		if (res != NULL) {
			res->skip(0x55ac);
			for (int i = 0; i < 29; i++) {
				byte *pattern = (byte *)malloc(8);

				res->read(pattern, 8);
				_patterns->push_back(pattern);
			}
		}
		delete res;
	}

	res = resMan->getResource(MKTAG('M','E','N','U'), 2001);
	if (res != NULL) {
		Common::StringArray *menu = readMenu(res);
		_aboutMenuItemName.clear();
		Common::String string = menu->operator[](1);

		for (uint i = 0; i < string.size() && string[i] != ';'; i++) // Read token
			_aboutMenuItemName += string[i];

		delete menu;
		delete res;
	}
	res = resMan->getResource(MKTAG('M','E','N','U'), 2004);
	if (res != NULL) {
		Common::StringArray *menu = readMenu(res);
		_commandsMenuName = menu->operator[](0);
		_commandsMenu = menu->operator[](1);
		delete menu;
		delete res;
	}
	res = resMan->getResource(MKTAG('M','E','N','U'), 2005);
	if (res != NULL) {
		Common::StringArray *menu = readMenu(res);
		_weaponsMenuName = menu->operator[](0);
		delete menu;
		delete res;
	}
	// TODO: Read Apple menu and get the name of that menu item..

	// store global info in state object for use with save/load actions
	//world.setCurrentState(initialState);	// pass off the state object to the world

	return true;
}

void World::addSound(Sound *sound) {
	Common::String s = sound->_name;
	s.toLowercase();
	_sounds[s] = sound;
	_orderedSounds.push_back(sound);
}

Common::StringArray *World::readMenu(Common::SeekableReadStream *res) {
	res->skip(10);
	int enableFlags = res->readUint32BE();
	Common::String menuName = readPascalString(res);
	Common::String menuItem = readPascalString(res);
	int menuItemNumber = 1;
	Common::String menu;
	byte itemData[4];

	while (!menuItem.empty()) {
		if (!menu.empty()) {
			menu += ';';
		}
		if ((enableFlags & (1 << menuItemNumber)) == 0) {
			menu += '(';
		}
		menu += menuItem;
		res->read(itemData, 4);
		static const char styles[] = {'B', 'I', 'U', 'O', 'S', 'C', 'E', 0};
		for (int i = 0; styles[i] != 0; i++) {
			if ((itemData[3] & (1 << i)) != 0) {
				menu += '<';
				menu += styles[i];
			}
		}
		if (itemData[1] != 0) {
			menu += '/';
			menu += (char)itemData[1];
		}
		menuItem = readPascalString(res);
		menuItemNumber++;
	}

	Common::StringArray *result = new Common::StringArray;
	result->push_back(menuName);
	result->push_back(menu);

	debug(4, "menuName: %s", menuName.c_str());
	debug(4, "menu: %s", menu.c_str());

	return result;
}

void World::loadExternalSounds(Common::String fname) {
	Common::File in;

	in.open(fname);
	if (!in.isOpen()) {
		warning("Cannot load sound file <%s>", fname.c_str());
		return;
	}
	in.close();

	Common::MacResManager resMan;
	resMan.open(fname);

	Common::MacResIDArray resArray;
	Common::SeekableReadStream *res;
	Common::MacResIDArray::const_iterator iter;

	resArray = resMan.getResIDArray(MKTAG('A','S','N','D'));
	for (iter = resArray.begin(); iter != resArray.end(); ++iter) {
		res = resMan.getResource(MKTAG('A','S','N','D'), *iter);
		addSound(new Sound(resMan.getResName(MKTAG('A','S','N','D'), *iter), res));
	}
}

Common::String *World::loadStringFromDITL(Common::MacResManager *resMan, int resourceId, int itemIndex) {
	Common::SeekableReadStream *res = resMan->getResource(MKTAG('D','I','T','L'), resourceId);
	if (res) {
		int itemCount = res->readSint16BE();
		for (int i = 0; i <= itemCount; i++) {
			// int placeholder; short rect[4]; byte flags; pstring str;
			res->skip(13);
			Common::String message = readPascalString(res);
			if (i == itemIndex) {
				Common::String *msg = new Common::String(message);
				delete res;
				return msg;
			}
		}

		delete res;
	}

	return NULL;
}

static bool invComparator(const Obj *l, const Obj *r) {
	return l->_index < r->_index;
}

void World::move(Obj *obj, Chr *chr) {
	if (obj == NULL)
		return;

	Designed *from = obj->removeFromCharOrScene();
	obj->_currentOwner = chr;
	chr->_inventory.push_back(obj);

	Common::sort(chr->_inventory.begin(), chr->_inventory.end(), invComparator);

	_engine->onMove(obj, from, chr);
}

static bool objComparator(const Obj *o1, const Obj *o2) {
	bool o1Immobile = (o1->_type == Obj::IMMOBILE_OBJECT);
	bool o2Immobile = (o2->_type == Obj::IMMOBILE_OBJECT);
	if (o1Immobile == o2Immobile) {
		return o1->_index < o2->_index;
	}
	return o1Immobile;
}

void World::move(Obj *obj, Scene *scene, bool skipSort) {
	if (obj == NULL)
		return;

	Designed *from = obj->removeFromCharOrScene();
	obj->_currentScene = scene;
	scene->_objs.push_back(obj);

	if (!skipSort)
		Common::sort(scene->_objs.begin(), scene->_objs.end(), objComparator);

	_engine->onMove(obj, from, scene);
}

static bool chrComparator(const Chr *l, const Chr *r) {
	return l->_index < r->_index;
}

void World::move(Chr *chr, Scene *scene, bool skipSort) {
	if (chr == NULL)
		return;
	Scene *from = chr->_currentScene;
	if (from == scene)
		return;
	if (from != NULL)
		from->_chrs.remove(chr);
	scene->_chrs.push_back(chr);

	if (!skipSort)
		Common::sort(scene->_chrs.begin(), scene->_chrs.end(), chrComparator);

	if (scene == _storageScene) {
		chr->resetState();
	} else if (chr->_playerCharacter) {
		scene->_visited = true;
		_player->_context._visits++;
	}
	chr->_currentScene = scene;

	_engine->onMove(chr, from, scene);
}

Scene *World::getRandomScene() {
	// Not including storage:
	return _orderedScenes[1 + _engine->_rnd->getRandomNumber(_orderedScenes.size() - 2)];
}

Scene *World::getSceneAt(int x, int y) {
	for (uint i = 0; i < _orderedScenes.size(); i++) {
		Scene *scene = _orderedScenes[i];

		if (scene != _storageScene && scene->_worldX == x && scene->_worldY == y) {
			return scene;
		}
	}
	return NULL;
}

static const int directionsX[] = { 0, 0, 1, -1 };
static const int directionsY[] = { -1, 1, 0, 0 };

bool World::scenesAreConnected(Scene *scene1, Scene *scene2) {
	if (!scene1 || !scene2)
		return false;

	int x = scene2->_worldX;
	int y = scene2->_worldY;

	for (int dir = 0; dir < 4; dir++)
		if (!scene2->_blocked[dir])
			if (getSceneAt(x + directionsX[dir], y + directionsY[dir]) == scene1)
				return true;

	return false;
}

const char *World::getAboutMenuItemName() {
	static char menu[256];

	*menu = '\0';

	if (_aboutMenuItemName.empty()) {
		sprintf(menu, "About %s...", _name.c_str());
	} else { // Replace '@' with name
		const char *str = _aboutMenuItemName.c_str();
		const char *pos = strchr(str, '@');
		if (pos) {
			strncat(menu, str, (pos - str));
			strncat(menu, _name.c_str(), 255);
			strncat(menu, pos + 1, 255);
		}
	}

	return menu;
}

} // End of namespace Wage
