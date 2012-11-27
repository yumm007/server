#include <stdio.h>
#include <time.h>
#include <string.h>

#define MAX_DEV_ID	50000

//定位数据结构体
struct location_data_t {
	unsigned int id;	//设备ID
	time_t timed;		//定位数据时间戳
	char ssid1[64];	//基站A名字
	char ssid2[64];	//基站B名字
	int rssi1;			//基站A信号值
	int rssi2;			//基站B信号值
	float battery;		//设备电量
};

//每个客户端需要保存的信息
struct location_saved_t {
	time_t last_data_time;	//最后一次收到数据的时间
	double last_sec_speed;	//最后一次的移动速度向量
	double last_location;	//最后一次所在的位置
};

static struct location_saved_t loca_his[MAX_DEV_ID];

static void conver_location_data(struct location_data_t *loca, const char *data) {
	struct tm tms;
	//定位数据格式为
	//[11009];A=00:19:70:2b:d9:ef;AValue=-56;B=00:19:70:2b:d9:ef;BValue=-56;Battery=4.0;120522151002
	sscanf(data, "[%d];A=%[0-9:abcdefABCDEF];AValue=%d;B=%[0-9:abcdefABCDEF];BValue=%d;Battery=%f;%2d%2d%2d%2d%2d%2d", &loca->id, loca->ssid1, &loca->rssi1, loca->ssid2, &loca->rssi2, &loca->battery, &tms.tm_year,&tms.tm_mon, &tms.tm_mday, &tms.tm_hour, &tms.tm_min, &tms.tm_sec);

	tms.tm_year += 2000 - 1900;
	tms.tm_mon  -= 1;
	tms.tm_isdst = 0;

	loca->timed = mktime(&tms);
}


static int theor_cal_val(const struct location_data_t *loca) {
	
	return 10;
}

int proc_location_data(const char *data) {
	float predict_weight = 0.0, theor_weight = 0.0;
	int current_location, predict_location, theor_location;
	double sec_mid;
	struct location_saved_t *his;
	
	struct location_data_t loca;
	memset(&loca, 0, sizeof(loca));

	//从定位数据中提取出字段值
	conver_location_data(&loca, data);
	his = &loca_his[loca.id];

	//预测位置 = 上次的速度*（当前数据包时间-上次数据包时间）+ 上次的位置
	sec_mid = difftime(loca.timed, his->last_data_time);
	predict_location = his->last_sec_speed * sec_mid + his->last_location;
	
	//理论位置 = 提取本次信号值，计算出本次的理论位置
	theor_location = theor_cal_val(&loca);

	//根据本次数据的可靠性高低，得出不同的权值
	if (predict_location == 0) {
		//首笔数据
		predict_weight = 0.0;
		theor_weight = 1.0;
	} else if (0) {
		//单笔数据
		if (0) {
			//单笔数据有效
			predict_weight = 0.0;
			theor_weight = 1.0;
		} else {
			//单笔数据无效
			predict_weight = 0.0;
			theor_weight = 1.0;
		}
	} else if(0) {
		//二笔数据
		if (0) {
			//二笔数据皆有效
			predict_weight = 0.0;
			theor_weight = 1.0;
		} else {
			//只有一笔有效
			predict_weight = 0.0;
			theor_weight = 1.0;
		}

	} else {
		//零笔数据
		predict_weight = 0.0;
		theor_weight = 1.0;
	} 

	current_location = predict_location * predict_weight + theor_location * theor_weight;
	//更新static值：最后一笔数据时间，最后一次的速度，最后一次的位置
	his->last_data_time = loca.timed;
	his->last_sec_speed = (current_location - his->last_location) / sec_mid;
	his->last_location = current_location;

	return his->last_location;
}

//for test
#if 0
int main(void) {
	char *data = "[11012];A=00:19:70:2b:d9:ef;AValue=-48;B=00:19:70:2b:d9:ef;BValue=-48;Battery=4.1;120522151004";
	struct location_data_t loca;
	memset(&loca, 0, sizeof(loca));

	conver_location_data(&loca, data);
	//for test
	printf("[%d]\nA=%s\nAValue=%d\nB=%s\nBvalue=%d\nBattery=%.1f\nTime=%s\n", loca.id, loca.ssid1, loca.rssi1, loca.ssid2, loca.rssi2, loca.battery, ctime((&loca.timed)));

	return 0;
}

#endif
