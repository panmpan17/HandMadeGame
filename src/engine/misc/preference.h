#include "../../utils/player_preference.h"





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

private:
    static PlayerPreference& getPlayerPreferenceInstance()
    {
        static PlayerPreference instance("player_preferences.cfg");
        return instance;
    }
};
