#include <time.h>
#include <string.h>

//从new_time 字符串中找到时间值，格式为：121126084610, 减去last_time
static double __sec_mid(time_t last_time, const char *new_time) {
	struct tm tms;
	time_t tim;
	char *p;
	double n;

	if (last_time == 0)
		return 0;

	//将121126084610 => struct tm 
	p = strrchr(new_time, ';');
	if (p == NULL)
		return 0;
	p++;
	
	tms.tm_year = (p[0] - '0') * 10 + p[1] - '0' + 2000;
	tms.tm_mon	= (p[2] - '0') * 10 + p[3] - '0' - 1;
	tms.tm_mday	= (p[4] - '0') * 10 + p[5] - '0';
	tms.tm_hour = (p[6] - '0') * 10 + p[7] - '0';
	tms.tm_min 	= (p[8] - '0') * 10 + p[9] - '0';
	tms.tm_sec 	= (p[10] - '0') * 10 + p[11] - '0';

	tms.tm_isdst = 0;

	tim = mktime(&tms);

	n = difftime(tim, last_time);

	if (n <= 0)
		return 0;

	return n;
}

static int theor_cal_val(const char *data) {
	
	return 10;
}

static int get_date(const char *data) {
	
	return 10;
}

int proc_location_data(const char *data) {
	static time_t last_data_time = 0;
	static double last_sec_speed = 0;
	static double last_location = 0;

	float predict_weight = 0.0, theor_weight = 0.0;
	int current_location, predict_location, theor_location;
	double sec_mid;

	//预测位置 = 上次的速度*（当前数据包时间-上次数据包时间）+ 上次的位置
	sec_mid = __sec_mid(last_data_time, data);
	predict_location = last_sec_speed * sec_mid + last_location;
	
	//理论位置 = 提取本次信号值，计算出本次的理论位置
	theor_location = theor_cal_val(data);

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
	last_data_time = get_date(data);
	last_sec_speed = (current_location - last_location) / sec_mid;
	last_location = current_location;

	return last_location;
}
