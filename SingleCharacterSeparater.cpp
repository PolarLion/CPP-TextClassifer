#include "SingleCharacterSeparater.h"
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <string.h>
using std::vector;
using std::string;
using std::cout;
using std::endl;
using namespace codingtype;
const char SPACE = 32; 

SingleCharacterSeparater::SingleCharacterSeparater()
	: buffer(nullptr)
{
}


SingleCharacterSeparater::~SingleCharacterSeparater()
{
	freebuffer();
}


void SingleCharacterSeparater::gbk_separater(const string& s, vector<string>& vc)
{
	char str[3] = {0, 0, 0};
	int state = 0;
	string asc_s;
	for (auto p = s.begin(); p != s.end(); ++p) {
		char c = *p;
		if (0 == state) {
			if (!(c & 0x80)) {
				// do nothing
				if ( ('a' <= c && 'z' >= c) || ('A' <= c && 'Z' >= c)) { 
					asc_s += c;
					state = 11;
				}
			}
			else if (c ^ 0x80 || c ^ 0xff) {
				str[0] = c;
				++state;
			}
		}
		else if(1 == state) {
			////cout << std::setbase(16) << (unsigned)c << " " << std::setbase(16) << (c & 0xc0) << endl;
			////cout << std::setbase(16) << (unsigned)c << " " << std::setbase(16) << (c & 0x80) << endl;
			if ((!((c & 0xc0) ^ 0x40) && (c ^ 0x7f)) || (!((c & 0x80) ^ 0x80) && (c ^ 0xff))) {
				str[1] = c;
				vc.push_back(str);
			}
			str[1] = 0;
			state = 0;
		}
		else if(11 == state) {
			if ( ('a' <= c && 'z' >= c) || ('A' <= c && 'Z' >= c)) { 
				asc_s += c;
				state = 11;
			}
			else {
				vc.push_back(asc_s);
				asc_s.clear();
				state = 0;
			}
		}
	}
	if (asc_s.size() > 1 && 11 == state)
		vc.push_back(asc_s);
}


void SingleCharacterSeparater::utf8_separater(const string& s, vector<string>& vc)
{
	int state = 0;
	int character_length = 0;
	char str[6] = {0, 0, 0, 0, 0, 0};
	string asc_s;
	for (auto p = s.begin(); p != s.end(); ++p) {
		//unsigned short c = s[i] - 0xff00;
		char c = *p;
		if (0 == state) {
			if (!(c & 0x80)) {
				//do nothing
				if ( ('a' <= c && 'z' >= c) || ('A' <= c && 'Z' >= c)) { 
					asc_s += c;
					state = 11;
				}
			}
			else if(!((c & 0xfe) ^ 0xfc)) { //1111 110x
				str[state++] = c;
				character_length = 5;
			}
			else if(!((c & 0xfc) ^ 0xf8)) { //1111 10xx
				str[state++] = c;
				character_length = 4;
			}
			else if(!((c & 0xf8) ^ 0xf0)) { //1111 00xx
				str[state++] = c;
				character_length = 3;
			}
			else if(!((c & 0xf0) ^ 0xe0)) { //1110 xxxx
				str[state++] = c;
				character_length = 2;
			}
			else if (!((c & 0xe0) ^ 0xc0)) { //1100 xxxx
				str[state++] = c;
				character_length = 1;
			}
		}
		else if(1 == state){
			if (!((c & 0xc0) ^ 0x80)) {
				str[state++] = c;
				if(state > character_length){
					//cout << "2 2\n";
					vc.push_back(str);
					for (int i = 1; i <= state; ++i)
						str[i] = 0;
					state = 0;
					character_length = 0;
				}
			}
			else {
				for (int i = 1; i <= state; ++i)
					str[i] = 0;
				state = 0;
				character_length = 0;
			}
		}
		else if(11 == state) {
			if ( ('a' <= c && 'z' >= c) || ('A' <= c && 'Z' >= c)) { 
				asc_s += c;
				state = 11;
			}
			else {
				vc.push_back(asc_s);
				asc_s.clear();
				state = 0;
			}
		}
	}
	if (asc_s.size() > 1 && 11 == state)
		vc.push_back(asc_s);
}


