#pragma once

// STD C Headers
#include "std_headers.h"

// STD C++ Headers
#include "stl_headers.h"

#ifdef ANDROID
#   define sprintf_s( buffer, format, ... ) sprintf( buffer, format, __VA_ARGS__ )
#   define vsprintf_s( buffer, format, ... ) vsprintf( buffer, format, __VA_ARGS__ )
#   define _snprintf_s(a,b,c,...) snprintf(a,b,__VA_ARGS__)
#   define _vsnprintf_s(a,b,c,...) vsnprintf(a, b, __VA_ARGS__)
#   define strcat_s(a,b) strcat(a,b)
#   define strcpy_s(a,b) strcpy(a,b)
#   define strncpy_s(d,s,n) strncpy(d,s,n)
#   define fopen_s(pfp,fn,m) *pfp=fopen(fn,m)
#   define nullptr NULL
#   define _stricmp(a,b) strcasecmp(a,b)
#endif

#ifdef USE_MEMORY_MANAGER
#	define new new( __FILE__, __LINE__ )
#endif

// Typedefs for core types
#include "MageTypes.h"
#include "Color.h"

// Debug
#include "DebugIO.h"
#include "Assertion.h"

// Memory
#include "MageMemory.h"

// IO
#include "Console.h"

// Utility
#include "Base64.h"
#include "BitHacks.h"
#include "HashUtil.h"
#include "StringUtil.h"
#include "HashString.h"
#include "Resource.h"
#include "FileSystem.h"
#include "XmlReader.h"
#include "XmlWriter.h"

// Data structures
#include "CommandArgs.h"
#include "HashMap.h"
#include "Dictionary.h"
#include "CircularBuffer.h"
#include "Event.h"
#include "Clock.h"
#include "ProfilingSystem.h"
#include "ArrayList.h"
#include "Transform2D.h"

// Threads
#include "Mutex.h"
#include "Thread.h"
#include "Job.h"
#include "JobManager.h"

// Object management system
#include "RTTI.h"
#include "Object.h"
#include "Factory.h"
#include "Controller.h"
#include "ControlledObject.h"
#include "AnimationController.h"
#include "KeyframeController.h"

// Scene management
#include "NodeVisualizer.h"
#include "Node.h"
#include "IKJoint.h"
#include "IKChain.h"
#include "IKController.h"
#include "IKNode.h"