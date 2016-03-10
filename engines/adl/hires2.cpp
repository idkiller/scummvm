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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "common/system.h"
#include "common/debug.h"
#include "common/error.h"
#include "common/file.h"
#include "common/stream.h"

#include "adl/hires2.h"
#include "adl/display.h"

namespace Adl {

void HiRes2Engine::runIntro() const {
	Common::File f;

	if (!f.open(IDS_HR2_DISK_IMAGE))
		error("Failed to open file '" IDS_HR2_DISK_IMAGE "'");

	f.seek(IDI_HR2_OFS_INTRO_TEXT);

	_display->setMode(DISPLAY_MODE_TEXT);

	Common::String str = readStringAt(f, IDI_HR2_OFS_INTRO_TEXT);

	if (f.eos() || f.err())
		error("Error reading disk image");

	_display->printString(str);
	delay(2000);
}

void HiRes2Engine::loadData() {
	Common::File f;

	if (!f.open(IDS_HR2_DISK_IMAGE))
		error("Failed to open file '" IDS_HR2_DISK_IMAGE "'");

	f.seek(IDI_HR2_OFS_VERBS);
	loadWords(f, _verbs);

	f.seek(IDI_HR2_OFS_NOUNS);
	loadWords(f, _nouns);
}

void HiRes2Engine::initState() {
	Common::File f;

	if (!f.open(IDS_HR2_DISK_IMAGE))
		error("Failed to open file '" IDS_HR2_DISK_IMAGE "'");

	_state.rooms.clear();
	f.seek(IDI_HR2_OFS_ROOMS);
	for (uint i = 0; i < IDI_HR2_NUM_ROOMS; ++i) {
		Room room = { };
		f.readByte(); // number
		for (uint j = 0; j < 6; ++j)
			room.connections[j] = f.readByte();
		room.track = f.readByte();
		room.sector = f.readByte();
		room.offset = f.readByte();
		f.readByte(); // always 1, possibly disk?
		room.picture = f.readByte();
		room.curPicture = f.readByte();
		f.readByte(); // always 1, possibly disk?
		_state.rooms.push_back(room);
	}
}

void HiRes2Engine::restartGame() {
	initState();
}

void HiRes2Engine::drawPic(byte pic, Common::Point pos) const {
}

void HiRes2Engine::showRoom() const {
}

Engine *HiRes2Engine_create(OSystem *syst, const AdlGameDescription *gd) {
	return new HiRes2Engine(syst, gd);
}

} // End of namespace Adl