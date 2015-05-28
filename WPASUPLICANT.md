To set up a secure WPA2 connection for the gumstix, one must set up wpa\_supplicant.

**1. Create the /etc/wpa\_supplicant.conf file using the wpa\_passphrase command:**

```
   $ wpa_passphrase NETWORKSSID PASSWORD > /etc/wpa_supplicant.conf
```

**2. Add to the newly-created file:**

```
network={
        ssid="id of router"
        proto=WPA2
        key_mgmt=WPA-PSK
        pairwise=CCMP TKIP
        group=CCMP TKIP
        scan_ssid=1
        psk= a giant number
        priority=10
}
```

**3. Edit /etc/network/interfaces**

```
auto wlan0
iface wlan0 inet dhcp
	pre-up wpa_supplicant -Dmarvell -iwlan0 -c/etc/wpa_supplicant.conf -Bw
	down killall wpa_supplicant
```


Also: http://www.acroname.com/robotics/info/gumstix/networking.html