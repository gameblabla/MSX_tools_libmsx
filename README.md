MSX Tools libmsx
================

A bunch of tools developed for my use with libmsx.
I encountered an issue with ResourcesIndex and loading files from that being extremely slow.
I was using bmem_copy_to_vmem to push an uncompressed image to the screen and it turns out
bmem_copy_to_vmem was very inefficient, look for yourself :
```c
void bmem_copy_to_vmem(bmemptr_t src, vmemptr_t dst, uint32_t len) {
  uint8_t bank = bmem_bank_of(src);
  const uint16_t offset = (uint16_t)(src & 0x3fff);
  const uint8_t * p = (const uint8_t *)0x8000 + offset;
  uint32_t n = (uint32_t)0x4000 - offset;

  const uint8_t old_bank = bmem_get_bank();
  vmem_set_write_address(dst);
  while (len) {
    if (len < n) {
      n = len;
    }
    len -= n;
    bmem_set_bank(bank);
    while (n--) {
      vmem_set(*p++);
    }
    bank++;
    p = (const uint8_t *)0x8000;
    n = (uint32_t)0x4000;
  }
  bmem_set_bank(old_bank);
}
```

In contrast, this is what i would use with the tools i made :
```c
#define PUSH_IMAGE_SCREEN(b) \
    bmem_set_bank(b##_BANK); \
    vmem_write_unsafe(0x0000, b[0], SIZE_##b##_PART_1); \
    bmem_set_bank(b##_BANK + 1); \
    vmem_write_unsafe(0x0000+SIZE_##b##_PART_1, b[1], SIZE_##b##_PART_2);
```

This is 200%+ faster than using bmem copy.

In your C program, if you have the macro defined,
you can just do instead :
```c
    PUSH_IMAGE_SCREEN(BAKURA_OG_SR5);
	vdp_write_palette((uint16_t*)msx_palette);
```

# Usage

First, make sure your image is in SC5 format.

Then run it like so :
```sh
./crop.sh inputfile first_bank second_bank [filesize in bytes]
```

Input file is your SC5 image.
first_bank is the first bank your program should be loaded from.
second_bank is the second bank it should end for part2.
filesize in bytes is only if you want to update a part of the screen.

Note that this does not handle files that are less than 16kb properly.
It will still work but they will have a size of 0.

Run crop.sh with the paramters and you should have an output folder with C files inside.
If you want to add more files to it, go ahead because the final step will be to create a header for these.

Run
```
./output_to_header.sh
```

And you should end up with a header file called "files_msx.h".
You can simply add it to your C file and run the above C code to push an uncompressed image to the screen.


# TODO

- Handle compressed formats like zx0/1/7 eventually in a proper way once they get ZX7toVRAM code. (this assumes SC5 for now)

