
override INCLUDES += -I..
override CFLAGS += -pipe $(INCLUDES)

CXX = c++

.PHONY: clean

all: $(patsubst %.test.cpp,%.test,$(wildcard *.cpp))
	@echo Done

%.test: %.test.cpp
	$(CXX) $(CFLAGS) $(LIBS) -MD $< -o $@

include $(wildcard *.d)

clean:
	rm -f *.test *.d
