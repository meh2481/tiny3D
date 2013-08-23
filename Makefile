objects := main.o tiny3d.o

all: convert

convert: $(objects)
	g++ -o $@ $^

%.o: %.cpp
	g++ -c -MMD -o $@ $<

-include $(objects:.o=.d)

clean:
	rm -f *.o *.d convert.exe
