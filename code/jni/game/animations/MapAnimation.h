#pragma once

namespace mage
{
	class MapView;
	class UnitSprite;


	class MapAnimation
	{
	protected:
		MapAnimation();
		virtual ~MapAnimation();

	public:
		enum State
		{
			STATE_NOT_STARTED,
			STATE_PLAYING,
			STATE_FINISHED
		};

		typedef Delegate< void > OnFinishedDelegate;

		State GetState() const;
		bool IsNotStarted() const;
		bool IsPlaying() const;
		bool IsFinished() const;

		float GetElapsedTime() const;
		MapView* GetMapView() const;

	protected:
		void Start( MapView* mapView );
		void Finish();

		void Update( float elapsedTime );

		virtual void OnStart() = 0;
		virtual void OnUpdate( float elapsedTime ) = 0;
		virtual void OnFinish() = 0;

	private:
		State mState;
		float mTimer;
		MapView* mMapView;

	public:
		Event<> OnFinished;

		friend class MapView;
	};


	class UnitMapAnimation : public MapAnimation
	{
	protected:
		UnitMapAnimation( UnitSprite* unitSprite );
		virtual ~UnitMapAnimation();

	public:
		UnitSprite* GetUnitSprite() const;

	private:
		UnitSprite* mUnitSprite;
	};
}
