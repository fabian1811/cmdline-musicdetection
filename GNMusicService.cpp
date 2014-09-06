/****************************************************************************************************************
 File :         GNMusicService.c

 Version :      1.0

 Date:          09/06/2014

 Author:        fplepp

 Description:   Simple Wrapper for using MID Stream hacked during the Music HackDay Berlin 2014

****************************************************************************************************************/


#include "GNMusicService.h"
#include "gnsdk.h"
#include "GNMusicServiceError.h"
//#include <dirent.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>



GN_Music_Service::GN_Music_Service() {
	// TODO Auto-generated constructor stub
	this->mAudioChannels = 2;
	this->mSampleRate = 44100;
	this->mSampleSize = 16;
	mUser_handle = GNSDK_NULL;
}

GN_Music_Service::~GN_Music_Service() {
	// TODO Auto-generated destructor stub
}

int GN_Music_Service::Init(const char * SerializedUser) {

	gnsdk_manager_handle_t	sdkmgr_handle	= GNSDK_NULL;
	gnsdk_user_handle_t		user_handle		= GNSDK_NULL;
	gnsdk_str_t         	serialized_user  = GNSDK_NULL;
	gnsdk_error_t			error			= GNSDK_SUCCESS;
	std::ofstream 			User_handle_file;


	/* Initialize the GNSDK Manager */
	error = gnsdk_manager_initialize(	&sdkmgr_handle,
										this->mLicensePath,
										GNSDK_MANAGER_LICENSEDATA_FILENAME);

	ERROR_CHECK("gnsdk_manager_initialize FAILED \n");

	/* Initialize the Storage SQLite Library */
	error = gnsdk_storage_sqlite_initialize(sdkmgr_handle);

	ERROR_CHECK("gnsdk_storage_sqlite_initialize FAILED \n");

	/* Initialize local */
	error = gnsdk_lookup_local_initialize(sdkmgr_handle);

	ERROR_CHECK("gnsdk_lookup_local_initialize FAILED \n");

	/* Initialize the DSP Library - used for generating fingerprints */
	error = gnsdk_dsp_initialize(sdkmgr_handle);

	ERROR_CHECK("gnsdk_dsp_initialize FAILED \n");


	/* Initialize the MusicID Library */
	error = gnsdk_musicid_initialize(sdkmgr_handle);

	ERROR_CHECK("gnsdk_musicid_initialize FAILED \n");



	if (SerializedUser == NULL) /* We don't have any user */
	{

		/*register new user*/
		error = gnsdk_manager_user_register(	GNSDK_USER_REGISTER_MODE_ONLINE,
												this->mClient_ID,
												this->mClient_ID_Tag,
												"1",
												&serialized_user);

		ERROR_CHECK("gnsdk_manager_user_register FAILED \n");

		/*Save to file*/
		User_handle_file.open("user_handle.txt");
		User_handle_file.write(serialized_user,strlen(serialized_user));
		User_handle_file.close();

		error = gnsdk_manager_user_create(	serialized_user,
											this->mClient_ID,
											&user_handle);

	}
	else
	{	/*we have a stored user*/
		error = gnsdk_manager_user_create(SerializedUser, this->mClient_ID, &user_handle);
		ERROR_CHECK("gnsdk_manager_user_create FAILED \n");
	}

	/*Set up the lookup mode to online */
	error = gnsdk_manager_user_option_set(	user_handle,					/* user handle */
											GNSDK_USER_OPTION_LOOKUP_MODE,	/* Set the Lookup mode */
											GNSDK_LOOKUP_MODE_ONLINE);      /* set the lookup mode to online */




	/* Store Handle*/
	this->mUser_handle = user_handle;

	return 0;
}





