TARGET = server
OBJECT = test.o open62541.o
CFLAGS = -g -std=c99 -lrt
$(TARGET):$(OBJECT)
	gcc -o $@ $^ $(CFLAGS) -lpthread
	
$(OBJECT):%.o:%.c
	 gcc -c $(CFLAGS) $< -o $@
	 
clean:
	rm -f $(TARGET) $(OBJECT)

