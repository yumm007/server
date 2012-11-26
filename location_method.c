
static double __sec_mid(double last_time, const char *new_time) {

	return 10;
}

static int theor_cal_val(const char *data) {
	
	return 10;
}

static int get_date(const char *data) {
	
	return 10;
}

int proc_location_data(const char *data) {
	static double last_data_time = 0;
	static double last_sec_speed = 0;
	static double last_location = 0;

	float predict_weight = 0.0, theor_weight = 0.0;
	int current_location, predict_location, theor_location, sec_mid;

	//预测位置 = 上次的速度*（当前数据包时间-上次数据包时间）+ 上次的位置
	sec_mid = __sec_mid(last_data_time, data);
	predict_location = last_sec_speed * sec_mid + last_location;
	
	//理论位置 = 提取本次信号值，计算出本次的理论位置
	theor_location = theor_cal_val(data);

	//根据本次数据的可靠性高低，得出不同的权值
	if (last_data_time == 0 || last_sec_speed == 0 || last_location == 0) {
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