int GN_Music_Service::SetupLocate(bool PlayList)
{
	gnsdk_locale_handle_t	locale_handle	= GNSDK_NULL;
	gnsdk_error_t			error			= GNSDK_SUCCESS;

	if (PlayList)
	{
	/* Specify the region in order to limit the db processing */
		error = gnsdk_manager_locale_load(	GNSDK_LOCALE_GROUP_PLAYLIST,		/* Local GNSDK product group defined for locale handle, GNSDK_LOCALE_GROUP_EPG, GNSDK_LOCALE_GROUP_MUSIC, GNSDK_LOCALE_GROUP_PLAYLIST,GNSDK_LOCALE_GROUP_VIDEO */
											GNSDK_LANG_ENGLISH,				/* Language defined for the locale handle, lot's of options   */
											GNSDK_REGION_DEFAULT,			/* Region defined for the locale handle, options are GNSDK_REGION_CHINA,GNSDK_REGION_DEFAULT,GNSDK_REGION_EUROPE_GLOBAL, GNSDK_REGION_JAPAN, GNSDK_REGION_KOREA, GNSDK_REGION_LATIN_AMERICA, GNSDK_REGION_TAIWAN, GNSDK_REGION_US*/
											GNSDK_DESCRIPTOR_SIMPLIFIED,	/* Descriptor, options are GNSDK_DESCRIPTOR_SIMPLIFIED, GNSDK_DESCRIPTOR_DETAILED, GNSDK_DESCRIPTOR_DEFAULT,  */
											this->mUser_handle,					/* User handle */
											GNSDK_NULL,						/* User callback function for status and progress*/
											0,								/* callback function data */
											&locale_handle);					/* Return handle */

	}
	else
	{
	/* Specify the region in order to limit the db processing */
	error = gnsdk_manager_locale_load(	GNSDK_LOCALE_GROUP_MUSIC,		/* Local GNSDK product group defined for locale handle, GNSDK_LOCALE_GROUP_EPG, GNSDK_LOCALE_GROUP_MUSIC, GNSDK_LOCALE_GROUP_PLAYLIST,GNSDK_LOCALE_GROUP_VIDEO */
										GNSDK_LANG_ENGLISH,				/* Language defined for the locale handle, lot's of options   */
										GNSDK_REGION_DEFAULT,			/* Region defined for the locale handle, options are GNSDK_REGION_CHINA,GNSDK_REGION_DEFAULT,GNSDK_REGION_EUROPE_GLOBAL, GNSDK_REGION_JAPAN, GNSDK_REGION_KOREA, GNSDK_REGION_LATIN_AMERICA, GNSDK_REGION_TAIWAN, GNSDK_REGION_US*/
										GNSDK_DESCRIPTOR_SIMPLIFIED,	/* Descriptor, options are GNSDK_DESCRIPTOR_SIMPLIFIED, GNSDK_DESCRIPTOR_DETAILED, GNSDK_DESCRIPTOR_DEFAULT,  */
										this->mUser_handle,					/* User handle */
										GNSDK_NULL,						/* User callback function for status and progress*/
										0,								/* callback function data */
										&locale_handle);					/* Return handle */
	}



	ERROR_CHECK("gnsdk_manager_locale_load FAILED \n");

	/* We have to set the settings to default */
	error = gnsdk_manager_locale_set_group_default(locale_handle);

	ERROR_CHECK("gnsdk_manager_locale_set_group_default FAILED \n");

	/* Free the locale handle, we don't need it anymore */
	gnsdk_manager_locale_release(locale_handle);

	return 0;
}


