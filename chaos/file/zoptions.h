#ifndef ZOPTIONS_H
#define ZOPTIONS_H

#include "ztypes.h"
#include "zstring.h"
#include "zpath.h"
#include "zmap.h"
#include "zuid.h"

namespace LibChaos {

class ZOptions {
public:
    //! Construct empty options.
    ZOptions();
    //! Load options from file.
    ZOptions(ZPath file);

    ~ZOptions();

    //! Check if option is set.
    bool hasOption(ZUID uid);
    bool hasOption(ZString name);
    bool hasOption(int key);

    //! Get an option value, with a fallback value if the option is not set.
    ZString getOption(ZUID uid, ZString fallback);
    ZString getOption(ZString name, ZString fallback);
    ZString getOption(int key, ZString fallback);

    //! Set an option value.
    void setOption(ZUID uid, ZString value);
    void setOption(ZString name, ZString value);
    void setOption(int key, ZString value);

private:
    ZUID _makeUID(ZString name);

private:
    ZMap<ZString, ZUID> _names;
    ZMap<int, ZUID> _keys;
    ZMap<ZUID, ZString> _options;
};

}

#endif // ZOPTIONS_H
