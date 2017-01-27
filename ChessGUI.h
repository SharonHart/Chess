#ifndef CHESS_GUI_H
#define CHESS_GUI_H

#include "ChessGenericUIInterface.h"
#include "GenericGraphicsFramework.h"
#include "ChessGUILayouts.h"
#include "ChessGUIResources.h"

#define GUI_NOTIFICATIONS_DELAY		(3000)	// ms

/* Public API */

BOOL ChessGUIInitialize(CHESS_UI*);

#endif