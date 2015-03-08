#Charles Sedgwick
#Compile commands for module_hello.c

obj-m += module_hello.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
#	rm *.o *.ko *.order *.symvers *.mod* .module*
