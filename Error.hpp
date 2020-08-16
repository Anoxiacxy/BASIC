#pragma once
#include <memory>
#include "ASTNode.hpp"
#include <vector>
#include <string>
#include <map>

std::map<int, std::string> idToLine;
std::map<int, std::vector<std::string> > errorMsg;

void setCode(std::vector<std::string>code) {
	for (auto line : code) {
		int id = 0, p = 0;
		while (p < line.size() && isspace(line[p])) p++;
		while (p < line.size() && isdigit(line[p])) 
			id = id * 10 + line[p] - '0', p++;
		idToLine[id] = line;
	}
}

void logError(std::string str, int lineno = 0) {
	if (lineno != 0) {
		errorMsg[lineno].push_back(str);	
	}
}

int printError(std::ostream &os) {
	for (auto line : errorMsg) {
		os << "In :\n\t" + idToLine[line.first] << std::endl;
		for (auto msg : line.second)
			os << msg << std::endl;
	}
	return errorMsg.size();
}
