#include "stdafx.h"

// Configure catch main so we can extend it
#define CATCH_CONFIG_RUNNER

#include <catch2\catch.hpp>

bool isEnvSet(std::string env)
{
	char* val = getenv(env.c_str());
	if (!val) return false;

	std::string envVal(val);
	// convert to int to check 0 or 1
	std::string::size_type sz;   // alias of size_t
	int intVal = stoi(envVal, &sz);
	return intVal;
}

char** convert(const std::vector<std::string>& input)
{
	const size_t size = input.size();
	char** output = new char*[size];
	for (size_t i = 0; i < size; ++i)
	{
		std::string s = input[i];
		char* c = new char[s.length()];
		strcpy(c, s.c_str());
		output[i] = c;
	}
	return output;
}

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		std::vector<std::string> newArgs;
		newArgs.push_back(argv[0]);
		newArgs.push_back("-s"); // Show passed tests as well

		// Show durations
		newArgs.push_back("--durations");
		newArgs.push_back("yes");
		
		// Use colours
		newArgs.push_back("--use-colour");
		newArgs.push_back("yes");

		if (!isEnvSet("BATCH_RUN"))
		{
			// Wait for key-press before exit
			newArgs.push_back("--wait-for-keypress");
			newArgs.push_back("exit");
		}

		// Run tests in a random order
		newArgs.push_back("--order");
		newArgs.push_back("rand");

		char** newArgsList = convert(newArgs);
		{
			Catch::Session().run((int) newArgs.size(), newArgsList);
		}
		delete[] newArgsList;
	}
	else
	{
		std::cout << "---------------- Running with custom args ------------------" << std::endl;
		Catch::Session().run(argc, argv);
	}
}
