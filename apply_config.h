#ifndef __APPLY_CONF_H__
#define __APPLY_CONF_H__

#define CONFIG_VAL_MAX_LEN	256

enum {
	OS_ID = 0,
	LOG_FILE_DIRECTORY_ID,
	LOG_FDS_MIN,		//
	TOTAL_ONLINE_LOG_FILE_ID = LOG_FDS_MIN,
	LOCATION_DATA_LOG_FILE_ID,
	CLIENT_ON_OFF_LOG_FILE_ID,
	SEND_MSG_FROM_FILE_ID,

	LOG_FDS_MAX,		//×î´óµÄlogÎÄ¼þÃèÊö·ûÊý

	TOTAL_CLIENT_LIMIT_ID = LOG_FDS_MAX,
	CLIENT_OFFLINE_PERIOD_ID,
	DEVICE_CONFIG_FILE_DIR_ID,

	MAX_CONF_ID,
};

extern FILE *LOG_FDS[LOG_FDS_MAX];
char CONF_FILTER[MAX_CONF_ID][CONFIG_VAL_MAX_LEN];
void close_config(void);
void apply_config(const char *conf_file_path);

#endif
