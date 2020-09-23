#pragma once
#include "type.h"
#include <opencv2/opencv.hpp>

class itexture {
public:
	virtual vector3f sample(const vector2f &uv) const = 0;
};

class texture : public itexture {
public:
	texture(const char *name) {
		image_data = cv::imread(std::string(name));
		cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
		width = image_data.cols;
		height = image_data.rows;
	}
	vector3f sample(const vector2f &uv) const override {
		assert(uv.x() <= 1.f);
		assert(uv.y() <= 1.f);
		auto u_img = uv.x() * (width-1);
		auto v_img = (1-uv.y()) * (height-1);
		auto color = image_data.at<cv::Vec3b>(v_img, u_img);
		float b = std::pow(color[0] / 255.f, 1/0.6f);
		float g = std::pow(color[1] / 255.f, 1/0.6f);
		float r = std::pow(color[2] / 255.f, 1/0.6f);
		return vector3f(b, g, r);
	}
private:
	int width;
	int height;
	cv::Mat image_data;
};


