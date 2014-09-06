/****************************************************************************************************************
 File :        main.c

 Version :      1.0

 Date:          09/06/2014

 Author:        fplepp

 Description:   Simple cmd line tool to fingerprint songs

****************************************************************************************************************/
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include "GNMusicService.h"

#define INBUFF 32768

int main(int argc, char* argv[])
{
	GN_Music_Service Test1;
	std::ifstream User_handle_file;
	std::ifstream Music_file_wav;
	std::ofstream User_handle_save;
	std::string SerializedUser;
	unsigned int i;
	unsigned int timeslice;

	unsigned char buf[INBUFF];  /* input buffer  */
	unsigned char * PCM_buffer;
	unsigned int buffersize;
	unsigned int total_len;
	unsigned int len;
	unsigned int wait_iteration;


	/* Check whether all parameters are present */
	if (argc != 5)
	{
		printf("\nUsage:\n%s clientid clientidtag license savefile\n", argv[0]);
		return -1;
	}

	/* Set the License */
	Test1.Set_Client_ID(argv[1]);
	Test1.Set_Client_ID_Tag(argv[2]);
	Test1.Set_License_File(argv[3]);

	User_handle_file.open("user_handle.txt", std::fstream::in);

	if (User_handle_file.good())
	{
		getline(User_handle_file,SerializedUser);
		User_handle_file.close();
		printf("User Handle File loaded: %s\n", SerializedUser.c_str());
		printf("Length Serialized USer String : %d ",SerializedUser.length());
		Test1.Init(SerializedUser.c_str());
	}
	else
	{
		printf("No User handle\n");
		/*init GN service without a user handle*/
		Test1.Init(NULL);
	}

	Test1.SetupLocate(false);

	/*Set Audio to SR44100, 2 channels, 16 bin*/
	Test1.Set_Audio(2,16,44100);

	timeslice = 44100*2*2; // one second of audio 16

	/*calc size of buffer */
	buffersize = timeslice*4*sizeof(short);

	/*allocate a 4 seconds buffer*/
	PCM_buffer = (unsigned char *) malloc(timeslice*4*sizeof(short));

	total_len =0;
	while (1)
	{

		len = read(0,buf,INBUFF);
		for (i=0;i<len;i++)
		{
			if (total_len<buffersize)
			{
				PCM_buffer[total_len++] = buf[i];
			}
		}
		if (total_len >= buffersize)
		{
			printf("Doing a query \n");
			Test1.MusicID_stream(argv[4],PCM_buffer,timeslice*4);
			total_len = 0;
			wait_iteration = 0;
			while(wait_iteration<3)
			{
				len = read(0,PCM_buffer,buffersize);
				wait_iteration++;
			}
		}
	}


	free(PCM_buffer);

	return 0;
}
