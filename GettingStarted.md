  * Affirm that you have Virtual Box installed on your machine.
  * Get the Virtual Box development image, squeeze\_upbot.  This Debian Squeeze image contains a number of development tools, most notably the cross-compiler for the gumstix controlling the robot.  All subsequent steps should be taken from within the development image.
  * Connect the gumstix SERIAL-USB port to your development machine.
  * To power the gumstix from the robot's battery, connect the DB25 on the robot to the gumstix.  LEDs on the gumstix should turn on.  The blue LED should turn on after ~30 seconds indicating that its wireless device is on.
  * Open a terminal into the gumstix.  From the development machine,

```
$ sudo minicom -o
```

  * From minicom, hit Enter to get a prompt from the gumstix.
  * Login to the gumstix.  If you don't know the login and password, contact your upbot admin.
  * Get the IP address of the gumstix.  For the wireless device IP, look for wlan0.

```
$ ifconfig
```

  * Write down the IP address.  You will be using it a lot.  Subsequent steps will use the IP 10.0.0.1.
  * From the development machine, ssh into the IP.

```
$ ssh root@10.0.0.1
```

  * The previous step may not have worked.  This is likely due to a bad configuration of the Virtual Box Network Adapter.  There may be two muddled configurations of eth0 and eth1 in /etc/udev/rules.d/70-persistent-net.rules.  Make sure there's just one called eth0.  Then go to the Devices menu in Virtual Box, select 'Network Adapters' and then 'NAT'.  Then try to ssh again.  Hopefully that works.  If not, allow me to introduce you to Google.

  * Now that you know how to access the robot over wireless, it's time to run the code.  Currently, the code is structured as a client-server architecture.  A good demo to run is 'brainstem' on the robot and 'client' on the desktop machine.  This will allow you to type commands from the desktop machine which control the motion of the robot.

  * If you haven't already, clone the development code.

  * From the development machine, compile the demo:

```
$ cd upbot
$ cd communication
$ make brainstem
$ make client
```

  * When making brainstem, the server side of the demo, you might get some error like 'arm-linux-gcc not found'.  To solve this issue, create a symbolic link from the new compiler to the old compiler's name.

```
$ sudo ln -s /usr/bin/arm-linux-gnueabi-gcc /usr/bin/arm-linux-gcc
```

  * Move the brainstem.out executable to the robot:

```
$ scp brainstem.out root@10.0.0.1:/home/root
```

  * From the ssh window on the gumstix, start the server:

```
$ ./brainstem.out
```

  * From the development machine, start the client:

```
$ ./client.out 10.0.0.1
```

  * From the client, you can type a single letter and then Enter to issue a command to the robot.
    * w: forward.
    * s: backward.
    * a: turn left.
    * d: turn right.
    * x: stop.
    * h: small turn left.
    * j: small turn right.