#include "Preferences.h"

Preferences::Preferences(const RegistryKey &miniViewsKey) :
    MiniViewsKey(miniViewsKey.GetKey(), miniViewsKey.GetSubKey().c_str()),
    ShowDialogOnStart(miniViewsKey, "ShowDialogOnProgramStart", true),
    UseNotificationIcon(miniViewsKey, "UseNotificationIcon", true),
    MinimizeToNotificationIcon(miniViewsKey, "MinimizeToNotificationIcon", true),
    ConfirmExit(miniViewsKey, "ConfirmExitByDialog", true),
    DefaultOpacity(miniViewsKey, "DefaultOpacity", 175),
    DefaultMatchSourcePosition(miniViewsKey, "DefaultMatchSourcePosition", false),
    DefaultMatchSourceSize(miniViewsKey, "DefaultMatchSourceSize", false),
    DefaultLockSizeRatio(miniViewsKey, "DefaultLockSizeRatio", true),
    DefaultHideWhenSourceOnTop(miniViewsKey, "DefaultHideWhenSourceOnTop", true),
    DefaultClickableWhenOpaque(miniViewsKey, "DefaultClickableWhenOpaque", true)
{

}

bool Preferences::IsFirstRun()
{
    return MiniViewsKey.KeyExists();
}

bool Preferences::SetNotFirstRun()
{
    return MiniViewsKey.CreateKey();
}

bool Preferences::ClearSavedPreferences()
{
    return ShowDialogOnStart.Clear() &&
        UseNotificationIcon.Clear() &&
        MinimizeToNotificationIcon.Clear() &&
        ConfirmExit.Clear() &&
        DefaultOpacity.Clear() &&
        DefaultMatchSourcePosition.Clear() &&
        DefaultMatchSourceSize.Clear() &&
        DefaultLockSizeRatio.Clear() &&
        DefaultHideWhenSourceOnTop.Clear() &&
        DefaultClickableWhenOpaque.Clear() &&
        MiniViewsKey.Clear();
}
