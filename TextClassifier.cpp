
#include "TextClassifier.h"
#include "BayesianTextClassifier.h"
#include "RandomForestClassifier.h"
#include "LogitRegressionClassifier.h"
#include "scanfile.h"

#include <cmath>
#include <string>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <cstdio>
#include <stdlib.h>
#include <sstream>
#include <string.h>
#include <algorithm>
#include <vector>
#include <ctime>
#include <chrono>
#include <ratio>


#ifdef __linux__
  const char* training_file_path = "io/train.txt";
  const char* model_file_path = "io/model.txt";
  const char* gbk_features_file_path = "io/gbkfeatures.txt";
  const char* utf8_features_file_path = "io/utf8features.txt";
  const char* result_file_path = "io/result.txt";
  const char* classes_file_path = "io/classes.txt";
#else
  const char* training_file_path = "io\\train.txt";
  const char* model_file_path = "io\\model.txt";
  const char* gbk_features_file_path = "io\\gbkfeatures.txt";
  const char* utf8_features_file_path = "io\\utf8features.txt";
  const char* result_file_path = "io\\result.txt";
  const char* classes_file_path = "io\\classes.txt";
#endif


void char_separator(std::vector<std::string>& tokens, const std::string& s, const std::string& drop_char)
{
  std::unordered_set<char> drops;
  std::for_each(drop_char.begin(), drop_char.end(), [&drops](char c){drops.insert(c);});
  std::string temp;
  for(auto p = s.begin(); p != s.end(); ++p) {
    if ( drops.end() != drops.find(*p)) {
      if ( !temp.empty() )
        tokens.push_back(temp);
      temp.clear();
    }
    else {
      temp += *p;
    }
  }
  if ( !temp.empty() )
    tokens.push_back(temp);
  temp.clear();
}


TextClassifier::TextClassifier(
  long featurenum, 
  codingtype::SeparaterType encoding_t, 
  classifiertype::ClassifierType classifier_t,
  const char* classifier_config_f 
  )
  : features_num(featurenum)
  , encoding_type(encoding_t)
  , classifier_type(classifier_t)
  , classifier_config_file(classifier_config_f)
  , count_classnum(0)
  , count_training_set(0)
  , prepare_cts(false)
  , classifier(NULL)
{
  if ( classifiertype::ClassifierType::Bayesian == classifier_type ) {
    classifier = new bayesianc::BayesianTextClassifier();
  }
  else if ( classifiertype::ClassifierType::RandomForest == classifier_type ) {
    classifier = new randomforestc::RandomForestClassifier();
  }
  else if ( classifiertype::ClassifierType::LogitRegression == classifier_type ) {
    classifier = new logitregressionc::LogitRegressionClassifier();
  }
  else {

  }

  if ( NULL == classifier ) {
    printf ("TextClassifier::TextClassifier(): can't allocate memory for classifier\n");
  }
  load_features();
  std::ofstream outfile(training_file_path);
  if ( outfile.fail() ) {
    printf("TextClassifier::TextClassifier(): error in opening %s\n", training_file_path);
  }
  outfile.close();
}


TextClassifier::~TextClassifier()
{
  classifier->free_model();
  if ( NULL != classifier )
    delete classifier;
  classifier = NULL;
}

void TextClassifier::load_features()
{
  std::ifstream infile;
  if (codingtype::UTF8 == encoding_type)
    infile.open(utf8_features_file_path);
  else if(codingtype::GBK == encoding_type)
    infile.open(gbk_features_file_path);

  if (infile.fail()) {
    printf("TextClassifier::load_features(): error open %s\n", gbk_features_file_path);
    return;
  }
  long i = 0;
  while (!infile.eof() && i < features_num) {
    std::string line;
    std::getline(infile, line);
    std::vector<std::string> v;
    separater(line, v, encoding_type);
    for (auto p = v.begin(); p != v.end(); ++p) {
      if (p->size() > 0) {
        features[i] = *p;
        ++i;
        break;
      }
    }
  }
  printf ("TextClassifier::load_features(): features num = %ld, size of features = %ld\n", features_num, i);
  infile.close();
}

