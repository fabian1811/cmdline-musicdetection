/*
 * GNMusicServiceError.h
 *
 *  Created on: Sep 6, 2014
 *      Author: fplepp
 */

#ifndef GNMUSICSERVICEERROR_H_
#define GNMUSICSERVICEERROR_H_

#define ERROR_CHECK(X)	\
		if (error!=0) 		\
		{					\
			printf(X);		\
			printf("Reason: %s \n\n", (GNSDK_API gnsdk_manager_error_info())->error_description); \
			return -1;		\
		}					\




#endif /* GNMUSICSERVICEERROR_H_ */
