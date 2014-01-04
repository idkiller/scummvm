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
 */

#include "fullpipe/fullpipe.h"

#include "fullpipe/objectnames.h"
#include "fullpipe/constants.h"

#include "fullpipe/gameloader.h"
#include "fullpipe/motion.h"
#include "fullpipe/scenes.h"
#include "fullpipe/statics.h"

#include "fullpipe/interaction.h"
#include "fullpipe/behavior.h"
#include "fullpipe/floaters.h"

namespace Fullpipe {

bool sceneHandler23_testCalendar() {
	warning("STUB: sceneHandler23_testCalendar()");

	return false;
}

void scene23_initScene(Scene *sc) {
	g_vars->scene23_var01 = 200;
	g_vars->scene23_var02 = 200;
	g_vars->scene23_var03 = 300;
	g_vars->scene23_var04 = 300;
	g_vars->scene23_calend0 = sc->getStaticANIObject1ById(ANI_CALENDWHEEL, 0);
	g_vars->scene23_calend1 = sc->getStaticANIObject1ById(ANI_CALENDWHEEL, 1);
	g_vars->scene23_calend2 = sc->getStaticANIObject1ById(ANI_CALENDWHEEL, 2);
	g_vars->scene23_calend3 = sc->getStaticANIObject1ById(ANI_CALENDWHEEL, 3);
	g_vars->scene23_var05 = 0;
	g_vars->scene23_var06 = 0;
	g_vars->scene23_var07 = 0;
	g_vars->scene23_giraffeTop = sc->getStaticANIObject1ById(ANI_GIRAFFE_TOP, -1);
	g_vars->scene23_giraffee = sc->getStaticANIObject1ById(ANI_GIRAFFEE, -1);

	g_fp->_floaters->init(g_fp->getGameLoaderGameVar()->getSubVarByName("SC_23"));

	Scene *oldsc = g_fp->_currentScene;
	g_fp->_currentScene = sc;

	if (g_fp->getObjectState(sO_UpperHatch_23) == g_fp->getObjectEnumState(sO_UpperHatch_23, sO_Opened)) {
		sc->getPictureObjectById(PIC_SC23_BOXOPEN, 0)->_flags |= 4;
		sc->getPictureObjectById(PIC_SC23_BOXCLOSED, 0)->_flags &= 0xFFFB;
		sc->getPictureObjectById(PIC_SC23_BTN1, 0)->_flags |= 4;
		sc->getPictureObjectById(PIC_SC23_BTN2, 0)->_flags |= 4;
		sc->getPictureObjectById(PIC_SC23_BTN3, 0)->_flags |= 4;
		sc->getPictureObjectById(PIC_SC23_BTN4, 0)->_flags |= 4;

		if (g_vars->scene23_giraffee->_statics->_staticsId == ST_GRFG_EMPTY || !(g_vars->scene23_giraffee->_flags & 4)) {
			g_vars->scene23_giraffee->changeStatics2(ST_GRFG_BALD);
			g_vars->scene23_giraffee->_flags |= 4;
		}
		g_vars->scene23_calend0->show1(-1, -1, -1, 0);
		g_vars->scene23_calend1->show1(-1, -1, -1, 0);
		g_vars->scene23_calend2->show1(-1, -1, -1, 0);
		g_vars->scene23_calend3->show1(-1, -1, -1, 0);

		sc->getStaticANIObject1ById(ANI_LUK23_U, -1)->changeStatics2(ST_LUK23U_OPEN);
	} else {
		sc->getPictureObjectById(PIC_SC23_BOXOPEN, 0)->_flags &= 0xFFFB;
		sc->getPictureObjectById(PIC_SC23_BOXCLOSED, 0)->_flags |= 4;
		sc->getPictureObjectById(PIC_SC23_BTN1, 0)->_flags &= 0xFFFB;
		sc->getPictureObjectById(PIC_SC23_BTN2, 0)->_flags &= 0xFFFB;
		sc->getPictureObjectById(PIC_SC23_BTN3, 0)->_flags &= 0xFFFB;
		sc->getPictureObjectById(PIC_SC23_BTN4, 0)->_flags &= 0xFFFB;

		g_vars->scene23_giraffee->hide();
		g_vars->scene23_calend0->hide();
		g_vars->scene23_calend1->hide();
		g_vars->scene23_calend2->hide();
		g_vars->scene23_calend3->hide();

		sc->getStaticANIObject1ById(ANI_LUK23_U, -1)->changeStatics2(ST_LUK23U_CLOSED);

		g_fp->_floaters->genFlies(sc, 600, 90, 0, 0);
	}

	if (g_fp->getObjectState(sO_LowerHatch_23) == g_fp->getObjectEnumState(sO_LowerHatch_23, sO_Opened)) {
		g_vars->scene23_giraffeTop->show1(-1, -1, -1, 0);
		g_vars->scene23_giraffeTop->changeStatics2(ST_GRFU_UP);

		if (g_fp->getObjectState(sO_LowerPipe_21) == g_fp->getObjectEnumState(sO_LowerPipe_21, sO_IsOpened)) {
			g_vars->scene23_giraffeTop->changeStatics2(ST_GRFU_KISS);
			g_vars->scene23_giraffee->hide();
		} else {
			if (g_fp->getObjectState(sO_UpperHatch_23) == g_fp->getObjectEnumState(sO_UpperHatch_23, sO_Opened)
				&& (g_vars->scene23_giraffee->_flags & 4))
				g_vars->scene23_giraffeTop->setOXY(614, 362);
			else
				g_vars->scene23_giraffeTop->setOXY(618, 350);

			if (sceneHandler23_testCalendar())
				g_vars->scene23_calend1->_statics = g_vars->scene23_calend1->getStaticsById(ST_CND_5);
		}

		sc->getStaticANIObject1ById(ANI_LUK23_D, -1)->changeStatics2(ST_LUK23_OPEN);

		if (g_fp->getObjectState(sO_Lever_23) == g_fp->getObjectEnumState(sO_Lever_23, sO_Taken))
			sc->getStaticANIObject1ById(ANI_INV_LEVERHANDLE, -1)->hide();

		sc->getStaticANIObject1ById(ANI_HANDLE23, -1)->hide();
	} else {
		g_vars->scene23_giraffeTop->hide();

		sc->getStaticANIObject1ById(ANI_LUK23_D, -1)->changeStatics2(ST_LUK23_WHANDLE2);

		sc->getStaticANIObject1ById(ANI_INV_LEVERHANDLE, -1)->hide();
	}
	
	g_fp->_currentScene = oldsc;
}

void scene23_setGiraffeState() {
	if (g_fp->getObjectState(sO_UpperHatch_23) == g_fp->getObjectEnumState(sO_UpperHatch_23, sO_Opened)) {
		g_fp->_behaviorManager->setBehaviorEnabled(g_vars->scene23_giraffeTop, ST_GRFU_UP, QU_GRFU_TURN_UL, 0);
		g_fp->_behaviorManager->setBehaviorEnabled(g_vars->scene23_giraffeTop, ST_GRFU_UP, QU_GRFU_TURN_UD, 0);
	}
}

int scene23_updateCursor() {
	g_fp->updateCursorCommon();

	if (g_fp->_objectIdAtCursor == PIC_SC23_LADDERU) {
		if (g_vars->scene23_var05)
			return g_fp->_cursorId;

		g_fp->_cursorId = getGameLoaderInventory()->getSelectedItemId() ? PIC_CSR_GOU : PIC_CSR_ITN; // FIXME check
	}

	if (g_fp->_objectIdAtCursor == PIC_SC23_BTN1 || g_fp->_objectIdAtCursor == PIC_SC23_BTN2
		|| g_fp->_objectIdAtCursor == PIC_SC23_BTN3 || g_fp->_objectIdAtCursor == PIC_SC23_BTN4
		|| g_fp->_objectIdAtCursor == ANI_CALENDWHEEL)
		g_fp->_cursorId = PIC_CSR_LIFT;

	return g_fp->_cursorId;
}

void sceneHandler23_showStool() {
	warning("STUB: sceneHandler23_showStool()");
}

void sceneHandler23_hideStool() {
	g_fp->_currentScene->getStaticANIObject1ById(ANI_TABURETTE, -1)->hide();
}

void sceneHandler23_spinWheel1() {
	warning("STUB: sceneHandler23_spinWheel1()");
}

void sceneHandler23_spinWheel2() {
	warning("STUB: sceneHandler23_spinWheel2()");
}

void sceneHandler23_spinWheel3() {
	warning("STUB: sceneHandler23_spinWheel3()");
}

void sceneHandler23_spinWheel4() {
	warning("STUB: sceneHandler23_spinWheel4()");
}

void sceneHandler23_pushButton(ExCommand *cmd) {
	warning("STUB: sceneHandler23_pushButton(cmd)");
}

void sceneHandler23_sendClick(StaticANIObject *ani) {
	warning("STUB: sceneHandler23_sendClick(ani)");
}

void sceneHandler23_checkReachingTop() {
	warning("STUB: sceneHandler23_checkReachingTop()");
}

void sceneHandler23_exitCalendar() {
	warning("STUB: sceneHandler23_exitCalendar()");
}

void sceneHandler23_lowerFromCalendar(ExCommand *cmd) {
	warning("STUB: sceneHandler23_lowerFromCalendar(cmd)");
}

void sceneHandler23_fromStool(ExCommand *cmd) {
	warning("STUB: sceneHandler23_fromStool(cmd)");
}

int sceneHandler23(ExCommand *cmd) {
	if (cmd->_messageKind != 17)
		return 0;

	switch (cmd->_messageNum) {
	case MSG_SC23_FROMSTOOL:
		g_vars->scene23_var06 = 0;

		getCurrSceneSc2MotionController()->setEnabled();
		getGameLoaderInteractionController()->enableFlag24();

		g_fp->_behaviorManager->setFlagByStaticAniObject(g_fp->_aniMan, 1);
		break;

	case MSG_SC23_HIDEGIRAFFEE:
		g_vars->scene23_giraffee->queueMessageQueue(0);
		g_vars->scene23_giraffee->_flags &= 0xFFFB;
		break;

	case MSG_SC23_ONSTOOL:
		g_vars->scene23_var06 = 1;

		getCurrSceneSc2MotionController()->clearEnabled();
		getGameLoaderInteractionController()->disableFlag24();

		g_fp->_behaviorManager->setFlagByStaticAniObject(g_fp->_aniMan, 0);
		break;

	case MSG_SC22_SHOWSTOOL:
		sceneHandler23_showStool();
		break;

	case MSG_SC22_HIDESTOOL:
		sceneHandler23_hideStool();
		break;

	case MSG_SC23_SPINWHEEL1:
		sceneHandler23_spinWheel1();
		break;

	case MSG_SC23_SPINWHEEL2:
		sceneHandler23_spinWheel2();
		break;

	case MSG_SC23_SPINWHEEL3:
		sceneHandler23_spinWheel3();
		break;

	case MSG_SC23_SPINWHEEL4:
		sceneHandler23_spinWheel4();
		break;

	case MSG_SC23_CLICKBTN1:
	case MSG_SC23_CLICKBTN2:
	case MSG_SC23_CLICKBTN3:
	case MSG_SC23_CLICKBTN4:
		sceneHandler23_pushButton(cmd);
		break;

	case 33:
		if (g_fp->_aniMan2) {
			int x = g_fp->_aniMan2->_ox;

			if (x < g_fp->_sceneRect.left + g_vars->scene23_var01)
				g_fp->_currentScene->_x = x - g_vars->scene23_var03 - g_fp->_sceneRect.left;

			if (x > g_fp->_sceneRect.right - g_vars->scene23_var01)
				g_fp->_currentScene->_x = x + g_vars->scene23_var03 - g_fp->_sceneRect.right;
		}

		g_fp->_floaters->update();
		g_fp->_behaviorManager->updateBehaviors();

		g_fp->startSceneTrack();

		break;

	case 29:
		{
			StaticANIObject *ani = g_fp->_currentScene->getStaticANIObjectAtPos(cmd->_sceneClickX, cmd->_sceneClickY);
			int picId;

			if (ani && ani->_id == ANI_CALENDWHEEL) {
				sceneHandler23_sendClick(ani);
				cmd->_messageKind = 0;
			}

			sceneHandler23_checkReachingTop();

			if (g_vars->scene23_var05) {
				picId = g_fp->_currentScene->getPictureObjectIdAtPos(cmd->_sceneClickX, cmd->_sceneClickY);

				if (picId == PIC_SC23_LADDER) {
					sceneHandler23_exitCalendar();

					cmd->_messageKind = 0;
					break;
				}

				if (cmd->_sceneClickY > 450) {
					sceneHandler23_lowerFromCalendar(cmd);

					cmd->_messageKind = 0;
					break;
				}
				break;
			}

			if (!g_vars->scene23_var06) {
				picId = g_fp->_currentScene->getPictureObjectIdAtPos(cmd->_sceneClickX, cmd->_sceneClickY);

				if (picId == PIC_SC23_LADDERU && !g_vars->scene23_var05) {
					sceneHandler23_pushButton(cmd);

					cmd->_messageKind = 0;
					break;
				}
				break;
			}

			if (ani && ani->_id == ANI_HANDLE23) {
				handleObjectInteraction(g_fp->_aniMan, ani, cmd->_keyCode);
				cmd->_messageKind = 0;
			} else {
				sceneHandler23_fromStool(cmd);

				cmd->_messageKind = 0;
			}

			break;
		}
	}

	return 0;
}

} // End of namespace Fullpipe