void TextClassifier::train()
{
  classifier->train_on_file (training_file_path);
  printf ("TextClassifier::training_on_file() : finished training on file\n");
  classifier->save_model (model_file_path);
  printf ("TextClassifier::training_on_file() : finished saving model\n");
  classifier->free_model ();
  printf ("TextClassifier::training_on_file() : finished free model\n");
}

bool TextClassifier::load_data()
{
  load_features();
  classifier->load_model (model_file_path);
  load_classes();
  return true;
}

void TextClassifier::prepare_classname_to_string()
{
  for (int i = 0; i < count_classnum; ++i) {
    std::string str;
    for (int j = 0; j < count_classnum; ++j) {
      if (i == j) {
        str += "1 ";
      }
      else {
        str += "0 ";
      }
    }
    class_to_string_map[i] = str;
  }
  prepare_cts = true;
}

void TextClassifier::add_train_data(const std::string& classname, const std::string& buffer)
{
  if ( !prepare_cts) {
    prepare_classname_to_string();
  }
  std::unordered_map<std::string, int> bag;
  std::vector<std::string> tok;
  separater(buffer, tok, encoding_type);
  int count_word = 0;
  std::for_each (tok.begin(), tok.end(), [&bag, &count_word](std::string s){
    bag[s]++;
    count_word++;});

  //printf ("%ld, %ld\n", bag.size(), count_word);

  std::ofstream outfile(training_file_path, std::ios::app);
  if ( outfile.fail() ) {
    printf("TextClassifier::add_train_data: error in opening %s\n", training_file_path);
    return;
  }

  for (int i = 0; i < features_num; ++i) {
    auto tp = bag.find(features[i]);
    if (tp != bag.end()) {
      outfile << tp->second << " ";// / (double)count_word << " ";
      // std::cout << tp->second / (double)count_word<< " ";
    }
    else {
      outfile << 0 << " ";
    }
  }
  outfile << std::endl << class_to_string_map[classname_to_int(classname)] << std::endl;
  count_training_set++;
  outfile.close();
}

void TextClassifier::preprocessor()
{
  std::ifstream infile (training_file_path);
  if (infile.fail()) {
    printf("TextClassifier::preprocessor: error in opening train.txt\n");
    return;
  }
  infile.seekg (0, infile.end);
  int length = infile.tellg();
  infile.seekg (0, infile.beg);
  char *buffer = new char[length];
  if (nullptr == buffer) {
    printf("TextClassifier::preprocessor:allocate memory error\n");
    return;
  }
  infile.read(buffer, length);
  infile.close();

  std::ofstream outfile(training_file_path, std::fstream::binary);
  if (outfile.fail()) {
    printf("TextClassifier::preprocessor: error in opening train.txt\n");
    delete buffer;
    buffer = nullptr;
    return;
  }
  sprintf(first_trainfile_line, "%ld %ld %ld\n", count_training_set, features_num, count_classnum);
  outfile.write(first_trainfile_line, strlen(first_trainfile_line));
  outfile.write(buffer, length);
  outfile.close();
  printf("TextClassifier::preprocessor() : finish making train.txt\n");
  // printf("TextClassifier::preprocessor() : finish free memory\n");
  // printf("TextClassifier::preprocessor() : clear training set\n");
  save_classes();
  if (nullptr != buffer) {
    delete buffer;
    printf("delete buffer successed\n");
  }
}

void TextClassifier::save_classes() const
{
  if (classname_int.size() < 1) {
    printf ("TextClassifier::save_classes : no classes\n");
    return;
  }
  std::ofstream outfile(classes_file_path);
  if (outfile.fail()) {
    printf ("TextClassifier::save_classes: can't open file %s\n", classes_file_path);
    return;
  }

  std::for_each(classname_int.begin(), classname_int.end(), 
    [&outfile](std::pair<std::string, int> p){ outfile << p.first << " " << p.second << std::endl;});
  outfile.close();
  printf("TextClassifier::save_classes() : save classes successed\n");
}

