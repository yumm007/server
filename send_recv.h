#ifndef __SEND_RECV_H__
#define __SEND_RECV_H__

#define PKG_MTH_SND_LCT_S 	"SetLocateData"
#define PKG_MTH_GET_DT_S	"GetDateTime"
#define PKG_MTH_GET_LCT_S 	"GetLocateData"
#define PKG_MTH_GET_PSN_S 	"GetWorkerInfo"
#define PKG_MTH_GET_DEV_CONF_S		"GetDevConfig"
#define PKG_MTH_ACTIVE_S	"Active"
#define PKG_MTH_WL_UPDATE_S "WirelessUpdate"
#define PKG_MTH_GET_ALARM_S	"GetAlarmInfo"

typedef enum PKG_MTH_TYPE {
    PKG_MTH_UNKN,
    PKG_MTH_SND_LCT,
    PKG_MTH_GET_DT,
    PKG_MTH_GET_LCT,
    PKG_MTH_GET_PSN,
    PKG_MTH_GET_DEV_CONF,
	 PKG_MTH_ACTIVE,
	 PKG_MTH_WL_UPDATE,
	 PKG_MTH_GET_ALARM_INFO,
} PKG_MTH_TYPE_T;

typedef unsigned char	UINT8;
typedef unsigned int		UINT32;

typedef enum PKG_TYPE {
    LOGIN_REQ	= 0x01,	//	login request
    SND_MSG	= 0x02,		//	send message
    MSG_RET	= 0x03,		//	reply message
    SND_MTH	= 0x04,		//	send method
    MTH_RET	= 0x05,		//	reply method
	 SHR_LD	= 0x15,		// short package of locations
	 MID_LD	= 0x16,		// medium long package of locations
} PKG_TYPE_T;


typedef struct sndrcv_header{
    UINT8 		pkg_ver[8];	//
    PKG_TYPE_T 	pkg_type;	//
    UINT32 		data_len;	//
} SNDRCV_H_T;

typedef struct sndrcv_id {
    UINT8	pkg_guid[32];		//°üÎšÒ»±êÊŸ,ÓÉ·¢ËÍ·œÌîÐŽ£¬œÓÊÕ·œÔ­Ñù·µ»Ø
    UINT8	pkg_from[32];		//from
    UINT8 	pkg_to[32];		//to
} SNDRCV_ID_T;

typedef struct sndrcv_data_t {

    UINT8	pkg_mth[32];	//ÃüÁî
    UINT8	mth_val[400];	//²ÎÊý»òÕß·µ»ØÖµ

    UINT32 	pkg_total;	//°ü×ÜÊý
    UINT32	pkg_ind;	//°üË÷Òý
    UINT32	mth_val_len;	//²ÎÊý»òÕß·µ»ØÖµ³€¶È
} SNDRCV_DATA_T;

//¶ÌÐÅÀàÐÍ
enum {
    COMM_MSG = 0,	//ÆÕÍš¶ÌÐÅ
    PUBL_MSG,		//¶ÌÐÅ¹«¹²
    ALARM_MSG,		//±šŸ¯¶ÌÐÅ
    OTHER_MSG,		//ÆäËû¶ÌÐÅ
};

enum {
    MSG_DEL_FAIL = 0,	//·¢ËÍÊ§°Ü
    MSG_DEL_SER,	//·þÎñÆ÷³É¹Š
    MSG_DEL_TER,	//¶Ô·œœÓÊÕ³É¹Š
    MSG_READ,		//¶Ô·œÒÑ²é¿Ž
};

typedef struct sndrcv_msg_t {    
    UINT8	msg_val[400];	//¶ÌÐÅÄÚÈÝ
    UINT8	msg_type;	//¶ÌÐÅÀàÐÍ 0 ÆÕÍš£¬1¹«¹²£¬2±šŸ¯
    UINT32	msg_len;	//¶ÌÐÅ³€¶È
} SNDRCV_MSG_T;

typedef struct NW_PKG {
    SNDRCV_H_T 	header;
    SNDRCV_ID_T id;
    union {
	SNDRCV_DATA_T 	mth_data;
	SNDRCV_MSG_T 	msg_data;
	UINT32 		msg_ret;
    };
    
} NW_PKG_T;

typedef struct SHR_LD_DATA {
	UINT8 pkg_ver[8];
	UINT32 pkg_type;
	unsigned int from_id;
	UINT8 ap1_addr[6];
	char ap1_val;
	UINT8 ap2_addr[6];
	char ap2_val;
	UINT8 batt;
	unsigned long times;
} SHR_LD_DATA_T;

typedef struct MID_LD_DATA {
   SNDRCV_H_T 	header;
	UINT8 from_id[32];
	UINT8 pkg_val[200];
	UINT32 pkg_len;
} MID_LD_DATA_T;


void open_socket(void);
void process_socket(int sd);
char *get_mac(const char *mac_addr);
char *get_mac_from_net(const char *ip_addr);

#endif
