#pragma once
#include "type.h"
#include "auxiliary.h"
#include <opencv2/opencv.hpp>

class screen {
public:
	screen(int w, int h) {
		size.x() = w;
		size.y() = h;
		framebuf.resize(w*h);
		depthbuf.resize(w*h);
		clear();
	}
	void clear() {
		Eigen::Vector3f ZERO(0, 0, 0);
		auto INF = std::numeric_limits<float>::infinity();
		std::fill(framebuf.begin(), framebuf.end(), ZERO);
		std::fill(depthbuf.begin(), depthbuf.end(), INF);
	}
	bool ztest(int x, int y, float z) {
		if (x < 0 || y < 0)
			return false;
		int idx = x + size.x() * (size.y() - y - 1);
		if (idx >= depthbuf.size())
			return false;
		return depthbuf[idx] > z;
	}
	void set(int x, int y, const vector3f &color, float depth = 0.f) {
		if (x < 0 || y < 0)
			return ;
		int idx = x + size.x() * (size.y() - y - 1);
		if (idx >= depthbuf.size())
			return ;
		if (depthbuf[idx] > depth) {
			depthbuf[idx] = depth;
			framebuf[idx] = color;
		}
	}
	void dump(const char *name) {
		FILE* fp = fopen(name, "wb");
		fprintf(fp, "P6\n%d %d\n255\n", size.x(), size.y());
		for (int i = 0; i < size.x() * size.y(); i++) {
			unsigned char color[3];
			vector3f &c = framebuf[i];
			color[0] = c.x() * 255;//(unsigned char)255 * std::pow(clamp(c.x()), 0.6f);
			color[1] = c.y() * 255;//(unsigned char)255 * std::pow(clamp(c.y()), 0.6f);
			color[2] = c.z() * 255;//(unsigned char)255 * std::pow(clamp(c.z()), 0.6f);
			fwrite(color, 1, 3, fp);
		}
		fclose(fp);
	}
	void show() {
		std::string filename = "output.png";
		cv::Mat image(size.x(), size.y(), CV_32FC3, framebuf.data());
		image.convertTo(image, CV_8UC3, 1.0f);
		cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
		cv::imshow("image", image);
	}
	const vector2i &getsize() const {
		return size;
	}
private:
	vector2i size;
	std::vector<float> depthbuf;
	std::vector<vector3f> framebuf;
};
