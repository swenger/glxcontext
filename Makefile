APP:=glxtest
SHELL:=sh
CXX:=g++ -Wall -O2
LD:=g++ -lX11 -lGL
MAKEDEP:=g++ $(CXX) -M

SOURCES:=$(wildcard *.cpp)
OBJECTS:=$(SOURCES:.cpp=.o)
DEPS:=$(SOURCES:.cpp=.d)

.PHONY: all clean

all: $(APP)

$(APP): $(OBJECTS)
	$(LD) $^ -o $@

$(OBJECTS): %.o: %.cpp Makefile
	$(CXX) -c $< -o $@

$(DEPS): %.d: %.cpp Makefile
	@$(SHELL) -ec '$(CXX) -M $< | sed "s/$*.o/& $@/g" > $@'

clean:
	@rm -rf $(APP) $(OBJECTS) $(DEPS) *~

-include $(SOURCES:.cpp=.d)

