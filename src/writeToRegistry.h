
#include <string>
namespace writeToRegistry {
    static void writeToRegistry(const std::string& fileExtension,const std::string& fileName,const std::string& iconName) {
        //writes to registry using cmd commands
        system(std::string(R"(REG ADD HKLM\SOFTWARE\Classes\)"+ fileExtension +"\\ShellNew /v FileName /t REG_SZ /f /d \"" + fileName + "\"").c_str());
        if (!iconName.empty()) {
            system(std::string(R"(REG ADD HKLM\SOFTWARE\Classes\)" + fileExtension + R"(\ShellNew /v IconPath /t REG_SZ /f /d "%windir%\ShellNew\)" + iconName + "\"").c_str());
        }
    }
}

