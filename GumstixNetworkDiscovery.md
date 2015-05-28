# Introduction #

The struggles with getting a static IP on the network have introduced a motivation for making it easy to discover the IP addresses of gumstix on the network.  This page describes how to:

# Place a small server program on the gumstix; the program simply opens port 10005 and responds to requests with a quote from a poem.
# Configure the gumstix so that the server program is automatically started at boot time.
# Use nmap to search the subnet for open ports 10005.

# Steps #

  * Get the IP address of your gumstix.

```
  $ ifconfig
```

  * Obtain the binary for the server program.  It is located in the repository at ~/communication/portlistener.out.
  * Copy portlistener.out to the target gumstix using scp.

```
   $ scp portlistener.out root@<ip address>:/home/root
```

  * Affirm that portlistener.out is working.  Then run portlistener.out on the gumstix.  From another machine, use telnet to access the server:

```
   $ telnet <ip address> 10005
```

> You should get a reply from J. Alfred Prufrock.

  * Obtain the script for the server program.  It is located in the repository at ~/gumstix/portlistner.sh.
  * Copy portlistener.out to the target gumstix using scp.

```
   $ scp porstlistner.out root@<ip address>:/etc/init.d
```

  * On the gumstix, create a symbolic link to the server script in /etc/rcS.d.  Doing so means that portlistener.sh is on the list of scripts that the system calls at boot time.

```
  $ cd /etc/rcS.d
  $ ln -s /etc/init.d/portlistener.sh S100portlistener.sh
```

The "S100" means its one of the last scripts that are called.  Make sure that no other script in rcS.d has that number.

  * Reboot the gumstix.  Affirm that the portlistener was automatically called by using telnet again.
  * Use nmap to discover the open port.

```
  $ nmap <ip address>/25 -p 10005 --open 
```

This means that nmap will search all IP addresses matching the first 25 bits of the ip address for open ports 10005.  For this subnet, it should only take about 5 seconds.