#ifndef PREFERENCES_H
#define PREFERENCES_H
#include "RegistryTypes.h"

class Preferences
{
    public:
        Preferences(const RegistryKey &miniViewsKey);

        RegistryBool ShowDialogOnStart;
        RegistryBool UseNotificationIcon;
        RegistryBool MinimizeToNotificationIcon;
        RegistryBool ConfirmExit;
        Registryu32 DefaultOpacity;
        RegistryBool DefaultMatchSourcePosition;
        RegistryBool DefaultMatchSourceSize;
        RegistryBool DefaultLockSizeRatio;
        RegistryBool DefaultHideWhenSourceOnTop;
        RegistryBool DefaultClickableWhenOpaque;

        bool IsFirstRun();
        bool SetNotFirstRun();

        bool ClearSavedPreferences();

    private:
        RegistryKey MiniViewsKey;
};



#endif