#pragma once


namespace classifiertype{
  enum ClassifierType{
    Bayesian = 0,
    SVM,
    NeuralNetwork
  };
}


class Classifier
{
private:
  virtual void train_model() = 0;
public:
  Classifier(){}
  ~Classifier(){}
  virtual void init_model(const char* config_file) = 0;
	virtual void train_on_file(const char* training_file) = 0;
	virtual void save_model(const char* model_file) = 0;
  virtual void load_model(const char* model_file) = 0;
	virtual void predicted_category(const char* input_file) = 0;
  virtual void free_model() = 0;
};