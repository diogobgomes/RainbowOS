# RainbowOS

RainbowOS is a small, hobbyist, x86 operating system built from scratch as a learning experiment. It's based largely on my previous attempt, [PatroclusOS](https://github.com/diogobgomes/PatroclusOS), but dropping the x86_64 ambitions, because that's way too complicated with compilers and other things.

## Dependencies
- Cross compiler toolchain supporting C and C++: see [here](https://wiki.osdev.org/GCC_Cross-Compiler) for more details
- CMake
- GRUB2 (optional, as an alternative bootloader, instead of the custom one we're using)
    - Xorriso (to build the iso)
- qemu-system-i386 (optional, to emulate)
- bochs (optional, to emulate. Must be built from source, see [this](docs/misc/build_bochs.md))
- Doxygen (optional, to build the documentation)

## Building
The first step is building the cross compiler toolchain, as stated. The build is performed using CMake. By default, the toolchain is assumed to be installed to $(HOME)/opt/i686-elf-cross. If another location is chosen to install the toolchain, you'll have to tell that to CMake, by modifying the configuration file ```config/toolchain.cmake```.

On that subject, there is another configuration file, ```config/Findqemu.cmake``` that attempts to find a valid ```qemu-system-i386``` installation; if it can't find it automatically, CMake will disable the ```qemu``` targets, so you should modify this file, if you have ```qemu``` installed, and CMake isn't finding it.

The same is true for the ```bochs``` target, with the configuration file ```config/Findbochs.cmake```. This is configured by default to *only* search the directory ```$HOME/opt/bochs/bin```, and not the normal system directories, since bochs must be built from source to enable some extra features, as described above.

With all of that done, you just have to run CMake to build the operating system. The target ```diskimage``` builds a file ```diskimage.dd``` containing a fully built and configured bootloader + kernel, that should run. A target ```qemu``` is provided that automatically runs qemu with the diskimage.

## Roadmap
- [x] Bootloader
    - [x] Stage0
    - [x] Stage1
    - [x] Multiboot-loader
- [ ] GDT
- [ ] Paging
- [ ] IDT
- [ ] Keyboard

## More information
Check out the [docs](docs/README.md)