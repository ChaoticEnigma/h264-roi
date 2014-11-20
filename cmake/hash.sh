#!/bin/bash

dir=$1
(find "$dir" -type f -exec md5sum {} +; find "$dir" -type d) | LC_ALL=C sort | md5sum | sed 's/-//'
