# Make file for imap program
# Structure was taken from Anita Devi and Rini Simon's make file
# from milestone 2.

SOURCE_FILES = \
ass5/Ass5.java \

JAVAC = javac -source 1.8 -sourcepath src/ 
JFLAGS = -encoding UTF-8

vpath %.class bin
vpath %.java src

build: $(SOURCE_FILES:.java=.class)

# pattern rule
%.class: %.java
	$(JAVAC) -cp .:lib/gnuprologjava-0.2.6.jar:bin -d bin $(JFLAGS) $<

clean:
	rm -frv bin/*