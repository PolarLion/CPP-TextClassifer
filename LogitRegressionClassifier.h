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
  bool init_model (const char* config_file);
  bool train_on_file (const char* training_file);
  bool save_model (const char* model_file);
  bool load_model (const char* model_file);
  bool predicted_category (const double* features, int& res);
  bool free_model ();
};

}

