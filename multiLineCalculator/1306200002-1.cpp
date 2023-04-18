// Name: Zekiye Ozdemir
// Student Number: 130620002
// Date: 17.01.2020
// Development Environment: Visual Studio2019

// *** File system will only work with C++ 17 make sure you have enabled the C++ 17 
// support. Project -> Settings -> C++/C -> LANGUAGE -> LANGUAGE STANDARD
// *** <filesystem> header dosyasý sadece C++ 17 ile calisir bunun icin c++ 17 kullanmak gerekiyor.

// Base required libraries
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

// Libraries for stack implementation
#include<stack>
#include<map>
#include<algorithm>

// Libraries for multiple filereading. Only enable if it is required to handle multiple .inp tests. Also uncomment the code in main func. 
#include<filesystem>
//C++ 17
namespace fs = std::filesystem;

using namespace std;

class Variable {
public:
	string name;
	double doubValue;
	bool isDouble;
	bool defined;

	Variable() {
		name = "";
		doubValue = -1;
		defined = false;
	}

	// Checks if a passed value is double or not
	bool checkDouble(string str) {
		for (int i = 0; i < str.size(); i++) {
			if (str[i] == '.') return true;
		}
		return false;
	}
};

class Keywords {
private:
	vector<string> keywords = { "OUT","IN","IF","THEN","ELSE","LOOP","TIMES" };

	//returns true if an already present value is double
	bool checkDouble(vector<Variable> vec) {
		for (int i = 0; i < vec.size(); i++) {
			if (vec.at(i).isDouble) return true;
		}
		return false;
	};
public:
	// Used for outputting to a file
	void outFILE(string filename, vector<Variable> vec, string str) {
		string variable;
		for (int i = 4; i < str.length(); i++) {
			variable += str[i];
		}

		double value{};
		for (int i = 0; i < vec.size(); i++) {
			if (vec.at(i).name == variable) value = vec.at(i).doubValue;
		}
		string extension = ".out";
		filename.append(extension);
		//!OUTPUT FILE NAME CHECK
		string fs = "../Debug/";
		fs.append(filename);
		ofstream myfile(fs);

		if (!checkDouble(vec)) {
			int intValue = int(value);
			myfile << to_string(intValue);
		}
		else {
			myfile << to_string(value);
		}
		cout << filename << " written in the debug folder." << endl;
	};
};

class Operators {
private:
	vector<string> operators = { "=","*","/","+","-","<",">","==","<=",">=","!","!=" };
public:
	// returns a string after identifying an operator
	string identifyOperator(string str) {
		for (int i = 0; i < operators.size(); i++) {
			if (operators[0] == str) return "equals";
			else if (operators[1] == str) return "multiply";
			else if (operators[2] == str) return "divide";
			else if (operators[3] == str) return "add";
			else if (operators[4] == str) return "subtract";
			else if (operators[5] == str) return "less than";
			else if (operators[6] == str) return "greater than";
			else if (operators[7] == str) return "equates";
			else if (operators[8] == str) return "less than or equal to";
			else if (operators[9] == str) return "greater than or equal to";
			else if (operators[10] == str) return "negates";
			else if (operators[11] == str) return "not equal to";
		}
	}

	// checks if a passed value is operator or not
	bool isOperator(string str) {
		for (int i = 0; i < operators.size(); i++) {
			if (operators.at(i) == str) return true;
		}
		return false;
	}
};

class Identifier {
private:
	string keywords[7] = { "OUT","IN","IF","THEN","ELSE","LOOP","TIMES" };
	string operators[12] = { "=","*","/","+","-","<",">","==","<=",">=","!","!=" };

	// checks if a passed value is only digits. Also checks for double value
	bool is_digits(const string& str)
	{
		return str.find_first_not_of("0123456789.") == string::npos;
	}

public:
	string identified;

	// identifies a value and returns a string accordingly
	string identify(string value) {
		if (value.empty()) return "";

		for (int i = 0; i < 7; i++) {
			if (keywords[i] == value) return "keyword";
		}

		for (int i = 0; i < 12; i++) {
			if (operators[i] == value) return "operator";
		}

		if (is_digits(value)) return "number";

		return "variable";
	}
};

//helper functions for variable calculation
bool containsVariable(vector<Variable>, string);
double calculateValue(vector<Variable>, string);
double variableValue(vector<Variable>, string);
int variablePosition(vector<Variable>, string);

//functions for code reduction
void calculateTotalSum(vector<Variable> vec, string var, string finalOperator, double& totalValue);

