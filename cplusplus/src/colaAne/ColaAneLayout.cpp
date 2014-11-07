#include "ColaAneLayout.h"
#include <math.h>

using namespace std;

ColaAneLayout::ColaAneLayout() :
	mLayout(NULL), mTestConvergence(NULL), mPreIteration(NULL) {

}

ColaAneLayout::~ColaAneLayout() {
	cleanup();
}

void ColaAneLayout::cleanup() {
	for (vector<vpsc::Rectangle*>::iterator it = mRectangles.begin(); it != mRectangles.end(); ++it) {
		delete (*it);
	}

	mRunning = false;

	mRectangles.clear();
	mFixed.clear();
	mEdges.clear();
	mEdgeLengths.clear();

	delete mLayout;
	delete mTestConvergence;
	delete mPreIteration;

	mLayout = NULL;
	mTestConvergence = NULL;
	mPreIteration = NULL;
}

void ColaAneLayout::start(
	const vector<InputVertex>& inputVertices,
	const vector<InputEdge>& inputEdges,
	double defaultLength,
	int iterationsWithoutOverlapChecks,
	double threshold,
	int maxIterations,
	double stepSizeMultiplier,
	bool adjustEdges,
	double adjustEdgeLengthWeight
	)
{
	cleanup();

	processInputVertices(inputVertices);
	processInputEdges(inputEdges, defaultLength);

	if (adjustEdges) {
		adjustEdgeLengths(inputVertices, inputEdges, adjustEdgeLengthWeight);
	}

	mTestConvergence = new cola::TestConvergence(threshold, maxIterations);
	mPreIteration = new FixedNodeHandler(mRectangles, mFixed);

	if (iterationsWithoutOverlapChecks) {
		cola::TestConvergence *nonOverlapConvergenceTest = new cola::TestConvergence(threshold, iterationsWithoutOverlapChecks);
		cola::ConstrainedFDLayout *nonOverlapLayout = new cola::ConstrainedFDLayout(
			mRectangles,
			mEdges,
			1,
			false,
			mEdgeLengths,
			nonOverlapConvergenceTest,
			mPreIteration
		);

		nonOverlapLayout->run();

		delete nonOverlapLayout;
		delete nonOverlapConvergenceTest;
	}

	mLayout = new cola::ConstrainedFDLayout(
		mRectangles,
		mEdges,
		1,
		true,
		mEdgeLengths,
		mTestConvergence,
		mPreIteration
	);

	mLayout->stepSizeMultiplier = stepSizeMultiplier;

	mRunning = true;
}

void ColaAneLayout::removeOverlaps() {
	set<unsigned int> fixedSet;
	for (unsigned int i = 0; i < mFixed.size(); i++) {
		if (mFixed[i]) {
			fixedSet.insert(i);
		}
	}
	vpsc::removeoverlaps(mRectangles);
}

bool ColaAneLayout::tick(bool removeOverlapsAfterwards) {
	mLayout->runOnce();
	if (removeOverlapsAfterwards) {
		removeOverlaps();
	}
	bool isDone = mLayout->isDone();
	mRunning = !isDone;
	return isDone;
}

void ColaAneLayout::run() {
	mLayout->run();
	mRunning = false;
}

void ColaAneLayout::processInputVertices(const vector<InputVertex>& inputVertices) {
	int i = 0;

	mFixed.reserve(inputVertices.size());
	mRectangles.reserve(inputVertices.size());

	for (vector<InputVertex>::const_iterator it = inputVertices.begin(); it != inputVertices.end(); ++it) {
		const InputVertex &v = *it;
		vpsc::Rectangle *rect = new vpsc::Rectangle(v.x, v.x + v.width, v.y, v.y + v.height, v.ghost);

		mRectangles.push_back(rect);
		mFixed.push_back(v.fixed);

		i++;
	}
}

void ColaAneLayout::processInputEdges(const vector<InputEdge>& inputEdges, double defaultLength) {
	mEdges.reserve(inputEdges.size());
	mEdgeLengths.reserve(inputEdges.size());

	for (vector<InputEdge>::const_iterator it = inputEdges.begin(); it != inputEdges.end(); ++it) {
		const InputEdge &e = *it;
		mEdgeLengths.push_back(e.hasIdealLength ? e.idealLength : defaultLength);
		mEdges.push_back(make_pair(e.source, e.target));
	}
}


void ColaAneLayout::adjustEdgeLengths(const std::vector<InputVertex>& inputVertices, const std::vector<InputEdge>& inputEdges, double weight) {
	int i = 0;
	for (vector<InputEdge>::const_iterator it = inputEdges.begin(); it != inputEdges.end(); ++it) {
		const InputEdge &e = *it;

		int unionCount = 0;
		int intersectionCount = 0;

		const InputVertex &v1 = inputVertices[e.source];
		const InputVertex &v2 = inputVertices[e.target];

		set< pair<int, int> > visited;
		for (vector<InputEdge>::const_iterator it2 = inputEdges.begin(); it2 != inputEdges.end(); ++it2) {
			const InputEdge &e2 = *it2;

			int otherVertex = -1;
			int thisVertex = -1;
			if ((e2.source == e.source) ^ (e2.target == e.target)) {
				otherVertex = (e2.source == e.source ? e2.target : e2.source);
				thisVertex = (e2.source == e.source ? e.source : e.target);
			}

			if ((e2.source == e.target) ^ (e2.target == e.source)) {
				otherVertex = (e2.source == e.target ? e2.target : e2.source);
				thisVertex = (e2.source == e.target ? e.target : e.source);
			}

			if (otherVertex == -1) {
				continue;
			}

			if (visited.find(make_pair(thisVertex, otherVertex)) != visited.end()) {
				intersectionCount++;
			}
			else{
				unionCount++;
				visited.insert(make_pair(thisVertex, otherVertex));
			}
		}

		mEdgeLengths[i] += mEdgeLengths[i]*weight * (sqrt((double)(unionCount - intersectionCount)));

		i++;
	}
}

FixedNodeHandler::FixedNodeHandler(const vector<vpsc::Rectangle*>& rects, const vector<bool>& rectsFixed) : PreIteration(mLocks) {
	for (unsigned int i = 0; i < rects.size(); i++) {
		if (rectsFixed[i]) {
			locks.push_back(cola::Lock(i, rects[i]->getCentreX(), rects[i]->getCentreY()));
		}
	}
}

bool FixedNodeHandler::operator()() {
	return true;
}
