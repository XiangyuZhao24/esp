#ifndef _KPROP_H_
#define _KPROP_H_

#ifdef __KERNEL__
#include <linux/ioctl.h>
#include <linux/types.h>
#else
#include <sys/ioctl.h>
#include <stdint.h>
#ifndef __user
#define __user
#endif
#endif /* __KERNEL__ */

#include <esp.h>
#include <esp_accelerator.h>

struct kprop_access {
	struct esp_access esp;
	/* <<--regs-->> */
	unsigned input_dimension;
	unsigned nodes_per_layer;
	unsigned possible_outputs;
	unsigned learning_rate;
	unsigned training_sets;
	unsigned src_offset;
	unsigned dst_offset;
};

#define KPROP_IOC_ACCESS	_IOW ('S', 0, struct kprop_access)

#endif /* _KPROP_H_ */
