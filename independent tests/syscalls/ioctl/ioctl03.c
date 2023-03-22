#include "incl.h"
#ifndef TUNGETFEATURES
#define TUNGETFEATURES _IOR('T', 207, unsigned int)
#endif
#ifndef IFF_VNET_HDR
#define IFF_VNET_HDR	0x4000
#endif
#ifndef IFF_MULTI_QUEUE
#define IFF_MULTI_QUEUE	0x0100
#endif
#ifndef IFF_NAPI
#define IFF_NAPI	0x0010
#endif
#ifndef IFF_NAPI_FRAGS
#define IFF_NAPI_FRAGS	0x0020
#endif
#ifndef IFF_NO_CARRIER
#define IFF_NO_CARRIER	0x0040
#endif

struct {
	unsigned int flag;
	const char *name;
} known_flags[] = {
	{IFF_TUN, "TUN"},
	{IFF_TAP, "TAP"},
	{IFF_NO_PI, "NO_PI"},
	{IFF_ONE_QUEUE, "ONE_QUEUE"},
	{IFF_VNET_HDR, "VNET_HDR"},
	{IFF_MULTI_QUEUE, "MULTI_QUEUE"},
	{IFF_NAPI, "IFF_NAPI"},
	{IFF_NAPI_FRAGS, "IFF_NAPI_FRAGS"},
	{IFF_NO_CARRIER, "IFF_NO_CARRIER"}
};

int  verify_features(
{
	unsigned int features, i;
	int netfd = open("/dev/net/tun", O_RDWR);
	if (netfd == -1 && (errno == ENODEV || errno == ENOENT))
		netfd = open("/dev/tun", O_RDWR);
	if (netfd == -1) {
		if (errno == ENODEV || errno == ENOENT)
			tst_brk(TCONF, "TUN support is missing?");
		tst_brk(TBROK | TERRNO, "opening /dev/net/tun failed");
	}
	ioctl(netfd, TUNGETFEATURES, &features);
	tst_res(TINFO, "Available features are: %#x", features);
	for (i = 0; i < ARRAY_SIZE(known_flags); i++) {
		if (features & known_flags[i].flag) {
			features &= ~known_flags[i].flag;
			tst_res(TPASS, "%s %#x", known_flags[i].name,
				 known_flags[i].flag);
		}
	}
	if (features)
		tst_res(TFAIL, "(UNKNOWN %#x)", features);
	close(netfd);
}

void main(){
	setup();
	cleanup();
}
