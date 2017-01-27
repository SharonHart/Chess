// un-comment to enable
//#define DEBUG_GRAPHICS

#ifdef __linux__ 
#include <SDL.h>
#include <SDL_video.h>
#endif // LINUX

#ifdef _WIN32 
#include "unit_tests/SDL_Windows_Mock.h"
#endif // _WIN32

#include "GenericGraphicsFramework.h"
#include "CommonUtils.h"

#define SDL_SUCCESS	0
#define WINDOW_BITS_PER_PIXEL	0	// default

/* LOCAL DATA */
static SDL_Event m_event;	// most recent key event
static BOOL m_initialized = false;

/********************************/
/* PRIVATE METHODS DECLARATIONS */
/********************************/
/* The boolean's semantic is:
* Whether DFS should stop its search after operating on the current node */
typedef BOOL (*DFS_OPERATOR_FUNC_P)(CONTROL * pControl, CONTROL * pRoot);
static BOOL GenericGraphicsFrameworkDrawDFSHelper(CONTROL * pControl, CONTROL * pRoot, DFS_OPERATOR_FUNC_P operator);
/* DFS Node Operators */
static BOOL GenericGraphicsFrameworkDrawControl(CONTROL * pControl, CONTROL * pRoot);

static CONTROL * GenericGraphicsFrameworkButtonPressDFSHelper(CONTROL * pControl, CONTROL * pRoot);
static BOOL GenericGraphicsFrameworkWasButtonPressed(CONTROL * pControl, CONTROL * pRoot);

static void GenericGraphicsFrameworkCreateControl(
	CONTROL * controlPtr,
	CONTROL * pParent,
	BOOL childWillBeAddedAsHeadOfParentsChildren,
	int xPosRelativeToParent, 
	int yPosRelativeToParent, 
	unsigned int controlWidth, 
	unsigned int controlHeight,
	const char* imageFilename
	);

/**************************************/	
/* PUBLIC API METHODS IMPLEMENTATIONS */
/**************************************/
BOOL GenericGraphicsFrameworkInit()
{
	/* Initialize SDL and make sure it quits*/
	if (SDL_Init(SDL_INIT_VIDEO) < SDL_SUCCESS) {
		PRINT_ERROR("ERROR: unable to init SDL: %s", SDL_GetError());
		return false;
	}
	m_initialized = true;
	return true;

	atexit(SDL_Quit);
}

void GenericGraphicsFrameworkTerminate()
{
	SDL_Quit();
}

BOOL GenericGraphicsFrameworkCreateWindow(
	CONTROL * windowPtr,
	unsigned int windowWidth, 
	unsigned int windowHeight,
	const char* backgroundImageFilename,
	ON_PRESS_CALLBACK OnPressCallback
	)
{
	// The surface returned is freed by SDL_Quit() and should not be freed by the caller.
	SDL_Surface *pSdlSurface  = SDL_SetVideoMode(windowWidth, windowHeight, WINDOW_BITS_PER_PIXEL, SDL_HWSURFACE | SDL_DOUBLEBUF);
	windowPtr->pParent = NULL;
	windowPtr->pHeadOfChildrenList = NULL;
	windowPtr->pTailOfChildrenList = NULL;
	windowPtr->pRightSibling = NULL;
	windowPtr->imageFilename = backgroundImageFilename;
	windowPtr->selectedImageFilename = NULL;
	windowPtr->buttonSelected = false;
	windowPtr->OnPressCallback = OnPressCallback;
	windowPtr->xPosition = 0;
	windowPtr->yPosition = 0;
	windowPtr->width = windowWidth;
	windowPtr->height = windowHeight;
	windowPtr->implementationSpecificFields = (void*)pSdlSurface;

	if (NULL == pSdlSurface) {
		PRINT_ERROR("failed to set video mode: %s", SDL_GetError());
		return false;
	}
	return true;
}

void GenericGraphicsFrameworkCreatePanel(
	CONTROL * controlPtr,
	CONTROL * pParent,
	int xPosRelativeToParent, 
	int yPosRelativeToParent, 
	unsigned int controlWidth, 
	unsigned int controlHeight,
	const char* imageFilename
	)
{

	GenericGraphicsFrameworkCreateControl(
		controlPtr,
		pParent,
		true,
		xPosRelativeToParent, 
		yPosRelativeToParent, 
		controlWidth, 
		controlHeight,
		imageFilename
		);

}

