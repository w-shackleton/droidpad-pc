#ifndef DP_LOG_H
#define DP_LOG_H

#include <wx/log.h>

#define LOGV(_m) wxLogVerbose(wxT(_m))
#define LOGM(_m) wxLogMessage(wxT(_m))
#define LOGW(_m) wxLogWarning(wxT(_m))
#define LOGE(_m) wxLogError  (wxT(_m))

#define LOGVwx(_m) wxLogVerbose(_m)
#define LOGMwx(_m) wxLogMessage(_m)
#define LOGWwx(_m) wxLogWarning(_m)
#define LOGEwx(_m) wxLogError  (_m)

#endif
