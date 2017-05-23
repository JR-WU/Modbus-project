TARGET = server
OBJECT = Instrument.o open62541.o
CFLAGS = -g -std=c99 -lrt -rdynamic 
$(TARGET):$(OBJECT)
	arm-linux-gcc -o $@ $^ $(CFLAGS) -lpthread 
$(OBJECT):%.o:%.c
	arm-linux-gcc -c $(CFLAGS) $< -o $@
	 
clean:
	rm -f $(TARGET) $(OBJECT)
	
