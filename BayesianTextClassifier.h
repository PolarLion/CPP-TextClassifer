#pragma once

#include "Classifier.h"

namespace bayesianc {

struct Item {
  double *v;
};


class BayesianTextClassifier:public Classifier 
{
private:
  Item *bayesiantable;
  
  bool show_bayesiantable();
  bool init_bayesiantable(int f, int c);
public:
  BayesianTextClassifier();
  ~BayesianTextClassifier();
  // void init_model(const char* config_file);
  bool train_on_file(const char* training_file);
  bool save_model(const char* model_file);
  bool load_model(const char* model_file);
  bool predicted_category(const double* features, int& res);
  bool free_model();
  bool show_model();
};

}