int GN_Music_Service::MusicID_stream(   const char * 			filename,
										void*					buffer, /* audio buffer to analyze  */
										int 					buffersize) /* buffer Size              */
{
	gnsdk_error_t						error 					= GNSDK_SUCCESS;
	gnsdk_musicid_query_handle_t		query_handle 			= GNSDK_NULL;
	gnsdk_gdo_handle_t					response_gdo 			= GNSDK_NULL;
	gnsdk_gdo_handle_t					album_gdo 				= GNSDK_NULL;
	gnsdk_gdo_handle_t					followup_response_gdo 	= GNSDK_NULL;
	gnsdk_uint32_t						count					= 0;

	gnsdk_cstr_t						needs_decision			= GNSDK_NULL;
	gnsdk_cstr_t						is_full					= GNSDK_NULL;
	gnsdk_bool_t 						complete				= GNSDK_FALSE;
	gnsdk_gdo_handle_t					title_gdo				= GNSDK_NULL;
	gnsdk_gdo_handle_t					artist_gdo				= GNSDK_NULL;
	gnsdk_gdo_handle_t					artist_name				= GNSDK_NULL;
	gnsdk_gdo_handle_t					track_title_gdo			= GNSDK_NULL;
	gnsdk_gdo_handle_t					track_gdo				= GNSDK_NULL;

	gnsdk_cstr_t						song_title				= GNSDK_NULL;
	gnsdk_cstr_t						album_title				= GNSDK_NULL;
	gnsdk_cstr_t						album_artist			= GNSDK_NULL;
	gnsdk_cstr_t						track_artist			= GNSDK_NULL;

	FILE * 								savefile                = NULL;

	/* Create the query handle */
	error = gnsdk_musicid_query_create(	this->mUser_handle,
										GNSDK_NULL,	 /* User callback function */
										GNSDK_NULL,	 /* Optional data to be passed to the callback */
										&query_handle);

	ERROR_CHECK("gnsdk_musicid_query_create FAILED \n");


	/*do the fingerprint*/
	error = gnsdk_musicid_query_fingerprint_begin(	query_handle, /* */
													GNSDK_MUSICID_FP_DATA_TYPE_GNFPX,/* GNSDK_MUSICID_FP_DATA_TYPE_GNFPX (Gracenote Fingerprint Extraction for stream based recognition), GNSDK_MUSICID_FP_DATA_TYPE_CMX (Cantametrix, for file based recognition, takes first 16 secs)*/
													this->mSampleRate, /* Sample Rate */
													this->mSampleSize,/* Sample Size (8 bit, 16 bit or 32 bit)*/
													this->mAudioChannels); /* number of channels 1 or 2*/

	ERROR_CHECK("gnsdk_musicid_query_fingerprint_begin FAILED \n");




	/* This function shall be called in a loop until the status is set to complete, in this example we are passing the whole audio to it*/
	error = gnsdk_musicid_query_fingerprint_write(	query_handle,
													buffer,      /* Audio buffer */
													buffersize,  /* data size in bytes*/
													&complete);  /* feedback whether it was enough data to determine a fingerprint*/

	ERROR_CHECK("gnsdk_musicid_query_fingerprint_write FAILED \n");


	/* Call the routine when the audio stream ends*/
	error = gnsdk_musicid_query_fingerprint_end(query_handle);

	ERROR_CHECK("gnsdk_musicid_query_fingerprint_end FAILED \n");

	error =  gnsdk_musicid_query_option_set(	query_handle,
												GNSDK_MUSICID_OPTION_RESULT_SINGLE,
												GNSDK_VALUE_TRUE);


	/* Perform the query */
	error = gnsdk_musicid_query_find_albums(	query_handle, /* query handle*/
												&response_gdo /* response gracenote data object (response GDO) */
											);
	ERROR_CHECK("gnsdk_musicid_query_find_albums FAILED \n");


	/* let's see how many results we have got, GDO is a Gracenote Data Object */
	error = gnsdk_manager_gdo_child_count(	response_gdo, 			/* response GDO*/
											GNSDK_GDO_CHILD_ALBUM, 	/*Key to get the album*/
											&count);				/* Pointer to receive the result*/

	ERROR_CHECK("gnsdk_manager_gdo_child_count FAILED \n");


	/* let's see what we have got */
	if (count == 0)
	{
		printf("No matching album found\n");
	}
	else
	{


		/* lets see whether we have to decide */
		error = gnsdk_manager_gdo_value_get(	response_gdo,  								/*handle to GDO*/
												GNSDK_GDO_VALUE_RESPONSE_NEEDS_DECISION, 	/*Key to the decision*/
												1,											/*instance, starts with 1*/
												&needs_decision);							/* Pointer to receive the result*/

		ERROR_CHECK("gnsdk_manager_gdo_value_get FAILED \n");


		/* Get the child GDO which is on position 1 or we have selected */
		error = gnsdk_manager_gdo_child_get(	response_gdo,  			/*handle to GDO*/
												GNSDK_GDO_CHILD_ALBUM,	/*key to get the album*/
												1,			/*instance, starts with 1*/
												&album_gdo);			/* Pointer to receive the result*/

		ERROR_CHECK("gnsdk_manager_gdo_child_get Album FAILED \n");

		/* Let' see whether we get a partial GDO or a full one */
		error = gnsdk_manager_gdo_value_get(	album_gdo, 						/*handle to GDO*/
												GNSDK_GDO_VALUE_FULL_RESULT, 	/*key to get the full gdo*/
												1,								/*instance, starts with 1*/
												&is_full);						/* Pointer to receive the result*/

		ERROR_CHECK("gnsdk_manager_gdo_value_get FAILED \n");

		if (0 == strcmp(is_full, GNSDK_VALUE_FALSE)) /* we have got only partial data */
		{
			/* do followup query to get full object. Setting the partial album as the query input. */
			error = gnsdk_musicid_query_set_gdo(	query_handle, /* query handle*/
													album_gdo);

			//ERROR_CHECK("gnsdk_musicid_query_set_gdo FAILED \n");
			/* we can now release the partial album */
			gnsdk_manager_gdo_release(album_gdo);
			album_gdo = GNSDK_NULL;

			error = gnsdk_musicid_query_find_albums(	query_handle,				/* query handle	*/
														&followup_response_gdo); 	/*pointer to requested gdo*/

			error = gnsdk_manager_gdo_child_get(	followup_response_gdo, 	/*handle to GDO	*/
													GNSDK_GDO_CHILD_ALBUM,	/*get the album	*/
													1,						/*instance, starts with 1*/
													&album_gdo);			/*Pointer to receive the result	*/

			/* Release the followup query's response object */
			gnsdk_manager_gdo_release(followup_response_gdo);

		}; /* Endif partial data*/

		/*Get the title, which is located album->title*/



		error = gnsdk_manager_gdo_child_get(	album_gdo, 						/*handle to GDO*/
												GNSDK_GDO_CHILD_TITLE_OFFICIAL,	/* key to get the official title*/
												1,								/*instance, starts with 1*/
												&title_gdo);					/* Pointer to receive the result*/

		error = gnsdk_manager_gdo_value_get( 	title_gdo, 						/*handle to GDO*/
												GNSDK_GDO_VALUE_DISPLAY,		/* key to get the value*/
												1,								/*instance, starts with 1*/
												&album_title );						/* Pointer to receive the result*/

		error = gnsdk_manager_gdo_child_get(	album_gdo,
												GNSDK_GDO_CHILD_ARTIST,
												1,
												&artist_gdo);


		error = gnsdk_manager_gdo_child_get(	artist_gdo, 					/*handle to GDO*/
												GNSDK_GDO_CHILD_NAME_OFFICIAL,	/*key to get the name*/
												1,								/*instance, starts with 1*/
												&artist_name);					/* Pointer to receive the result*/

		error = gnsdk_manager_gdo_value_get( 	artist_name,			/*handle to GDO*/
												GNSDK_GDO_VALUE_DISPLAY,/* key to get the value*/
												1, 						/*instance, starts with 1*/
												&album_artist);				/* Pointer to receive the result*/

		error = gnsdk_manager_gdo_child_get(	album_gdo, 				/*handle to GDO*/
												GNSDK_GDO_CHILD_TRACK_MATCHED,	/*Key to get the artist*/
												1,				/*instance, starts with 1*/
												&track_gdo);			/* Pointer to receive the result*/

		error = gnsdk_manager_gdo_child_get(	track_gdo, 						/*handle to GDO*/
												GNSDK_GDO_CHILD_TITLE_OFFICIAL,	/*Key to get the artist*/
												1,								/*instance, starts with 1*/
												&track_title_gdo);				/* Pointer to receive the result*/

		/* put the same in track artist */
		error = gnsdk_manager_gdo_child_get(	track_gdo, 				/*handle to GDO*/
												GNSDK_GDO_CHILD_ARTIST,	/*Key to get the TRACK*/
												1,				/*instance, starts with 1*/
												&artist_gdo);			/* Pointer to receive the result*/

		error = gnsdk_manager_gdo_child_get(	artist_gdo, 					/*handle to GDO*/
												GNSDK_GDO_CHILD_NAME_OFFICIAL,	/*key to get the name*/
												1,								/*instance, starts with 1*/
												&artist_name);					/* Pointer to receive the result*/

		error = gnsdk_manager_gdo_value_get( 	artist_name,			/*handle to GDO*/
												GNSDK_GDO_VALUE_DISPLAY,/* key to get the value*/
												1, 						/*instance, starts with 1*/
												&track_artist);				/* Pointer to receive the result*/


		error = gnsdk_manager_gdo_value_get( 	track_title_gdo,				/*handle to GDO*/
												GNSDK_GDO_VALUE_DISPLAY,		/* key to get the value*/
												1, 								/*instance, starts with 1*/
												&song_title);						/* Pointer to receive the result*/

		/*Save to file*/
		savefile = fopen(filename,"at");
		if (track_artist==NULL)
		{
			printf("%s- %s \n",album_artist, song_title);
			fprintf(savefile,"%s- %s \n",album_artist, song_title);
		}
		else
		{
			printf("%s- %s \n",track_artist ,song_title);
			fprintf(savefile,"%s- %s \n",track_artist ,song_title);
		}
		fclose(savefile);


		/*free the memory */
		gnsdk_manager_gdo_release(album_gdo);
		gnsdk_manager_gdo_release(title_gdo);
		gnsdk_manager_gdo_release(artist_gdo);
		gnsdk_manager_gdo_release(artist_name);

		gnsdk_manager_gdo_release(track_gdo);
		gnsdk_manager_gdo_release(track_title_gdo);


	};
		/* endif : Have found something ?**/

		gnsdk_musicid_query_release(query_handle);
		gnsdk_manager_gdo_release(response_gdo);

	return 0;
}



