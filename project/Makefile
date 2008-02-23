PROGRAM := moto

source_dirs := ../../src/

override INCLUDES += `pkg-config --cflags gtkglext-1.0`
override CFLAGS += -pipe $(INCLUDES)
override LIBS   += `pkg-config --libs gtkglext-1.0`

CC = gcc

.PHONY: clean clean_obj clean_dep

VPATH := $(source_dirs)

$(PROGRAM): $(notdir $(patsubst %.c,%.o,$(wildcard $(addsuffix /*.c,$(source_dirs)))))
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c -MD $(addprefix -I,$(source_dirs)) $<

include $(wildcard *.d)

clean: clean_obj clean_dep
	rm -f $(PROGRAM)
clean_obj:
	rm -f *.o
clean_dep:
	rm -f *.d
