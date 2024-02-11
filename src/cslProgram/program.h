#pragma once

#ifndef CSLPROGRAM_PROGRAM_H
#define CSLPROGRAM_PROGRAM_H

#include "instruction.h"

#include <sstream>
#include <string>
#include <unordered_map>
#include <list>

namespace cslProgram
{
	struct Function
	{
		std::list<const Instruction*> instructions;
	};

	class Program
	{
	private:
		typedef std::list<const Instruction*>::const_iterator FunctionIterator;

		std::unordered_map<std::string, const Function*> functions;
		std::unordered_map<std::string, std::string> variables;

		bool RunFunctionInternal(const Function* function);

		static FunctionIterator SafeAdvance(FunctionIterator iter, FunctionIterator end, unsigned int n);

	public:

		Program(std::stringstream source);
		~Program();

		bool RunFunction(const std::string& functionName);
		bool ConvertToVarIfExists(std::string& valueOrVarName);
		void SetVar(const std::string& name, const std::string& valueOrVarName);
	};
}

#endif