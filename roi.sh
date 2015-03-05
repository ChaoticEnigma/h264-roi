#!/bin/bash

for var in "$@"
do
    echo "$var"
    args[i]="$var"
    ((++i))
done

args[0]="\"$1.h264\""
args[1]="\"$1.roi.h264\""

echo ffmpeg -i "$1" -c:v libx264 -pix_fmt yuv420p -preset slow -qp 10 -an "$1.h264"
echo ffmpeg -i "$1" -vn -c:a copy "$1.audio"
#ffmpeg -i "$1" -c:v libx264 -pix_fmt yuv420p -preset slow -qp 10 -an "$1.h264"
#ffmpeg -i "$1" -vn -c:a copy "$1.audio"

echo h264_roi ${args[@]}
#./h264_roi ${args[@]}

echo ffmpeg -i "$1.roi.h264" -i "$1.audio" -c:v copy -c:a copy "$2"
#ffmpeg -i "$1.roi.h264" -i "$1.audio" -c:v copy -c:a copy "$2"
