#ifndef DP_CALLBACKS_H
#define DP_CALLBACKS_H

namespace droidpad {
	class DroidPadCallbacks
	{
		public:
			virtual ~DroidPadCallbacks();
			virtual void dpInitComplete() = 0;
	};
};

#endif
