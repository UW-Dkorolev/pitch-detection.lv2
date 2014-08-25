all:
	g++ -fPIC -DPIC -std=c++11 -c pitch.cc
	g++ -fPIC -DPIC -std=c++11 -c musicalscale.cc
	gcc -shared -fPIC -DPIC -std=c++11 pitch-detection.cc `pkg-config --cflags --libs lv2` -o pitch-detection.so pitch.o musicalscale.o

clean:
	rm *.o pitch-detection.so
