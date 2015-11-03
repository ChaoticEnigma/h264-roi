#include "test.h"

int test_forward_iterator(ZConstSimplexIterator<ZString> *it, zu64 size){
    zu64 cf = 0;
    for(auto i = it; i->more(); i->advance()){
        LOG("f: " << i->get());
        ++cf;
    }
    TASSERT(cf == size);
    return 0;
}

int test_reverse_iterator(ZDuplexIterator<ZString> *it, zu64 size){
    zu64 cr = 0;
    for(auto i = it; i->less(); i->recede()){
        LOG("r: " << i->get());
        ++cr;
    }
    TASSERT(cr == size);
    return 0;
}

int test_duplex_iterator(ZDuplexIterator<ZString> *it, zu64 size){
    zu64 cd = 0;
    while(it->more()){
        LOG("f: " << it->get());
        it->advance();
        ++cd;
    }
    it->recede();
    it->recede();
    while(it->less()){
        LOG("r: " << it->get());
        it->recede();
        ++cd;
    }
    TASSERT(cd == size * 2 - 1);
    return 0;
}

int test_random_iterator(ZRandomIterator<ZString> *it, zu64 size){
    TASSERT(it->size() == size);
    for(zu64 i = 0; i < it->size(); ++i){
        LOG("i: " << it->at(i));
    }
    return 0;
}