int GN_Music_Service::LogToFile(const char * Log_File,bool ErrorsWarn_Only, bool New_Log_File)
{	gnsdk_uint32_t filter_mask =	0;
	gnsdk_uint64_t Max_File_Size =	0;
	gnsdk_error_t	error							= GNSDK_SUCCESS;


	if (ErrorsWarn_Only)
	{
		/*Display only warnings and errors*/
		filter_mask = GNSDK_LOG_LEVEL_ERROR|GNSDK_LOG_LEVEL_WARNING;

	}
	else
	{
		/*Display everything*/
		filter_mask = GNSDK_LOG_LEVEL_ALL;
	}

	if (New_Log_File)
	{
		Max_File_Size = 0; /* Start New File */
	}
	else
	{
		Max_File_Size = 0x10000000000; /*1GB*/
	}

	error = gnsdk_manager_logging_enable(	Log_File,					/* Log file path */
											GNSDK_LOG_PKG_ALL,			/* Include entries for all packages and subsystems */
											filter_mask,				/* Include only error and warning entries */
											GNSDK_LOG_OPTION_ALL,		/* All logging options: timestamps, thread IDs, etc */
											Max_File_Size,				/* Max size of log: 0 means a new log file will be created each run */
											GNSDK_FALSE);				/* GNSDK_TRUE = old logs will be renamed and saved */

	ERROR_CHECK("gnsdk_manager_logging_enable FAILED \n");

	/*Keep the Journal files of SQLIte*/

	error = gnsdk_storage_sqlite_option_set(GNSDK_STORAGE_SQLITE_OPTION_JOURNAL_MODE, /*Key how to set up the journal mode*/
											"PERSIST"); /* DELETE, TRUNCATE, PERSIST, MEMORY OFF */


	ERROR_CHECK("gnsdk_storage_sqlite_option_set GNSDK_STORAGE_SQLITE_OPTION_JOURNAL_MODE FAILED \n");



	return error;
}



