#!/bin/bash
# HELPER SCRIPT FOR CMAKE
#
# Script to generate the final diskimage. Mounts the dirs, copies the files over,
# and injects the fat info
# Expect invocation generate-diskimage.sh $(diskimage) $(diskimage_loopback) \
#        $(rootsrcdir) $(mount_dir) $(stage0) $(stage1) $(kernel)
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
    echo "\"${last_command}\" command failed with exit code $?. Aborting!"
    exit $((error_code))
}

# Get current user, to pass on to subscript
current_user=$(whoami)

diskimage=$1
diskimage_loopback=$2
rootsrcdir=$3
mount_dir=$4
stage0=$5
stage1=$6
kernel=$7

if ! sudo ${rootsrcdir}/build-scripts/mountdirs.sh ${mount_dir} \
          ${diskimage_loopback} ${diskimage} ${current_user}; then
    echo "Couldn't correctly mount the filesystems, aborting!"
    exit 1
fi

# Now copy the kernel over
cp ${kernel} ${mount_dir}/kernel.bin
sync

# Inject stage0/stage1 into diskimage
echo "Injecting stage0.bin in diskimage..."
dd if=${stage0} of=${diskimage} conv=notrunc bs=440 count=1
echo "Injecting stage1.bin in diskimage..."
dd if=${stage1} of=${diskimage} conv=notrunc bs=512 seek=1 count=$(ls -l ${stage1} | awk 'function ceil(x,y){y=int(x); return(x>y?y+1:y)} {print ceil($5/512)}')
sync

echo "Diskimage generated successfully!"