CODE_DIR = mcu

.PHONY: all clean program

all:
	$(MAKE) -C $(CODE_DIR) all
	
clean:
	$(MAKE) -C $(CODE_DIR) clean	

program:
	$(MAKE) -C $(CODE_DIR) program	
		