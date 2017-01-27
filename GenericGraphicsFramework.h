#ifndef GENERIC_GRAPHICS_FRAMEWORK_H
#define GENERIC_GRAPHICS_FRAMEWORK_H

/*
 * Generic Graphics Framework 1.0
 *
 * GenericGraphicsFrameworkInit() should be called before any other method
 *
 * Windows are the root of any view (see GenericGraphicsFrameworkCreateWindow)
 * All the rest of the controls should be descendants of a window
 * Possible controls are: Panels, Labels, and Buttons
 * All can have an image to display
 * Labels don't have children, whereas panels do
 * Only buttons are click-able (see GenericGraphicsFrameworkBlockingPollForEvents)
 */

#include "CommonUtils.h"

#define PERSISTENT_NOTIFICATION_ETERNAL_DELAY	0

typedef enum
{ 
	NO_TRANSPARENCY = false, 
	TRANSPARENT_WHITE = true
} GGF_TRANSPARENCY;

struct _CONTROL
{
	unsigned int		width;
	unsigned int		height;
	struct _CONTROL*	pHeadOfChildrenList;
	struct _CONTROL*	pTailOfChildrenList;
	const char*			imageFilename;
	BOOL				(*OnPressCallback)(struct _CONTROL*);

	struct _CONTROL*	pParent;
	struct _CONTROL*	pRightSibling;
	int					xPosition;			// relative to parent
	int					yPosition;			// relative to parent
	BOOL				transparent;
	BOOL				buttonEnabled;
	BOOL				buttonSelected;
	const char*			selectedImageFilename;

	void*				implementationSpecificFields;
};
typedef struct _CONTROL CONTROL;

/* Button OnPress Callback receives a reference to the button pressed 
 * Should return whether the polling should continue or not */
typedef BOOL (*ON_PRESS_CALLBACK)(CONTROL*);

typedef struct 
{
	unsigned int	 	width;
	unsigned int		height;
	const char*		   	imageFilename;
	struct _CONTROL*  	headOfChildrenList;
	ON_PRESS_CALLBACK   OnPressCallback;
} WINDOW;

/* Note: It is mandatory to call this function before any other */
BOOL GenericGraphicsFrameworkInit();

void GenericGraphicsFrameworkTerminate();

/* The user is the one to hold the Control Structure */
/* And merely passes a pointer to it */
/* Therefore, it is the user who should be responsible for allocation & release */

// Returns success status
BOOL GenericGraphicsFrameworkCreateWindow(
	CONTROL * windowPtr,
	unsigned int windowWidth, 
	unsigned int windowHeight,
	const char* backgroundImageFilename,
	ON_PRESS_CALLBACK OnPressCallback
	);

void GenericGraphicsFrameworkCreatePanel(
	CONTROL * controlPtr,
	CONTROL * pParent,
	int xPosRelativeToParent, 
	int yPosRelativeToParent, 
	unsigned int controlWidth, 
	unsigned int controlHeight,
	const char* imageFilename
	);

void GenericGraphicsFrameworkCreateLabel(
	CONTROL * controlPtr,
	CONTROL * pParent,
	int xPosRelativeToParent, 
	int yPosRelativeToParent, 
	unsigned int controlWidth, 
	unsigned int controlHeight,
	const char* imageFilename,
	BOOL transparent
	);

// Buttons are created enabled at first
void GenericGraphicsFrameworkCreateButton(
	CONTROL * controlPtr,
	CONTROL * pParent,
	int xPosRelativeToParent, 
	int yPosRelativeToParent, 
	unsigned int width, 
	unsigned int height,
	const char* imageFilename,
	const char* selectedImageFileName,
	BOOL transparent
	);

void GenericGraphicsFrameworkEnableButton(CONTROL * pButton);
void GenericGraphicsFrameworkDisableButton(CONTROL * pButton);
#define GenericGraphicsFrameworkSelectButton(button)	{	(button).buttonSelected = true;	}
#define GenericGraphicsFrameworkUnselectButton(button)	{	(button).buttonSelected = false;	}

// Scanning the UI tree with DFS and drawing each control in that order
// Controls that are drawn later are the ones that are drawn on top 
// of controls that were drawn previously
// Overlapping controls are drawn according to their position in the tree – 
// right children are drawn over left children
// Controls inside a panel are drawn relative to the panel
// Controls inside a panel must reside within its borders.
// If a control exceeds its parents borders, it will only be drawn partially
void GenericGraphicsFrameworkDrawTree(CONTROL * pRoot);

// will show the notification over the parent for delayMilliseconds
// if delayMilliseconds is PERSISTENT_NOTIFICATION_ETERNAL_DELAY:
// then the notification is displayed infinitely
void GenericGraphicsFrameworkShowNotification(
	CONTROL * controlPtr,
	CONTROL * pParent,
	int xPosRelativeToParent, 
	int yPosRelativeToParent, 
	unsigned int controlWidth, 
	unsigned int controlHeight,
	const char* imageFilename,
	BOOL transparent,
	int delayMilliseconds
	);

// Note: Will be blocking for input, calling the window's ON_PRESS_CALLBACK
// until OnPressCallback will return false, for discontinuing of blocking
void GenericGraphicsFrameworkBlockingPollForEvents(CONTROL * pRoot);

#endif // GENERIC_GRAPHICS_FRAMEWORK_H