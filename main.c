#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include "nmrpd.h"

void usage(FILE *fp)
{
	fprintf(fp,
			"Usage: nmrpd [OPTIONS...]\n"
			"\n"
			"Options:\n"
			" -a <ipaddr>     IP address to assign to target device\n"
			" -f <firmware>   Firmware file\n"
			" -i <interface>  Network interface directly connected to device\n"
			" -m <mac>        MAC address of target device (xx:xx:xx:xx:xx:xx)\n"
			" -M <netmask>    Subnet mask to assign to target device\n"
			" -t <timeout>    Timeout (in milliseconds) for regular messages\n"
			" -T <timeout>    Time to wait after successfull TFTP upload\n"
			" -p <port>       Port to use for TFTP upload\n"
			" -h              Show this screen\n"
			"\n"
			"Options -a, -i and -f are mandatory!\n"
			"\n"
	  );
}

int main(int argc, char **argv)
{
	int c, val, max;
	struct nmrpd_args args = {
		.rx_timeout = 200,
		.ul_timeout = 60000,
		.filename = NULL,
		.ipaddr = NULL,
		.ipmask = "255.255.255.0",
		.intf = NULL,
		.mac = "ff:ff:ff:ff:ff:ff",
		.op = NMRP_UPLOAD_FW,
		.port = 69,
		.force_root = 1
	};

	opterr = 0;

	while ((c = getopt(argc, argv, "a:f:i:m:M:p:t:T:")) != -1) {
		max = 0xffffffff;
		switch (c) {
			case 'a':
				args.ipaddr = optarg;
				break;
			case 'f':
				args.filename = optarg;
				break;
			case 'i':
				args.intf = optarg;
				break;
			case 'm':
				args.mac = optarg;
				break;
			case 'M':
				args.ipmask = optarg;
				break;
			case 'p':
				max = 0xffff;
			case 'T':
			case 't':
				val = atoi(optarg);
				if (val <= 0 || val > max) {
					fprintf(stderr, "Invalid numeric value for -%c.\n", c);
					return 1;
				}

				if (c == 'p') {
					args.port = val;
				} else if (c == 't') {
					args.rx_timeout = val;
				} else {
					args.ul_timeout = val;
				}

				break;
			case 'h':
				usage(stdout);
				return 0;
			default:
				usage(stderr);
				return 1;
		}
	}

	if (!args.filename || !args.intf || !args.ipaddr) {
		usage(stderr);
		return 1;
	}

	return nmrp_do(&args);
}