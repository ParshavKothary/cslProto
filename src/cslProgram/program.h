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

		std::unordered_map<std::string, const Function*> functions; // program instructions stored in functions
		std::unordered_map<std::string, std::string> variables; // program state stored in variables
		bool m_init; // did program 'compile' when constructed

		void DeleteFunctions();
		bool RunFunctionInternal(const Function* function);

	public:

		Program(std::istream& source);
		~Program();

		bool RunFunction(const std::string& functionName);

		// Converts to valueOrVarName to value, and sets or creates var 'name'
		// forwards return value of std::unordered_map insert (should almost always be true)
		bool SetVar(const std::string& name, const std::string& valueOrVarName);

		// if input string is var name, converts to value of that var.
		// returns true if input was var and did convert, false if left input unchanged
		bool GetValueFromValueOrName(std::string& valueOrVarName);

		// Converts to value and then converts to float.
		// returns true if value was a valid float string, false if not a number
		bool GetFloatFromValueOrName(const std::string& valueOrVarName, float& outFloat);
	};
}

#endif