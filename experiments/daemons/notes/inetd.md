inetd and xinetd
================

* inetd is a network daemon that handles tcp connections to a server
* When a request comes to a port managed by inetd, it routes that request to the appropriate server
* xinetd provides access control above and beyond what inetd provides
    * access control for TCP, UDP and RPC services
    * access control based on time segments
    * logging
    * containment against DoS
    * limitation on the number of servers of the same type to run at a time
    * limitation on the total number of servers
    * limitation on the size of log files
    * binding of a service to a specific interface
        * Allows us to make services available to the private network but not the outside world
    * Can be used as a proxy to other systems

### Signals to modify xinetd behavior
1. SIGUSR1
    * Re-read the configuration file to change service parameters accordingly
2. SIGUSR2
    * Re-read the configuration file and the daemon is restarted
3. SIGTERM
    * Exits xinetd and any daemons that it spawned

Configuration
-------------
* default options follow a similar pattern to the following

```
defaults
{
    attribute operator value (s)
}
```

* `only_from`: Narrow down the list of valid addresses that can access this machine`
    * Compose helper service/cron job that pulls down a master list of IP addresses, updates conf accordingly, and sends SIGUSR1 to restart xinetd
* [This](http://www.linuxfocus.org/English/November2000/article175.shtml#tableau1) table provides some common configuration options

Access Control
--------------
* In addition to preventing access by IP address, we can also manage access in the following ways
    * by hostname resolution
        * xinetd performs a lookup of the hostnames specified for every connection and compares the connecting address to the address returned for the hostnames
    * by domain
        * xinetd will reverse lookup the connecting address and see if it is in the specified domain (e.x. campus.up.edu)
* For optimization purposes, IP addresses are the quickest
* hostname resolution can likewise be efficient with the stipulation that a local caching name server is present
    * This may or may not be difficult on an embedded system

* It occurs to me that xinetd is only useful for routing connections, but does not provide the bidirectional routing that we are looking for.

In other words...

We Probably Don't Want to Use xinetd
------------------------------------
