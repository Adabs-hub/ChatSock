#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
class TextStream{

public:
	void LoadUsers(std::string filename);
	bool userExit(const std::string file);
	void ReadText(std::string& file);
	void WriteToText(const std::string textfile, const std::string str);

private:
	std::vector<std::string> users;

	};							
