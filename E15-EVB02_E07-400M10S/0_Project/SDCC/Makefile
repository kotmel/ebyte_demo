.POSIX:

#Compiler
CC = sdcc

#Platform
PLATFORM = stm8
PROCESSOR=stm8l151g4
PROBE=stlinkv2

TARGET = main


LIBROOT=../../4_STM8_L15x_StdPeriph_Drivers

ODIR_LIB = bin_lib
ODIR = bin


MAINSRC =../IAR_for_Stm8/ExMx_Demo/$(TARGET).c


EXTRASRCS = \
	$(LIBROOT)/src/stm8l15x_gpio.c \
	$(LIBROOT)/src/stm8l15x_tim3.c \
	$(LIBROOT)/src/stm8l15x_usart.c \
	$(LIBROOT)/src/stm8l15x_clk.c \
	$(LIBROOT)/src/stm8l15x_spi.c \
	$(LIBROOT)/src/stm8l15x_exti.c \
	$(LIBROOT)/src/stm8l15x_comp.c \
	$(LIBROOT)/src/stm8l15x_syscfg.c \
	../IAR_for_Stm8/ExMx_Demo/stm8l15x_it.c \
	../../1_Middleware/Kfifo/ebyte_kfifo.c \
	../../1_Middleware/Produce/ebyte_debug.c \
	../../2_Ebyte_Board_Support/E15-EVB02/board_button.c \
	../../2_Ebyte_Board_Support/E15-EVB02/board_mini_printf.c \
	../../2_Ebyte_Board_Support/E15-EVB02/board.c \
	../../3_Ebyte_WirelessModule_Drivers/ebyte_core.c \
	../../3_Ebyte_WirelessModule_Drivers/E07xMx/ebyte_callback.c \
	../../3_Ebyte_WirelessModule_Drivers/E07xMx/ebyte_e07x.c \
	../../3_Ebyte_WirelessModule_Drivers/E07xMx/ebyte_port.c 

# The list of .rel files can be derived from the list of their source files
RELS = $(EXTRASRCS:.c=.rel)

INCLUDES = -I$(LIBROOT)/inc \
       -I$(LIBROOT) \
		   -I. \
		   -I../IAR_for_Stm8/ExMx_Demo \
		   -I../../1_Middleware/Kfifo \
		   -I../../1_Middleware/Produce \
		   -I../../2_Ebyte_Board_Support/E15-EVB02 \
		   -I../../3_Ebyte_WirelessModule_Drivers \
		   -I../../3_Ebyte_WirelessModule_Drivers/E07xMx

CFLAGS   = -m$(PLATFORM) -DSTM8L15x -DSTM8L15X_LD -DEBYTE_E07_900M10S
LIBS     = -l$(PLATFORM)


all: $(TARGET)

$(TARGET): $(MAINSRC) $(RELS)
	@mkdir -p $(ODIR)
	$(CC) $(INCLUDES) $(CFLAGS) $(LIBS) $(MAINSRC) $(wildcard $(ODIR_LIB)/*.rel) -o$(ODIR)/

.c.rel:
	@mkdir -p $(ODIR_LIB)
	$(CC) -c $(INCLUDES) $(CFLAGS) $(LIBS) $< -o$(ODIR_LIB)/

.SUFFIXES: .c .rel

.PHONY:	clean flash

clean:
	@echo "Remove $(ODIR)..."
	@rm -rf $(ODIR)
	@echo "Remove $(ODIR_LIB)..."
	@rm -rf $(ODIR_LIB)
	@echo "Done."
flash:
	stm8flash -c$(PROBE) -p$(PROCESSOR) -w$(ODIR)/$(TARGET).ihx