//stack functions
int evaluate(string);
int applyOp(int, int, char);
int precedence(char);

// main program runs here
void runProgram(string path, string filename);

// Filereading from directory is done here.
int main() {
	//!INPUT FILE NAME
	string path("../Debug");
	
	
	string ext(".inp");
	if (fs::is_directory(path)) {
		for (auto& p : fs::recursive_directory_iterator(path))
		{
			if (p.path().extension() == ext) {
				runProgram(p.path().string(), p.path().stem().string());
			}
		}
	}
	else {
		path = "./Debug";
		if (fs::is_directory(path)) {
			for (auto& p : fs::recursive_directory_iterator(path))
			{
				if (p.path().extension() == ext) {
					runProgram(p.path().string(), p.path().stem().string());
				}
			}
		}
		else {
			path = "./";
			for (auto& p : fs::recursive_directory_iterator(path))
			{
				if (p.path().extension() == ext) {
					runProgram(p.path().string(), p.path().stem().string());
				}
			}
		}
	}

	return 0;
}

// checks the contents of file and does the required steps
void runProgram(string pathname, string filename) {
	Identifier identifier;
	vector<Variable> variables;
	Keywords keyword;
	Operators _operator;

	string str;
	string extension = ".inp";
	string _filename = "../" + filename;
	_filename.append(extension);
	ifstream file;

	if (!pathname.empty()) file.open(pathname);
	else return;

	//if (!file.is_open()) {
	//	pathname = "../Debug//challange7.inp";
	//	file.open(pathname);

	//	if (!file.is_open()) {
	//		pathname = "./challange7.inp";
	//		file.open(pathname);
	//		if (!file.is_open()) cout << "challange7.inp could not be opened" << endl;
	//	}
	//}
	//else {
		while (getline(file, str))
		{
			string reader;
			Variable variable;

			for (int i = 0; i <= str.length(); i++) {
				if (str[i] == ' ' || str[i] == '\0') {
					identifier.identified = identifier.identify(reader);

					if (identifier.identified == "variable") {
						if (!containsVariable(variables, reader)) {
							variable.name = reader;
							variable.defined = true;

							string innerStr;
							for (int j = i; j < str.length(); j++) {
								innerStr += str[j];
							}
							double value = calculateValue(variables, innerStr);
							if (value != 0) {
								variable.doubValue = value;
								variables.push_back(variable);
							}

						}
					}
					else if (identifier.identified == "keyword") {
						if (reader == "OUT") keyword.outFILE(filename, variables, str);
					}
					else if (identifier.identified == "number") {
						if (variable.defined) {
							variable.doubValue = stod(reader);
							variable.isDouble = variable.checkDouble(reader);
							variables.push_back(variable);
						}
					}

					reader = "";
				}
				else {
					reader += str[i];
				}
			}
		}
	}
//}

