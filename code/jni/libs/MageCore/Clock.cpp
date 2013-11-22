#include "CoreLib.h"

using namespace mage;

Clock* Clock::msMasterClock = nullptr;
const double Clock::DEFAULT_MAX_DELTA_SECONDS = 1.0 / 60.0;

#ifdef WIN32
#	include <Windows.h>

//---------------------------------------
double GetInverseFrequency()
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency( &freq );
	return 1.0 / (double) freq.QuadPart;
}
//---------------------------------------

class ClockWin32
	: public ClockPDI
{
public:
	//---------------------------------------
	ClockWin32()
		: mIsQueryTime( false )
		, mStartTime( 0.0 )
		, mEndTime( 0.0 )
	{
		mInverseFrequency = GetInverseFrequency();
		mStartTick.QuadPart = 0;
		mEndTick.QuadPart   = 0;
	}
	//---------------------------------------
	void BeginTimeQuery()
	{
		mIsQueryTime = true;
		QueryPerformanceCounter( &mStartTick );
		mStartTime = mStartTick.QuadPart * mInverseFrequency;
	}
	//---------------------------------------
	void EndTimeQuery()
	{
		mIsQueryTime = false;
		QueryPerformanceCounter( &mEndTick );
	}
	//---------------------------------------
	double QueryTimeElapsed( Clock::TimeFormat timeFormat )
	{
		if ( mIsQueryTime )
		{
			QueryPerformanceCounter( &mEndTick );
		}

		mEndTime = mEndTick.QuadPart * mInverseFrequency;

		double timeSec = mEndTime - mStartTime;

		return Clock::GetFormatedTime( timeFormat, timeSec );
	}
	//---------------------------------------
	static double QueryTime( Clock::TimeFormat timeFormat )
	{
		LARGE_INTEGER time;
		static double invFreq = GetInverseFrequency();

		QueryPerformanceCounter( &time );

		return Clock::GetFormatedTime( timeFormat, (double) time.QuadPart * invFreq );
	}
	//---------------------------------------
private:
	bool mIsQueryTime;
	double mStartTime;
	double mEndTime;

	double mInverseFrequency;
	LARGE_INTEGER mStartTick;
	LARGE_INTEGER mEndTick;

};
#endif

#ifdef ANDROID
class ClockUnix
	: public ClockPDI
{
public:
	//---------------------------------------
	ClockUnix()
		: mIsQueryTime( false )
		, mStartTime( 0.0 )
		, mEndTime( 0.0 )
	{}
	//---------------------------------------
	void BeginTimeQuery()
	{
		mIsQueryTime = true;
		mStartTime = QueryTime( Clock::TIME_SEC );
	}
	//---------------------------------------
	void EndTimeQuery()
	{
		mIsQueryTime = false;
		mEndTime = QueryTime( Clock::TIME_SEC );
	}
	//---------------------------------------
	double QueryTimeElapsed( Clock::TimeFormat timeFormat )
	{
		if ( mIsQueryTime )
		{
			mEndTime = QueryTime( Clock::TIME_SEC );
		}

		double timeSec = mEndTime - mStartTime;

		return Clock::GetFormatedTime( timeFormat, timeSec );
	}
	//---------------------------------------
	static double QueryTime( Clock::TimeFormat timeFormat )
	{
		timespec lTimeVal;
		clock_gettime( CLOCK_MONOTONIC, &lTimeVal );
		return Clock::GetFormatedTime( timeFormat, lTimeVal.tv_sec + (lTimeVal.tv_nsec * 1.0e-9) );
	}
	//---------------------------------------
private:
	bool mIsQueryTime;
	double mStartTime;
	double mEndTime;
};
#endif

