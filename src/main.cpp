
#include <windows.h>
#include <iostream>
#include "writeToRegistry.h"
#include "copyToShellnew.h"
#include <cstdio>
#include <commdlg.h>

#define START_PROGRAM 5
#define CHOOSE_ICON 6
#define CHOOSE_FILE 7
#define CHANGE_CHECKBOX_STATE 8

const int HEIGHT = 350;
const int WIDTH = 500;

HWND fileText;
HWND iconText;
HWND doneText;
HWND chooseFileButton;
HWND chooseIconButton;
HWND startButton;
HWND overwriteCheckBox;
LPSTR openFileBoxIcon;
LPSTR openFileBoxFile;
std::string iconName;
std::string fileName;

LRESULT CALLBACK windowProcedure(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam);
void createButtons(const HWND& hwnd);
LPSTR openFileBox(const HWND& hwnd,const char* filter);
void changeText(const HWND& text, const HWND& button,const char* newText);
void changeFont(const HWND& hwnd);
void createText(const HWND& hwnd);
bool hasExtension(const LPSTR& file);
std::string getFileName(const LPSTR& directory);
std::string getExtension();
void reset();


int WINAPI WinMain(HINSTANCE hinstance,HINSTANCE hinstancePrev,LPSTR args,int nCmdShow) {
    WNDCLASSW wc = {0};
    wc.hbrBackground = (HBRUSH) COLOR_WINDOW;
    wc.hInstance = hinstance;
    wc.hCursor = LoadCursor(wc.hInstance,IDC_ARROW);
    wc.lpszClassName = L"newTabFileAdder";
    wc.lpfnWndProc = windowProcedure;
    wc.lpszMenuName = L"New Tab File Adder";
    //Adds icon in corner (117 is icon)
    wc.hIcon = (HICON) LoadImageW(wc.hInstance, MAKEINTRESOURCEW(117), IMAGE_ICON, 10, 0, LR_DEFAULTSIZE | LR_SHARED);

    if (!RegisterClassW(&wc)) {
        return -1;
    }

    //Gets screen to center window
    RECT screen;
    GetWindowRect(GetDesktopWindow(),&screen);

    //Creates Window
    CreateWindowW(wc.lpszClassName,wc.lpszMenuName, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE ,screen.right / 2 - HEIGHT / 2,screen.bottom / 2 - WIDTH / 2,HEIGHT,WIDTH, nullptr, nullptr,wc.hInstance, nullptr);

    MSG msg = {nullptr};

    while (GetMessageW(&msg, nullptr,0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}
LRESULT CALLBACK windowProcedure(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam) {
    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_CREATE:
            createButtons(hwnd);
            createText(hwnd);
            break;
        case WM_COMMAND:
            switch(wparam) {
                case CHANGE_CHECKBOX_STATE: {
                    if (IsDlgButtonChecked(hwnd, CHANGE_CHECKBOX_STATE)) {
                        CheckDlgButton(hwnd, CHANGE_CHECKBOX_STATE, BST_UNCHECKED);
                    } else {
                        CheckDlgButton(hwnd, CHANGE_CHECKBOX_STATE, BST_CHECKED);
                    }
                    break;
                }
                case CHOOSE_ICON: {
                    openFileBoxIcon = openFileBox(hwnd, "Icons (*.ico)\0*.ico\0");
                    if (openFileBoxIcon[0] != 0) {
                        iconName = getFileName(openFileBoxIcon);
                        changeText(iconText, chooseIconButton,std::string("Icon: " + iconName).c_str());
                    } else {
                        reset();
                    }
                    break;
                }
                case CHOOSE_FILE: {
                    openFileBoxFile = openFileBox(hwnd, "All Files (*.*)\0*.*\0");
                    if (openFileBoxFile[0] != 0) {
                        //checks if the text has a dot/extension
                        if (hasExtension(openFileBoxFile)) {
                            fileName = getFileName(openFileBoxFile);
                            changeText(fileText, chooseFileButton, std::string("File: " + fileName).c_str());
                        } else {
                            reset();
                            changeText(fileText,chooseFileButton, "File Has No Extension, Choose File Again");
                        }
                    } else {
                        reset();
                    }
                    break;
                }
                case START_PROGRAM: {
                    if (!copyToShellnew::copyToShellnew(openFileBoxFile, openFileBoxIcon,IsDlgButtonChecked(hwnd, CHANGE_CHECKBOX_STATE))) {
                        MessageBox(nullptr, "File doesn't exist (FAILED, TRY AGAIN)","ERROR",MB_ICONSTOP);
                        reset();
                    } else {
                        writeToRegistry::writeToRegistry(getExtension(),fileName,iconName);
                        changeText(doneText, startButton, "Successfully finished");
                    }
                    reset();
                    break;
                }
                default:break;
            }
            break;

        default:break;
    }
    return DefWindowProcW(hwnd,msg,wparam,lparam);
}

void createButtons(const HWND& hwnd) {
    //creates start button
    startButton = CreateWindowW(L"Button",L"Start",WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_TABSTOP,40,400,100,30,hwnd,(HMENU) START_PROGRAM, nullptr, nullptr);
    //creates choose file buttons
    chooseFileButton = CreateWindowW(L"Button",L"Choose File",WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_TABSTOP ,120,100,100,30,hwnd,(HMENU) CHOOSE_FILE, nullptr, nullptr);
    chooseIconButton = CreateWindowW(L"Button",L"Choose Icon",WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_TABSTOP,120,250,100,30,hwnd,(HMENU) CHOOSE_ICON, nullptr, nullptr);

    //creates checkbox
    overwriteCheckBox = CreateWindowW(L"Button",L"Overwrite Files if they exist?",WS_VISIBLE | WS_CHILD | BS_CHECKBOX,150,405,WIDTH,20,hwnd, (HMENU) CHANGE_CHECKBOX_STATE , nullptr,nullptr);
    CheckDlgButton(hwnd, CHANGE_CHECKBOX_STATE, BST_CHECKED);

    changeFont(startButton);
    changeFont(chooseIconButton);
    changeFont(chooseFileButton);
    changeFont(overwriteCheckBox);
}
void createText(const HWND& hwnd) {
    fileText = CreateWindowW(L"static",L"Choose File",WS_VISIBLE | WS_CHILD,120,80,WIDTH,20,hwnd, nullptr, nullptr,nullptr);
    iconText = CreateWindowW(L"static",L"Choose Icon (Optional)",WS_VISIBLE | WS_CHILD,120,230,WIDTH,20,hwnd, nullptr, nullptr,nullptr);
    doneText = CreateWindowW(L"static",L"",WS_VISIBLE | WS_CHILD,40,385,WIDTH,15,hwnd, nullptr, nullptr,nullptr);

    changeFont(fileText);
    changeFont(iconText);
    changeFont(doneText);
}
LPSTR openFileBox(const HWND& hwnd, const char* filter) {
    OPENFILENAME ofn = {};

    ZeroMemory(&ofn,sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = new char[MAX_PATH];
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    GetOpenFileName(&ofn);

    return ofn.lpstrFile;
}
void changeText(const HWND& text, const HWND& button, const char* newText) {
    //Changes Text
    SendMessage(text,WM_SETTEXT,0, (LPARAM)newText);
    //Positions text to button
    //Gets button rectangle
    RECT buttonRect = {0};
    GetClientRect(button,&buttonRect);
    MapWindowPoints(button,GetParent(button),(LPPOINT)&buttonRect,2);
    //Sets position to button
    SetWindowPos(text, nullptr, buttonRect.left,buttonRect.top - 20,0,0,SWP_NOSIZE);
}
void changeFont(const HWND& hwnd) {
    NONCLIENTMETRICS metrics = {};
    metrics.cbSize = sizeof(metrics);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, metrics.cbSize, &metrics, 0);
    HFONT font = CreateFontIndirect(&metrics.lfCaptionFont);
    SendMessage(hwnd,WM_SETFONT,(WPARAM)font,true);
}
bool hasExtension(const LPSTR& file) {
    return std::string(file).find('.') != std::string::npos;
}
std::string getFileName(const LPSTR& directory) {
    return std::string(directory).substr(std::string(directory).find_last_of('\\') + 1);
}
std::string getExtension() {
    return fileName.substr(fileName.find_last_of('.'));
}
void reset() {
    changeText(fileText,chooseFileButton,"Choose File");
    changeText(iconText,chooseIconButton,"Choose Icon (Optional)");
    openFileBoxFile = nullptr;
    openFileBoxIcon = nullptr;
    fileName = "";
    iconName = "";
}
