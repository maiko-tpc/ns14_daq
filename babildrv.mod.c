#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x41086e, "module_layout" },
	{ 0x5af0314, "module_refcount" },
	{ 0x2bc95bd4, "memset" },
	{ 0x642e54ac, "__wake_up" },
	{ 0xd9c0e271, "sbs620_check_interrupt" },
	{ 0x1d2e87c6, "do_gettimeofday" },
	{ 0x5b005e78, "sbs620_define_intlevel" },
	{ 0xedd040c3, "sbs620_read_intvector" },
	{ 0x338d66dc, "sbs620_enable_interrupt" },
	{ 0x4b07e779, "_spin_unlock_irqrestore" },
	{ 0x712aa29b, "_spin_lock_irqsave" },
	{ 0x2da418b5, "copy_to_user" },
	{ 0xfecd3026, "sbs620_set_amsr" },
	{ 0x5ee7b5ea, "sbs620_vwrite32" },
	{ 0x2e60bace, "memcpy" },
	{ 0x52e2e538, "sbs620_dma_vread32_store" },
	{ 0xef7d8f40, "sbs620_dma_vread32_start" },
	{ 0xfcf870be, "sbs620_vwrite16" },
	{ 0x592fc7a4, "sbs620_vread16" },
	{ 0x51ea5b85, "sbs620_vread32" },
	{ 0x859c6dc7, "request_threaded_irq" },
	{ 0xffc7c184, "__init_waitqueue_head" },
	{ 0x4b46f0b0, "sbs620_get_irq" },
	{ 0xcfdbba1a, "__register_chrdev" },
	{ 0x31f9d4b, "sbs620_disable_interrupt" },
	{ 0xb72397d5, "printk" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0xf20dabd8, "free_irq" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=bb-sbs620";


MODULE_INFO(srcversion, "658CEF6745A8FEBE97A1969");

static const struct rheldata _rheldata __used
__attribute__((section(".rheldata"))) = {
	.rhel_major = 6,
	.rhel_minor = 6,
};
