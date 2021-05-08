# MITROI EDUARD IONUT 312CA
build: image_editor clean

image_editor: image_editor.o functions.o
		gcc -Wall -Wextra functions.o image_editor.o -o image_editor -lm

image_editor.o: image_editor.c
		gcc -c image_editor.c

functions.o: functions.c
		gcc -c functions.c

clean:
		rm -f *.o