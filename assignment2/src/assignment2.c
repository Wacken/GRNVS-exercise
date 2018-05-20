#include <netinet/ether.h>
#include <net/ethernet.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/ip6.h>
#include <netinet/icmp6.h>
#include <asm/byteorder.h>

#include "arguments.h"
#include "raw.h"
#include "hexdump.h"
#include "checksums.h"

/*====================================TODO===================================*/
/* Put your required struct definitions */

uint8_t mymac[ETH_ALEN];

/* Put your ancillary functions here*/

/*===========================================================================*/

void assignment2(int fd, int frames)
{
	unsigned int timeout = 10000;
	uint8_t recbuffer[1514];
	size_t ret;

/*====================================TODO===================================*/
	/* If you want to set up any data/counters before the receive loop,
	 * this is the right location
	 */

	memcpy(&mymac, grnvs_get_hwaddr(fd), ETH_ALEN);
	int etherType0800Frames = 0;
	int etherType0806Frames = 0;
	int etherType86ddFrames = 0;
	int etherType0000Frames = 0;
	int etherTypeffffFrames = 0;
	int etherType0800Bytes = 0;
	int etherType0806Bytes = 0;
	int etherType86ddBytes = 0;
	int etherType0000Bytes = 0;
	int etherTypeffffBytes = 0; 
	int multicast = 0;
	int notMulticast = 0;
/*===========================================================================*/

	/* This is the ready marker! do not remove! */
	fprintf(stdout, "I am ready!\n");

/*====================================TODO===================================*/
	/* Update the loop condition */
	while(1) {
/*===========================================================================*/
		ret = grnvs_read(fd, recbuffer, sizeof(recbuffer), &timeout);
		if (ret == 0) {
			fprintf(stderr, "Timed out, this means there was nothing to receive. Do you have a sender set up?\n");
			break;
		}
/*====================================TODO===================================*/
	/* This is the receive loop, 'recbuffer' will contain the received
	 * frame. 'ret' tells you the length of what you received.
	 * Anything that should be done with every frame that's received
	 * should be done here.
	 */
		uint8_t etherTypeF = recbuffer[12];
		uint8_t etherTypeB = recbuffer[13];
		if(etherTypeF == 8 && etherTypeB == 0){

			etherType0800Bytes += ret;
			etherType0800Frames++;
		}
		if(etherTypeF == 8 && etherTypeB == 6){

			etherType0806Bytes += ret;
			etherType0806Frames++;
		}
		if(etherTypeF == 134 && etherTypeB == 221){

			etherType86ddBytes += ret;
			etherType86ddFrames++;
		}
		if(etherTypeF == 0 && etherTypeB == 0){

			etherType0000Bytes += ret;
			etherType0000Frames++;
		}
		if(etherTypeF == 255 && etherTypeB == 255){

			etherTypeffffBytes += ret;
			etherTypeffffFrames++;
		}
		if(recbuffer[0] > 127){

			multicast++;
		}
		else{

			notMulticast++;
		}
/*===========================================================================*/
	}

/*====================================TODO===================================*/
	/* Print your summary here */
	if(etherType0800Frames > 0){

		printf("0x0800: %d frames, %d bytes\n", etherType0800Frames, etherType0800Bytes);
	}
	if(etherType0806Frames > 0){

		printf("0x0806: %d frames, %d bytes\n", etherType0806Frames, etherType0806Bytes);
	}
	if(etherType86ddFrames > 0){

		printf("0x86dd: %d frames, %d bytes\n", etherType86ddFrames, etherType86ddBytes);
	}
	if(etherType0000Frames > 0){

		printf("0x0000: %d frames, %d bytes\n", etherType0000Frames, etherType0000Bytes);
	}
	if(etherTypeffffFrames > 0){

		printf("0xffff: %d frames, %d bytes\n", etherTypeffffFrames, etherTypeffffBytes);
	}
	printf("%d of them were for me\n", notMulticast);
	printf("%d of them were multicast\n", multicast);
	int allBytes = etherType0000Bytes + etherType0800Bytes + etherType0806Bytes + etherType86ddBytes + etherTypeffffBytes;
	printf("IPv4 accounted for %f%% and IPv6 for %f%% of traffic\n", allBytes/etherType0800Bytes, allBytes/etherType0806Bytes);
/*===========================================================================*/
}

int main(int argc, char ** argv)
{
	struct arguments args;
	int sock;

	setvbuf(stdout, NULL, _IOLBF, 0);

	if ( parse_args(&args, argc, argv) < 0 ) {
		fprintf(stderr, "Failed to parse arguments, call with "
			"--help for more information\n");
		return -1;
	}

	if ( (sock = grnvs_open(args.interface, SOCK_RAW)) < 0 ) {
		fprintf(stderr, "grnvs_open() failed: %s\n", strerror(errno));
		return -1;
	}

	assignment2(sock, args.frames);

	grnvs_close(sock);

	return 0;
}
