#include "androidwars.h"

using namespace mage;


JavaVM* JNI::sJavaVM = nullptr;
JNIEnv* JNI::instance = nullptr;
int JNI::referenceCount = 0;