void TextClassifier::add_classname(const std::string& classname)
{
  if (classname_int.find(classname) != classname_int.end()) 
    return;
  int_classname[count_classnum] = classname;
  classname_int[classname] = count_classnum;
  count_classnum++;
}

bool TextClassifier::load_classes()
{
  int_classname.clear();
  classname_int.clear();
  std::ifstream infile(classes_file_path);
  if (infile.fail()) {
    printf("can't open classesfile : %s\n", classes_file_path);
    return false;
  }
  while (!infile.eof()) {
    std::string line;
    std::getline(infile, line);
    if (line.size() < 1)
      break;
    // printf ("%s\n", line.c_str());
    std::string s1;
    std::string s2;
    int num = -1;
    bool state = 0;
    for (auto p = line.begin(); p != line.end(); ++p) {
      if (0 == state) {
        if (*p != 32)
          s1 += *p;
        else
          state = 1;
      }
      else if (1 == state) {
        if (*p >= '0' && *p <= '9')
          s2 += *p;
        else {
          std::stringstream ss(s2);
          ss >> num;
          break;
        }
      }
      else
        break;
    }
    if (num < 0) {
      std::stringstream ss(s2);
      ss >> num;
    }
    if (s1.size() > 0 && num > -1) {
      classname_int[s1] = num;
      int_classname[num] = s1;
      // printf ("%s %ld\n", s1.c_str(), num);
    }
  }
  infile.close();
  // for (auto p = classname_int.begin(); p != classname_int.end(); ++p) {
  //   printf ("%s %ld\n", (p->first).c_str(), p->second);
  // }
  // printf("heeeeeeeeeeeeeeeeeeeeeeeeeee\n");
  return true;
}

const char* TextClassifier::predicted_category(const char* data) const
{
  int res;
  std::unordered_map<std::string, int> bag;
  std::vector<std::string> tok;
  separater(data, tok, encoding_type);
  int count_word = 0;
  for (auto p = tok.begin(); p != tok.end(); ++p) {
    bag[*p]++;
    ++count_word;
  }
  double *fv = new double[features_num];
  for (int i = 0; i < features_num; ++i) {
    auto p = bag.find (features[i]);
    if ( p != bag.end() ) {
      fv[i] = p->second;// / (double)count_word;
    }
    else 
      fv[i] = 0.0;
  }
  classifier->predicted_category (fv, res);
  delete fv;
  return int_to_classname(res).c_str();
}

void TextClassifier::show_model()
{
  classifier->show_model();
}

bool TextClassifier::add_training_set (const std::string& train_dir)
{
  std::vector<std::string> dirs;
  get_dirs (train_dir, dirs);

  for (std::vector<std::string>::iterator p = dirs.begin(); p != dirs.end(); ++p) {
    add_classname (*p);
  }

  for (std::vector<std::string>::iterator p = dirs.begin(); p != dirs.end(); ++p) {
    #ifdef __linux__
    std::string sub_dir = train_dir + *p + "/";
    #else 
    std::string sub_dir = train_dir + *p + "\\";
    #endif
    std::vector<std::string> files;
    get_files (sub_dir, files);
    if (files.size() <= 0) {
      printf ("TextClassifier::add_training_set() : There's a empty class %s\n", sub_dir.c_str());
      return false;
    }

    for (size_t i = 0; i < files.size(); ++i) {
      if ( (i+1) / 10 == 0 || (i+1) == files.size()) {
        printf ("\rTextClassifier::add_training_set() : processing %s %ld%% : %ld",
         sub_dir.c_str(), ((i+1) * 100)/ files.size(), i);
      }
      std::ifstream infile(sub_dir+files[i]);
      if (infile.fail()) {
        printf ("TextClassifier::add_training_set() : error in opening %s\n", files[i].c_str());
        exit (1);
      } 
      infile.seekg(0, infile.end); 
      int length = infile.tellg(); 
      infile.seekg(0, infile.beg); 
      char* buffer = new char[length+1]; 
      if (NULL == buffer) { 
        printf ("TextClassifier::add_training_set() : can't allocate memory\n");
        exit (1);
      }
      buffer[length] = 0;
      infile.read (buffer, length);
      add_train_data (*p, buffer);
      delete buffer;
      infile.close();
    }
    printf("\n");
    // printf(" : %ld\n", files.size());
  }
  return true;
}

