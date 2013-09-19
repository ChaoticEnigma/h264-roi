#include "zparcel.h"

namespace LibChaos {

ZParcel::ZParcel(ZPath file) : parcel(file), ready(false){
    index = readIndex(parcel);
    ready = true;
}

bool ZParcel::makeParcel(ZPath out, AssocArray<ZPath> in){
    ZPath tmppath = out;
    tmppath.last() << ".tmp";
    LOG(tmppath);
    //ZFile tmpfile(WRITE, tmppath.str());
    //if(!tmpfile.isOpen())
    ZFile::remove(tmppath);
    std::fstream tmpfile;
    tmpfile.open(tmppath.str().cc(), std::fstream::out | std::fstream::binary | std::fstream::app);
    LOG(tmpfile.is_open() << " " << tmpfile.good() << tmpfile.bad() << tmpfile.fail() << tmpfile.eof() << tmpfile.rdstate());
    if(!tmpfile.is_open())
        return false;

    AsArZ inx;
    unsigned long currpos = 0;
    for(unsigned i = 0; i < in.size(); ++i){
        bool ok;
        ZString tmp = ZFile::readFile(in[i], ok);
        if(!ok)
            return false;
        //tmpfile.write(tmp.cc(), tmp.size());
        //tmpfile.flush();
        tmpfile << tmp.cc() << std::flush;
        ZString sect = ZString::ItoS(currpos);
        currpos += tmp.size();
        sect << "," << tmp.size();
        ZString last = in[i].last();
        inx[last.str()] = sect;
    }
    tmpfile.close();
    std::fstream bffile;
    bffile.open(tmppath.str().cc(), std::fstream::in | std::fstream::binary);
    LOG(bffile.is_open() << " " << bffile.good() << bffile.bad() << bffile.fail() << bffile.eof() << bffile.rdstate());
    if(!bffile.is_open())
        return false;

    ZString head = head.toJSON(inx, false) + ";";
    ZFile::remove(out);
    std::ofstream outfile(out.str().cc(), std::fstream::binary | std::fstream::app);
    if(!outfile.is_open())
        return false;
    outfile.write(head.cc(), head.size());
    while(bffile.good()){
        char buff[4096];
        bffile.read(buff, 4096);
        ZString bf = buff;
        outfile.write(bf.cc(), bf.size());
    }
    bffile.close();
    outfile.close();
    ZFile::remove(tmppath);
    return true;
}

ZParcel::ParcelSectionList ZParcel::readIndex(ZPath file){
    std::ifstream in(file.str().cc());
    if(!in)
        return ZParcel::ParcelSectionList();
    in.seekg(0, std::ios::beg);

    ZString indexstr;
    char c = in.get();
    while(in.good()){
        if(c == ';')
            break;
        indexstr << c;
        c = in.get();
    }
    unsigned long offset = indexstr.size() + 1;
    if(!indexstr.validJSON())
        return ZParcel::ParcelSectionList();
    ParcelSectionList inxbuff;
    AsArZ tmpindex = indexstr.fromJSON();
    for(unsigned i = 0; i < tmpindex.size(); ++i){
        ZString tmp = tmpindex[i];
        AsArZ tmp2 = tmp.explode(',');
        if(tmp2.size() != 2)
            return ZParcel::ParcelSectionList();
        ParcelSection pcl(tmp2[0].tint() + offset, tmp2[1].tint());
        inxbuff[tmpindex.I(i)] = pcl;
    }

    in.close();
    return inxbuff;
}

ZParcel::ParcelSectionList ZParcel::getIndex(){
    return index;
}

ZString ZParcel::getSection(ParcelSection locat){
    if(ready){
        std::ifstream in(parcel.str().cc());
        if(!in)
            return ZString();
        in.seekg(locat.pos, std::ios::beg);
        std::string buffer;
        buffer.resize(locat.len);
        in.read(&buffer[0], locat.len);
        return ZString(buffer);
    }
    return ZString();
}

}
