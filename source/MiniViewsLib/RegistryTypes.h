#ifndef REGISTRYTYPES_H
#define REGISTRYTYPES_H
#include <string>
#include <Windows.h>
#include <stdint.h>
using u32 = uint32_t;

class RegistryKey
{
    public:
        RegistryKey(HKEY key, const char* subKey);
        bool KeyExists() const; // Checks whether this key exists in the registry
        bool CreateKey(); // Creates this key in the registry
        bool Clear(); // Deletes this key from the registry if it exists

        HKEY GetKey() const;
        std::string GetSubKey() const;

    protected:
        bool keyExists; // True if the key has been confirmed to be in the registry
        HKEY key; // The key this entry belongs to (i.e. HKEY_CURRENT_USER)
        std::string subKey; // The path (including the name of this key) of this key

    private:
        RegistryKey(); // Disallow ctor
};

class RegistryValue
{
    public:
        RegistryValue(HKEY key, const char* subKey, const char* valueName);
        RegistryValue(const RegistryKey &key, const char* valueName);
        virtual bool Clear(); // Deletes the value from the registry if it exists and clears any cached data

    protected:
        bool GetBool(bool &outValue); // outValue is unchanged if the function fails
        bool CreateOrSetBool(const bool value);

        bool GetDword(DWORD &outValue); // outValue is unchanged if the function fails
        bool CreateOrSetDword(const DWORD value);

        bool isAssigned; // True if a value has been read from the registry or set by client code
        HKEY key; // The key this entry belongs to (i.e. HKEY_CURRENT_USER)
        std::string subKey; // The subKey this entry belongs to
        std::string valueName; // The name of this specific value

    private:
        RegistryValue(); // Disallow ctor
};

class Registryu32 : public RegistryValue
{
    public:
        /** Constructs a Registryu32 that is stored in the given key
            (i.e. HKEY_CURRENT_USER, see RegCreateKeyEx for details on the key param),
            in the given subKey, with the given name, and a defaultValue that is used
            when no existing registry value can be found/retrieved */
        Registryu32(HKEY key, const char* subKey, const char* name, bool defaultValue);
        Registryu32(const RegistryKey &key, const char* name, u32 defaultValue);

        /** Gets the u32 that this class stores/fetches/caches,
        returns the cached value if the class has previously retrived or set
        the value, the value in the registry if it has not, and the defaultValue
        if the value could not be retrived from the registry */
        u32 Get();

        /** Sets the u32 value this class stores/fetches/caches,
        returns whether the value was sucessfully adjusted, adjusts
        the cached value even if the registry change fails */
        bool Set(u32 newValue);

    private:
        u32 cachedValue;
        Registryu32(); // Disallow ctor
};

class RegistryBool : public RegistryValue
{
    public:
        /** Constructs a RegistryBool that is stored in the given key
        (i.e. HKEY_CURRENT_USER, see RegCreateKeyEx for details on the key param),
        in the given subKey, with the given name, and a defaultValue that is used
        when no existing registry value can be found/retrieved */
        RegistryBool(HKEY key, const char* subKey, const char* name, bool defaultValue);
        RegistryBool(const RegistryKey &key, const char* name, bool defaultValue);

        /** Gets the boolean value this class stores/fetches/caches,
        returns the cached value if the class has previously retrived or set
        the value, the value in the registry if it has not, and the defaultValue
        if the value could not be retrived from the registry */
        bool Get();

        /** Sets the boolean value this class stores/fetches/caches,
        returns whether the value was sucessfully adjusted, adjusts
        the cached value even if the registry change fails */
        bool Set(bool newValue);

        /** Toggles the boolean value this class stores/fetches/caches,
        returns whether the value was successfully adjusted, adjusts
        the cached value even if the registry change fails */
        bool Toggle();

    private:
        bool cachedValue;
        RegistryBool(); // Disallow ctor
};



#endif