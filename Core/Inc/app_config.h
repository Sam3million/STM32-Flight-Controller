/*
 * app_config.h
 *
 *  Created on: 15/10/2026
 *      Author: samuelrose
 */

#ifndef INC_APP_CONFIG_H_
#define INC_APP_CONFIG_H_

#define USE_KALMAN 2
#if USE_KALMAN == 1
	#define FILTER_NAME   "Kalman-1 Filter"
	#define FILTER_UPDATE MPU6050_Kalman1Filter
#elif USE_KALMAN == 2
	#define FILTER_NAME   "Kalman-2 Filter"
	#define FILTER_UPDATE MPU6050_Kalman2Filter
#else
	#define FILTER_NAME   "Compl. Filter"
	#define FILTER_UPDATE MPU6050_ComplementaryFilter
#endif

#define DISPLAY_INTERVAL_MS 50

#endif /* INC_APP_CONFIG_H_ */
