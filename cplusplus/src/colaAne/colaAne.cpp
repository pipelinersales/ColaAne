#include "colaAne.h"
#include "ColaAneLayout.h"
#include "AneUtils.h"
#include <libcola/cola.h>
#include <vector>
#include <utility>
#include <map>
#include <string>

using namespace std;


void COLAANE_API ANEInitializer(void** extDataToSet, FREContextInitializer* ctxInitializerToSet, FREContextFinalizer* ctxFinalizerToSet) {
	*extDataToSet = NULL;
	*ctxInitializerToSet = &ANEContextInitializer;
	*ctxFinalizerToSet = &ANEContextFinalizer;
}

void COLAANE_API ANEContextInitializer(void* extData, const uint8_t* ctxType, FREContext ctx, uint32_t* numFunctionsToSet, const FRENamedFunction** functionsToSet) {
	*numFunctionsToSet = 5;

	FRENamedFunction* func = (FRENamedFunction*)malloc(sizeof(FRENamedFunction)* (*numFunctionsToSet));

	registerAneFunction(func, 0, &ColaStartLayout, "ColaStartLayout");
	registerAneFunction(func, 1, &ColaTick, "ColaTick");
	registerAneFunction(func, 2, &ColaGetVertices, "ColaGetVertices");
	registerAneFunction(func, 3, &ColaRun, "ColaRun");
	registerAneFunction(func, 4, &ColaRemoveOverlaps, "ColaRemoveOverlaps");

	*functionsToSet = func;

	ColaAneLayout *aneLayout = new ColaAneLayout();
	FRESetContextNativeData(ctx, aneLayout);
}

void COLAANE_API ANEContextFinalizer(FREContext ctx) {
	ColaAneLayout *aneLayout;
	FREGetContextNativeData(ctx, (void**)&aneLayout);

	delete aneLayout;
}

void COLAANE_API ANEFinalizer(void *extData) {

}

FREObject COLAANE_API ColaStartLayout(FREContext ctx, void* functionData, uint32_t argc, FREObject argv[]) {
	ColaAneLayout *aneLayout;
	FREGetContextNativeData(ctx, (void**)&aneLayout);

	AneConstArray pointsIn(argv[0]);
	AneConstArray edgesIn(argv[1]);
	double defaultLength = getAneDouble(argv[2]);
	int iterationsWithoutOverlapChecks = getAneInt(argv[3]);
	double threshold = getAneDouble(argv[4]);
	int maxIterations = getAneInt(argv[5]);
	double stepSizeMultiplier = getAneDouble(argv[6]);
	bool adjustEdges = getAneBool(argv[7]);
	double adjustEdgeLengthWeight = getAneDouble(argv[8]);

	vector<InputVertex> inputVertices;
	vector<InputEdge> inputEdges;

	inputVertices.reserve(pointsIn.length());
	inputEdges.reserve(edgesIn.length());

	for (int i = 0; i < pointsIn.length(); i++) {
		AneObject point = pointsIn.elementAt(i);
		
		double x = getAneDouble(point.getProperty("x"));
		double y = getAneDouble(point.getProperty("y"));
		double width = getAneDouble(point.getProperty("width"));
		double height = getAneDouble(point.getProperty("height"));

		bool fixed = point.hasProperty("fixed") && getAneBool(point.getProperty("fixed"));
		bool ghost = point.hasProperty("ghost") && getAneBool(point.getProperty("ghost"));

		inputVertices.push_back(InputVertex(x, y, width, height, fixed, ghost));
	}

	for (int i = 0; i < edgesIn.length(); i++) {
		AneObject edge = edgesIn.elementAt(i);

		uint32_t source = getAneUint(edge.getProperty("source"));
		uint32_t target = getAneUint(edge.getProperty("target"));

		bool hasIdealLength = edge.hasProperty("idealLength");
		double idealLength = getAneDouble(edge.getProperty("idealLength"));
		
		inputEdges.push_back(InputEdge(source, target, hasIdealLength, idealLength));
	}

	aneLayout->start(
		inputVertices,
		inputEdges,
		defaultLength,
		iterationsWithoutOverlapChecks,
		threshold,
		maxIterations,
		stepSizeMultiplier,
		adjustEdges,
		adjustEdgeLengthWeight
	);

	AneObject result;
	result.setProperty("error", NULL);
	result.setProperty("result", aneBool(true));
	return result.mObject;
}

