#Set this to @ to keep the makefile quiet


UNIXTOOLS = "C:\Program Files (x86)\Git\usr\bin\"
RUNTEST=false
RUNPROD=true

ifeq ($(MAKECMDGOALS),start)
	RUNTEST=true
	RUNPROD=false
endif

ifeq ($(MAKECMDGOALS),test)
	RUNTEST=true
	RUNPROD=false
endif

ifeq ($(MAKECMDGOALS),clean)
	RUNTEST=false
	RUNPROD=true
endif

ifeq ($(RUNTEST),true)
	SILENCE = @

	TEST_SRC_DIRS = tests
	TEST_SRC_DIRS += tests/hellotest

		

	SRC_DIRS += src/hellotest

			
	INCLUDE_DIRS = include/
	INCLUDE_DIRS += $(CPPUTEST_HOME)/include/
	INCLUDE_DIRS += tests/
	
	LD_LIBRARIES = -L$(CPPUTEST_HOME)/lib -lCppUTest  -lCppUTestExt -lpthread.dll
	
	CPPUTEST_HOME=C:\Users\ark\entwicklung\eclipse_workspace\cpputest-3.6
	
	CPPUTEST_OBJS_DIR=buildtest/obj
	CPPUTEST_LIB_DIR=buildtest/lib
	#---- Outputs ----#
	COMPONENT_NAME = rpcFreeRTOS_WTTemplate
	#--- Inputs ----#
	PROJECT_HOME_DIR = .

	CPP_PLATFORM = Gcc
	CPPUTEST_EXE_FLAGS =
	CPPUTEST_WARNINGFLAGS += -Wall 
	CPPUTEST_WARNINGFLAGS += -Werror 
	CPPUTEST_WARNINGFLAGS += -Wswitch-default 
	CPPUTEST_WARNINGFLAGS += -Wswitch-enum  
	CPPUTEST_CFLAGS += -std=c99
	CPPUTEST_CFLAGS += -Wextra 
	#CPPUTEST_CFLAGS += -pedantic 
	CPPUTEST_CFLAGS += -Wstrict-prototypes



 

	ifeq ($(CPPUTEST_HOME),)
		$(info CPPUTEST_HOME not set! See README.txt)
	else  
		include $(CPPUTEST_HOME)/build/MakefileWorker.mk
	endif
endif
	
ifeq ($(RUNPROD),true)
	include Makefile_arm.mk
endif


