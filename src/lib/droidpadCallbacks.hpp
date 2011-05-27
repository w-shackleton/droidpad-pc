#ifndef DP_CALLBACKS_H
#define DP_CALLBACKS_H

namespace droidpad {
	class DroidPadCallbacks
	{
		public:
			virtual ~DroidPadCallbacks();
			virtual void dpInitComplete(bool complete) = 0;
			virtual void dpCloseComplete() = 0;
	};
};

#endif
