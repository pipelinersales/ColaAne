#ifdef COLAANE_WIN
	#ifdef COLAANE_EXPORTS
		#define COLAANE_API __declspec(dllexport)
	#else
		#define COLAANE_API __declspec(dllimport)
	#endif
#endif

#include <FlashRuntimeExtensions.h>

#ifdef COLAANE_MAC
    #define COLAANE_API __attribute__((visibility("default")))
#endif

extern "C" {
	void COLAANE_API ANEInitializer(void** extDataToSet, FREContextInitializer* ctxInitializerToSet, FREContextFinalizer* ctxFinalizerToSet);
	void COLAANE_API ANEContextInitializer(void* extData, const uint8_t* ctxType, FREContext ctx, uint32_t* numFunctionsToSet, const FRENamedFunction** functionsToSet);
	void COLAANE_API ANEContextFinalizer(FREContext ctx);
	void COLAANE_API ANEFinalizer(void *extData);

	FREObject COLAANE_API ColaStartLayout(FREContext ctx, void* functionData, uint32_t argc, FREObject argv[]);
	FREObject COLAANE_API ColaTick(FREContext ctx, void* functionData, uint32_t argc, FREObject argv[]);
	FREObject COLAANE_API ColaRun(FREContext ctx, void* functionData, uint32_t argc, FREObject argv[]);
	FREObject COLAANE_API ColaGetVertices(FREContext ctx, void* functionData, uint32_t argc, FREObject argv[]);
	FREObject COLAANE_API ColaRemoveOverlaps(FREContext ctx, void* functionData, uint32_t argc, FREObject argv[]);
}
