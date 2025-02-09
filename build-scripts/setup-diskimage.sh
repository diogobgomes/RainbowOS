#!/bin/bash
# HELPER SCRIPT FOR CMAKE
#
# Script to generate the initial diskimage, and partition it
# It checks if it exists, and if so, doesn't generate it
# Expect invocation setup-diskimage.sh $(diskimage) $(rootsrcdir) $(loopback)
#
#
# 2025 Diogo Gomes

set -e
# keep track of the last executed command
trap 'last_command=$current_command; current_command=$BASH_COMMAND' DEBUG
# echo an error message before exiting
trap cleanup ERR

cleanup() {
    error_code=$?
    echo "In setup-diskimage.sh: \"${last_command}\" command failed with exit code $?. Aborting!"
    exit $((error_code))
}

diskimage=$1
rootsrcdir=$2
loopback=$3

if [ -f "$diskimage" ]; then
    echo "Diskimage ${diskimage} detected, skipping generation"
else
    echo "Diskimage ${diskimage} not detected, generating it..."
    echo "dd:"
    dd if=/dev/zero of=${diskimage} bs=1048576 count=12
    echo "Super user permissions are needed for this step"
    echo "sfdisk:"
    sudo -A sfdisk ${diskimage} < ${rootsrcdir}/config/diskimage.sfdisk
    echo "losetup:"
    sudo -A losetup -o $((2048*512)) ${loopback} ${diskimage}
    echo "mkfs.fat:"
    sudo -A mkfs.fat -F 32 ${loopback}
    sudo -A losetup -d ${loopback}
    #sudo -A losetup -o $((4096*512)) ${loopback} ${diskimage}
    #echo "mkfs.fat:"
    #sudo -A mkfs.fat -F 32 ${loopback}
    #sudo -A losetup -d ${loopback}
    echo "Diskimage ${diskimage} generated!"
fi