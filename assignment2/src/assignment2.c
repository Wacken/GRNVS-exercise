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
	int allEtherBytes[65535];
	int allEtherFrames[65535];
	
	int multicast = 0;
	int notMulticast = 0;
	int allBytes = 0;
/*===========================================================================*/

	/* This is the ready marker! do not remove! */
	fprintf(stdout, "I am ready!\n");

/*====================================TODO===================================*/
	/* Update the loop condition */
	while(frames-- > 0) {
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
		int index = etherTypeF << 8 | etherTypeB;
		allEtherBytes[index] += ret;
		allEtherFrames[index]++;
		allBytes += ret;
		if((recbuffer[0] & (1<<7)) == 1)//recbuffer[0] & 1000 0000
		{
			multicast++;
		}
		else 
		{
			notMulticast++;
		}
/*===========================================================================*/
	}

/*====================================TODO===================================*/
	/* Print your summary here */
	if(allEtherFrames[2048] > 0)
	{
		printf("0x0800: %d frames, %d bytes\n", allEtherFrames[2048], allEtherBytes[2048]);
	}
	if(allEtherFrames[2054] > 0){

		printf("0x0806: %d frames, %d bytes\n", allEtherFrames[2054], allEtherBytes[2054]);
	}
	if(allEtherFrames[34525] > 0){

		printf("0x86dd: %d frames, %d bytes\n", allEtherFrames[34525], allEtherBytes[34525]);
	}
	printf("%d of them were for me\n", notMulticast);
	printf("%d of them were multicast\n", multicast);
	if(allBytes > 0)
	{
		printf("IPv4 accounted for %f%% and IPv6 for %f%% of traffic\n", 100.0 * allEtherBytes[2048]/allBytes, 100.0 * allEtherBytes[34525]/allBytes);
	}
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
