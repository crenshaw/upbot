Editing is ongoing.


## Step 1 | Get the stuff ##

Obtain the necessary hardware for burning.
  * A microSD card, no larger than 2GB.
  * A USB SD/MMC/MicroSD Card Reader/Writer.

Insert the microSD card into the reader/writer.

## Step 2 |  Start it up ##

Start up the virtual development appliance.  Insert the microSD card reader/writer into the USB port of the host machine.  Confirm that the disk is there:

```
$ df
...
/dev/sdb1    1943232    0    1943232    0%    /media/1C76-DA1A
```

If /dev/sdb1 is not listed, the virtual appliance may not have captured the microSD card reader/writer.  It may be necessary to write a filter for Virtual Box which will automatically capture the hardware for you.  Otherwise, the host machine may automatically mount the disk and it won't be available under the Virtual Box Devices menu to capture.

## Step 3 | Calculate card size ##

Calculate the number of bytes for the given card size.  In the above, the card is listed as having 1,943,232 1k-blocks.

```
1,943,232 x 1024 = 1,989,869,568 bytes or 2GB
```

Divide the number of bytes by 255 heads per drive, 63 sectors per track, and 512 bytes per sector to get the number of cylinders.

```
1,989,869,568 bytes / 255 heads per drive / 63 sectors per track / 512 bytes per sector = 241 cylinders per drive
```

## Step 4 | Partition the card ##

Use the dd and sfdisk system tools to partition the disk based on the calculation made in step 3.  Though the disk name is listed as /dev/sdb1, note that I use /dev/sdb to refer to the microSD card disk.

Copy a whole bunch of zeros to the start of the disk:

```
$ sudo dd if = /dev/zero of=/dev/sdb bs=1024 count=1024
```

Un-mount the disk and affirm that the disk is ready

```
$ umount /dev/sdb1
$ sudo sfdisk -R /dev/sdb
```

Format the disk:

```
$ sudo sfdisk --force -D -uS -H 255 -S 63 -C 241 /dev/sdb
```

An interactive process will begin, asking to set up four partitions on the disk.   In this interactive mode, Note that sfdisk reads lines of the form:

```
start, size, <c,h,s>, bootable
```

See the sfdisk man pages for more details.

We only want two partitions, where the first partition is bootable, indicated by **.**

```
/dev/sdb1: 128,130944,0x0C,*
/dev/sdb2: 131072,,,-
/dev/sdb3: 0,0,,-
/dev/sdb4: 0,0,,-
```

You may get the warning,

"Warning: partition 1 does not end at a cylinder boundary"

This is expected.

## Step 5 | Format the partitions ##

Format partition 1 as a FAT filesystem.

```
$ mkfs.vfat -F 32 /dev/sdb1 -n boot
```

Format partition 2 for Linux

```
$ mke2fs -j -L rootfs /dev/sdb2
```

## Step 6 | Mount and affirm ##

It may be possible to mount the partitions just made and affirm their existence.

```
$ sudo mkdir /media/{boot,rootfs}
$ sudo mount -t vfat /dev/sdb1 /media/boot
$ sudo mount -t ext3 /dev/sdb2 /media/rootfs
```

However, it may also be necessary to eject the card reader/writer from the virtual machine and then the host machine.  It may also be necessary to restart the virtual appliance and reinsert the card reader/writer.  After one of these approaches, you should have two partitions, /dev/sdb1 and /dev/sdb2 called boot and rootfs.

```
$ df
/dev/sdb1          6448           5       64443    1%  /media/boot_
/dev/sdb2    1848448   35564  1718984     3%  /media/rootffs_
```

## Step 7 | Copy images ##

Get the custom linux image that shall be burned to the disk.

There should be three pieces to the image:

  * The boot loader.  A u-boot.bin file.
  * The root file system.  A uImage file.
  * The kernel binary image.  A rootfs.tar.gz file.

```
$ sudo cp 
$ sudo cp uImage-gumstix-verdex.bin /media/boot/uImage
$ sudo tar xaf verdex-console-image-gumstix-verdex.tar.gz -C /media/rootfs
$ sync
```

## Step 8 | Unmount the card ##

```
$ sudo umount /dev/sdb1
$ sudo umount /dev/sdb2
```

## Step 9 | Boot the image ##

Remove the microSD card from the card reader/writer and place it in the microSD card slot on the gumstix.

To get the gumstix to boot from the microSD card, it may be necessary to