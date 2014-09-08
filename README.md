#Description
A small wrapper to implement stream based fingerprinting, with an example which takes a PCM stream as an input.
The code has been part of a hack coded in the Music Hackday Berlin 2014, it includes some last minute lines, which are just implemented to make it work, so don't wonder why some code passages are looking creepy ;) 


#how to use
musicident clientid clientidtag license logfile

Comment: Set the LD_LIBRARY_PATH accordingly

The code snipped hasn't implemented any decoder like ffmpeg yet, so you have to decode the stream with an external program 

e.g for mp3 streams:

mpg123 http://mystream -s | ./musicident clientid ClientTAG license.txt logfile.txt

#How to build

Go to <a href="http://developer.gracenote.com">Gracenote Developer</a>, register and download the GNSDK and the license


mkdir include

mkdir shared_objects

copy the dynamic libraries from GNSDK/lib/arch into a shared_objects folder

copy the include files from GNSDK/include into your project /include folder

copy the include file from GNSDK/include/arch into your project /include folder 

make all






