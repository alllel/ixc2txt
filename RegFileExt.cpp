//
// Created by sasha on 17.12.25.
//


#ifdef __MINGW32__
#include "RegKey.hpp"

bool RegisterFileExt()
{
  {
    RegKey key{R"(Software\Classes\.ixc)"};
    if (!key) return false;
    if (!key.SetDefStr("dxw32_file")) return false;
  }
  {
    RegKey key{R"(Software\Classes\dxw32_file)"};
    if (!key) return false;
    if (!key.SetDefStr("Dxw32 file")) return false;
  }
  {
    RegKey open{R"(Software\Classes\dxw32_file\shell\ixc2txt\command)"};
    if (!open) return false;
    char buf[MAX_PATH + 3];
    GetModuleFileName(nullptr, buf, MAX_PATH);
    std::strcat(buf, " %1");
    if (!open.SetDefStr(buf)) return false;
  }
  return true;
}
#else
bool RegisterFileExt() { return true; }
#endif
