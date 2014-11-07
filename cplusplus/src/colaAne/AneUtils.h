#include <FlashRuntimeExtensions.h>

#include <string>

class AneObject {
public:
	AneObject();
	AneObject(FREObject obj);
	bool hasProperty(const char *prop);
	FREObject getProperty(const char *prop);
	bool setProperty(const char *prop, FREObject value);

	FREObject mObject;
};

class AneConstArray {
protected:
	AneConstArray() { }

public:
	AneConstArray(FREObject obj);

	FREObject elementAt(int index);
	AneObject objectAt(int index);

	virtual int length();

	int mCachedLength;
	FREObject mObject;
};

class AneArray : public AneConstArray {
public:
	AneArray(int initialLength = 0);
	AneArray(FREObject obj);

	virtual int length();

	bool setElementAt(int index, FREObject obj);
	bool setObjectAt(int index, const AneObject &obj);
};

void registerAneFunction(FRENamedFunction *fnArray, int index, FREFunction function, const char *name);

double getAneDouble(FREObject obj);
FREObject aneDouble(double d);

int getAneInt(FREObject obj);
FREObject aneInt(int i);

unsigned int getAneUint(FREObject obj);
FREObject aneUint(unsigned int i);

bool getAneBool(FREObject obj);
FREObject aneBool(bool b);

std::wstring getAneString(FREObject strObj);
std::string getAneUtfString(FREObject strObj);
FREObject aneString(const std::string &str);
