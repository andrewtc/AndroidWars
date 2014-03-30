#pragma once


namespace mage
{
	/**
	 * Pure STL-style template magic wrapping an arbitrary function or lambda type
	 * (to avoid using std::function, which is not supported by stl-port on Android).
	 */
	template< typename ReturnType, typename... ParameterTypes >
	class Delegate
	{
	private:
		/**
		 * Abstract base class that allows us to call an arbitrary function type and return the result.
		 */
		struct Callable
		{
			virtual ReturnType Invoke( ParameterTypes... parameters ) const = 0;
			virtual Callable* Clone() const = 0;
			virtual bool operator==( const Callable& other ) const = 0;
		};

		/**
		 * Template wrapper class that implements the invocation function for any lambda or function type.
		 */
		template< typename Function >
		struct FunctionWrapper : public Callable
		{
			FunctionWrapper( Function function ) :
				function( function )
			{ }

			virtual ReturnType Invoke( ParameterTypes... parameters ) const
			{
				return function( parameters... );
			}

			/**
			 * Creates and returns a new wrapper of the same type around the same internal function or lambda.
			 */
			virtual Callable* Clone() const
			{
				return new FunctionWrapper< Function >( function );
			}

			virtual bool operator==( const Callable& other ) const
			{
				const FunctionWrapper< Function >* otherDerived = dynamic_cast< const FunctionWrapper< Function >* >( &other );
				DebugPrintf( "Types %s equal.", ( otherDerived ? "ARE" : "ARE NOT" ) );
				return ( otherDerived != nullptr ) && ( memcmp( &function, &otherDerived->function, sizeof( Function ) ) == 0 );
			}

			Function function;
		};

		/**
		 * Template wrapper class that implements the invocation function for any class method on an object (callee).
		 */
		template< class Callee, typename Method >
		struct MethodWrapper : public Callable
		{
			MethodWrapper( Callee* callee, Method method ) :
				callee( callee ), method( method )
			{ }

			virtual ReturnType Invoke( ParameterTypes... parameters ) const
			{
				// Invoke the method call on the object and return the result.
				return ( callee->*method )( parameters... );
			}

			/**
			 * Creates and returns a new wrapper of the same type around the same internal object and method.
			 */
			virtual Callable* Clone() const
			{
				return new MethodWrapper< Callee, Method >( callee, method );
			}

			virtual bool operator==( const Callable& other ) const
			{
				const MethodWrapper< Callee, Method >* otherDerived = dynamic_cast< const MethodWrapper< Callee, Method >* >( &other );
				return ( otherDerived != nullptr ) && ( callee == otherDerived->callee ) && ( method == otherDerived->method );
			}

			Callee* callee;
			Method method;
		};

	public:
		/**
		 * Default constructor that wraps an invalid (i.e. null) function.
		 */
		Delegate() :
			mWrapper( nullptr )
		{
			//DebugPrintf( "Created Delegate wrapping 0x%x!", mWrapper );
		}

		/**
		 * Copy constructor that clones another Delegate.
		 */
		Delegate( const Delegate< ReturnType, ParameterTypes... >& other ) :
			mWrapper( other.IsValid() ? other.mWrapper->Clone() : nullptr )
		{
			//DebugPrintf( "Cloned Delegate wrapping 0x%x! (Clone wraps 0x%x.)", other.mWrapper, mWrapper );
		}

		/**
		 * Templated constructor allowing the creation of a Delegate that wraps any valid function or lambda type.
		 */
		template< typename Function >
		Delegate( Function function ) :
			mWrapper( new FunctionWrapper< Function >( function ) )
		{
			//DebugPrintf( "Created Delegate wrapping 0x%x!", mWrapper );
		}

		/**
		 * Templated constructor allowing the creation of a Delegate that wraps any valid function or lambda type.
		 */
		template< class Callee, typename Method >
		Delegate( Callee* callee, Method method ) :
			mWrapper( new MethodWrapper< Callee, Method >( callee, method ) )
		{
			assertion( callee, "Cannot create Delegate wrapping object method without a valid object!" );
			//DebugPrintf( "Created Delegate wrapping 0x%x!", mWrapper );
		}

		/**
		 * Basic destructor.
		 */
		~Delegate()
		{
			Clear();
		}

