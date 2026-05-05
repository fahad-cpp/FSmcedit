#ifndef OPTIONS_H
#define OPTIONS_H
#include <string>
struct FSmceditOptions{
    std::string worldPath;
    std::string structPath;
    bool ok=true;
};
class Options{
    public:
    static FSmceditOptions parse(int argc,char* argv[]);
};
#endif