/*
                                _                       
                       __ _  __| |_   _____ _ __   __ _ 
                      / _` |/ _` \ \ / / _ \ '_ \ / _` |
                     | (_| | (_| |\ V /  __/ | | | (_| |
                      \__,_|\__,_| \_/ \___|_| |_|\__, |
                                                  |___/ 
*/
#ifndef _CLTARGET_H
#define _CLTARGET_H
#include <clposition.h>

class ClTarget {
private:
protected:
	size_t m_x;
	size_t m_y;
	size_t m_r;
	size_t m_width;
	size_t m_height;
	bool m_enabled;
	bool m_search;
	int m_step;
public:
	void open(size_t width, size_t height, int step) {
		m_width = width;
		m_height = height;
		m_step = step;
		m_enabled = false;
		m_search = false;
		set(m_width/2, m_height/2, 80);
	};
	void search(bool s) {
		m_search = s;
	};
	bool is_search() {
		return m_search;
	};
	int get_x() {
		return m_x;
	};
	int get_y() {
		return m_y;
	};
	int get_r() {
		return m_r;
	};
	bool set(ClPosition &postion) {
		return set(postion.m_x, postion.m_y, postion.m_z);
	}
	bool set(size_t x, size_t y, size_t r) {
		if(x > (m_width - r))
			return false;
		if(x < r)
			return false;
		if(y > (m_height - r))
			return false;
		if(y < r)
			return false;
		m_x = x;
		m_y = y;
		m_r = r;
		return true;
	};
	bool is_enabled() {
		return m_enabled;
	};
	void enable() {
		m_enabled = true;
	};
	void disable() {
		m_enabled = false;
	};
	void left() {
		if(m_search == true)
			return;
		if((m_x - m_step) >= (m_r))
			m_x = m_x - m_step;
	};
	void right() {
		if(m_search == true)
			return;
		if((m_x + m_step) <= (m_width - m_r))
			m_x = m_x + m_step;
	};
	void up() {
		if(m_search == true)
			return;
		if((m_y - m_step) >= (m_r))
			m_y = m_y - m_step;
	};
	void down() {
		if(m_search == true)
			return;
		if((m_y + m_step) <= (m_height - m_r))
			m_y = m_y + m_step;
	};
	void inc() {
		if(m_search == true)
			return;
		if(m_r < 100)
			m_r = m_r + 2;
	};
	void dec() {
		if(m_search == true)
			return;
		if(m_r > 5)
			m_r = m_r - 2;
	}
};

#endif //_CLTARGET_H
