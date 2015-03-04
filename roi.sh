#!/bin/bash

i=0
for var in "$@"
do
    echo "$var"
    args[i]="$var"
    ((++i))
done

ffmpeg -i "$1" -c:v libx264 -pix_fmt yuv420p -preset slow -qp 10 -an "$1.h264"
ffmpeg -i "$1" -vn -c:a copy "$1.audio"

args[0]

echo ${args[@]}
#./h264_roi ${args[@]}

ffmpeg -i "$1.roi.h264" -i "$1.audio" -c:v copy -c:a copy "$2"
