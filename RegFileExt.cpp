//
// Created by sasha on 17.12.25.
//


#ifdef __MINGW32__
#include "RegKey.hpp"

bool RegisterFileExt() {
    char buf[MAX_PATH];
    GetModuleFileName(nullptr, buf, MAX_PATH);
    auto fn_len = std::strlen(buf); {
        RegKey key{R"(Software\Classes\.ixc)"};
        if (!key) return false;
        if (!key.SetDefStr("dxw32_file")) return false;
    } {
        RegKey key{R"(Software\Classes\dxw32_file)"};
        if (!key) return false;
        if (!key.SetDefStr("Dxw32 file")) return false;
    } {
        RegKey open{R"(Software\Classes\dxw32_file\shell\ixc2txt\command)"};
        if (!open) return false;
        std::strcpy(buf + fn_len, " %1");
        if (!open.SetDefStr(buf)) return false;
    }
    return true;
}
#else
bool RegisterFileExt() { return true; }
#endif
