#ifndef ZOPTIONS_H
#define ZOPTIONS_H

#include "ztypes.h"
#include "zstring.h"
#include "zpath.h"
#include "zmap.h"
#include "zuid.h"

namespace LibChaos {

static const char *ZOPTIONS_DEFAULT_NAMESPACE = "2c5e1b32-a1fd-11e6-a9b7-f832e4a20a6d";

class ZOptions {
private:
    struct Option;
public:
    //! Construct empty options, with optional non-default namespace UUID.
    ZOptions(ZUID name_space = ZUID(ZOPTIONS_DEFAULT_NAMESPACE));
    //! Load options from file.
    ZOptions(ZPath file);

    ~ZOptions();

    //! Check if option is set.
    bool hasOption(ZUID uid) const;
    bool hasOption(ZString name) const;
    bool hasOption(int key) const;

    //! Get an option value, with a fallback value if the option is not set.
    ZString getOption(ZUID uid, ZString fallback) const;
    ZString getOption(ZString name, ZString fallback) const;
    ZString getOption(int key, ZString fallback) const;

    //! Set an option value.
    void setOption(ZUID uid, ZString value);
    void setOption(ZString name, ZString value);
    void setOption(int key, ZString value);

    //! Unset an option.
    void unsetOption(ZUID uid);
    void unsetOption(ZString name);
    void unsetOption(int key);

    //! Register a name and a key to an option uid.
    ZUID registerOption(ZString name, int key, bool persist = true);

private:
    Option &_getOption(ZUID uid);

    ZUID _getUID(ZString name);
    ZUID _getUID(int key);

    ZUID _makeUID(ZString name) const;

private:
    struct Option {
        bool set;
        ZString value;
        bool persist;
    };

private:
    ZUID _namespace;
    ZMap<ZString, ZUID> _names;
    ZMap<int, ZUID> _keys;
    ZMap<ZUID, Option> _options;
};

}

#endif // ZOPTIONS_H