int GN_Music_Service::StopLogging(const char * Log_File)
{
	gnsdk_error_t	error							= GNSDK_SUCCESS;
	error = gnsdk_manager_logging_disable(	Log_File,			/* Log file path */
											GNSDK_LOG_PKG_ALL); /* Include entries for all packages and subsystems */

	/*Keep the Journal files of SQLIte*/

	error = gnsdk_storage_sqlite_option_set(GNSDK_STORAGE_SQLITE_OPTION_JOURNAL_MODE, /*Key how to set up the journal mode*/
											"MEMORY"); /* DELETE, TRUNCATE, PERSIST, MEMORY, OFF */



	return error ;
}

gnsdk_error_t GN_Music_Service::Close(char ** SerializedUser)
{



	/* Shutdown all components - actually not necessary see below*/

	gnsdk_musicid_shutdown();
	gnsdk_lookup_local_shutdown();

	gnsdk_dsp_shutdown();

	/* Shutdown the Manager to shutdown all libraries */
	gnsdk_manager_shutdown();


	return 0;

}


int GN_Music_Service::Set_Client_ID(const char* clientID)
{
	strcpy(this->mClient_ID, clientID);
	return 0;
}

int GN_Music_Service::Set_Client_ID_Tag(const char* clientIDTag)
{
	strcpy(this->mClient_ID_Tag, clientIDTag);
	return 0;
}

int GN_Music_Service::Set_License_File(const char* LicenseFile)
{
	strcpy(this->mLicensePath, LicenseFile);
	return 0;
}

int GN_Music_Service::Set_Audio(int channels, int sample_Size, int sample_Rate )
{
	this->mAudioChannels = channels;
	this->mSampleRate = sample_Rate;
	this->mSampleSize = sample_Size;
	return 0;
}










