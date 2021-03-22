# SunxiPlatformPkg

## Supported boards
Currently only XW711 (also known as szenio 1207c4) is supported, hovewer it should work on any Q8 A13 tablet.

## Building
```bash
# Download latest stable edk2
git clone --depth=1 https://github.com/tianocore/edk2 --branch edk2-stable202102
cd edk2
git submodule update --init
git clone --depth=1 https://github.com/arturkow2000/SunxiPlatformPkg
make -C BaseTools/Source/C
. edksetup.sh
env GCC5_ARM_PREFIX=arm-none-eabi- build -a ARM -p SunxiPlatformPkg/XW711.dsc -t GCC5
```

## Booting
First patch SUNXI_SPL.fd header checksum using sunxiboot tool from [here](https://github.com/arturkow2000/sunxiboot).
Then boot using
```bash
sunxi-fel --verbose spl SUNXI_SPL.fd write 0x42000000 SUNXI_EFI.fd exe 0x42000000
```
