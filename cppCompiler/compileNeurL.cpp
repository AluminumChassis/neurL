#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include <direct.h>

using namespace std;

string file = "";
string filePath;
string fileName;
string path;
char *buildPath;
string assembly = "";
vector<string> lines;
vector<string> dependencies;
const int bit32Max = 4294967295;
int n;


void replaceAll(std::string& str, const std::string& from, const std::string& to) {
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}
string readFile(string fn) {
	string returnVal = "";
	ifstream stream;
	stream.open(fn);
	string line;
	n = 0;
	if (stream.is_open())
	{
		while (getline(stream, line))
		{
			n++;
			returnVal += line + "\n";
		}
		stream.close();
	}
	return returnVal;
}
void readNLFile(string fn) {
	file = readFile(fn);
	for (int i = 0; i < n; i++) {
		lines.push_back(file.substr(0, file.find("\n")));
		file = file.substr(file.find("\n")+1);
	}
	path = fn.substr(0,fn.rfind('/'))+"/"+fileName+"_build/";
	buildPath = new char[path.size() + 1];
	std::copy(path.begin(), path.end(), buildPath);
	buildPath[path.size()] = '\0';
}
void include(string p) {
	cout<<"Using: "<<p<<"\n";
	assembly += readFile("chunks/"+p);
	return;
}
void dependency(string d) {
	if (d=="standard") {
		include("defaultLibs.chunk");
	}
}
void chunk(string p, vector<string> args) {
	string temp = readFile("chunks/" + p);
	for (int i = 0; i < args.size(); i++) {
		replaceAll (temp, "arg"+std::to_string(i), args[i]);
	}
	assembly += temp;
}
void resolveDependencies() {
	cout << "\nResolving dependencies\n";
	lines[0] = lines[0].substr(lines[0].find("{")+1, lines[0].find("}") - lines[0].find("{")-1);
	while (lines[0].find(" ") < bit32Max){
		dependencies.push_back(lines[0].substr(0,lines[0].find(" ")));
		lines[0] = lines[0].substr(lines[0].find(" ") + 1);
	}

	for_each(dependencies.begin(), dependencies.end(), dependency);
	cout << ">Dependencies resolved\n\n";
}

void writeAsm() {
	ofstream stream;
	stream.open(path + "/source.s");
	stream << assembly;
	stream.close();
}

void readLines() {
	for (int i = 1; i < lines.size(); i++) {
		vector<string> args;
		replaceAll(lines[i], " ", "");
		string temp = lines[i].substr(lines[i].find("<<") + 2);
		while (temp.find(",") < bit32Max) {
			args.push_back(temp.substr(0, temp.find(",")));
			temp = temp.substr(temp.find(",") + 1);
		}
		string ck = lines[i].substr(0, lines[i].find("<<")) + ".chunk";
		cout << ck;
		chunk(ck, args);
	}
}

void startBuild() {
	mkdir(buildPath);
	assembly += readFile("chunks/header.chunk");
	resolveDependencies();

	include("start.chunk");
	readLines();
	assembly += "\n	exit\nend _start";
	writeAsm();
}

int main(int argc, char** argv)
{
	cout << "Starting Compiler for Galak-C\n";
	if (argc > 2) {
		cout << "You have entered " << (argc-1) << " arguments, this is more than is accepted" << "\n";
		return 0;
	}
	else if(argc < 2) {
		cout << "You have entered " << (argc-1) << " arguments, this is less than is accepted" << "\n";
		return 0;
	}
	filePath = argv[1];
	cout << "compiling: " << filePath << "\n";
	if (filePath.find(".")== bit32Max) {
		cout << "'" << filePath << "' contains no file extension";
		return 0;
	}
	if (filePath.substr(filePath.rfind("."))!=".nl") {
		cout << "Error: file extension " << filePath.substr(filePath.rfind(".")) << " is not accepted";
		return 0;
	}
	fileName = filePath.substr(filePath.rfind("/"), filePath.rfind(".")- filePath.rfind("/"));
	readNLFile(filePath);
	startBuild();
	
	cout << ">Compilation Completed\n";

	return 0;
}