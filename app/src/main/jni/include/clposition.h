/*
                               _    _ _       
                     __ _  ___| | _| (_)_ __  
                    / _` |/ __| |/ / | | '_ \
                   | (_| | (__|   <| | | | | |
                    \__,_|\___|_|\_\_|_|_| |_|
                                              
*/
#ifndef _CLPOSITION_H
#define _CLPOSITION_H

#include <map>
#include <fstream>

class ClPosition {
private:
protected:
public:
	int m_x;
	int m_y;
	int m_z;
	int m_dx;
	int m_dy;
	int m_dz;
	bool m_diff;
	int idx;
	ClPosition() {
		m_diff = false;
		m_dx = 0;
		m_dy = 0;
		m_dz = 0;
		idx = 0;
	};
	~ClPosition() {
	};
	void set(int x, int y, int z) {
		m_x = x;
		m_y = y;
		m_z = z;
		if(m_diff == false) {
			m_diff = true;
			m_dx = x;
			m_dy = y;
			m_dz = z;
			//TRACE("init dir(x,y,z) (%d,%d,%d) (%d,%d,%d)\n", x, y, z, m_dx, m_dy, m_dx);
		}
	};
	void direction(int x, int y, int z) {
		if(m_diff == false)
			return;
		if((x < 1) || (y < 1))
			return;
		int m_ox = x;
		int m_oy = y;
		int m_oz = z;

		const char *px = (x > m_dx) ? "right" : "left";
		const char *py = (y > m_dy) ? "up" : "down";

		x = m_dx - x;
		y = m_dy - y;

		if((x == 0) || (y == 0)) // colordiff found but did not cause movement of center
			return;
		if(idx++ > 99999)
			idx = 1;
		TRACE("%d -- dir(x,y,z) (%d,%d,%d) (%d,%d,%d) (%d,%d,%d) %s,%s\n", 
				idx, x, y, z, m_ox, m_oy, m_oz, m_dx, m_dy, m_dx, px, py);
		m_dx = m_ox;
		m_dy = m_oy;
		m_dz = m_oz;
	};
};

// clposition
#endif //_CLPOSITION_H
