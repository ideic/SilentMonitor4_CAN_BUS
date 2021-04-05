sudo sdptool del 0x10000;
sudo sdptool del 0x10001;
sudo sdptool del 0x10002;
sudo sdptool add --channel=1 FTP;
sudo hciconfig hci0 piscan;
