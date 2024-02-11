#include "program.h"

#include "common/common.h"

namespace cslProgram
{
	inline bool IsCondResult(const EInstructionResult result)
	{
		return result == EInstructionResult::CondTrue || result == EInstructionResult::CondFalse;
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

		EInstructionResult result = EInstructionResult::Success;
		
		// iterate over instructions
		while (iter != end)
		{
			result = (*iter)->Execute(this);

			if (IsCondResult(result)) // if we just ran a conditional instruction, skip first instruction after this if false, second if true
			{
				const bool isCondTrue = result == EInstructionResult::CondTrue;
				iter = SafeAdvance(iter, end, isCondTrue ? 1 : 2); // if conditional was true, advance to first instruction after this, if false, skip first and advance to second

				assert(iter != end); // parsing should have caught lack of instructions after conditional

				result = (*iter)->Execute(this);

				assert(IsCondResult(result) == false); // parsing should have caught nested conditional

				if (isCondTrue)
				{
					iter = SafeAdvance(iter, end, 1); // skip second instruction if conditional was true
				}
			}

			if (result == EInstructionResult::Fail)
			{
				break;
			}

			// result is success here
			iter = SafeAdvance(iter, end, 1);
		}

		if (result == EInstructionResult::Fail)
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

		RunFunctionInternal(functions[functionName]);
		return true;
	}

	bool Program::ConvertToVarIfExists(std::string& valueOrVarName)
	{
		if (variables.find(valueOrVarName) != variables.end())
		{
			valueOrVarName = variables[valueOrVarName];
			return true;
		}

		return false;
	}

	void Program::SetVar(const std::string& name, const std::string& inValueOrName)
	{
		std::string valueOrVarName = inValueOrName;
		ConvertToVarIfExists(valueOrVarName);
		variables.insert_or_assign(name, valueOrVarName);
	}
}