double calculateValue(vector<Variable> vec, string str) {
	//check if it contains equals or not
	bool flag = false;
	for (int i = 0; i < str.length(); i++) { if (str[i] == '=') flag = true; }
	if (!flag) return 0;


	Operators op;
	Identifier identifier;
	string finalOperator;
	Keywords keyword;

	if (str.find("IF") != string::npos && str.find("THEN") != string::npos && str.find("ELSE") != string::npos) {

		str.erase(remove(str.begin(), str.end(), ' '), str.end());

		
		int id = -1;

		if (str.find("<=") != std::string::npos) {
			id = 0;
		}
		else if (str.find(">=") != std::string::npos) {
			id = 1;
		}

		str.erase(remove(str.begin(), str.end(), '='), str.end());

		stringstream stringStream(str);
		string line;

		vector<string> wordVector;

		//extracting stuff after demlimiting it
		while (getline(stringStream, line))
		{
			size_t prev = 0, pos;
			while ((pos = line.find_first_of("IFTHENELSE", prev)) != string::npos)
			{
				if (pos > prev)
					wordVector.push_back(line.substr(prev, pos - prev));
				prev = pos + 1;
			}
			if (prev < line.length())
				wordVector.push_back(line.substr(prev, string::npos));
		}

		string var;
		string cpy = wordVector.at(0);

		for (int j = 0; j < cpy.length(); j++) {
			if (cpy[j] == '(') cpy.erase(remove(cpy.begin(), cpy.end(), '('), cpy.end());
			else if (cpy[j] == ')') cpy.erase(remove(cpy.begin(), cpy.end(), ')'), cpy.end());
			else if (cpy[j] == '{') cpy.erase(remove(cpy.begin(), cpy.end(), '{'), cpy.end());
			else if (cpy[j] == '}') cpy.erase(remove(cpy.begin(), cpy.end(), '}'), cpy.end());
		}


		for (int i = 0; i < wordVector.at(0).length(); i++) {
			if (!op.isOperator(string(1, wordVector.at(0)[i])))
			{
				var += wordVector.at(0)[i];
			}
			else {
				for (int j = 0; j < var.length(); j++) {
					if (var[j] == '(') var.erase(remove(var.begin(), var.end(), '('), var.end());
					else if (var[j] == ')') var.erase(remove(var.begin(), var.end(), ')'), var.end());
					else if (var[j] == '{') var.erase(remove(var.begin(), var.end(), '{'), var.end());
					else if (var[j] == '}') var.erase(remove(var.begin(), var.end(), '}'), var.end());
				}
				if (containsVariable(vec, var)) {
					int varPos = variablePosition(vec, var);
					cpy.replace(cpy.find(vec.at(varPos).name), vec.at(varPos).name.length(), to_string(int(vec.at(varPos).doubValue)));
				}
				var = "";
			}
		}

		if (containsVariable(vec, var)) {
			int varPos = variablePosition(vec, var);
			cpy.replace(cpy.find(vec.at(varPos).name), vec.at(varPos).name.length(), to_string(int(vec.at(varPos).doubValue)));
		}

		string buffer;
		double valueOne = 0, valueTwo = 0;
		bool valueOneFlag = false, valueTwoFlag = false;

		for (int i = 0; i < cpy.length(); i++) {
			if (cpy[i] == '>' || cpy[i] == '<' || cpy[i] == '=') {
				for (int i = 0; i < buffer.length(); i++) {
					if (buffer[i] >= '0' && buffer[i] <= '9') {
						continue;
					}
					else {
						buffer = to_string(evaluate(buffer));
					}
				}

				if (!valueOneFlag) {
					valueOne = stod(buffer);
					valueOneFlag = true;
					buffer = "";
				}
			}
			else if (i == cpy.length() - 1) {
				for (int j = i; j < cpy.length(); j++) buffer += cpy[j];

				if (!valueTwoFlag) {
					valueTwo = stod(buffer);
					valueTwoFlag = true;
					buffer = "";
				}
			}
			else {
				buffer += cpy[i];
			}
		}

		bool finalFlag;

		for (int i = 0; i < cpy.length(); i++) {
			if (cpy[i] == '>') {
				if (id == 1)finalFlag = valueOne >= valueTwo;
				else finalFlag = valueOne > valueTwo;
			}
			else if (cpy[i] == '<') {
				if (id == 0)  finalFlag = valueOne <= valueTwo;
				else finalFlag = valueOne < valueTwo;
			}
		}
		if (finalFlag) {
			return variableValue(vec, wordVector.at(1));
		}
		else if (!finalFlag) {
			return variableValue(vec, wordVector.at(2));
		}
	}

	// checking the number of operators and the final operator is set,
	// final operator helps in identifiying the steps for single operator calculation
	int operatorCount = 0;
	for (int i = 0; i <= str.length(); i++) {
		if (identifier.identify(string(1, str[i])) == "operator") {
			string identifiedOP = op.identifyOperator(string(1, str[i]));
			if (identifiedOP != "equals") {
				finalOperator = identifiedOP;
				operatorCount++;
			}
		}
	}

	double totalValue = 0;
	//if operator count is one the does the required islemler
	if (operatorCount == 1) {
		str.erase(remove(str.begin(), str.end(), ' '), str.end());
		str.erase(remove(str.begin(), str.end(), '='), str.end());
		string var;

		for (int i = 0; i < str.length(); i++) {
			if (!op.isOperator(string(1, str[i])))
			{
				var += str[i];
			}
			else {
				calculateTotalSum(vec, var, finalOperator, totalValue);
				var = "";
			}
		}
		calculateTotalSum(vec, var, finalOperator, totalValue);

	}
	else if (operatorCount > 0) { //used for multiple operators 
		bool flag = false;
		for (int i = 0; i < str.length(); i++) { if (str[i] == '=') flag = true; }

		if (flag) {
			str.erase(remove(str.begin(), str.end(), ' '), str.end());
			str.erase(remove(str.begin(), str.end(), '='), str.end());
			string var;
			string cpy = str;

			for (int i = 0; i < str.length(); i++) {
				if (!op.isOperator(string(1, str[i])))
				{
					var += str[i];
				}
				else {
					for (int j = 0; j < var.length(); j++) {
						if (var[j] == '(') var.erase(remove(var.begin(), var.end(), '('), var.end());
						else if (var[j] == ')') var.erase(remove(var.begin(), var.end(), ')'), var.end());
						else if (var[j] == '{') var.erase(remove(var.begin(), var.end(), '{'), var.end());
						else if (var[j] == '}') var.erase(remove(var.begin(), var.end(), '}'), var.end());
					}
					if (containsVariable(vec, var)) {
						int varPos = variablePosition(vec, var);
						cpy.replace(cpy.find(vec.at(varPos).name), vec.at(varPos).name.length(), to_string(int(vec.at(varPos).doubValue)));
					}
					var = "";
				}
			}

			if (containsVariable(vec, var)) {
				int varPos = variablePosition(vec, var);
				cpy.replace(cpy.find(vec.at(varPos).name), vec.at(varPos).name.length(), to_string(int(vec.at(varPos).doubValue)));
			}

			return double(evaluate(cpy));
		}
	}

	return totalValue;
};

