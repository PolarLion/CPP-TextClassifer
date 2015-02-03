#include "Features.h"
#include <unordered_map>
#include <fstream>

std::vector<double> Features::doc_to_vec (const std::vector<std::string>& doc) const
{	
	std::vector<double> vec;
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
	}
	return vec;
}


double* Features::doc_to_ptr (const std::vector<std::string>& doc) const
{
	double* ptr = new double[get_features_num ()];
	if (NULL == ptr) {
		printf ("Features::doc_to_ptr() : can't allocae memory\n");
		return ptr;
	}
	for (auto p = doc.begin (); p != doc.end (); ++p) {
		if (features_index.find (*p) != features_index.end () ) {
			std::vector<long> indexs = get_indexs (*p);
			for (auto pp = indexs.begin (); pp != indexs.end (); ++pp ) {
				ptr[*pp] += 1.0;
			}
		}
		else
			continue;
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
	long count_classes = 0;
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
	features_num = count_classes;
	infile.close ();
	return true;
}
