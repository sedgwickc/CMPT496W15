#Charles Sedgwick
#Compile commands for hello_module.c

obj-m += module_restart.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
