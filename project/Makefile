PROGRAM := moto

source_dirs := ../../src ../../src/common ../../src/motocore ../../src/gui

override INCLUDES += `pkg-config --cflags gthread-2.0 gtkglext-1.0`
override CFLAGS += -pipe $(INCLUDES) -DWITH_MOTO_WOBJ_MESH_LOADER
override LIBS   += `pkg-config --libs gthread-2.0 gtkglext-1.0 gio-2.0` -lGL -lGLU

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
