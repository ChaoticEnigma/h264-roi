@echo off
::ffmpeg -i "%1" -vcodec copy -an -bsf:v h264_mp4toannexb "%1.h264"

SETLOCAL ENABLEDELAYEDEXPANSION
SETLOCAL ENABLEEXTENSIONS
ffmpeg -i "%%A" -vcodec rawvideo -f yuv4mpegpipe -an -pix_fmt yuv420p - | x264.x64 - --stdin y4m --crf 0 --bframes 5 --b-adapt 2 --ref 4 --mixed-refs --no-fast-pskip --direct auto --deblock -3:-3 --subme 10 --trellis 2 --analyse all --8x8dct --me umh --output "%%~nA.noaudio.mkv"

h264_roi.exe %*
