#pragma once

namespace mage
{
	class OnlineGameRequest
	{
	public:
		enum Status
		{
			STATUS_NOT_STARTED,
			STATUS_PENDING,
			STATUS_CANCELLED,
			STATUS_COMPLETE,
			STATUS_ERROR
		};

		virtual ~OnlineGameRequest();

		void start();
		void cancel();

	protected:
		OnlineGameRequest();

		void error();

		virtual void onStart() = 0;
		virtual void onCancel() { }
		virtual void onError() { }

		int mID;
		Status mStatus;

		friend class OnlineGameService;
	};
}
