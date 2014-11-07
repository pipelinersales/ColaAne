#include "AneUtils.h"

using std::string;

void registerAneFunction(FRENamedFunction *fnArray, int index, FREFunction function, const char *name) {
	fnArray[index].name = (const uint8_t*) name;
	fnArray[index].functionData = NULL;
	fnArray[index].function = function;
}

string getAneUtfString(FREObject strObj) {
	unsigned int utfLen;
	const uint8_t *utfStr;

	FREGetObjectAsUTF8(strObj, &utfLen, &utfStr);

	string str = (const char*)utfStr;
	return str;
}

double getAneDouble(FREObject obj) {
	double d = 0;
	FREGetObjectAsDouble(obj, &d);
	return d;
}

FREObject aneDouble(double d) {
	FREObject obj;
	FRENewObjectFromDouble(d, &obj);
	return obj;
}

int getAneInt(FREObject obj) {
	int i = 0;
	FREGetObjectAsInt32(obj, &i);
	return i;
}

FREObject aneInt(int i) {
	FREObject obj;
	FRENewObjectFromInt32(i, &obj);
	return obj;
}

unsigned int getAneUint(FREObject obj) {
	unsigned int i = 0;
	FREGetObjectAsUint32(obj, &i);
	return i;
}

FREObject aneUint(unsigned int i) {
	FREObject obj;
	FRENewObjectFromUint32(i, &obj);
	return obj;
}

bool getAneBool(FREObject obj) {
	uint32_t b = 0;
	FREGetObjectAsBool(obj, &b);
	return (b != 0);
}

FREObject aneBool(bool b) {
	FREObject obj;
	FRENewObjectFromBool(b, &obj);
	return obj;
}

FREObject aneString(const std::string &str) {
	FREObject obj;
	FRENewObjectFromUTF8(str.length(), (const uint8_t*)str.c_str(), &obj);
	return obj;
}

AneObject::AneObject() {
	FREObject exc;
	FRENewObject((const uint8_t*)"Object", 0, NULL, &mObject, &exc);
}

AneObject::AneObject(FREObject obj) : mObject(obj) {
}

bool AneObject::hasProperty(const char *prop) {
	FREObject propStr = aneString(prop);
	FREObject result, exc;
	FRECallObjectMethod(mObject, (const uint8_t*)"hasOwnProperty", 1, &propStr, &result, &exc);
	return getAneBool(result);
}

FREObject AneObject::getProperty(const char *prop) {
	FREObject val, exc;
	FREGetObjectProperty(mObject, (const uint8_t*)prop, &val, &exc);
	return val;
}

bool AneObject::setProperty(const char *prop, FREObject value) {
	FREObject exc;
	return (FRESetObjectProperty(mObject, (const uint8_t*)prop, value, &exc) == FRE_OK);
}

AneConstArray::AneConstArray(FREObject obj) : mObject(obj) {
	unsigned int len = 0;
	FREGetArrayLength(mObject, &len);
	mCachedLength = len;
}

FREObject AneConstArray::elementAt(int index) {
	FREObject obj;
	FREGetArrayElementAt(mObject, index, &obj);
	return obj;
}

AneObject AneConstArray::objectAt(int index) {
	return AneObject(elementAt(index));
}

int AneConstArray::length() {
	return mCachedLength;
}

AneArray::AneArray(int initialLength) {
	FREObject exc;
	FREObject freLen = aneInt(initialLength);
	FRENewObject((const uint8_t*)"Array", 1, &freLen, &mObject, &exc);
}

AneArray::AneArray(FREObject obj) {
	mObject = obj;
}

int AneArray::length() {
	unsigned int len;
	FREGetArrayLength(mObject, &len);
	return len;
}

bool AneArray::setElementAt(int index, FREObject obj) {
	return (FRESetArrayElementAt(mObject, index, obj) == FRE_OK);
}

bool AneArray::setObjectAt(int index, const AneObject &obj) {
	return setElementAt(index, obj.mObject);
}
