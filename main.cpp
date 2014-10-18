#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <ctime>
#include <cmath>
#include <chrono>
#include <unordered_map>
#include "scanfile.h"
#include "SVMTextClassifier.h"

#include <stdio.h>
#include "svm.h"    //包涵本目录下的SVM.H头文件

using namespace std;
using std::chrono::system_clock;

void keep_window_open()
{
    char c;
    printf("enter a character to exit\n");
    cin >> c;
}


using std::ifstream;


void test_svmtextc(int num)
{
    const double ratio = 0.8;
    const string working_path = "io/";
    SVMTextClassifier svmtc(working_path, num, codingtype::GBK);

    map<string, vector<string>> class_map;
    string path = "../training_set/easy_train/";
    vector<string> dirs;
    get_dirs(path, dirs);
    for (auto p = dirs.begin(); p != dirs.end(); ++p) {
	svmtc.add_classname(*p);	
	string spath = path+*p+"/";
	cout << "process : "<< spath << endl;
	vector<string> files;
	get_files(spath, files);
	class_map[*p] = files;
	//cout << files.size() << endl;
    }

    dirs.clear();
    for (auto p = class_map.begin(); p != class_map.end(); ++p) {
	int count = 0;
	const int maxindex = p->second.size() * ratio;
	for (int i = 0; i < maxindex; ++i) {
	    string spath = path + p->first + "/" + p->second[i];
	    ifstream infile(spath);
	    //cout << "process : "<< spath<< endl;
	    if (infile.fail()) {
		printf("no such file %s\n", p->second[i].c_str());
		return;
	    }
	    infile.seekg(0, infile.end);
	    int length = infile.tellg();
	    infile.seekg(0, infile.beg);
	    char* buffer = new char[length+1];
	    buffer[length] = 0;
	    infile.read(buffer, length);
	    svmtc.add_train_data(p->first, buffer);
	    ++count;
	    delete buffer;
	    infile.close();
	}
    }
    svmtc.preprocessor();
    svmtc.train_svm();
    svmtc.load_data();

    //记录被标记为该类别的样本数（用于计算precision）
    unordered_map<string, int> c_info1;
    //记录被正确标注的样本数
    unordered_map<string, int> c_info2;
    unordered_map<string, double> recall;
    int count_all_right = 0;
    int count_all = 0;
    for (auto p = class_map.begin(); p != class_map.end(); ++p) {
	c_info2[p->first] = 0;
    }

    for (auto p = class_map.begin(); p != class_map.end(); ++p) {
	int count_right = 0;
	int count = 0;
	const int min = p->second.size() * ratio;
	for (int i = min; i < p->second.size(); ++i) {
	    //for (int i = 0; i < min; ++i) {
	    string spath = path + p->first + "/" + p->second[i];
	    ifstream infile(spath);
	    if (infile.fail()) {
		printf("no such file %s\n", p->second[i].c_str());
		return;
	    }
	    infile.seekg(0, infile.end);
	    int length = infile.tellg();
	    infile.seekg(0, infile.beg);
	    char* buffer = new char[length+1];
	    buffer[length] = 0;
	    infile.read(buffer, length);

	    string sc = svmtc.classification(buffer);
	    ++count;
	    ++count_all;
	    c_info1[sc]++;
	    //cout << p->first << endl;
	    if(p->first == sc) {
		++count_right;
		++count_all_right;
	    }
	    delete buffer;
	    infile.close();
	}
	c_info2[p->first] = count_right;
	cout << endl;
	cout << p->first <<  " ： right number : " << count_right << endl;
	//cout << "Recall Rate : " << count_right / (double)vf.size() << endl;
	recall[p->first] = count_right / (double)count;
	}
	int all = 0;
	cout << c_info1.size() << endl;
	ofstream outfile(working_path+"result.txt", ios::app);
	if (outfile.fail()) {
	    printf("open outifle error\n");
	    return;
	}
	system_clock::time_point today = system_clock::now();
	std::time_t tt = system_clock::to_time_t (today);
	outfile << string(ctime(&tt)) << "features : \t" << num << endl;
	outfile << "class name\t"<<"training set\t"<<"testing set\t"<<"precision\t"<<"recall rate\t"<<"F value\n";
	for (auto p = c_info1.begin(); p != c_info1.end(); ++p) {
	    all += p->second;
	    const double precision = c_info2[p->first] / (double)p->second;
	    cout << "class " << p->first << " : \n";
	    cout << "precision : " << precision << endl;
	    cout << "recall rate : " << recall[p->first] << endl;
	    cout << "F : " << precision * recall[p->first] * 2 /(recall[p->first] + precision) << endl;
	    outfile << p->first << "\t" << int(class_map[p->first].size() * ratio) << "\t"
		<< (int) (class_map[p->first].size() * (1.0-ratio)) << "\t" << precision << "\t" << recall[p->first] << "\t"
		<< precision * recall[p->first] * 2 /(recall[p->first] + precision) << endl;
	}
	const double drecall = count_all_right / (double)count_all;
	outfile << "total :\t" << drecall << endl << endl;
	//const double precision = count_all_right / (double)all;
	//cout << "total recall rate : " << drecall << endl;
	//cout << "total precision : " << precision << endl;
	//cout << "total F : " << 2 * precision * drecall /(precision + drecall) << endl;
	cout << "total accuracy :\t" << drecall << endl;
	return ;
    }


    int main()
    {
	test_svmtextc(800);
	vector<string> t;
	char_separator(t, "I love 000 haha!", " \n\t");
	keep_window_open();
	return 0;
    }
