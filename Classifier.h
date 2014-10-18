#pragma once


namespace classifiertype{
  enum ClassifierType{
    Bayesian = 0,
    SVM,
    NeuralNetwork
  };
}


class CClassifier
{
public:
	//virtual void add_train_data(const char* s) = 0;
	virtual void train_on_file(const char* training_file) = 0;
	virtual void save_model(const char* model_file) = 0;
	virtual void predicted_category(const char* input_file) = 0;
};