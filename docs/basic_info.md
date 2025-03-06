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

## Chapter 3 - Kernel
We can now start talking about a proper "operating system kernel", so to speak. The first big advantage compared to before is that we can compile the final kernel executable to be an ELF binary instead of just a straight up binary, which gives us more control particularly over uninitialized memory (the ``.bss`` section), as well as the location of stuff in memory. This will prove important when setting up paging, although we're not there yet.

Contrary to the bootloader, which is a non-interactive program (the user does not interact with at all), the kernel must set up an execution environment, but after that it just stays there, waiting for stuff to happen.

How does the kernel wait for stuff to happen? Though the use of processor interrupts. These form one of the basis of the x86 (and may other) architecture. To activate these, we must set up a valid **IDT**, or Interrupt Descriptor Table. This object, similar to the **GDT**, is essentially an array of pointers to functions, which are the Interrupt Handlers, i.e. the functions that are called when some interrupt is raised.

We must also set up the **Local APIC**, which is a per core component that controls external hardware interrupts, and how they are sent to the processor via interrupts. Additionally, there's also the **I/O APIC**, which is a per processor component, that directly interacts with external devices, and functions essentially as a traffic signaler, directing different events to different cores to be handled.

After all of these are set up, we can enable interrupts, and start dealing with other pieces of hardware.

One basic piece of hardware is a timer. There are many options on modern processors, with different characteristics and different capabilities and shortcomings. Some common examples are the **HPET** and the **Local APIC Timer**, which will be the two we're going to set up. The **Local APIC Timer** is better, more precise, lower latency, and just overall better, but it must be calibrated against some known timer, which is where the **HPET** comes in, since it self-reports it's calibration.