		/**
		 * Clears the internal function pointer (i.e. sets it to null).
		 */
		void Clear()
		{
			if( mWrapper )
			{
				// Delete the internal function wrapper.
				//DebugPrintf( "Deleting wrapper 0x%x...", mWrapper );
				delete mWrapper;
			}

			// Make the internal function pointer null.
			mWrapper = nullptr;
		}

		/**
		 * Assignment operator that clones another Delegate.
		 */
		void operator=( const Delegate< ReturnType, ParameterTypes... >& other )
		{
			if( &other != this )
			{
				// Clear the function pointer.
				Clear();

				if( other.IsValid() )
				{
					// If the other Delegate wraps a valid function, clone the internal function wrapper of the other object.
					mWrapper = other.mWrapper->Clone();
				}
			}

			//DebugPrintf( "Cloned Delegate wrapping 0x%x! (Clone wraps 0x%x.)", other.mWrapper, mWrapper );
		}

		bool operator==( const Delegate& other ) const
		{
			// Return true if both Delegates point to the same internal pointer.
			return ( *other.mWrapper == *mWrapper );
		}

		/**
		 * Returns true if the Delegate wraps a valid function or lambda (i.e. the function pointer is not null).
		 */
		bool IsValid() const
		{
			return ( mWrapper != nullptr );
		}

		/**
		 * Invokes the internal function or lambda.
		 */
		ReturnType Invoke( ParameterTypes... parameters ) const
		{
			assertion( IsValid(), "Cannot call Delegate because the internal function pointer is invalid!" );
			//DebugPrintf( "Invoking Delegate..." );
			return mWrapper->Invoke( parameters... );
		}

	private:
		Callable* mWrapper;
	};


	template< typename... ParameterTypes >
	class Event
	{
	public:
		typedef Delegate< void, ParameterTypes... > DelegateType;

		Event() { }
		~Event() { }

		template< typename... Parameters >
		void AddCallback( const Parameters&... parameters )
		{
			DelegateType delegate( parameters... );
			assertion( delegate.IsValid(), "Cannot register an invalid (null) Delegate as an Event callback!" );

			if( !HasCallback( delegate ) )
			{
				// If the Delegate wasn't already in the invocation list, add it.
				mCallbacks.push_back( delegate );
			}
			else
			{
				WarnFail( "Could not add callback to Event because the requested Delegate was already registered!" );
			}
		}

		template< typename... Parameters >
		void RemoveCallback( const Parameters&... parameters )
		{
			// Find the existing Delegate (if any).
			DelegateType delegate( parameters... );
			auto it = FindCallback( delegate );

			if( it != mCallbacks.end() )
			{
				// If the Delegate was found in the callback list, remove it.
				mCallbacks.erase( it );
			}
			else
			{
				WarnFail( "Could not remove callback from Event because the requested Delegate was not registered!" );
			}
		}

		void RemoveAllCallbacks()
		{
			mCallbacks.clear();
		}

		template< typename... Parameters >
		bool HasCallback( const Parameters&... parameters ) const
		{
			DelegateType delegate( parameters... );
			auto it = FindCallback( delegate );
			return ( it != mCallbacks.end() );
		}

		void Invoke( ParameterTypes... parameters ) const
		{
			// Make a copy of the Delegates to invoke.
			// TODO: Don't copy the list of Delegates.
			std::vector< DelegateType > callbacks = mCallbacks;

			for( auto it = callbacks.begin(); it != callbacks.end(); ++it )
			{
				// Invoke each callback in the list.
				it->Invoke( parameters... );
			}
		}

	private:
		typename std::vector< DelegateType >::iterator FindCallback( const DelegateType& delegate )
		{
			auto it = mCallbacks.begin();

			for( ; it != mCallbacks.end(); ++it )
			{
				if( *it == (DelegateType) delegate )
				{
					// If the Delegate was found, return an iterator to it.
					break;
				}
			}

			return it;
		}

		typename std::vector< DelegateType >::const_iterator FindCallback( const DelegateType& delegate ) const
		{
			auto it = mCallbacks.begin();

			for( ; it != mCallbacks.end(); ++it )
			{
				if( *it == delegate )
				{
					// If the Delegate was found, return an iterator to it.
					break;
				}
			}

			return it;
		}

		std::vector< DelegateType > mCallbacks;
	};
}