FREObject COLAANE_API ColaTick(FREContext ctx, void* functionData, uint32_t argc, FREObject argv[]) {
	ColaAneLayout *aneLayout;
	FREGetContextNativeData(ctx, (void**)&aneLayout);

	bool removeOverlaps = getAneBool(argv[0]);

	bool hasEnded = aneLayout->tick(removeOverlaps);

	AneObject result;
	result.setProperty("error", NULL);
	result.setProperty("result", aneBool(hasEnded));
	return result.mObject;
}

FREObject COLAANE_API ColaGetVertices(FREContext ctx, void* functionData, uint32_t argc, FREObject argv[]) {
	ColaAneLayout *aneLayout;
	FREGetContextNativeData(ctx, (void**)&aneLayout);

	const vector<vpsc::Rectangle*>& rectangles = aneLayout->getRectangles();

	AneArray data(rectangles.size());
	unsigned int i = 0;
	for (vector<vpsc::Rectangle*>::const_iterator it = rectangles.begin(); it != rectangles.end(); ++it) {
		const vpsc::Rectangle  *rect = *it;

		AneObject pt;
		pt.setProperty("x", aneDouble(rect->getMinX()));
		pt.setProperty("y", aneDouble(rect->getMinY()));
		pt.setProperty("width", aneDouble(rect->getMaxX() - rect->getMinX()));
		pt.setProperty("height", aneDouble(rect->getMaxY() - rect->getMinY()));
		
		pt.setProperty("fixed", aneBool(aneLayout->isFixed(i)));
		pt.setProperty("ghost", aneBool(aneLayout->isGhost(i)));

		data.setObjectAt(i, pt);

		i++;
	}

	AneObject result;
	result.setProperty("error", NULL);
	result.setProperty("result", data.mObject);
	return result.mObject;
}

FREObject COLAANE_API ColaRun(FREContext ctx, void* functionData, uint32_t argc, FREObject argv[]) {
	ColaAneLayout *aneLayout;
	FREGetContextNativeData(ctx, (void**)&aneLayout);

	aneLayout->run();

	const vector<vpsc::Rectangle*>& rectangles = aneLayout->getRectangles();

	AneArray data(rectangles.size());
	unsigned int i = 0;
	for (vector<vpsc::Rectangle*>::const_iterator it = rectangles.begin(); it != rectangles.end(); ++it) {
		const vpsc::Rectangle  *rect = *it;

		AneObject pt;
		pt.setProperty("x", aneDouble(rect->getMinX()));
		pt.setProperty("y", aneDouble(rect->getMinY()));
		pt.setProperty("width", aneDouble(rect->getMaxX() - rect->getMinX()));
		pt.setProperty("height", aneDouble(rect->getMaxY() - rect->getMinY()));
		
		pt.setProperty("fixed", aneBool(aneLayout->isFixed(i)));
		pt.setProperty("ghost", aneBool(aneLayout->isGhost(i)));

		data.setObjectAt(i, pt);

		i++;
	}

	AneObject result;
	result.setProperty("error", NULL);
	result.setProperty("result", data.mObject);
	return result.mObject;
}

FREObject COLAANE_API ColaRemoveOverlaps(FREContext ctx, void* functionData, uint32_t argc, FREObject argv[]) {
	ColaAneLayout *aneLayout;
	FREGetContextNativeData(ctx, (void**)&aneLayout);

	aneLayout->removeOverlaps();

	AneObject result;
	result.setProperty("error", NULL);
	result.setProperty("result", aneBool(true));
	return result.mObject;
}
