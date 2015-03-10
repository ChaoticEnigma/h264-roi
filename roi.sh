#!/bin/bash

for var in "$@"
do
    echo "$var"
    args[i]="$var"
    ((++i))
done

in=$1
out=$2

args[0]="\"$in.h264\""
args[1]="\"$in.roi.h264\""

echo
echo "### Extract Video ##############################################################"
echo ffmpeg -i "$in" -codec:v libx264 -pix_fmt yuv420p -preset slow -qp 10 -an "$in.h264"
echo
ffmpeg -i "$in" -codec:v libx264 -pix_fmt yuv420p -preset slow -qp 10 -an "$in.h264"

echo
echo "### Extract Audio ##############################################################"
echo ffmpeg -i "$in" -codec:a libmp3lame -qscale:a 3 -vn "$in.mp3"
echo
ffmpeg -i "$in" -codec:a libmp3lame -qscale:a 3 -vn "$in.mp3"

echo
echo "### ROI Video ##################################################################"
echo ./h264_roi ${args[@]}
echo
./h264_roi ${args[@]}

echo
echo "## Mux Audio/Video ############################################################"
echo ffmpeg -i "$in.roi.h264" -i "$in.mp3" -c:v copy -c:a copy "$out"
echo
ffmpeg -i "$in.roi.h264" -i "$in.mp3" -c:v copy -c:a copy "$out"
