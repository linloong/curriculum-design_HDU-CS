obj-m:=m1.o    
m1-objs:=lab2_1.o
KDIR:= /lib/modules/$(shell uname -r)/build
PWD:= $(shell pwd) 

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules  
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
