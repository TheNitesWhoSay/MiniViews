#include "RegistryTypes.h"
#include "../CrossCutLib/SimpleIcu.h"

RegistryKey::RegistryKey(HKEY key, const std::string & subKey) : keyExists(false), key(key), subKey(subKey)
{

}

bool RegistryKey::KeyExists() const
{
    if ( !keyExists )
    {
        HKEY hKey = NULL;
        if ( RegOpenKeyEx(HKEY_CURRENT_USER, icux::toFilestring(subKey).c_str(), 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS )
        {
            RegCloseKey(hKey);
            return true;
        }
    }
    return keyExists;
}

bool RegistryKey::CreateKey()
{
    HKEY hKey = NULL;
    if ( RegCreateKeyEx(key, icux::toFilestring(subKey).c_str(), 0, NULL, REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS, NULL, &hKey, NULL) == ERROR_SUCCESS )
    {
        keyExists = true;
        RegCloseKey(hKey);
    }
    return keyExists;
}

bool RegistryKey::Clear()
{
    LONG errorCode = ::RegDeleteKeyEx(HKEY_CURRENT_USER, icux::toFilestring(subKey).c_str(), NULL, 0);
    if ( errorCode == ERROR_SUCCESS || errorCode == ERROR_FILE_NOT_FOUND )
        keyExists = false;

    return errorCode == ERROR_SUCCESS || errorCode == ERROR_FILE_NOT_FOUND;
}

HKEY RegistryKey::GetKey() const
{
    return key;
}

const std::string & RegistryKey::GetSubKey() const
{
    return subKey;
}

RegistryValue::RegistryValue(HKEY key, const std::string & subKey, const std::string & valueName)
    : isAssigned(false), key(key), subKey(subKey), valueName(valueName)
{

}

RegistryValue::RegistryValue(const RegistryKey &key, const std::string & valueName)
    : isAssigned(false), key(key.GetKey()), subKey(key.GetSubKey()), valueName(valueName)
{

}

bool RegistryValue::Clear()
{
    LONG errorCode = ::RegDeleteKeyValue(key, icux::toFilestring(subKey).c_str(), icux::toFilestring(valueName).c_str());
    if ( errorCode == ERROR_SUCCESS || errorCode == ERROR_FILE_NOT_FOUND )
        isAssigned = false;

    return errorCode == ERROR_SUCCESS || errorCode == ERROR_FILE_NOT_FOUND;
}

bool RegistryValue::GetBool(bool &outValue)
{
    DWORD storedValue = 0;
    if ( GetDword(storedValue) )
    {
        outValue = (storedValue != 0);
        return true;
    }
    return false;
}

bool RegistryValue::CreateOrSetBool(const bool value)
{
    return CreateOrSetDword(value ? 1 : 0);
}

bool RegistryValue::GetDword(DWORD &outValue)
{
    HKEY hKey = NULL;
    LONG errorCode = RegOpenKeyEx(key, icux::toFilestring(subKey).c_str(), 0, KEY_QUERY_VALUE, &hKey);
    if ( errorCode == ERROR_SUCCESS )
    {
        DWORD dwordSize = sizeof(DWORD);
        errorCode = RegQueryValueEx(hKey, icux::toFilestring(valueName).c_str(), NULL, NULL, (LPBYTE)&outValue, &dwordSize);
        RegCloseKey(hKey);
        if ( errorCode == ERROR_SUCCESS )
            return true;
    }
    SetLastError(errorCode);
    return false;
}

bool RegistryValue::CreateOrSetDword(const DWORD value)
{
    HKEY hKey = NULL;
    LONG errorCode = RegCreateKeyEx(key, icux::toFilestring(subKey).c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
    if ( errorCode == ERROR_SUCCESS )
    {
        errorCode = RegSetKeyValue(hKey, NULL, icux::toFilestring(valueName).c_str(), REG_DWORD, (LPCVOID)&value, sizeof(DWORD));
        RegCloseKey(hKey);
        if ( errorCode == ERROR_SUCCESS )
            return true;
    }
    SetLastError(errorCode);
    return false;
}

Registryu32::Registryu32(HKEY key, const std::string & subKey, const std::string & name, bool defaultValue)
    : RegistryValue(key, subKey, name), cachedValue(defaultValue)
{

}

Registryu32::Registryu32(const RegistryKey &key, const std::string & name, u32 defaultValue)
    : RegistryValue(key, name), cachedValue(defaultValue)
{

}

u32 Registryu32::Get()
{
    DWORD regValue = 0;
    if ( !RegistryValue::isAssigned && RegistryValue::GetDword(regValue) )
    {
        cachedValue = (u32)regValue;
        RegistryValue::isAssigned = true;
    }

    return cachedValue;
}

bool Registryu32::Set(u32 newValue)
{
    cachedValue = newValue;
    RegistryValue::isAssigned = RegistryValue::CreateOrSetDword((DWORD)newValue);
    return RegistryValue::isAssigned;
}

RegistryBool::RegistryBool(HKEY key, const std::string & subKey, const std::string & name, bool defaultValue) :
    RegistryValue(key, subKey, name), cachedValue(defaultValue)
{

}

RegistryBool::RegistryBool(const RegistryKey &key, const std::string & name, bool defaultValue) :
    RegistryValue(key, name), cachedValue(defaultValue)
{

}

bool RegistryBool::Get()
{
    if ( !RegistryValue::isAssigned && RegistryValue::GetBool(cachedValue) )
        RegistryValue::isAssigned = true;

    return cachedValue;
}

bool RegistryBool::Set(bool newValue)
{
    cachedValue = newValue;
    RegistryValue::isAssigned = RegistryValue::CreateOrSetBool(newValue);
    return RegistryValue::isAssigned;
}

bool RegistryBool::Toggle()
{
    cachedValue = !cachedValue;
    RegistryValue::isAssigned = RegistryValue::CreateOrSetBool(cachedValue);
    return RegistryValue::isAssigned;
}