void TextClassifier::batch_predict (const std::string& dir, const std::string& outfilename)
{
  using namespace std::chrono;

  std::ofstream outfile (outfilename);
  if (outfile.fail()) {
    printf("TextClassifier::batch_predict() : error in opening %s\n", outfilename.c_str());
    exit (1);
  }
  system_clock::time_point start_time = system_clock::now();
  std::time_t tt = system_clock::to_time_t (start_time);
  outfile << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
  outfile << "<start_time>\n  " << std::string (ctime (&tt)) << "</start_time>\n\n";
  // outfile.write (line.c_str(), line.size());
  std::unordered_map<std::string, std::vector<std::string>> class_of_files;
  std::vector<std::string> files;
  get_files (dir, files);
  size_t count_files = 0;
  for (std::vector<std::string>::iterator p = files.begin(); p != files.end(); ++p) {
    std::ifstream infile (dir+(*p));
    ++count_files;
    if (infile.fail()) {
      printf("TextClassifier::batch_predict() : error in opening %s\n", (dir+(*p)).c_str());
      exit (1);
    }
    infile.seekg(0, infile.end); 
    long length = infile.tellg(); 
    infile.seekg(0, infile.beg); 
    char* buffer = new char[length+1]; 
    if (NULL == buffer) { 
      printf ("TextClassifier::add_training_set() : can't allocate memory\n");
      exit (1);
    }
    buffer[length] = 0;
    infile.read (buffer, length);
    class_of_files [predicted_category (buffer)].push_back (*p);
    delete buffer;
    infile.close();
  }

  for (auto p = class_of_files.begin(); p != class_of_files.end(); ++p) {
    outfile << "<class name = \"" << p->first << "\">\n";
    for (auto pp = (p->second).begin(); pp != (p->second).end(); ++pp) {
      outfile << "  <file>" << *pp << "</file>\n";
    }
    outfile << "</class>\n\n";
  }
  system_clock::time_point end_time = system_clock::now();
  tt = system_clock::to_time_t (end_time);
  outfile << "<end_time>\n  " << std::string (ctime (&tt)) << "</end_time>\n\n";
  // system_clock::time_point past_time = end_time - start_time;
  // tt = system_clock::to_time_t (past_time);
  outfile << "<processing_speed measurement = \"doc/s\">\n  ";
  outfile << (double) count_files * 1E3 / duration_cast<milliseconds> (end_time-start_time).count() ;
  outfile << "\n</processing_speed>\n\n";
  //hours,minutes,seconds,milliseconds,microseconds,nanoseconds
  outfile.close ();
}

