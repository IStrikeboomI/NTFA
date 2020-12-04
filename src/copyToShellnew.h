#include <windows.h>
#include <string>
namespace copyToShellnew{

    static bool copyToShellnew(const LPSTR file,const LPSTR icon,const bool overwrite) {
        //Checks if shellnew folder exists and if it doesn't then it creates it
        DWORD shellnewFolder = GetFileAttributes(std::string(std::getenv("windir")).append("/ShellNew").c_str());
        if (shellnewFolder == INVALID_FILE_ATTRIBUTES) {
            CreateDirectory(std::string(std::getenv("windir")).append("/ShellNew").c_str(), nullptr);
        }
        //Checks if @param file directory exists
        if (file == nullptr || GetFileAttributes(file) == INVALID_FILE_ATTRIBUTES) {
            return false;
        }
        //copies file
        //gets string of just the name of the file
        std::string fileName = std::string(file).substr(std::string(file).find_last_of('\\') + 1);
        //Gets %windir% then attaches ShellNew then attaches the File Name (@param false overwrites files while true doesn't)
        CopyFile(file, std::string(std::getenv("windir")).append("/ShellNew/").append(fileName).c_str(), !overwrite);
        if (icon != nullptr && GetFileAttributes(icon) != INVALID_FILE_ATTRIBUTES) {
            std::string iconName = std::string(icon).substr(std::string(icon).find_last_of('\\') + 1);
            CopyFile(icon, std::string(std::getenv("windir")).append("/ShellNew/").append(iconName).c_str(), !overwrite);
        }
        return true;
    }
}