void GenericGraphicsFrameworkCreateLabel(
	CONTROL * controlPtr,
	CONTROL * pParent,
	int xPosRelativeToParent, 
	int yPosRelativeToParent, 
	unsigned int controlWidth, 
	unsigned int controlHeight,
	const char* imageFilename,
	BOOL transparent
	)
{
	GenericGraphicsFrameworkCreateControl(
		controlPtr,
		pParent,
		true,
		xPosRelativeToParent, 
		yPosRelativeToParent, 
		controlWidth, 
		controlHeight,
		imageFilename
		);
	controlPtr->transparent = transparent;									
}

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
	//BUTTON_CALLBACK buttonPressCallback
	)
{
	GenericGraphicsFrameworkCreateControl(
		controlPtr,
		pParent,
		true,
		xPosRelativeToParent, 
		yPosRelativeToParent, 
		width, 
		height,
		imageFilename
		);
	//controlPtr->OnPressCallback = buttonPressCallback;
	controlPtr->transparent = transparent;
	controlPtr->buttonEnabled = true;
	controlPtr->selectedImageFilename = selectedImageFileName;
}

void GenericGraphicsFrameworkEnableButton(CONTROL * pButton)
{
	pButton->buttonEnabled = true;
}

void GenericGraphicsFrameworkDisableButton(CONTROL * pButton)
{
	pButton->buttonEnabled = false;
}

// TODO:
// // If a control exceeds its parents borders, it will only be drawn partially
void GenericGraphicsFrameworkDrawTree(CONTROL * pRoot)
{
	DEBUG_PRINT("%p", (void*)pRoot);
	assert(pRoot);
	assert(pRoot->pHeadOfChildrenList);
	assert(pRoot->pTailOfChildrenList);
	assert(pRoot->implementationSpecificFields);
	GenericGraphicsFrameworkDrawDFSHelper(pRoot, pRoot, GenericGraphicsFrameworkDrawControl);
}

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
	)
{
	CONTROL* pPreviousLastChild = NULL;

	FUNCTION_DEBUG_TRACE;
	assert(controlPtr);

	if (NULL != pParent)
	{
		// save previous tail
		pPreviousLastChild = pParent->pTailOfChildrenList;
	}

	// CreateLabel under parent
	GenericGraphicsFrameworkCreateControl(
		controlPtr,
		pParent,
		// But we have to make sure that it's added at the end of the children list
		// so that it would be drawn on top		
		false,
		xPosRelativeToParent, 
		yPosRelativeToParent, 
		controlWidth, 
		controlHeight,
		imageFilename
		);
	controlPtr->transparent = transparent;

	GenericGraphicsFrameworkDrawTree(pParent);

	// remove the control added
	controlPtr->pParent->pTailOfChildrenList = pPreviousLastChild;
	pPreviousLastChild->pRightSibling = NULL;

	if (delayMilliseconds > 0)
	{
		SDL_Delay(delayMilliseconds);

		// re-draw without the notification control
		GenericGraphicsFrameworkDrawTree(pParent);
	};	
}

#ifdef __linux__ 

// On Button Press
// DFS over all window's children
void GenericGraphicsFrameworkBlockingPollForEvents(CONTROL * pRoot)
{
	CONTROL *pControl = NULL;
	BOOL keepPolling = true;
	assert(pRoot);
	//assert(pRoot->pHeadOfChildrenList);
	//assert(pRoot->pTailOfChildrenList);
	assert(pRoot->OnPressCallback);
	DEBUG_PRINT("%p", (void*)pRoot);
	while (keepPolling)
	{
		//DEBUG_PRINT("keepPolling");
		while (SDL_PollEvent(&m_event) != 0)
		{
			switch (m_event.type)
			{
			case (SDL_MOUSEBUTTONUP):
				DEBUG_PRINT("Mouse button at: %d, %d", m_event.button.x, m_event.button.y);	
				pControl = GenericGraphicsFrameworkButtonPressDFSHelper(pRoot, pRoot);
				if (NULL != pControl)
				{
					// Attention: callback is called before redrawing the selection
					keepPolling = pRoot->OnPressCallback(pControl);

					// draw the selection
					if (NULL != pControl->selectedImageFilename)
					{
						DEBUG_PRINT("Redrawing Window for representation of selection");
						pControl->buttonSelected = true;         
						GenericGraphicsFrameworkDrawTree(pRoot);
						// after drawing the button selected
						// turn it off so that it would be off 
						// when the next button is selected
						// enhancement:
						// remember the last selected button and compare to it
						pControl->buttonSelected = false;
					}

				}
				break;

			default:
				break;
			}
		}
	}
}

