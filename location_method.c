#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define MAX_DEV_ID	50000

//定位数据结构体
struct location_data_t {
	unsigned int id;	//设备ID
	time_t timed;		//定位数据时间戳
	char mac1[64];	//基站A名字
	char mac2[64];	//基站B名字
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
	sscanf(data, "[%d];A=%[0-9:abcdefABCDEF];AValue=%d;B=%[0-9:abcdefABCDEF];BValue=%d;Battery=%f;%2d%2d%2d%2d%2d%2d", &loca->id, loca->mac1, &loca->rssi1, loca->mac2, &loca->rssi2, &loca->battery, &tms.tm_year,&tms.tm_mon, &tms.tm_mday, &tms.tm_hour, &tms.tm_min, &tms.tm_sec);

	tms.tm_year += 2000 - 1900;
	tms.tm_mon  -= 1;
	tms.tm_isdst = 0;

	loca->timed = mktime(&tms);
}

struct ap_table_t {
	char *ap_mac;
	int ap_loca;
} ap_table[] = {
	{"00:19:70:2b:d9:ed", 0},
	{"00:19:70:2b:d9:ee", 100},
	{"00:19:70:2b:d9:ef", 200},
	{NULL, 0},
};

static int get_ap_loca(const char *ap_mac) {
	struct ap_table_t *tmp = &ap_table[0];
	for (;tmp->ap_mac != NULL; tmp++) 
		if (strcasecmp(tmp->ap_mac, ap_mac) == 0)
			return tmp->ap_loca;
	return 0;
}

static int theor_cal_val(struct location_data_t *loca) {
	//RSSI = A - 10*n*lg(d)
	//d = pow(10, (A - RSSI)/(10 * n))
	//需要在实际环境下测量出常数A和与n, 暂时取A=-48.73, N = 1.9
	#define A -30.73
	#define N 1.3

	int point1, point2;
	int prov, next;
	char mac[64];
	int rssi, tmp;

	prov = get_ap_loca(loca->mac1);
	next = get_ap_loca(loca->mac2);

	if (prov >= next) {
		//swap ap1, ap2的数据，保证当前位置已过A基站但未到B基站
		memcpy(mac, loca->mac1, sizeof(loca->mac1));
		memcpy(loca->mac1, loca->mac2,sizeof(loca->mac1));
		memcpy(loca->mac2, mac, sizeof(loca->mac1));

		rssi = loca->rssi1;
		loca->rssi1 = loca->rssi2;
		loca->rssi2 = loca->rssi1;
	
		tmp = prov;
		prov = next;
		next = prov;
	}

	point1 = prov + pow(10, (A - loca->rssi1) / (10 * N));
	point2 = next - pow(10, (A - loca->rssi2) / (10 * N));

	printf("between %d m --- %d m\n", point1, point2);

	return (point1 + point2)/2;
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
	//for test
	printf("[%d]\nA=%s\nAValue=%d\nB=%s\nBvalue=%d\nBattery=%.1f\nTime=%s\n", loca.id, loca.mac1, loca.rssi1, loca.mac2, loca.rssi2, loca.battery, ctime((&loca.timed)));
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
#if 1
int main(void) {
	char *data = "[11012];A=00:19:70:2b:d9:ee;AValue=-50;B=00:19:70:2b:d9:ef;BValue=-48;Battery=4.1;120522151004";
	struct location_data_t loca;
	memset(&loca, 0, sizeof(loca));

	proc_location_data(data);

	return 0;
}

#endif
