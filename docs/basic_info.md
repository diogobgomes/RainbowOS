# Basic info about osdev

Some thoughts on how an OS works

## Chapter 1
Ok, really basic, we need an assembly source, which we build using ``as``, and then link using:\
```ld -oformat binary -o boot.bin -Ttext 0x7C00 boot.o```\
where ``-oformat binary`` specifies we're outputting a binary file, and ``-Ttext 0x7C00`` is basically our linker script: we're telling our linker to put the ``.text`` section at address ``0x7C00``. I'll get to that.

### So, how does a PC boot?
Well, the BIOS will look for a valid boot signature to boot. It will look at sector 0 in memory devices and look for the magic number ``0xaa55`` at byte offsets 510 and 511. Having found that, it loads the first 512 bytes into ``0x0000:0x7c00`` (segment 0, offset thingy), and transfers execution. That's why the ``-Ttext 0x7C00``: we're letting the linker know that we'll be at that location.

For our assembly, then, we need a couple of things. First, set ``%ds`` to 0 (since linker took care of data segment), and do stuff.

Simplest thing we can do is write hello world, rellying on int 10h (the holly grail is here).

### Hello world with int 10h
Our program is like this:


```assembly
.code16
.global _start
_start:
    mov $0, %ax
    mov %ax, %ds #Data segment=0 because Ttext in loader takes care of it

    leaw msg, %si #Load msg address to si register
    cld #Clear load flags

    call write_loop

loop:
    jmp loop

write_loop:
    xor %bh, %bh #Set page 0 for int 10h
    lodsb #Load *(%si) to %al (and increment)
    orb %al, %al #Zero is end of string
    jz write_done
    mov $0x0e, %ah #Teletype mode
    int $0x10
    jmp write_loop
write_done:
    ret

msg:
    .asciz "Hello world!"

#Boot sector is 512 bytes long, 510 + 2 bytes for the signature
.org 510
.word 0xaa55
```

The ``.org`` directive at the end just tells us to fill up to 510 bytes with zeros, and the the ``.word``puts the magic number: 512 bytes

``_start`` is our entry point, we declare that with the ``.global _start``

The rest you can just read, it's not that hard.

For booting, use qemu: ``qemu-system-i386 -hda boot.bin``

This is a hello world done just in assembly, basically.

## Chapter 2 - Bootloaders
We now want to start writing a bootloader. What is it? We want something that can prepare the computer, read our OS (or a necessary part of it) from the disk, and then jump to it. We also want a couple more things, but those are not important right now.

First thing, though. We want our bootloader to do all this, but as before, the BIOS only loads the first 512 bytes from the disk (called the MBR, master boot record). Even worse, on a HDD, some of it will be occupied by partition tables, useless information, etc. We end up with only around 440 bytes of space. That's not a lot of space.

One way out is with a 2-stage bootloader: the first, **stage0**, only needs to load the second, **stage1**, that can now be much bigger, and that can then load the rest of the kernel. We'll actually also have a small **Multiboot** stage, that just implements the Multiboot protocol.

To see more about the design, visit the [design doc](bootloader/README.md).