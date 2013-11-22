/*
 * Author      : Matthew Johnson
 * Date        : 15/Sep/2013
 * Description :
 *   
 */
 
#pragma once

namespace mage
{

	class Uniform
	{
	public:
		Uniform( int location=Effect::INVALID_LOCATION );
		virtual ~Uniform();

		void SetLocation( int location )		{ mLocation = location; }

		virtual void Apply() = 0;

	protected:
		int mLocation;
	};




	class Uniform1i
		: public Uniform
	{
	public:
		Uniform1i( int location=Effect::INVALID_LOCATION );
		virtual ~Uniform1i();

		void Apply();

		void SetValue( int value );
		inline int GetValue() const		{ return mValue; }

	private:
		int mValue;
	};




	class Uniform1f
		: public Uniform
	{
	public:
		Uniform1f( int location=Effect::INVALID_LOCATION );
		virtual ~Uniform1f();
		
		void Apply();

		void SetValue( float value );
		inline float GetValue() const		{ return mValue; }

	private:
		float mValue;
	};




	class Uniform1fv
		: public Uniform
	{
	public:
		Uniform1fv( int location=Effect::INVALID_LOCATION );
		virtual ~Uniform1fv();

		void Apply();

		void SetValue( float* value, int count );
		inline float* GetValue() const		{ return mValue; }
		inline int GetCount() const			{ return mCount; }

	private:
		float* mValue;
		int mCount;
	};




	class Uniform2fv
		: public Uniform
	{
	public:
		Uniform2fv( int location=Effect::INVALID_LOCATION );
		virtual ~Uniform2fv();

		void Apply();

		void SetValue( Vec2f* value, int count );
		inline Vec2f* GetValue() const		{ return mValue; }
		inline int GetCount() const			{ return mCount; }

	private:
		Vec2f* mValue;
		int mCount;
	};

}