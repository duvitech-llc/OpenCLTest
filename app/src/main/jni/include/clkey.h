/*
                          _                       
                 __ _  __| |_   _____ _ __   __ _ 
                / _` |/ _` \ \ / / _ \ '_ \ / _` |
               | (_| | (_| |\ V /  __/ | | | (_| |
                \__,_|\__,_| \_/ \___|_| |_|\__, |
                                            |___/ 
*/
#ifndef _CLKEY_H
#define _CLKEY_H
#include <map>
#include <fstream>

#ifdef _WITH_KEYLOGGER
#define KEY_MX_INC(VAR_A, VAR_B) \
	case VAR_A: \
		m_thekey.key_color_inc(#VAR_A, VAR_B); \
		break;
#define KEY_MX_DEC(VAR_A, VAR_B) \
	case VAR_A: \
		m_thekey.key_color_dec(#VAR_A, VAR_B); \
		break;
#else
#define KEY_MX_INC(VAR_A, VAR_B) 
#define KEY_MX_DEC(VAR_A, VAR_B)
#endif //_WITH_KEYLOGGER

#ifdef _WITH_KEYLOGGER
	#define KEYLOG(VAR_A, VAR_B) m_thekey.log(#VAR_A, #VAR_B, VAR_B);
#else
	#define KEYLOG(VAR_A, VAR_B) 
#endif //_WITH_KEYLOGGER

class ClKey {
private:
protected:
#ifdef _WITH_KEYLOGGER
	std::fstream m_keylogger;
#endif //_WITH_KEYLOGGER
	float color_step;
public:
	float m_mx[5];
	char color_map[5];
	ClKey() {
#ifdef _INTEL
		//float m_mx_orig[5] = {1.4702, 0.34414, 0.71414, 1.772, 1.164};
		//float m_mx_orig[5] = {0.9352 , 0.105 , 0.005 , 0.842001, 1.164};
		//float m_mx_orig[5] = { 0.9852, 0.1305, 0.0055, 0.887001, 1.164 };
		//float m_mx_orig[5] = { 0.893699, 0.0839999, 0.0015, 0.926001, 1.164, };
		//float m_mx_orig[5] = { 1.0092, 0, 0, 0.999501, 1.164, };
		//float m_mx_orig[5] = { 1.0047, 0, 0, 0.999501, 1.164, };
		float m_mx_orig[5] = { 1.0047, 0, 0, 0.699501, 1.164, };
#else
		//float m_mx_orig[5] = { 0.05, 0.55, 0.55, 0.549501, 1.164, };
		float m_mx_orig[5] = { 0.75, 0.2, 2.15, 0.499501, 1.164, };
#endif


		//color_step = 0.0015;
		color_step = 0.025;
		m_mx[0] = m_mx_orig[0];
		m_mx[1] = m_mx_orig[1];
		m_mx[2] = m_mx_orig[2];
		m_mx[3] = m_mx_orig[3];
		m_mx[4] = m_mx_orig[4];
		color_map[0] = 64;
		color_map[1] = 35;
		color_map[2] = 31;
		color_map[3] = 0;
#ifdef _WITH_KEYLOGGER
		m_keylogger.open("key.log", std::fstream::in | std::fstream::out | std::fstream::trunc);
#endif //_WITH_KEYLOGGER
	};
	~ClKey() {
#ifdef _WITH_KEYLOGGER
		m_keylogger << "float m_mx_orig[5] = { ";
		for(int x = 0; x < 5; x++) {
			m_keylogger << m_mx[x] << ", ";
		}
		m_keylogger << "};" << std::endl;
		m_keylogger << "int color_map[5] = { ";
		for(int x = 0; x < 4; x++) {
			m_keylogger << (int)color_map[x] << ", ";
		}
		m_keylogger << "};" << std::endl;
		m_keylogger.close();
#endif //_WITH_KEYLOGGER
	};
	void key_color_inc(const char *name, int ndx) {
		m_mx[ndx] = m_mx[ndx] + color_step;
		if(m_mx[ndx] < 0) m_mx[ndx] = color_step;
		if(m_mx[ndx] > 2.5) m_mx[ndx] = color_step;
#ifdef _WITH_KEYLOGGER
		m_keylogger << name << " -- " << ndx << " -- " << m_mx[ndx] << std::endl;
#endif //_WITH_KEYLOGGER
	};
	void key_color_dec(const char *name, int ndx) {
		m_mx[ndx] = m_mx[ndx] - color_step;
		if(m_mx[ndx] < 0) m_mx[ndx] = color_step;
		if(m_mx[ndx] > 2.5) m_mx[ndx] = color_step;
#ifdef _WITH_KEYLOGGER
		m_keylogger << name << " -- " << ndx << " -- " << m_mx[ndx] << std::endl;
#endif //_WITH_KEYLOGGER
	};
	void log(const char *name, const char *key, int val) {
#ifdef _WITH_KEYLOGGER
		m_keylogger << name << " -- " << key << " -- " << val << std::endl;
#endif //_WITH_KEYLOGGER
	}
};

// clkey
#endif //_CLKEY_H

