#pragma once
#include <vector>
#include <string>


namespace codingtype {
	enum SeparaterType{
		GBK = 0,
		UTF8 = 1
	};
}


class SingleCharacterSeparater{
private:
	char* buffer;
	
	void freebuffer() {
		if (nullptr != buffer) {
			delete buffer;
			buffer = nullptr;
		}
	}
public:
	SingleCharacterSeparater();
	~SingleCharacterSeparater();

	void gbk_separater(const std::string& s, std::vector<std::string>& vc);
	void utf8_separater(const std::string& s, std::vector<std::string>& vc);

	const char* gbk_separater(const char* s);
	const char* utf8_separater(const char* s);

	void operator()(const std::string& s, std::vector<std::string>& vc, codingtype::SeparaterType stype) {
		if (codingtype::GBK == stype) {
			gbk_separater(s, vc);
		}
		else if(codingtype::UTF8 == stype) {
			utf8_separater(s, vc);
		}
	}

	const char* operator()(const std::string& s, codingtype::SeparaterType stype) {
		if (codingtype::GBK == stype) {
			return gbk_separater(s.c_str());
		}
		else if(codingtype::UTF8 == stype) {
			return utf8_separater(s.c_str());
		}
		return nullptr;
	}
};