const char* SingleCharacterSeparater::gbk_separater(const char* s)
{
	freebuffer();
	const unsigned old_length = strlen(s);
	const unsigned new_length = old_length * 2 + 1;
	char* buffer = new char[new_length];
	if (nullptr == buffer) {
		printf("can't allocate memory\n");
		return nullptr;
	}
	buffer[new_length-1] = 0;
	int count = 0;
	int state = 0;
	bool lack_space = false;
	for (unsigned i = 0; i < old_length; ++i) {
		//unsigned short c = s[i] - 0xff00;
		char c = s[i];
		if (0 == state) {
			if (!(c & 0x80)) {
				buffer[count++] = c;
				if (!lack_space)
					lack_space = true;
				//buffer[count++] = SPACE;
			}
			else if (c ^ 0x80 || c ^ 0xff) {
				if (lack_space)
					buffer[count++] = SPACE;
				buffer[count++] = c;
				++state;
			}
		}
		else if(1 == state) {
			////cout << std::setbase(16) << (unsigned)c << " " << std::setbase(16) << (c & 0xc0) << endl;
			////cout << std::setbase(16) << (unsigned)c << " " << std::setbase(16) << (c & 0x80) << endl;
			if (((!((c & 0xc0) ^ 0x40) && (c ^ 0x7f))) || ((!((c & 0x80) ^ 0x80) && (c ^ 0xff)))) {
				buffer[count++] = c;
				buffer[count++] = SPACE;
			}
			else {
				buffer[count-1] = SPACE;
			}
			lack_space = false;
			state = 0;
		}
	}
	buffer[count] = 0;
	return buffer;
}


const char* SingleCharacterSeparater::utf8_separater(const char* s)
{
	freebuffer();
	const unsigned old_length = strlen(s);
	const unsigned new_length = old_length * 2 + 1;
	char* buffer = new char[new_length];
	if (nullptr == buffer) {
		printf("can't allocate memory\n");
		return nullptr;
	}
	buffer[new_length-1] = 0;
	int count = 0;
	int state = 0;
	int character_length;
	bool lack_space = false;
	for (unsigned i = 0; i < old_length; ++i) {
		//unsigned short c = s[i] - 0xff00;
		char c = s[i];
		if (0 == state) {
			if (!(c & 0x80)) {
				buffer[count++] = c;
				if (!lack_space)
					lack_space = true;
			}
			else if(!((c & 0xfe) ^ 0xfc)) { //1111 110x
				//cout << "1 1\n";
				if (lack_space)
					buffer[count++] = SPACE;  ;
				++state;
				buffer[count++] = c;
				character_length = 5;
			}
			else if(!((c & 0xfc) ^ 0xf8)) { //1111 10xx
				//cout << "1 1\n";
				if (lack_space)
					buffer[count++] = SPACE;  ;
				++state;
				buffer[count++] = c;
				character_length = 4;
			}
			else if(!((c & 0xf8) ^ 0xf0)) { //1111 00xx
				//cout << "1 1\n";
				if (lack_space)
					buffer[count++] = SPACE;  ;
				++state;
				buffer[count++] = c;
				character_length = 3;
			}
			else if(!((c & 0xf0) ^ 0xe0)) { //1110 xxxx
				//cout << "1 1\n";
				if (lack_space)
					buffer[count++] = SPACE;  ;
				++state;
				buffer[count++] = c;
				character_length = 2;
			}
			else if (!((c & 0xe0) ^ 0xc0)) { //1100 xxxx
				//cout << "1 1\n";
				if (lack_space)
					buffer[count++] = SPACE;  ;
				++state;
				buffer[count++] = c;
				character_length = 1;
			}

		}
		else {
			if (!((c & 0xc0) ^ 0x80)) {
				//cout << "2 1\n";
				buffer[count++] = c;
				if(state >= character_length){
					//cout << "2 2\n";
					buffer[count++] = SPACE; 
					lack_space = false;
					state = 0;
					character_length = 0;
				}
				else
					state++;
			}
			else {
				buffer[count++] = SPACE;
				lack_space = false;
				character_length = 0;
				state = 0;
			}
		}
	}
	buffer[count] = 0;
	return buffer;
}