#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef enum
{
	false = 0, 
	true
} BOOL;

#ifdef _DEBUG
#define DEBUG_PRINT(...)	{				    \
	fprintf(stderr, "%s: ", __FUNCTION__);		\
	fprintf(stderr, __VA_ARGS__);		      	\
	fprintf(stderr, "\n");		            	\
	}

#ifdef _VERBOSE
#define VERBOSE_PRINT(...)	{				    \
	fprintf(stderr, "%s: ", __FUNCTION__);		\
	fprintf(stderr, __VA_ARGS__);		      	\
	fprintf(stderr, "\n");		            	\
	}
	
#else
#define VERBOSE_PRINT(...)		;	
#endif // _VERBOSE
	
#define FUNCTION_DEBUG_TRACE	fprintf(stderr, "%s\n", __FUNCTION__);

#define FUNCTION_DEBUG_INSTRUMENTATION	fprintf(stderr, "%s %d\n", __FUNCTION__, __LINE__);

#define PRINT_ERROR(...) {				\
	fprintf(stderr, "ERROR: ");			\
	fprintf(stderr, __VA_ARGS__);	   	\
	fprintf(stderr, "\n");              \
	}
	
#else
	
#define VERBOSE_PRINT(...)		;
#define DEBUG_PRINT(...)		;
#define FUNCTION_DEBUG_TRACE	;
#define FUNCTION_DEBUG_INSTRUMENTATION	;
#define PRINT_ERROR(...)		;

#endif // _DEBUG

#define CRITICAL_ERROR(...)	{										\
	fprintf(stdout, "CRITICAL ERROR: ");	\
	fprintf(stdout, __VA_ARGS__);	   		\
	fprintf(stdout, "\n");                 \
	exit(-1);							      	\
	}


#endif