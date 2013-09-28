#include "zparcel.h"
#include <iterator>
#include <algorithm>
#include "ztypes.h"

namespace LibChaos {

ZParcel::ZParcel(ZPath file) : parcel(file), ready(false){
    index = readIndex(parcel);
    ready = true;
}

zu16 readzu16(std::istream& file){
    zu16 val;
    zu8 bytes[2];

    file.read( (char*)bytes, 2 );  // read 2 bytes from the file
    val = bytes[0] | (bytes[1] << 8);  // construct the 16-bit value from those bytes

    return val;
}
void writezu16(std::ostream& file, zu16 val){
    zu8 bytes[2];

    bytes[0] = (val) & 0xFF;  // low byte
    bytes[1] = (val >> 8) & 0xFF;  // high byte

    file.write( (char*)bytes, 2 );
}

zu32 readzu32(std::istream& file){
    zu32 val;
    zu8 bytes[4];

    file.read((char*)bytes, 4);  // read 2 bytes from the file
    val = bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);  // construct the 16-bit value from those bytes

    return val;
}
void writezu32(std::ostream& file, zu32 val){
    zu8 bytes[4];

    bytes[0] = (val) & 0xFF;  // low byte
    bytes[1] = (val >> 8) & 0xFF;  // high byte
    bytes[2] = (val >> 16) & 0xFF;  // low byte
    bytes[3] = (val >> 24) & 0xFF;  // high byte

    file.write((char*)bytes, 4);
}

std::string readstring(std::istream& file){
  zu32 len = readzu32(file);

  char* buffer = new char[len];
  file.read(buffer, len);

  std::string str(buffer, len);
  delete[] buffer;

  return str;
}
void writestring(std::ostream& file, std::string str){
  zu32 len = str.length();

  writezu32(file, len);
  file.write(str.c_str(), len);
}



bool ZParcel::makeParcel(ZPath out, ZAssoc<ZString, ZPath> in){
    ZPath tmppath = out;
    tmppath.last() << ".tmp";
    LOG(tmppath);
    //ZFile tmpfile(WRITE, tmppath.str());
    //if(!tmpfile.isOpen())
    ZFile::remove(tmppath);
    std::ofstream tmpfile(tmppath.str().cc(), std::ios::binary | std::ios::app);
    LOG(tmpfile.is_open() << " " << tmpfile.good() << tmpfile.bad() << tmpfile.fail() << tmpfile.eof() << tmpfile.rdstate());
    if(!tmpfile.is_open())
        return false;

    ZJSON inx;
    unsigned long currpos = 0;
    for(unsigned i = 0; i < in.size(); ++i){
        bool ok;

        //std::ifstream infl(in[i].str().cc());
        //std::string cont((std::istreambuf_iterator<char>(infl)), std::istreambuf_iterator<char>());

        std::ifstream input(in[i].str().cc(), std::ios::binary);

        tmpfile << input.rdbuf();
        unsigned long flsize = input.gcount();
        /*
        std::vector<char> buffer;
        buffer.resize(4096);
        int readcnt;
        while(input.read(&buffer[0], buffer.size()) || (readcnt = input.gcount()) != 0){
            //LOG(readcnt);
            flsize += readcnt;
            tmpfile.write(&buffer[0], readcnt);
        }
        */

        //std::string buffer = readstring(input);
        //unsigned long len = buffer.size();
        //writestring(tmpfile, buffer);
        //LOG(len);

        //std::ofstream output( "C:\\myfile.gif", std::ios::binary );
        //std::vector<unsigned char> buffer(std::istreambuf_iterator<unsigned char>(input), std::istreambuf_iterator<unsigned char>());

        //tmpfile.write((const char*)&(buffer[0]), len);
        //buffer.clear();
        //std::copy(std::istreambuf_iterator<unsigned char>(buffer), std::istreambuf_iterator<unsigned char>(), std::ostreambuf_iterator<unsigned char>(tmpfile));

        //ZString tmp = ZFile::readFile(in[i], ok);
        //ZString tmp = cont;

        //if(!ok)
        //    return false;
        //tmpfile.write(tmp.cc(), tmp.size());
        //tmpfile.flush();
        //tmpfile << tmp.cc() << std::flush;
        currpos += flsize;
        ZString loc;
        loc << ZString::ItoS(currpos) << ',' << ZString::ItoS(flsize);
        inx[in[i].last().str()] = loc;
    }
    tmpfile.close();
    std::ifstream bffile(tmppath.str().cc(), std::ios::binary);
    LOG(bffile.is_open() << " " << bffile.good() << bffile.bad() << bffile.fail() << bffile.eof() << bffile.rdstate());
    if(!bffile.is_open())
        return false;

    ZString head = inx.toJSON() + ";";
    ZFile::remove(out);
    std::ofstream outfile(out.str().cc(), std::ios::binary | std::ios::app);
    if(!outfile.is_open())
        return false;
    //outfile.write(head.cc(), head.size());
    LOG(head);
    /*
    while(bffile.good()){
        char buff[4096];
        bffile.read(buff, 4096);
        ZString bf = buff;
        outfile.write(bf.cc(), bf.size());
    }
    */
    std::vector<char> buffer;
    buffer.resize(4096);
    int readcnt;
    while(bffile.read(&buffer[0], buffer.size()) || (readcnt = bffile.gcount()) != 0){
        outfile.write(&buffer[0], readcnt);
    }

    LOG("done");
    bffile.close();
    outfile.close();
    //ZFile::remove(tmppath);
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
    if(!ZJSON::validJSON(indexstr))
        return ZParcel::ParcelSectionList();
    ParcelSectionList inxbuff;
    ZJSON tmpindex;
    tmpindex.fromJSON(indexstr);
    for(unsigned i = 0; i < tmpindex.size(); ++i){
        ZString tmp = tmpindex[i];
        ArZ tmp2 = tmp.explode(',');
        if(tmp2.size() != 2)
            return ZParcel::ParcelSectionList();
        ParcelSection pcl(tmp2[0].tint() + offset, tmp2[1].tint());
        inxbuff[tmpindex.key(i)] = pcl;
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
