# Module name (without .ko)
obj-m := sndw_probe.o

# Kernel source directory
KDIR := /lib/modules/$(shell uname -r)/build

# Current directory
PWD := $(shell pwd)

# Build target
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Clean target
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean