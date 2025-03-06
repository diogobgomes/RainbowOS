# Building BOCHS

BOCHS is a powerful emulator that gives us more information that qemu, sometimes, despite being slower, so it's a useful tool, especially when debugging paging stuff.

One particularly useful feature it has is that it allows for the setup of _magic breakpoints_ using the assembly instruction ```xchgw %bx, %bx```. However, to use it, BOCHS must be compiled from source.

The configuration options that worked for me were:
```
./configure --enable-smp \
            --enable-cpu-level=6 \
            --enable-all-optimizations \
            --enable-x86-64 \
            --enable-pci \
            --enable-vmx \
            --enable-debugger \
            --enable-disasm \
            --enable-debugger-gui \
            --enable-logging \
            --enable-fpu \
            --enable-3dnow \
            --enable-sb16=dummy \
            --enable-cdrom \
            --enable-x86-debugger \
            --enable-iodebug \
            --disable-plugins \
            --disable-docbook \
            --with-x --with-x11 --with-term --with-sdl2 \
            --prefix=$HOME/opt/bochs
```

I also had to manually specify that certain libraries existed (and make sure they were installed):

```
pkg-config --exists gtk+-2.0
pkg-config --exists gtk+-3.0
```

After that, it should be a case of ```make && make install```