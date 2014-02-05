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
			virtual ReturnType operator()( ParameterTypes... parameters ) = 0;
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

			FunctionWrapper( const FunctionWrapper< Function >& other ) :
				function( other.function )
			{ }

			virtual ReturnType operator()( ParameterTypes... parameters )
			{
				return function( parameters... );
			}

			/**
			 * Creates and returns a new wrapper of the same type around the same internal function or lambda.
			 */
			virtual FunctionBase* Clone() const
			{
				return new FunctionWrapper< Function >( *this );
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
		{ }

		/**
		 * Copy constructor that clones another Callback.
		 */
		Callback( const Callback& other ) :
			mWrapper( other.IsValid() ? other.mWrapper->Clone() : nullptr )
		{ }

		/**
		 * Templated constructor allowing the creation of a Callback that wraps any valid function or lambda type.
		 */
		template< typename Function >
		Callback( Function function ) :
			mWrapper( new FunctionWrapper< Function >( function ) )
		{ }

		/**
		 * Basic destructor.
		 */
		~Callback()
		{
			if( mWrapper )
			{
				// Delete the internal function wrapper.
				delete mWrapper;
			}
		}

		/**
		 * Assignment operator that clones another Callback.
		 */
		void operator=( const Callback& other )
		{
			// Clone the internal function wrapper of the other object.
			mWrapper = other.mWrapper->Clone();
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
		ReturnType operator()( ParameterTypes... parameters )
		{
			assertion( IsValid(), "Cannot call Callback because the internal function pointer is invalid!" );
			return ( *mWrapper )( parameters... );
		}
	};
}
