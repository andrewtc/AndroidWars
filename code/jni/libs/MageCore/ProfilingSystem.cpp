#include "CoreLib.h"

using namespace mage;

std::vector< ProfilingData > ProfilingSystem::msProfilerData;
std::map< std::string, int > ProfilingSystem::msTagToIdMap;

//---------------------------------------
ProfilingSystem::ProfilingSystem()
{}
//---------------------------------------
ProfilingSystem::~ProfilingSystem()
{}
//---------------------------------------
void ProfilingSystem::EndFrame()
{
	for ( std::vector< ProfilingData >::iterator itr = msProfilerData.begin(); itr != msProfilerData.end(); ++itr )
	{
		itr->CountPerFrame = 0;
		itr->TotalFrameTimeSeconds = 0.0;
	}
}
//---------------------------------------
int ProfilingSystem::GetIdFromName( const std::string& tag )
{
	int id;
	std::map< std::string, int >::const_iterator found = msTagToIdMap.find( tag );
	if ( found == msTagToIdMap.end() )
	{
		id = msProfilerData.size();
		msTagToIdMap[ tag ] = msProfilerData.size();
		msProfilerData.push_back( ProfilingData( tag ) );
	}
	else
	{
		id = found->second;
	}

	return id;
}
//---------------------------------------
ProfilingData& ProfilingSystem::GetDataFromId( int id )
{
	return msProfilerData[ id ];
}
//---------------------------------------
const std::vector< ProfilingData >& ProfilingSystem::GetProfilerData()
{
	return msProfilerData;
}
//---------------------------------------


//---------------------------------------
Profiler::Profiler( int id )
	: mId( id )
{
	mStartTimeSeconds = Clock::QueryTime();
}
//---------------------------------------
Profiler::~Profiler()
{
	ProfilingData& data = ProfilingSystem::GetDataFromId( mId );
	double deltaSeconds = Clock::QueryTime() - mStartTimeSeconds;

	data.TotalTimeSeconds += deltaSeconds;
	data.TotalFrameTimeSeconds += deltaSeconds;
	data.AverageFrameTimeSeconds = ( 0.9 * data.AverageFrameTimeSeconds ) + ( 0.1 * deltaSeconds );
	data.CountPerFrame++;
	data.Count++;

	if ( data.TotalFrameTimeSeconds > data.MaxTimeEver )
	{
		data.MaxTimeEver = data.TotalFrameTimeSeconds;
	}
}
//---------------------------------------


//---------------------------------------
ProfilingData::ProfilingData( const std::string& tag )
	: Tag( tag )
	, TotalTimeSeconds( 0.0 )
	, TotalFrameTimeSeconds( 0.0 )
	, AverageFrameTimeSeconds( 0.0 )
	, MaxTimeEver( 0.0 )
	, Count( 0 )
	, CountPerFrame( 0 )
{}
//---------------------------------------
double ProfilingData::GetAverageTime() const
{
	if ( Count == 0 ) return 0.0;
	return TotalTimeSeconds / Count;
}
//---------------------------------------