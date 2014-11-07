#include <stddef.h>
#include <vector>
#include <libcola/cola.h>
#include <libvpsc/rectangle.h>

struct InputVertex {
	InputVertex(double _x, double _y, double _width, double _height, bool _fixed, bool _ghost) :
		x(_x), y(_y), width(_width), height(_height), fixed(_fixed), ghost(_ghost)
	{}
	double x;
	double y;
	double width;
	double height;
	bool fixed;
	bool ghost;
};

struct InputEdge {
	InputEdge(unsigned int _source, unsigned int _target, bool _hasIdealLength, double _idealLength) :
		source(_source), target(_target), hasIdealLength(_hasIdealLength), idealLength(_idealLength)
	{}
	unsigned int source;
	unsigned int target;
	bool hasIdealLength;
	double idealLength;
};

class ColaAneLayout {
public:
	ColaAneLayout();
	~ColaAneLayout();

	void start(
		const std::vector<InputVertex>& inputVertices,
		const std::vector<InputEdge>& inputEdges,
		double defaultLength,
		int iterationsWithoutOverlapChecks = 10,
		double threshold = 0.0001,
		int maxIterations = 100,
		double stepSizeMultiplier = 1.0,
		bool adjustEdges = false,
		double adjustEdgeLengthWeight = 0.5
	);
	void removeOverlaps();
	bool tick(bool removeOverlapsAfterwards = false);
	void run();

	const std::vector<vpsc::Rectangle*>& getRectangles() { return mRectangles; }
	bool isFixed(int i) { return mFixed[i]; }
	bool isGhost(int i) { return mRectangles[i]->allowOverlap(); }

private:
	void cleanup();

	void processInputVertices(const std::vector<InputVertex>& inputVertices);
	void processInputEdges(const std::vector<InputEdge>& inputEdges, double defaultLength);

	void adjustEdgeLengths(const std::vector<InputVertex>& inputVertices, const std::vector<InputEdge>& inputEdges, double weight);

	bool mRunning;
	cola::ConstrainedFDLayout *mLayout;
	cola::TestConvergence *mTestConvergence;
	cola::PreIteration *mPreIteration;

	std::vector<vpsc::Rectangle*> mRectangles;
	std::vector<bool> mFixed;
	std::vector<cola::Edge> mEdges;
	std::vector<double> mEdgeLengths;
};

class FixedNodeHandler : public cola::PreIteration {
public:
	FixedNodeHandler(const std::vector<vpsc::Rectangle*>& rects, const std::vector<bool>& rectsFixed);
	virtual bool operator()();

private:
	std::vector<cola::Lock> mLocks;
};
