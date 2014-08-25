FLAGS=-fcx-limited-range -fPIC -DPIC -std=c++11
all:
	g++ $(FLAGS) -c pitch.cc
	g++ $(FLAGS) -c musicalscale.cc
	g++ $(FLAGS) -c smb-pitch-shift.cc
	gcc -shared $(FLAGS) pitch-detection.cc `pkg-config --cflags --libs lv2` -o pitch-detection.so pitch.o musicalscale.o smb-pitch-shift.o

clean:
	rm *.o pitch-detection.so
