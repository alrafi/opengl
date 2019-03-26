LIBS = -lglfw3 -lGL -lm -lXrandr -lXi -lX11 -lXxf86vm -lpthread -lGLEW
#-lGL -lGLEW -lglfw3 -lGLU -lglut #-lm -lXrandr -lXi -lX11 -lXxf86vm -lpthread -ldl -lXinerama -lXcursor -lrt
BDIR = build

all:
	mkdir -p ${BDIR}
	g++ dragon.cpp -o ${BDIR}/dragon ${LIBS}
	g++ pentagon.cpp -o ${BDIR}/pentagon ${LIBS}