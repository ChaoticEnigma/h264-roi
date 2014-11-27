#include "zpath.h"
#include "zfile.h"
#include "zlog.h"
using namespace LibChaos;

// Image Library Manager: tool for converting images to WebP to save space, and decoding them for platforms without WebP support
// Based on towebp.bat and fromwebp.bat by Chaos from FEROZE C:\bin

// Pack: Encodes PNG and JPEG images to lossless WebP and replaces originals in-place, and names and numers images with name of parent directory
// Unpack: Decodes WebP images to lossy JPEG (quality 80) in another directory

void packDir(ZPath dir);
void unPackDir(ZPath dir, ZPath dest);

int main(int argc, char **argv){
    ZLog::formatStdout(ZLogSource::all, "%log%");
    LOG("Image Library Manager");

    if(argc > 1){
        ZString command = argv[1];
        if(command == "packone"){  // Convert all non-WebP images in <dir> to WebP, and rename to fit
            if(argc != 3){
                LOG("Incorrect arguments for packing");
                return 3;
            }
            LOG("Packing one directory");
            packDir(argv[2]);
            LOG("Finished packing");

        } else if(command == "pack"){ // Convert all non-WebP images in all dirs under <dir> to WebP, and rename to fit, additional argument are dis to exclude, relative to first dir
            if(argc < 3){
                LOG("Incorrect arguments for packing");
                return 3;
            }
            LOG("Packing directory recursively");
            ZPath rootdir = argv[2];
            ArP dirs = ZFile::listDirs(rootdir, true);
            LOG(dirs.size());
            ArP exclude;
            for(int i = 3; i < argc; ++i){
                exclude.push(rootdir + argv[i]);
            }
            for(zu64 i = 0; i < dirs.size(); ++i){
                bool continueflag = false;
                for(zu64 j = 0; j < exclude.size(); ++j){
                    if(exclude[j] == dirs[i] || exclude[j].childTo(dirs[i]))
                        continueflag = true;
                }
                if(continueflag){
                    LOG("Skipping " << dirs[i]);
                    continue;
                }
                LOG("Packing Directory " << dirs[i]);
                packDir(dirs[i]);
            }
            LOG("Finished packing");

        } else if(command == "unpackone"){ // Convert all the WebP images in <srcdir> to JPEGs in <outdir>
            if(argc != 4){
                LOG("Incorrect arguments for unpacking");
                return 4;
            }
            LOG("Unpacking one directory");
            unPackDir(argv[2], argv[3]);
            LOG("Finished unpacking");

        } else if(command == "unpack"){ // Convert all the WebP images in <srcdir> to JPEGs in <outdir>, recursively
            if(argc != 4){
                LOG("Incorrect arguments for unpacking");
                return 4;
            }
            LOG("Unpacking directory recursively");
            //
            LOG("Finished unpacking");

        } else {
            LOG("Unknown Command");
            return 2;
        }
    } else {
        LOG("No Command");
        return 1;
    }
    return 0;
}

void packDir(ZPath dir){
    ZString prefix = dir.last();
    prefix.toLower();
    ArP tmpfiles = ZFile::listFiles(dir, false);
    for(zu64 j = 0; j < tmpfiles.size(); ++j){
        ZPath alt = tmpfiles[j];
        alt.last().prepend("~");
        if(!ZFile::rename(tmpfiles[j], alt)){
            LOG("Failed to rename temp file " << tmpfiles[j] << " to " << alt);
            continue;
        }
    }
    ArP files = ZFile::listFiles(dir, false);
    for(zu64 j = 0; j < files.size(); ++j){
        ZString filename = files[j].last();
        filename.toLower();
        zu64 seq = 10000 + j + 1;
        ZPath outpath = dir + (prefix + " (" + seq + ")");
        if(filename.endsWith(".png") || filename.endsWith(".jpg") || filename.endsWith(".jpeg")){
            outpath.last() += ".webp";
            LOG("Convert " << files[j] << " to " << outpath);

            ZString syscmd = "cwebp -short -q 100 -o \"" + outpath.str() + "\" \"" + files[j].str() + "\"";
            if(system((syscmd + " > NUL 2>&1").cc()) != 0){
                LOG("System Command failed: " << syscmd);
                continue;
            }

            if(!ZFile::remove(files[j])){
                LOG("Retrying to remove file: " << files[j]);
                ZThread::msleep(500);
                if(!ZFile::remove(files[j])){
                    LOG("Failed to remove file: " << files[j]);
                    continue;
                }
            }
        } else {
            outpath.last() += files[j].getExtension();
            LOG("Move " << files[j] << " to " << outpath);
            if(!ZFile::rename(files[j], outpath)){
                LOG("Failed to rename " << files[j] << " to " << outpath);
                continue;
            }
        }
    }
}

void unPackDir(ZPath dir, ZPath dest){
    ZPath outdir = dest + dir.last();
    if(!ZFile::isDir(outdir)){
        LOG("Creating directory " << outdir);
        if(!ZFile::createDirsTo(outdir)){
            LOG("Failed to create dirs to out path");
            return;
        }
        if(!ZFile::makeDir(outdir)){
            LOG("Failed to create dir at out path");
            return;
        }
    }

    ArP srcfiles = ZFile::listFiles(dir, false);
    for(zu64 i = 0; i < srcfiles.size(); ++i){
        ZPath outfile = srcfiles[i];
        outfile.relativeTo(dir);
        ZPath outpath = outdir + outfile;
        if(outpath.getExtension() == ".webp"){
            ZPath tmppath = outpath;
            tmppath.last().prepend("~").substr(0, tmppath.last().size() - 5).append(".ppm");
            outpath.last().substr(0, outpath.last().size() - 5).append(".jpg");
            LOG("Convert " << srcfiles[i] << " to " << outpath);

            ZString syscmd = "dwebp \"" + srcfiles[i].str() + "\" -ppm -o \"" + tmppath.str() + "\"";
            if(system((syscmd + " > NUL 2>&1").cc()) != 0){
                LOG("System Command failed: " << syscmd);
                continue;
            }

            ZString syscmd2 = "cjpeg-static -quality 80 -outfile \"" + outpath.str() + "\" \"" + tmppath.str() + "\"";
            if(system((syscmd2 + " > NUL 2>&1").cc()) != 0){
                LOG("System Command failed: " << syscmd2);
                continue;
            }

            if(!ZFile::remove(tmppath)){
                LOG("Retrying to remove file: " << tmppath);
                ZThread::msleep(500);
                if(!ZFile::remove(tmppath)){
                    LOG("Failed to remove file: " << tmppath);
                    continue;
                }
            }
        } else {
            LOG("Copy " << srcfiles[i] << " to " << outpath);
            if(!ZFile::copy(srcfiles[i], outpath)){
                LOG("Failed to copy");
                continue;
            }
        }
    }
}
