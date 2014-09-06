/****************************************************************************************************************
 File :         GNMusicService.h

 Version :      1.0

 Date:          09/06/2014

 Author:        fplepp

 Description:   Simple Wrapper for using MID Stream hacked during the Music HackDay Berlin 2014

****************************************************************************************************************/


#ifndef GNMUSICSERVICE_H_
#define GNMUSICSERVICE_H_


#include "gnsdk.h"

class GN_Music_Service {
public:
	/*Constructor and Destructor*/
	GN_Music_Service();
	virtual ~GN_Music_Service();

	/*Initialization */
	int Init(const char * SerializedUser); 			/* The initialization for Online Lookup*/
	int SetupLocate(bool PlayList); 				/* Use case dependent LOcale Setup*/

	/*Enable/Disable Logging */
	int LogToFile(	const char * Log_File,
					bool ErrorsWarn_Only,
					bool New_Log_File);
	int StopLogging(const char * Log_File);

	/* Feature Examples */
	int MusicID_stream(  const char	* 		 timecode,
						  void *				buffer, /* audio buffer to analyze  */
			 	 	 	  int 				buffersize); /* buffer Size              */

	/* Shutdown example*/
	gnsdk_error_t Close(char ** SerializedUser);

	/*License Management*/
	int Set_Client_ID(const char* clientID);
	int Set_Client_ID_Tag(const char* clientIDTag);
	int Set_License_File(const char* LicenseFile);

	/*  Audio Settings */
	int Set_Audio(int channels, int sample_Size, int sample_Rate );

private:
	char mClient_ID[100];
	char mClient_ID_Tag[100];
	char mLicensePath[100];
	int mAudioChannels;
	int mSampleSize;
	int mSampleRate;
	gnsdk_user_handle_t mUser_handle;

	};

#endif /* GNMUSICSERVICE_H_ */
