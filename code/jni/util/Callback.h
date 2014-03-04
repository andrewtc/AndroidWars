#pragma once


namespace mage
{
	/**
	 * Pure STL-style template magic wrapping an arbitrary function or lambda type
	 * (to avoid using std::function, which is not supported by stl-port on Android).
	 */
	template< typename ReturnType, typename... ParameterTypes >
	class Callback
	{
	private:
		/**
		 * Abstract base class that allows us to call an arbitrary function type and return the result.
		 */
		struct FunctionBase
		{
			virtual ReturnType Invoke( ParameterTypes... parameters ) const = 0;
			virtual FunctionBase* Clone() const = 0;
		};

		/**
		 * Template wrapper class that implements the invocation function for any lambda or function type.
		 */
		template< typename Function >
		struct FunctionWrapper : public FunctionBase
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
			virtual FunctionBase* Clone() const
			{
				return new FunctionWrapper< Function >( function );
			}

			Function function;
		};

		FunctionBase* mWrapper;

	public:
		/**
		 * Default constructor that wraps an invalid (i.e. null) function.
		 */
		Callback() :
			mWrapper( nullptr )
		{
			//DebugPrintf( "Created Callback wrapping 0x%x!", mWrapper );
		}

		/**
		 * Copy constructor that clones another Callback.
		 */
		Callback( const Callback< ReturnType, ParameterTypes... >& other ) :
			mWrapper( other.IsValid() ? other.mWrapper->Clone() : nullptr )
		{
			//DebugPrintf( "Cloned Callback wrapping 0x%x! (Clone wraps 0x%x.)", other.mWrapper, mWrapper );
		}

		/**
		 * Templated constructor allowing the creation of a Callback that wraps any valid function or lambda type.
		 */
		template< typename Function >
		Callback( Function function ) :
			mWrapper( new FunctionWrapper< Function >( function ) )
		{
			//DebugPrintf( "Created Callback wrapping 0x%x!", mWrapper );
		}

		/**
		 * Basic destructor.
		 */
		~Callback()
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
		 * Assignment operator that clones another Callback.
		 */
		void operator=( const Callback< ReturnType, ParameterTypes... >& other )
		{
			if( &other != this )
			{
				// Clear the function pointer.
				Clear();

				if( other.IsValid() )
				{
					// If the other callback wraps a valid function, clone the internal function wrapper of the other object.
					mWrapper = other.mWrapper->Clone();
				}
			}

			//DebugPrintf( "Cloned Callback wrapping 0x%x! (Clone wraps 0x%x.)", other.mWrapper, mWrapper );
		}

		/**
		 * Returns true if the Callback wraps a valid function or lambda (i.e. the function pointer is not null).
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
			assertion( IsValid(), "Cannot call Callback because the internal function pointer is invalid!" );
			//DebugPrintf( "Invoking callback..." );
			return mWrapper->Invoke( parameters... );
		}
	};
}
