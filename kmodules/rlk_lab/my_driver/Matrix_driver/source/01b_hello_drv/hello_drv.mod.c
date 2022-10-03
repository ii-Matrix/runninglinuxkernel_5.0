#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x58260648, "module_layout" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x54416ab7, "cdev_del" },
	{ 0xcc9f2e83, "class_destroy" },
	{ 0x33b3814f, "device_destroy" },
	{ 0xdecd0b29, "__stack_chk_fail" },
	{ 0x61033c1, "device_create" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x18383af5, "__class_create" },
	{ 0x7215701e, "cdev_add" },
	{ 0xceec4a6d, "cdev_init" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xdcb764ad, "memset" },
	{ 0x12a4e128, "__arch_copy_from_user" },
	{ 0x6cbbfc54, "__arch_copy_to_user" },
	{ 0x56470118, "__warn_printk" },
	{ 0xc5850110, "printk" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