bool TextClassifier::auto_test (const std::string& train_dir, const std::string& resfile, const double ratio)
{
  using std::chrono::system_clock;

  // const std::string working_path = "io/";
  std::unordered_map<std::string, std::vector<std::string>> class_map;
  // string train_dir = "../training_set/easy_train/";
  // string train_dir = "../training_set/test_train/";
  std::vector<std::string> dirs;
  get_dirs (train_dir, dirs);
  for (auto p = dirs.begin(); p != dirs.end(); ++p) {
    add_classname (*p);  
    std::string sub_path = train_dir + *p + "/";
    std::vector<std::string> files;
    get_files (sub_path, files);
    class_map [*p] = files;
    printf ("class %s has %ld files\n", sub_path.c_str (), (long)files.size());
  }
  // printf ("2\n");
  dirs.clear ();
  for (auto p = class_map.begin(); p != class_map.end(); ++p) {
    const int maxindex = p->second.size() * ratio;
    for (int i = 0; i < maxindex; ++i) {
      std::string spath = train_dir + p->first + "/" + p->second[i];
      std::ifstream infile (spath);
      if (infile.fail ()) {
        printf ("no such file %s\n", p->second[i].c_str());
        return false;
      }
      infile.seekg (0, infile.end);
      int length = infile.tellg();
      infile.seekg (0, infile.beg);
      char* buffer = new char[length+1];
      buffer [length] = 0;
      infile.read (buffer, length);
      // cout << buffer << endl;
      add_train_data (p->first, buffer);
      // ++count;
      delete buffer;
      infile.close();
    }
  }

  preprocessor();
  train();
  load_data();

  //记录被标记为该类别的样本数（用于计算precision）
  std::unordered_map<std::string, int> c_info1;
  //记录被正确标注的样本数
  std::unordered_map<std::string, int> c_info2;
  std::unordered_map<std::string, double> recall;
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
    // for (int i = 0; i < min; ++i) {
      std::string spath = train_dir + p->first + "/" + p->second[i];
      std::ifstream infile(spath);
      if (infile.fail()) {
        printf("no such file %s\n", p->second[i].c_str());
        return false;
      }
      infile.seekg (0, infile.end);
      int length = infile.tellg();
      infile.seekg (0, infile.beg);
      char* buffer = new char [length+1];
      buffer [length] = 0;
      infile.read (buffer, length);
      std::string sc = predicted_category (buffer);
      ++count;
      ++count_all;
      c_info1[sc]++;
      //cout << p->first << endl;
      if(p->first == sc) {
        ++count_right;
        ++count_all_right;
      }
      delete buffer;
      infile.close ();
    }
    c_info2[p->first] = count_right;
    std::cout << std::endl;
    std::cout << p->first <<  " ： right number : " << count_right << std::endl;
    //cout << "Recall Rate : " << count_right / (double)vf.size() << endl;
    recall [p->first] = count_right / (double)count;
  }
  int all = 0;
  std::cout << c_info1.size () << std::endl;
  std::ofstream outfile (resfile, std::ios::app);
  if (outfile.fail()) {
      printf("open outifle error\n");
      return false;
  }
  system_clock::time_point today = system_clock::now();
  std::time_t tt = system_clock::to_time_t (today);
  outfile << std::string(ctime(&tt)) << "features : \t" << features_num << std::endl;
  outfile << "class name\t"<<"training set\t"<<"testing set\t"<<"precision\t"<<"recall rate\t"<<"F value" << std::endl;
  for (auto p = c_info1.begin(); p != c_info1.end(); ++p) {
      all += p->second;
      const double precision = c_info2[p->first] / (double)p->second;
      std::cout << "class " << p->first << " : \n";
      std::cout << "precision : " << precision << std::endl;
      std::cout << "recall rate : " << recall[p->first] << std::endl;
      std::cout << "F : " << precision * recall[p->first] * 2 /(recall[p->first] + precision) << std::endl;
      outfile << p->first << "\t" << int(class_map[p->first].size() * ratio) << "\t"
    << (int) (class_map[p->first].size() * (1.0-ratio)) << "\t" << precision << "\t" << recall[p->first] << "\t"
    << precision * recall[p->first] * 2 /(recall[p->first] + precision) << std::endl;
  }
  const double drecall = count_all_right / (double)count_all;
  outfile << "total :\t" << drecall << std::endl << std::endl;
  //const double precision = count_all_right / (double)all;
  //cout << "total recall rate : " << drecall << endl;
  //cout << "total precision : " << precision << endl;
  //cout << "total F : " << 2 * precision * drecall /(precision + drecall) << endl;
  std::cout << "total accuracy :\t" << drecall << std::endl;


  // tc->show_model();
  return true;
}

