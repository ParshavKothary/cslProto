#include "program.h"

#include <cstdio>
#include <ranges>
#include <iterator>

namespace cslProgram
{
	inline bool IsCondResult(const Instruction::EResult result)
	{
		return result == Instruction::EResult::CondFail || result == Instruction::EResult::CondSuccess;
	}

	Program::FunctionIterator Program::SafeAdvance(FunctionIterator iter, FunctionIterator end, unsigned int n)
	{
		while (n > 0 && iter != end)
		{
			--n;
			++iter;
		}

		return iter;
	}

	bool Program::RunFunctionInternal(const Function* function)
	{
		if (function == nullptr)
		{
			printf("Something is wrong. RunFunctionInternal called with null function.\n");
			return false;
		}

		FunctionIterator iter = function->instructions.begin();
		const FunctionIterator end = function->instructions.end();

		InstructionResult result = InstructionResult::Success;
		
		// iterate over instructions
		while (iter != end)
		{
			result = (*iter)->Execute(this);
			unsigned int advanceCount = 1; // increment by 1 instruction at a time, this might change at conditionals as per syntax

			if (IsCondResult(result))
			{
				FunctionIterator orig = iter;

				const bool isSuccess = result == InstructionResult::CondSuccess;
				iter = SafeAdvance(iter, end, isSuccess ? 1 : 2); // as per syntax of conditionals, skip first instruction if conditional failed

				if (iter == end)
				{
					printf("Interpreter error: Expected 2 instructions after conditional.\n");
					// print orig iter line
					return false;
				}

				result = (*iter)->Execute(this);

				if (IsCondResult(result))
				{
					printf("Interpreter error: Nested conditionals detected. This is not allowed sorry.\n");
					// print iter line
					return false;
				}

				if (isSuccess)
				{
					++advanceCount; // as per syntax of conditionals, skip second instruction if conditional succeeded
				}
			}

			if (result == InstructionResult::Fail)
			{
				break;
			}

			// result is success here
			iter = SafeAdvance(iter, end, advanceCount);
		}

		if (result == InstructionResult::Fail)
		{
			printf("Instruction failed\n");
			// print iter instruction
			return false;
		}

		return true;
	}

	Program::Program(std::stringstream source)
	{
		// parse source
	}

	Program::~Program()
	{
		std::unordered_map<std::string, const Function*>::iterator iter = functions.begin();
		for (; iter != functions.end(); ++iter)
		{
			const Function* function = iter->second;
			for (const Instruction* instruction : function->instructions)
			{
				delete instruction;
			}
			delete function;
		}
	}

	bool Program::RunFunction(const std::string& functionName)
	{
		if (functions.find(functionName) == functions.end())
		{
			return false;
		}

		return RunFunctionInternal(functions[functionName]);
	}

	bool Program::ConvertToVarIfExists(std::string& varName)
	{
		if (variables.find(varName) != variables.end())
		{
			varName = variables[varName];
			return true;
		}

		return false;
	}
}