//---------------------------------------
Clock::Clock()
	: mElapsedSeconds( 0.0 )
	, mDeltaSeconds( 0.0 )
	, mMaxDeltaSeconds( DEFAULT_MAX_DELTA_SECONDS )
	, mTimeScale( 1.0 )
	, mIsPaused( false )
	, mParent( nullptr )
{
#ifdef WIN32
	mClockPDI = new ClockWin32();
#endif
#ifdef ANDROID
	mClockPDI = new ClockUnix();
#endif
}
//---------------------------------------
Clock::~Clock()
{
	delete mClockPDI;
}
//---------------------------------------
void Clock::SetTimeScale( double timeScale )
{
	mTimeScale = timeScale;
}
//---------------------------------------
void Clock::Pause()
{
	mIsPaused = true;
}
//---------------------------------------
void Clock::Resume()
{
	mIsPaused = false;
}
//---------------------------------------
void Clock::AdvanceTime( double deltaSeconds )
{
	// No delta if paused
	if ( mIsPaused )
	{
		deltaSeconds = 0.0;
	}

	// Limit max delta
	if ( deltaSeconds > mMaxDeltaSeconds )
	{
		deltaSeconds = mMaxDeltaSeconds;
	}

	// Advance time
	mDeltaSeconds = deltaSeconds * mTimeScale;
	mElapsedSeconds += mDeltaSeconds;

	// Check for events that need fired
	// Note: Cannot use iterators since container may be modified during an event callback.
	int numEvents = mTimeEvents.size();
	//for ( auto itr = mTimeEvents.begin(); itr != mTimeEvents.end(); ++itr )
	for ( int i = 0; i < numEvents; ++i )
	{
		if ( mTimeEvents[i].TimeToFireSeconds <= mElapsedSeconds )
		{
			EventManager::FireEvent( mTimeEvents[i].EventName, mTimeEvents[i].params );
		}
		// Since events are sorted by time, no other events will fire
		else	
		{
			break;
		}
	}

	// Remove fired events
    mTimeEvents.erase( std::remove_if( mTimeEvents.begin(), mTimeEvents.end(),
		[&]( const ClockTimeEventCallback& ev ) -> bool
	{
		return ev.TimeToFireSeconds <= mElapsedSeconds;
	}), mTimeEvents.end() );

	// Advance children
	for ( std::vector< Clock* >::iterator clockItr = mChildren.begin(); clockItr != mChildren.end(); ++clockItr )
	{
		(*clockItr)->AdvanceTime( deltaSeconds );
	}
}
//---------------------------------------
void Clock::ClearAllPostedEvents()
{
	mTimeEvents.clear();
}
//---------------------------------------
void Clock::SetMaxDeltaSeconds( double deltaSeconds )
{
	mMaxDeltaSeconds = deltaSeconds;
}
//---------------------------------------
void Clock::SetTime( double timeSeconds )
{
	mElapsedSeconds = timeSeconds;
}
//---------------------------------------
void Clock::PostEventCallbackAt( const std::string& eventName, double timeToFireSecods, const Dictionary& params )
{
	ClockTimeEventCallback ctec = { eventName, timeToFireSecods, params };
	mTimeEvents.push_back( ctec );
	std::sort( mTimeEvents.begin(), mTimeEvents.end() );
}
//---------------------------------------
void Clock::PostEventCallbackAfter( const std::string& eventName, double delaySeconds, const Dictionary& params )
{
	PostEventCallbackAt( eventName, mElapsedSeconds + delaySeconds, params );
}
//---------------------------------------
void Clock::BeginTimeQuery()
{
	mClockPDI->BeginTimeQuery();
}
//---------------------------------------
void Clock::EndTimeQuery()
{
	mClockPDI->EndTimeQuery();
}
//---------------------------------------
double Clock::QueryTimeElapsed( TimeFormat timeFormat )
{
	return mClockPDI->QueryTimeElapsed( timeFormat );
}
//---------------------------------------
double Clock::QueryTime( TimeFormat timeFormat )
{
#ifdef WIN32
	return ClockWin32::QueryTime( timeFormat );
#endif
#ifdef ANDROID
	return ClockUnix::QueryTime( timeFormat );
#endif
}
//---------------------------------------
double Clock::GetFormatedTime( TimeFormat timeFormat, double seconds )
{
	switch ( timeFormat )
	{
	case TIME_SEC:   return seconds;
	case TIME_MILLI: return seconds * 1000.0;
	case TIME_MICRO: return seconds * 1000000;
	default:		 assertion( false, "Invalid TimeForamt '%d'.\n", timeFormat ); return 0.0;
	}
}
//---------------------------------------
std::string Clock::GetStringFormatedTime( double seconds )
{
	char fmtStr[256];
	long totalMS = (long) ( seconds * 1000.0 );
	ldiv_t qr = div( totalMS, 1000L );
	int ms = (int) qr.rem;
	qr = div( qr.quot, 60L );
	int s = qr.rem;
	qr = div( qr.quot, 60L );
	int m = qr.rem;
	int h = qr.quot;
	sprintf_s( fmtStr, "%02d:%02d.%02d:%03d", h, m, s, ms );
	return fmtStr;
}
//---------------------------------------


//---------------------------------------
// Factory functions
//---------------------------------------
Clock& Clock::Initialize()
{
	if ( msMasterClock == nullptr )
	{
		msMasterClock = New0 Clock;
	}

	return *msMasterClock;
}
//---------------------------------------
Clock& Clock::GetMasterClock()
{
	return Initialize();
}
//---------------------------------------
Clock* Clock::CreateClock( Clock* parent )
{
	Clock* clock = New0 Clock;
	clock->mParent = parent;
	parent->mChildren.push_back( clock );
	return clock;
}
//---------------------------------------
void Clock::DestroyClock( Clock*& clock )
{
	if ( !clock ) return;
	// Destroying a child clock
	if ( clock->mParent )
	{
		std::vector< Clock* >::iterator clockItr = std::find( clock->mParent->mChildren.begin(),
			clock->mParent->mChildren.end(), clock );
		if ( clockItr != clock->mParent->mChildren.end() )
		{
			clock->mParent->mChildren.erase( clockItr,
				clock->mParent->mChildren.end() );
			Delete0( clock );
		}
	}
	// Destroying the root clock
	else if ( clock == msMasterClock )
	{
		Delete0( msMasterClock );
	}
}
//---------------------------------------