static BOOL GenericGraphicsFrameworkDrawControl(CONTROL * pControl, CONTROL * pRoot)
{
	int result;
	const char* imageFileToDraw = NULL;
	SDL_Surface * pSdlSurface = (SDL_Surface *)pRoot->implementationSpecificFields;
	assert(pSdlSurface);
	assert(pControl);

	if ((pControl->buttonSelected) && (NULL != pControl->selectedImageFilename))
	{
		imageFileToDraw = pControl->selectedImageFilename;
	}
	else
	{
		imageFileToDraw = pControl->imageFilename;
	}
	if (NULL == imageFileToDraw)
	{
		return false;
	}
	/* Load Image File */
	SDL_Surface *img = SDL_LoadBMP(imageFileToDraw);
	if (NULL == img) {
		PRINT_ERROR("failed to load image file: %s %s", imageFileToDraw, SDL_GetError());
		return false;
	}

	/* Set colorkey/transparency */
	if (pControl->transparent)
	{
		result = SDL_SetColorKey(img, SDL_SRCCOLORKEY, SDL_MapRGB(img->format, 255, 255, 255));
		if (SDL_SUCCESS != result) {
			SDL_FreeSurface(img);
			PRINT_ERROR("failed to set color key: %s", SDL_GetError());
			return false;
		}
	}

	/* Only the position is used in the dstrect (the width and height are ignored). */
	SDL_Rect imgDstRect = {pControl->xPosition, pControl->yPosition, 0, 0};

	/* The width and height in srcrect determine the size of the copied rectangle. */
	/* If srcrect is NULL, the entire surface is copied. */
	SDL_Rect * imgSrcRect = NULL;

	/* Draw image sprite */
	result = SDL_BlitSurface(img, imgSrcRect, pSdlSurface, &imgDstRect);
	if (SDL_SUCCESS != result) {
		SDL_FreeSurface(img);
		PRINT_ERROR("failed to blit image: %s", SDL_GetError());
		return false;
	}
	result = SDL_Flip(pSdlSurface);
	if (SDL_SUCCESS != result) {
		SDL_FreeSurface(img);
		PRINT_ERROR("failed to flip buffer: %s", SDL_GetError());
		return false;
	}
#ifdef DEBUG_GRAPHICS	
	if (NULL != (pControl->pParent))
	{
		VERBOSE_PRINT("pSdlSurface=%p, imageFileToDraw=%s, parent=%s", (void*)pSdlSurface, imageFileToDraw, pControl->pParent->imageFilename);
	}
	else
	{
		VERBOSE_PRINT("pSdlSurface=%p, imageFileToDraw=%s", (void*)pSdlSurface, imageFileToDraw);
	}
	
	
#endif	
	/* Free the memory */
	SDL_FreeSurface(img);
	return false;
}

#endif //LINUX

/***********************************/
/* PRIVATE METHODS IMPLEMENTATIONS */
/***********************************/
static BOOL GenericGraphicsFrameworkDrawDFSHelper(CONTROL * pControl, CONTROL * pRoot, DFS_OPERATOR_FUNC_P operator)
{
	CONTROL * pChild = NULL;
	BOOL shouldSearchStop = false;
	//DEBUG_PRINT("%p", (void*)pControl);
	assert(NULL != pControl);
	assert(NULL != pRoot);
	assert(operator);
#ifdef _WIN32
	assert((int)pControl != 0xCCCCCCCC);
#endif
	pChild = pControl->pHeadOfChildrenList;


	// NOTE that the operator on the current node can be called
	// either before or after the calling Helper for the children
	shouldSearchStop = operator(pControl, pRoot);
	if (shouldSearchStop)
	{
		return shouldSearchStop;
	}
	while (NULL != pChild)
	{
#ifdef _WIN32
		assert((int)pChild != 0xCCCCCCCC);
#endif
		shouldSearchStop = GenericGraphicsFrameworkDrawDFSHelper(pChild, pRoot, operator);
		if (shouldSearchStop)
		{
			return shouldSearchStop;
		} 
		pChild = pChild->pRightSibling;
	}

	return shouldSearchStop;
}

