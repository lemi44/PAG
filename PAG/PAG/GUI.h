#pragma once

#include "Model.h"

class GUI
{
public:
	static void ShowPropertyEditor(bool* p_open, std::vector<Model*>& all_models);
private:
	static void ShowObject(const char* prefix, Model* mdl);
	static std::string const labels[5];
	static float const min[4];
	static float const max[4];
};