#include "CoreLib.h"

#include <pthread.h>
#include <signal.h>
#include <sched.h>
#include <unistd.h>

using namespace mage;

//---------------------------------------
class MutexUnix
	: public PDIMutex
{
public:
	//---------------------------------------
	MutexUnix()
	{
		pthread_mutex_init( &mHandle, NULL );
	}
	//---------------------------------------
	virtual ~MutexUnix()
	{
		pthread_mutex_destroy( &mHandle );
	}
	//---------------------------------------
	void Lock()
	{
        pthread_mutex_lock( &mHandle );
	}
	//---------------------------------------
	bool TryLock()
	{
        return pthread_mutex_trylock( &mHandle ) == 0;
	}
	//---------------------------------------
	void Unlock()
	{
		pthread_mutex_unlock( &mHandle );
	}
	//---------------------------------------
private:
	pthread_mutex_t mHandle;
};
//---------------------------------------


//---------------------------------------
Mutex::Mutex()
	: mPDIMutex( new MutexUnix )
{}
//---------------------------------------
Mutex::~Mutex()
{
	delete mPDIMutex;
}
//---------------------------------------