LIBS = -lGL -lGLEW -lglfw -lGLU -lglut #-lm -lXrandr -lXi -lX11 -lXxf86vm -lpthread -ldl -lXinerama -lXcursor -lrt
BDIR = build

all:
	mkdir -p ${BDIR}
	g++ dragon.cpp -o ${BDIR}/dragon ${LIBS}
	g++ pentagon.cpp -o ${BDIR}/pentagon ${LIBS}