// calculates the sum for single operator 
void calculateTotalSum(vector<Variable> vec, string var, string finalOperator, double& totalValue) {
	if (containsVariable(vec, var)) {
		if (finalOperator == "add") totalValue += variableValue(vec, var);
		else if (finalOperator == "divide") {
			if (totalValue == 0) totalValue += variableValue(vec, var);
			else totalValue = totalValue / variableValue(vec, var);
		}
		else if (finalOperator == "multiply") {
			if (totalValue == 0) totalValue += variableValue(vec, var);
			else totalValue = totalValue * variableValue(vec, var);
		}
		else if (finalOperator == "subtract") {
			if (totalValue == 0) totalValue += variableValue(vec, var);
			else totalValue -= variableValue(vec, var);
		}
	}
}

// checks if a variable vector contains a vector or not
bool containsVariable(vector<Variable> vec, string value) {
	for (int i = 0; i < vec.size(); i++) {
		if (vec.at(i).name == value) return true;
	}
	return false;
}

// returns the required variable value from a vector
double variableValue(vector<Variable> vec, string value) {
	for (double i = 0; i < vec.size(); i++) {
		if (vec.at(i).name == value) return vec.at(i).doubValue;
	}
}

// returns the position of the variable from a vector
int variablePosition(vector<Variable> vec, string value) {
	for (double i = 0; i < vec.size(); i++) {
		if (vec.at(i).name == value) return i;
	}
}

// stack implementation is below
int precedence(char op) {
	if (op == '+' || op == '-')
		return 1;
	if (op == '*' || op == '/')
		return 2;
	return 0;
}

// Function to perform arithmetic operations.
int applyOp(int a, int b, char op) {
	switch (op) {
	case '+': return a + b;
	case '-': return a - b;
	case '*': return a * b;
	case '/': return a / b;
	}
}

int evaluate(string tokens) {
	int i;

	stack <int> values;

	stack <char> ops;

	for (i = 0; i < tokens.length(); i++) {
		if (tokens[i] == ' ')
			continue;
		else if (tokens[i] == '(') {
			ops.push(tokens[i]);
		}
		else if (isdigit(tokens[i])) {
			int val = 0;
			while (i < tokens.length() &&
				isdigit(tokens[i]))
			{
				val = (val * 10) + (tokens[i] - '0');
				i++;
			}

			values.push(val);
			i--;
		}
		else if (tokens[i] == ')')
		{
			while (!ops.empty() && ops.top() != '(')
			{
				int val2 = values.top();
				values.pop();

				int val1 = values.top();
				values.pop();

				char op = ops.top();
				ops.pop();

				values.push(applyOp(val1, val2, op));
			}
			if (!ops.empty())
				ops.pop();
		}
		else
		{
			while (!ops.empty() && precedence(ops.top())
				>= precedence(tokens[i])) {
				int val2 = values.top();
				values.pop();

				int val1 = values.top();
				values.pop();

				char op = ops.top();
				ops.pop();

				values.push(applyOp(val1, val2, op));
			}
			ops.push(tokens[i]);
		}
	}
	while (!ops.empty()) {
		int val2 = values.top();
		values.pop();

		int val1 = values.top();
		values.pop();

		char op = ops.top();
		ops.pop();

		values.push(applyOp(val1, val2, op));
	}

	return values.top();
}


