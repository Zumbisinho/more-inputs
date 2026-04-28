#include "keycodeToString.hpp"
#include "Geode/cocos/robtop/keyboard_dispatcher/CCKeyboardDelegate.h"

using namespace geode::prelude;
// ! Boring shit do dooooooooooooooooooo
std::string keyToString(int key) {

    
    if (key >= KEY_A && key <= KEY_Z)
        return std::string(1, 'a' + (key - KEY_A));

    
    if (key >= KEY_Zero && key <= KEY_Nine)
        return std::string(1, '0' + (key - KEY_Zero));

    
    if (key >= KEY_NumPad0 && key <= KEY_NumPad9)
        return "num " + std::to_string(key - KEY_NumPad0);

    switch (key) {

        
        case KEY_Space: return "space";
        case KEY_Enter: return "enter";
        case KEY_NumEnter: return "num enter";
        case KEY_Escape: return "esc";
        case KEY_Tab: return "tab";
        case KEY_Backspace: return "backspace";

        
        case KEY_Shift: return "shift";
        case KEY_Control: return "ctrl";
        case 164: return "alt";
        case KEY_CapsLock: return "caps lock";

        
        case KEY_Left: return "left";
        case KEY_Right: return "right";
        case KEY_Up: return "up";
        case KEY_Down: return "down";

        
        case KEY_Insert: return "insert";
        case KEY_Delete: return "delete";
        case KEY_Home: return "home";
        case KEY_End: return "end";
        case KEY_PageUp: return "pg up";
        case KEY_PageDown: return "pg down";
        
        case KEY_F1: return "f1";
        case KEY_F2: return "f2";
        case KEY_F3: return "f3";
        case KEY_F4: return "f4";
        case KEY_F5: return "f5";
        case KEY_F6: return "f6";
        case KEY_F7: return "f7";
        case KEY_F8: return "f8";
        case KEY_F9: return "f9";
        case KEY_F10: return "f10";
        case KEY_F11: return "f11";
        case KEY_F12: return "f12";
        case KEY_F13: return "f13";
        case KEY_F14: return "f14";
        case KEY_F15: return "f15";

        
        case KEY_Multiply: return "num *";
        case KEY_Add: return "num +";
        case KEY_Subtract: return "num -";
        case KEY_Decimal: return "num .";
        case KEY_Divide: return "num /";
        
        // For some reason some keys doesnt work

        case 4097: return "=";
        case KEY_OEMMinus: return "-";
        case KEY_OEMComma: return ",";
        case KEY_OEMPeriod: return ".";
        case 4103: return ";";
        case KEY_OEM2: return "/";
        case 4098: return "`";
        case 4099: return "[";
        case 4102: return "~";
        case 4106: return "\\";
        case 4100: return "]";
        

        
        case KEY_PrintScreen: return "print screen";
        case KEY_ScrollLock: return "scroll lock";
        case KEY_Pause: return "pause";
        // remaing
        case 4096: return "'";

        case KEY_LeftShift: return "shift"; 
        case 93 :return "menu";
        case KEY_LeftControl: return "control"; 
        case KEY_LeftWindowsKey: return "win";

        case KEY_RightShift: return "right shift";
        case KEY_RightControl: return "right control"; // some reason altGR doesnt detect, it gets translated to this
        case KEY_RightWindowsKey: return "right win";
        case KEY_Numlock: return  "num lock";
        
        
        
        default:
            return "unknown (" + std::to_string(key) + ")";
    }
}