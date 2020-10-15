#pragma once
#include "type.h"
#include "auxiliary.h"
#include <opencv2/opencv.hpp>

namespace render {

class screen : public gpu {
public:
	CPU screen(int w, int h) {
		size.x() = w;
		size.y() = h;
		framebuf.resize(w*h);
		depthbuf.resize(w*h);
		clear();
	}
	CPU void clear() {
		scale_ = 1.f;
		vector3f ZERO(0, 0, 0);
		auto INF = INFINITY;
		cpu_vector<vector3f> clr1;
		cpu_vector<float> clr2;
		clr1.resize(framebuf.size());
		clr2.resize(depthbuf.size());
		std::fill(clr1.begin(), clr1.end(), ZERO);
		std::fill(clr2.begin(), clr2.end(), INF);
		framebuf = clr1;
		depthbuf = clr2;
	}

	GPU float aspect() {
		return (float)size.x() / (float)size.y();
	}

	GPU bool ztest(int x, int y, float z) {
		if (x < 0 || y < 0)
			return false;
		unsigned idx = x + size.x() * (size.y() - y - 1);
		if (idx >= depthbuf.size())
			return false;
		return depthbuf[idx] > z;
	}
	GPU void zset(int x, int y, float depth) {
		if (x < 0 || y < 0)
			return ;
		int idx = x + size.x() * (size.y() - y - 1);
		depthbuf[idx] = depth;
	}
	GPU void set(int x, int y, const vector3f &color) {
		if (x < 0 || y < 0)
			return ;
		int idx = x + size.x() * (size.y() - y - 1);
		framebuf[idx] = color;
	}
	GPU void add(int x, int y, const vector3f &color) {
		if (x < 0 || y < 0)
			return ;
		int idx = x + size.x() * (size.y() - y - 1);
		framebuf[idx] += color;
	}
	ALL const vector2i &getsize() const {
		return size;
	}
	CPU void scale(float s) {
		scale_ = s;
	}
	CPU void dump(const char *name) {
		cpu_vector<vector3f> frameb = framebuf;
		FILE* fp = fopen(name, "wb");
		fprintf(fp, "P6\n%d %d\n255\n", size.x(), size.y());
		for (int i = 0; i < size.x() * size.y(); i++) {
			unsigned char color[3];
			vector3f &c = frameb[i];
			color[0] = (unsigned char)(255.f * std::pow(clamp(c.x() * scale_), 1.f/2.2f));
			color[1] = (unsigned char)(255.f * std::pow(clamp(c.y() * scale_), 1.f/2.2f));
			color[2] = (unsigned char)(255.f * std::pow(clamp(c.z() * scale_), 1.f/2.2f));
			fwrite(color, 1, 3, fp);
		}
		fclose(fp);
	}
	CPU void show() {
		cpu_vector<vector3f> tmp = framebuf;
		for (int i = 0; i < size.x() * size.y(); i++) {
			vector3f &c = tmp[i];
			c.x() = (unsigned char)(255.f * std::pow(clamp(c.x() * scale_), 1.f/2.2f));
			c.y() = (unsigned char)(255.f * std::pow(clamp(c.y() * scale_), 1.f/2.2f));
			c.z() = (unsigned char)(255.f * std::pow(clamp(c.z() * scale_), 1.f/2.2f));
		}
		std::string filename = "output.png";
		cv::Mat image(size.y(), size.x(), CV_32FC3, tmp.data());
		image.convertTo(image, CV_8UC3, 1.f);
		cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
		cv::imshow("image", image);
	}
private:
	float scale_;
	vector2i size;
	gpu_vector<float> depthbuf;
	gpu_vector<vector3f> framebuf;
};

}

