#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <sstream>
#include <vector>
#include <iterator>
#include <fstream>
#include <streambuf>
#include "json.hpp"
#include <dirent.h>
#include <sys/types.h>
#include "Command.hpp"

using namespace std;
using json = nlohmann::json;

template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
}

class Word
{
public:
	int index;
	string word;
	string wordType;
	int depth;
	string meaning;
};

ostream & operator<<(ostream & str, Word const & v) { 
	str << to_string(v.index) + " " + v.word + " " + v.wordType + " " + to_string(v.depth) + " " + v.meaning;
	return str;
}

vector<string> getFiles(const char *path)
{
	vector<string> files;
    struct dirent *entry;
    DIR *dir = opendir(path);
    if (dir == NULL) {
        return files;
    }

    while ((entry = readdir(dir)) != NULL) {
        files.push_back(entry->d_name);
    }

    closedir(dir);
    return files;
}

vector<Command> commands;

void loadConfigs()
{
	std::vector<string> configs = getFiles("configs/");

	for (int i = 0; i < configs.size(); ++i)
	{
		string file = configs[i];
		if (file.find(".cmd") != string::npos)
		{
			cout << "***FOUND CONFIG " << file << "***\n";
			ifstream t("configs/" + file);
			string str((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());

			json jcreate = json::parse(str);
			commands.push_back(jcreate);
		}
	}
}


int main()
{
	cout << "***CIRI****\n";

	loadConfigs();

	string response;
	getline(cin, response);

	string rawResult = exec(("python test.py \"" + response+"\"").c_str());

	vector<string> lines = split(rawResult, '\n');

	vector<Word> words;

	cout << rawResult;
	for (int i = 0; i < lines.size(); i++)
	{
		vector<string> parts = split(lines[i], '\t');
		if (parts.size() > 0)
		{
			Word word;
			word.index = stoi(parts[0]);
			word.word = parts[1];
			word.wordType = parts[3];
			word.depth = stoi(parts[6]);
			word.meaning = parts[7];

			words.push_back(word);
			cout << word << "\n";
		}
	}

	return 0;
}