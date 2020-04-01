#include <linux/of_device.h>
#include <linux/mm.h>

#include <asm/io.h>

#include <esp_accelerator.h>
#include <esp.h>

#include "kprop.h"

#define DRV_NAME	"kprop"

/* <<--regs-->> */
#define KPROP_INPUT_DIMENSION_REG 0x50
#define KPROP_NODES_PER_LAYER_REG 0x4c
#define KPROP_POSSIBLE_OUTPUTS_REG 0x48
#define KPROP_LEARNING_RATE_REG 0x44
#define KPROP_TRAINING_SETS_REG 0x40

struct kprop_device {
	struct esp_device esp;
};

static struct esp_driver kprop_driver;

static struct of_device_id kprop_device_ids[] = {
	{
		.name = "SLD_KPROP",
	},
	{
		.name = "eb_024",
	},
	{
		.compatible = "sld,kprop",
	},
	{ },
};

static int kprop_devs;

static inline struct kprop_device *to_kprop(struct esp_device *esp)
{
	return container_of(esp, struct kprop_device, esp);
}

static void kprop_prep_xfer(struct esp_device *esp, void *arg)
{
	struct kprop_access *a = arg;

	/* <<--regs-config-->> */
	iowrite32be(a->input_dimension, esp->iomem + KPROP_INPUT_DIMENSION_REG);
	iowrite32be(a->nodes_per_layer, esp->iomem + KPROP_NODES_PER_LAYER_REG);
	iowrite32be(a->possible_outputs, esp->iomem + KPROP_POSSIBLE_OUTPUTS_REG);
	iowrite32be(a->learning_rate, esp->iomem + KPROP_LEARNING_RATE_REG);
	iowrite32be(a->training_sets, esp->iomem + KPROP_TRAINING_SETS_REG);
	iowrite32be(a->src_offset, esp->iomem + SRC_OFFSET_REG);
	iowrite32be(a->dst_offset, esp->iomem + DST_OFFSET_REG);

}

static bool kprop_xfer_input_ok(struct esp_device *esp, void *arg)
{
	/* struct kprop_device *kprop = to_kprop(esp); */
	/* struct kprop_access *a = arg; */

	return true;
}

static int kprop_probe(struct platform_device *pdev)
{
	struct kprop_device *kprop;
	struct esp_device *esp;
	int rc;

	kprop = kzalloc(sizeof(*kprop), GFP_KERNEL);
	if (kprop == NULL)
		return -ENOMEM;
	esp = &kprop->esp;
	esp->module = THIS_MODULE;
	esp->number = kprop_devs;
	esp->driver = &kprop_driver;
	rc = esp_device_register(esp, pdev);
	if (rc)
		goto err;

	kprop_devs++;
	return 0;
 err:
	kfree(kprop);
	return rc;
}

static int __exit kprop_remove(struct platform_device *pdev)
{
	struct esp_device *esp = platform_get_drvdata(pdev);
	struct kprop_device *kprop = to_kprop(esp);

	esp_device_unregister(esp);
	kfree(kprop);
	return 0;
}

static struct esp_driver kprop_driver = {
	.plat = {
		.probe		= kprop_probe,
		.remove		= kprop_remove,
		.driver		= {
			.name = DRV_NAME,
			.owner = THIS_MODULE,
			.of_match_table = kprop_device_ids,
		},
	},
	.xfer_input_ok	= kprop_xfer_input_ok,
	.prep_xfer	= kprop_prep_xfer,
	.ioctl_cm	= KPROP_IOC_ACCESS,
	.arg_size	= sizeof(struct kprop_access),
};

static int __init kprop_init(void)
{
	return esp_driver_register(&kprop_driver);
}

static void __exit kprop_exit(void)
{
	esp_driver_unregister(&kprop_driver);
}

module_init(kprop_init)
module_exit(kprop_exit)

MODULE_DEVICE_TABLE(of, kprop_device_ids);

MODULE_AUTHOR("Emilio G. Cota <cota@braap.org>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("kprop driver");
