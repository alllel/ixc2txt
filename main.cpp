#include <algorithm>
#include <charconv>
#include <complex>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <cstring>
#include <vector>

#ifdef __MINGW32__
#include <windows.h>
#define MSG(what) {\
  std::stringstream msgs; \
  msgs << what ;\
  auto msg=msgs.str();\
  std::cerr << msg << std::endl; \
  MessageBox(NULL, msg.c_str(), "ixc2txt: error", MB_OK);\
  }
#else
#define MSG(what) (std::cerr << what << std::endl);
#endif

int main(int argc, char** argv)
{
  std::filesystem::path self{argv[0]};
  if (argc != 2 && argc != 3) {
    MSG("Usage: " << self.filename().string() << " ixc-file [n]" << std::endl <<
      "   n - optional parameter, default=1, write every n-th point");
    return 1;
  }
  int N = 1;
  if (argc == 3) {
    std::string_view arg_n{argv[2]};
    auto res = std::from_chars(arg_n.begin(), arg_n.end(), N);
    if (res.ec != std::errc{} || res.ptr != arg_n.end() || N <= 0) {
      MSG("Warning: wrong number " << argv[2]);
      return 1;
    }
  }

  std::filesystem::path ixc = argv[1];
  if (!ixc.has_extension()) {
    ixc.replace_extension(".ixc");
  }
  auto dat = ixc;
  dat.replace_extension(".dat");

  std::ifstream IXC{ixc};
  if (!IXC.is_open()) {
    MSG("Can't open file " << ixc);
    return 1;
  }
  std::ifstream DAT{dat, std::ios::binary};
  if (!DAT.is_open()) {
    MSG("Can't open file " << dat);
    return 1;
  }

  auto dir = ixc;
  dir.replace_extension("");
  if (std::filesystem::exists(dir)) {
    MSG("Directory " << dir << " already exists");
    return 1;
  }

  if (!std::filesystem::create_directory(dir)) {
    MSG("Can't create directory " << dir);
    return 1;
  }

  std::ofstream readme{dir / "readme.txt"};
  readme << "CH\tunit\tline\tR\tNpoints\n";

  std::string line;

  while (!IXC.eof()) {
    std::getline(IXC, line);
    if (line.empty())break;
    char ChNum[6];
    char ID[16];
    char unit[4];
    unit[3] = 0;
    ID[15] = 0;
    double dV, V0;
    unsigned short h, min, s, d, m, y;
    int c = sscanf(line.c_str(),
                   R"(%*1d:%[0-9A-Z#]\%16c\%lg\%lg\%3c\Time %hu.%hu.%hu \Date %hu:%hu:%hu)",
                   ChNum,
                   ID,
                   &dV,
                   &V0,
                   unit,
                   &h, &min, &s,
                   &d, &m, &y);
    if (c != 11) {
      MSG("Can't parse line: " << line);
      return 1;
    }

    if (IXC.eof()) {
      MSG("Premature end of file");
      return 1;
    }
    std::getline(IXC, line);

    long Np;
    double rate, Tstart;
    c = sscanf(line.c_str(), R"(\%lu\%lE\%lE)", &Np, &rate, &Tstart);
    if (c != 3) {
      MSG("Can't parse line: " << line);
      return 1;
    }

    int A;
    double R;
    char* p = std::strchr(ID, 'L');
    if (p)A = atoi(p + 1);
    else A = 0;
    p = std::strchr(ID, 'R');
    if (p)R = atof(p + 1);
    else R = 0;
    readme << ChNum << '\t' << unit << '\t' << A << '\t' << R << '\t' << Np << std::endl;

    std::vector<short> data(Np, 0);
    DAT.read(reinterpret_cast<char*>(data.data()), sizeof(data[0]) * Np);
    auto ch_fname = dir / ChNum;
    ch_fname.replace_extension(".txt");
    std::ofstream CHN{ch_fname};
    if (!CHN.is_open()) {
      MSG("Can't create file: " << ch_fname);
      return 1;
    }
    auto Tend = Tstart + rate * Np;
    auto tscale = std::max(std::fabs(Tend), std::fabs(Tstart));
    auto tdigits = static_cast<int>(std::ceil(std::log10(tscale / rate))) + 1;
    auto dmax = *std::ranges::max_element(data);
    auto Vend = V0 + dmax * dV;
    auto vscale = std::max(std::fabs(V0), std::fabs(Vend));
    auto vdigits = static_cast<int>(std::ceil(std::log10(vscale / dV))) + 1;

    CHN.precision(std::max({tdigits, vdigits, 6}));

    for (size_t i = 0; i < Np; i += N) {
      CHN << Tstart + rate * i << " " << V0 + data[i] * dV << '\n';
    }
  }

  return 0;
}
