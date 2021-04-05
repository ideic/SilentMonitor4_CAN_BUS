//#include <wiringPi.h>

// LED Pin - wiringPi pin 0 is BCM_GPIO 17.
// we have to use BCM numbering when initializing with wiringPiSetupSys
// when choosing a different pin number please use the BCM numbering, also
// update the Property Pages - Build Events - Remote Post-Build Event command
// which uses gpio export for setup for wiringPiSetupSys
//#define	LED	17
//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <sys/socket.h>
//#include <bluetooth/bluetooth.h>
//#include <bluetooth/hci.h>
//#include <bluetooth/hci_lib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <string>
#include <stdexcept>
#include "BluetoothServer.h"
using namespace std::string_literals;
int main(void)
{
	//wiringPiSetupSys();

	//pinMode(LED, OUTPUT);

	//while (true)
	//{
	//	digitalWrite(LED, HIGH);  // On
	//	delay(500); // ms
	//	digitalWrite(LED, LOW);	  // Off
	//	delay(500);
	//}
	//return 0;

    //inquiry_info* ii = NULL;
    //int max_rsp, num_rsp;
    //int dev_id, sock, len, flags;
    //int i;
    //char addr[19] = { 0 };
    //char name[248] = { 0 };

    //dev_id = hci_get_route(NULL);
    //sock = hci_open_dev(dev_id);
    //if (dev_id < 0 || sock < 0) {
    //    perror("opening socket");
    //    exit(1);
    //}

    //len = 8;
    //max_rsp = 255;
    //flags = IREQ_CACHE_FLUSH;
    //ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));

    //num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
    //if (num_rsp < 0) perror("hci_inquiry");

    //for (i = 0; i < num_rsp; i++) {
    //    ba2str(&(ii + i)->bdaddr, addr);
    //    memset(name, 0, sizeof(name));
    //    if (hci_read_remote_name(sock, &(ii + i)->bdaddr, sizeof(name),
    //        name, 0) < 0)
    //        strcpy(name, "[unknown]");
    //    printf("%s  %s\n", addr, name);
    //}

    //free(ii);
    //close(sock);
    //return 0;

    //struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
    //char buf[1024] = { 0 };
    //int socketId, client, bytes_read;
    //socklen_t opt = sizeof(rem_addr);

    //// allocate socket
    //socketId = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    //if (socketId < 0) {
    //    throw std::runtime_error("Bluetooth socket Init failed errno:"s + std::to_string(socketId) + " Reason: "s + std::string(strerror(socketId)));
    //}
    //// bind socket to port 1 of the first available 
    //// local bluetooth adapter
    //loc_addr.rc_family = AF_BLUETOOTH;
    //auto any = (bdaddr_t){ {0x00, 0x1A, 0x7D, 0xDA, 0x71, 0x15} };
    //loc_addr.rc_bdaddr = any;
    //loc_addr.rc_channel = (uint8_t)1;
    //bind(socketId, (struct sockaddr*)&loc_addr, sizeof(loc_addr));

    //// put socket into listening mode
    //listen(socketId, 1);

    //// accept one connection
    //client = accept(socketId, (struct sockaddr*)&rem_addr, &opt);

    //ba2str(&rem_addr.rc_bdaddr, buf);
    //fprintf(stderr, "accepted connection from %s\n", buf);
    //memset(buf, 0, sizeof(buf));

    //// read data from the client
    //bytes_read = read(client, buf, sizeof(buf));
    //if (bytes_read > 0) {
    //    printf("received [%s]\n", buf);
    //}

    //// close connection
    //close(client);
    //close(socketId);

    BluetoothServer server("00:1A:7D:DA:71:15");
    server.Run();
    return 0;
}