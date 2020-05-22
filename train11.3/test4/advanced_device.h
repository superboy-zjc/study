#ifndef _ADVANCED_DEVICE_H_
#define _ADVANCED_DEVICE_H_

#include<linux/device.h>
#include<linux/ioport.h>

#define PROBTBSIZE 5
struct advanced_device {
	int id;
	struct device dev;
	struct resource *resource;
};

extern int advanced_device_register(struct advanced_device *);
extern void advanced_device_unregister(struct advanced_device *);

struct advanced_driver {
	struct device_driver driver;
	int prob_tb[PROBTBSIZE];
};

extern int advanced_driver_register(struct advanced_driver *);
extern void advanced_driver_unregister(struct advanced_driver *);
#endif