static CONTROL * GenericGraphicsFrameworkButtonPressDFSHelper(CONTROL * pControl, CONTROL * root)
{
	CONTROL * pChild = NULL;
	CONTROL * pCurrControl = NULL;
	BOOL wasButtonPressed = false;
	assert(pControl != NULL);
	assert(root != NULL);   

	wasButtonPressed = GenericGraphicsFrameworkWasButtonPressed(pControl, root);
	if (wasButtonPressed)
	{
		return pControl;
	}
	pChild = pControl->pHeadOfChildrenList;
	while (NULL != pChild)
	{
		pCurrControl = GenericGraphicsFrameworkButtonPressDFSHelper(pChild, root);
		if (NULL != pCurrControl)
		{
			return pCurrControl;
		} 
		pChild = pChild->pRightSibling;
	}
	return NULL;
}

static BOOL GenericGraphicsFrameworkWasButtonPressed(CONTROL * pControl, CONTROL * pRoot)
{
	// check if node is an enabled button
	if (false == pControl->buttonEnabled)
	{
		return false;
	}

	// check if this is the button that was pressed (based on coordinates)
	if ((m_event.button.x > pControl->xPosition) && (m_event.button.x < pControl->xPosition + (int)pControl->width) && (m_event.button.y > pControl->yPosition) && (m_event.button.y < pControl->yPosition + (int)pControl->height))
	{
		return true;
	}
	return false;
}


static void GenericGraphicsFrameworkCreateControl(
	CONTROL * controlPtr,
	CONTROL * pParent,
	BOOL childWillBeAddedAsHeadOfParentsChildren,
	int xPosRelativeToParent, 
	int yPosRelativeToParent, 
	unsigned int controlWidth, 
	unsigned int controlHeight,
	const char* imageFilename
	)
{
	assert(controlPtr);
	controlPtr->pHeadOfChildrenList = NULL;
	controlPtr->pTailOfChildrenList = NULL;
	controlPtr->pRightSibling = NULL;
	if (NULL != pParent)
	{
		controlPtr->pParent = pParent;

		if (true == childWillBeAddedAsHeadOfParentsChildren)
		{
			controlPtr->pRightSibling = pParent->pHeadOfChildrenList;
			pParent->pHeadOfChildrenList = controlPtr;
			if (NULL == pParent->pTailOfChildrenList)
			{
				pParent->pTailOfChildrenList = controlPtr;
			}
		}
		else
		{

			if (NULL != pParent->pTailOfChildrenList)
			{
				pParent->pTailOfChildrenList->pRightSibling = controlPtr;
			}
			pParent->pTailOfChildrenList = controlPtr;
			if (NULL == pParent->pHeadOfChildrenList)
			{
				pParent->pHeadOfChildrenList = controlPtr;
			}			
		}

		controlPtr->xPosition = pParent->xPosition + xPosRelativeToParent;
		controlPtr->yPosition = pParent->yPosition + yPosRelativeToParent;
		controlPtr->implementationSpecificFields = pParent->implementationSpecificFields;

	}
	controlPtr->width = controlWidth;
	controlPtr->height = controlHeight;
	controlPtr->imageFilename = imageFilename;
	controlPtr->selectedImageFilename = NULL;
	controlPtr->buttonSelected = false;
	controlPtr->OnPressCallback = NULL;
	controlPtr->buttonEnabled = false;
	controlPtr->transparent = false;
}

// empty function definitions to at least enable compilation on windows
#ifdef _WIN32

void GenericGraphicsFrameworkBlockingPollForEvents(CONTROL * pRoot)
{


}

BOOL GenericGraphicsFrameworkDrawControl(CONTROL * pControl, CONTROL * pRoot)
{	
	SDL_Surface * pSdlSurface = NULL;
	assert(pRoot);
	assert(pControl);
	pSdlSurface = (SDL_Surface *)pRoot->implementationSpecificFields;
	assert(pSdlSurface);
	if (NULL == pControl->imageFilename)
	{
		return false;
	}
	printf("GenericGraphicsFrameworkDrawControlSTUB: img=%s, selected_img=%s, surface=%p, x=%d, y=%d, h=%d, w=%d\n", pControl->imageFilename, pControl->selectedImageFilename, pSdlSurface, pControl->xPosition, pControl->yPosition, pControl->height, pControl->width);
	//printf("GenericGraphicsFrameworkDrawControlSTUB: img=%s\n", pControl->imageFilename);

	return false;
}

int SDL_PollEvent(SDL_Event* e)
{
	e->type = SDL_MOUSEBUTTONUP;
	e->button.x = 400;
	e->button.y = 450;
	return 0;
}

#endif // WIN
