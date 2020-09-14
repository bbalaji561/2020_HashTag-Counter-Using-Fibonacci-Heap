hashtagcounter: hashtagcounter.o
	g++ hashtagcounter.o -o hashtagcounter
hashtagcounter.o: hashtag.cpp
	g++ -c hashtag.cpp -o hashtagcounter.o
clean:
	rm *.o hashtagcounter
