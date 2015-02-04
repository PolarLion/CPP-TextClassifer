#include "Features.h"
#include <unordered_map>
#include <fstream>
#include <iostream>

std::vector<double> Features::doc_to_vec (const std::vector<std::string>& doc) const
{	
	std::vector<double> vec;
	for (long i = 0; i < get_features_dim (); ++i) {
		vec.push_back (0);
	}
	/* 
	for (int i = 0; i < get_features_num() ; ++i) {
	 	vec.push_back (0.0);
	}
	for (auto p = doc.begin (); p != doc.end (); ++p) {
		if (features_index.find (*p) != features_index.end () ) {
			std::vector<long> indexs = get_indexs (*p);
			for (auto pp = indexs.begin (); pp != indexs.end (); ++pp ) {
				vec[*pp] += 1.0;
			}
		}
		else 
			continue;
	}*/  
	for (auto p = doc.begin (); p != doc.end (); ++p) {
		auto fv = features.find (*p);
		if (fv != features.end ()) {
			for (long i = 0; i < get_features_dim(); ++i) {
				vec[i] += fv->second[i];
			}
		}	
	}
	return vec;
}


double* Features::doc_to_ptr (const std::vector<std::string>& doc) const
{
	//double* ptr = new double[get_features_num ()];
	double* ptr = new double[get_features_dim ()];
	if (NULL == ptr) {
		printf ("Features::doc_to_ptr() : can't allocae memory\n");
		return ptr;
	}
	/*
	for (auto p = doc.begin (); p != doc.end (); ++p) {
		if (features_index.find (*p) != features_index.end () ) {
			std::vector<long> indexs = get_indexs (*p);
			for (auto pp = indexs.begin (); pp != indexs.end (); ++pp ) {
				ptr[*pp] += 1.0;
			}
		}
		else
			continue;
	}*/
	for (auto p = doc.begin (); p != doc.end (); ++p) {
		auto fv = features.find (*p);
		if (fv != features.end ()) {
			for (long i = 0; i < get_features_dim (); ++i) {
				ptr[i] += fv->second[i];
			}
		}
	}
	return ptr;
}


bool Features::load_features (const char* filename)
{
	std::ifstream infile (filename);
	if (infile.fail ()) {
		printf ("Features::load_features() : error in opening %s\n", filename);
		return false;
	}
	//long count_classes = 0;
	/*
	while (!infile.eof ()) {
		std::string line;
		std::getline (infile, line);
		if (line.size () <= 0) 
			continue;
		std::string ch;
		for (auto p = line.begin (); p != line.end (); ++p) {
			if (*p == ' ') {
				features_index[ch].push_back (count_classes);
				ch.clear ();
			} 
			else {
				ch += *p;
			}
		}
		if (ch.size () > 0) {
			features_index[ch].push_back (count_classes);
		}
		++count_classes;
	}
	  */
	std::string tmp;
	std::string line;
	std::getline (infile, line);
	std::cout << line << std::endl;
	long t[2] = {0};
	long t_index = 0;
	for (auto p = line.begin (); p != line.end (); ++p) {
		if (*p == ' ' && t_index < 2) {
			t[t_index++] = atol (tmp.c_str());
			tmp.clear ();
		}
		else {
			tmp += *p;
		}
	}
	t[1] = atol(tmp.c_str());
	if (!t[0] || !t[1]) {
		std::cout << t[0] << " " << t[1] << std::endl;
		printf ("Features::load_features() : error in first line\n");	 
		return false;
	}
	features_num = t[0];
	features_dim = t[1];
	t_index = 0;
	tmp.clear ();
	long count_line = 0;
	std::string tmp1;
	while (!infile.eof ()) {
		std::getline (infile, line);
		if (line.size () < 1) {
			continue;
		}
		if (count_line++ > features_num) {
			break;
		}
		std::vector<double> fv;
		for (auto p = line.begin (); p != line.end (); ++p) {
			if (*p == ' ') {
				if (0 == t_index) {
					t_index++;
					tmp1 = tmp;
					tmp.clear ();
					continue;
				}
				else if (t_index <= get_features_dim ()) {
					t_index++;
					fv.push_back (atof (tmp.c_str ()));
					//std::cout << atof (tmp.c_str ()) << " ";
					tmp.clear ();
				}
			}
			else {
				tmp += *p;
			}
		}
		//std::cout << std::endl;
		t_index = 0;
		if (fv.size () == get_features_dim ()) {
			features[tmp1] = fv;
		}
		else {
			printf ("Fatures::load_features() : wrong data in %ld line %d\n", count_line, fv.size ());
			std::cout << line << std::endl;
			return false;
		}
	}
	//features_num = count_classes;
	/*
	std::cout << features_num << " " << features_dim << std::endl;
	for (auto p = features.begin (); p != features.end(); ++p) {
		std::cout << p->first << " ";
		for (auto pp = p->second.begin() ; pp != p->second.end (); ++pp) {
			//std::cout << *pp << " ";
		}
		std::cout << std::endl;
		if (p->second.size () != features_dim) {
			std::cout << p->second.size () << std::endl;
		}
	}*/
	infile.close ();
	return true;
}


