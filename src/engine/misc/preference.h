#include "../../utils/player_preference.h"

#include "../core/math/vector.h"


class Preference
{
public:
    inline static bool getEnableIMGUI() { return getPlayerPreferenceInstance().getBool("EnableIMGUI", true); }
    inline static void setEnableIMGUI(bool bEnable) { getPlayerPreferenceInstance().setBool("EnableIMGUI", bEnable); }

    inline static bool getEnablePostProcess() { return getPlayerPreferenceInstance().getBool("EnablePostProcess", true); }
    inline static void setEnablePostProcess(bool bEnable) { getPlayerPreferenceInstance().setBool("EnablePostProcess", bEnable); }

    inline static void savePreferences() { getPlayerPreferenceInstance().writeToFile(); }

    inline static int getWindowWidth() { return getPlayerPreferenceInstance().getInt("WindowWidth", 1280); }
    inline static void setWindowWidth(int width) { getPlayerPreferenceInstance().setInt("WindowWidth", width); }

    inline static int getWindowHeight() { return getPlayerPreferenceInstance().getInt("WindowHeight", 720); }
    inline static void setWindowHeight(int height) { getPlayerPreferenceInstance().setInt("WindowHeight", height); }

    inline static int getWindowPositionX() { return getPlayerPreferenceInstance().getInt("WindowPositionX", 0); }
    inline static void setWindowPositionX(int x) { getPlayerPreferenceInstance().setInt("WindowPositionX", x); }

    inline static int getWindowPositionY() { return getPlayerPreferenceInstance().getInt("WindowPositionY", 0); }
    inline static void setWindowPositionY(int y) { getPlayerPreferenceInstance().setInt("WindowPositionY", y); }

    static PlayerPreference& getPlayerPreferenceInstance()
    {
        static PlayerPreference instance("player_preferences.cfg");
        return instance;
    }

private:
    static void setVector2(const std::string& strKey, const Vector2& vec)
    {
        getPlayerPreferenceInstance().setInt(strKey + "_x", static_cast<int>(vec.x));
        getPlayerPreferenceInstance().setInt(strKey + "_y", static_cast<int>(vec.y));
    }
    static Vector2 getVector2(const std::string& strKey, const Vector2& defaultValue)
    {
        int x = getPlayerPreferenceInstance().getInt(strKey + "_x", static_cast<int>(defaultValue.x));
        int y = getPlayerPreferenceInstance().getInt(strKey + "_y", static_cast<int>(defaultValue.y));
        return Vector2(static_cast<float>(x), static_cast<float>(y));
    }

    static void setVector3(const std::string& strKey, const Vector3& vec)
    {
        getPlayerPreferenceInstance().setInt(strKey + "_x", static_cast<int>(vec.x));
        getPlayerPreferenceInstance().setInt(strKey + "_y", static_cast<int>(vec.y));
        getPlayerPreferenceInstance().setInt(strKey + "_z", static_cast<int>(vec.z));
    }
    static Vector3 getVector3(const std::string& strKey, const Vector3& defaultValue)
    {
        int x = getPlayerPreferenceInstance().getInt(strKey + "_x", static_cast<int>(defaultValue.x));
        int y = getPlayerPreferenceInstance().getInt(strKey + "_y", static_cast<int>(defaultValue.y));
        int z = getPlayerPreferenceInstance().getInt(strKey + "_z", static_cast<int>(defaultValue.z));
        return Vector3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
    }
};
