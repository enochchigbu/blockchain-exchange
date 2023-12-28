#    Single Programmer Affidavit
#    I the undersigned promise that the submitted assignment is my own work. While I was
#    free to discuss ideas with others, the work contained is my own. I recognize that
#    should this not be the case; I will be subject to penalties as outlined in the course
#    syllabus.
#    Name: Enoch Chigbu
#    Red ID: 827078077
#
#    Name: Farhan Talukder
#    Red ID: 827061320


# CXX Make variable for compiler
CC=g++
# -std=c++11  C/C++ variant to use, e.g. C++ 2011
# -Wall       show the necessary warning files
# -g3         include information for symbolic debugger e.g. gdb 
CCFLAGS=-std=c++11 -Wall -g3 -c

# object files
OBJS = consumer.o producer.o report.o main.o 

# Program name
PROGRAM = tradecrypto

# The program depends upon its object files
$(PROGRAM) : $(OBJS)
	$(CC) -pthread -o $(PROGRAM) $(OBJS)

main.o : main.cpp
	$(CC) $(CCFLAGS) main.cpp
	
consumer.o : consumer.h consumer.cpp
	$(CC) $(CCFLAGS) consumer.cpp

producer.o : producer.cpp producer.h
	$(CC) $(CCFLAGS) producer.cpp

report.o: report.cpp report.h
	$(CC) $(CCFLAGS) report.cpp

# Once things work, people frequently delete their object files.
# If you use "make clean", this will do it for you.
# As we use gnuemacs which leaves auto save files termintating
# with ~, we will delete those as well.
clean :
	rm -f *.o *~ $(PROGRAM)

