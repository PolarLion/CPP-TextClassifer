#pragma once

#include "Classifier.h"
#include "dataanalysis.h"
#include <unordered_map>

namespace logitregressionc{


class LogitRegressionClassifier:public Classifier 
{
private:
  alglib::logitmodel lm;
public:
  LogitRegressionClassifier ();
  LogitRegressionClassifier (const char* config_file);
  ~LogitRegressionClassifier ();
  void init_model (const char* config_file);
  void train_on_file (const char* training_file);
  void save_model (const char* model_file);
  void load_model (const char* model_file);
  void predicted_category (const double* features, int& res);
  void free_model ();
};

}

