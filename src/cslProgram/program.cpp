#include "program.h"

#include "common/common.h"
#include "common/stringUtils.h"

namespace cslProgram
{
	inline bool IsCondResult(const EInstructionResult result)
	{
		return result == EInstructionResult::CondTrue || result == EInstructionResult::CondFalse;
	}

	Program::InstructionIterator Program::SafeAdvance(InstructionIterator iter, InstructionIterator end, unsigned int n)
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
			PRINTF("Something is wrong. RunFunctionInternal called with null function.\n");
			return false;
		}

		InstructionIterator iter = function->instructions.begin();
		const InstructionIterator end = function->instructions.end();

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
			PRINTF("Instruction failed\n");
			// print iter instruction
			return false;
		}

		return true;
	}
	
	//
	// Construct destruct
	//

	Program::Program(std::istream& source)
	{
		m_init = false;
		while (true)
		{
			std::string funcName = "";
			Function* function = nullptr;
			bool success = GetNextFunction(source, function, funcName);
			if (success == false)
			{
				m_init = false; // there was a 'compile time' error
				DeleteFunctions();
				break;
			}

			if (function == nullptr)
			{
				break;
			}

			if (functions.find(funcName) != functions.end())
			{
				m_init = false;
				DeleteFunctions();
				PRINTF("Compilation error: Duplicate function name: %s\n", funcName);
				break;
			}

			m_init = true;
			functions.insert({ funcName, function });
		}
	}

	void Program::DeleteFuncInstructions(const Function* func)
	{
		if (func == nullptr) return;
		for (const Instruction* instruction : func->instructions)
		{
			if (instruction == nullptr) continue;
			delete instruction;
		}
	}


	void Program::DeleteFunctions()
	{
		FunctionIterator iter = functions.begin();
		for (; iter != functions.end(); ++iter)
		{
			const Function* function = iter->second;
			DeleteFuncInstructions(function);
			delete function;
		}

		functions.clear();
	}

	Program::~Program()
	{
		DeleteFunctions();
		variables.clear();
	}

	//
	// Parsing functions
	//

	bool Program::IsValidFunctionLine(const std::vector<std::string>& words)
	{
		if (words.size() != 1) return false;
		if (stringUtils::hasSpace(words[0])) return false;

		return true;
	}

	bool Program::IsValidNonFunctionLine(const std::vector<std::string>& words)
	{
		if (words.size() < 2) return false;
		if (stringUtils::hasSpace(words[0])) return false;

		return true;
	}

	// trims copy of inLine, splits by ',' and trims each word (removes empties)
	void Program::GetFormattedWords(const std::string& inLine, std::vector<std::string>& words)
	{
		std::string line = stringUtils::trim_copy(inLine);
		stringUtils::split(line, ',', words);

		std::vector<std::string>::iterator iter = words.begin();

		while (iter != words.end())
		{
			stringUtils::trim(*iter);
			if ((*iter).empty())
			{
				iter = words.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}

	const std::unordered_map<std::string, Program::ExtractInstructionFunc> Program::s_extractionInstructionFuncs =
	{
		{ "Print", Program::ExtractPrintInstruction },
		{ "SetVar", Program::ExtractSetVarInstruction },
		{ "RunFunc", Program::ExtractRunFuncInstruction },
		{ "IsGreater", Program::ExtractIsGreaterConditional }
	};

	Instruction* Program::ExtractPrintInstruction(const std::vector<std::string>& words, const std::string& src)
	{
		return new PrintInstruction(src, words[0]);
	}

	Instruction* Program::ExtractSetVarInstruction(const std::vector<std::string>& words, const std::string& src)
	{
		if (words.size() != 2)
		{
			PRINTF("Expected 2 arguments to SetVar in line: %s\n", src);
			return nullptr;
		}
		
		if (stringUtils::hasSpace(words[0]))
		{
			PRINTF("Variable name (argument 1) has to be one word: %s\n", src);
			return nullptr;
		}

		return new SetVarInstruction(src, words[0], words[1]);
	}

	Instruction* Program::ExtractRunFuncInstruction(const std::vector<std::string>& words, const std::string& src)
	{
		if (words.size() != 1)
		{
			PRINTF("Expected 1 argument to RunFunc in line: %s\n", src);
			return nullptr;
		}

		if (stringUtils::hasSpace(words[0]))
		{
			PRINTF("Function name (argument 1) has to be one word: %s\n", src);
			return nullptr;
		}

		return new RunFuncInstruction(src, words[0]);
	}

	Instruction* Program::ExtractIsGreaterConditional(const std::vector<std::string>& words, const std::string& src)
	{
		if (words.size() != 2)
		{
			PRINTF("Expected 2 arguments to IsGreater in line: %s\n", src);
			return nullptr;
		}

		if (stringUtils::hasSpace(words[0]) || stringUtils::hasSpace(words[1]))
		{
			PRINTF("Variable names (arguments 1 and 2) have to be one word: %s\n", src);
			return nullptr;
		}

		return new IsGreaterConditional(src, words[0], words[1]);
	}

	bool Program::GetNextFunction(std::istream& source, Function*& pFunc, std::string& funcName)
	{
		std::string rawline;

		Function* newFunction = new Function();

		bool failed = false;
		bool inFunction = false;

		std::streampos oldPos = source.tellg();
		while (std::getline(source, rawline) && failed == false)
		{
			PRINTF("Parsing line: %s\n", rawline.c_str());
			
			stringUtils::trim(rawline);

			std::vector<std::string> words;
			GetFormattedWords(rawline, words);

			if (rawline.empty())
			{
				oldPos = source.tellg();
				continue;
			}

			if (inFunction == false) // look for function name while skipping whitespace
			{
				if (IsValidFunctionLine(words))
				{
					inFunction = true;
					funcName = words[0];
				}
				else if (IsValidNonFunctionLine(words))
				{
					failed = true;
					PRINTF("Cmd outside function: %s\n", rawline.c_str());
					break;
				}
				else
				{
					failed = true;
					PRINTF("Invalid line: %s\n", rawline.c_str());
					break;
				}

				oldPos = source.tellg();
				continue;
			}

			if (IsValidFunctionLine(words))
			{
				source.seekg(oldPos); // reset source position so next GetNextFunctionCall sees this function name
				break;
			}

			oldPos = source.tellg();

			if (IsValidNonFunctionLine(words) == false)
			{
				failed = true;
				PRINTF("Invalid line: %s\n", rawline.c_str());
				break;
			}

			std::string cmd = words[0];
			words.erase(words.begin());
			assert(words.size() > 0);

			if (s_extractionInstructionFuncs.find(cmd) != s_extractionInstructionFuncs.end())
			{
				Instruction* pNewInstruction = s_extractionInstructionFuncs.at(cmd)(words, rawline);
				if (pNewInstruction == nullptr)
				{
					failed = true;
					break;
				}

				PRINTF("Valid Instruction: %s\n", rawline.c_str());
				newFunction->instructions.push_back(pNewInstruction);
			}
			else
			{
				PRINTF("Unknown instruction: %s in line: %s\n", cmd.c_str(), rawline.c_str());
				continue;
				failed = true;
				break;
			}
		}
		
		if (failed)
		{
			DeleteFuncInstructions(newFunction);
			delete newFunction;
			return false;
		}

		if (inFunction == false)
		{
			// empty file
			DeleteFuncInstructions(newFunction);
			delete newFunction;
			return true;
		}
		
		pFunc = newFunction;
		return true;
	}

	// 
	// public funcs mostly used by instruction classes:
	// 

	bool Program::RunFunction(const std::string& functionName)
	{
		if (functions.find(functionName) == functions.end())
		{
			return false;
		}

		RunFunctionInternal(functions[functionName]);
		return true;
	}

	bool Program::GetValueFromValueOrName(std::string& valueOrVarName)
	{
		if (variables.find(valueOrVarName) != variables.end())
		{
			valueOrVarName = variables[valueOrVarName];
			return true;
		}

		return false;
	}

	bool Program::GetFloatFromValueOrName(const std::string& valueOrVarName, float& outFloat)
	{
		std::string valueString = valueOrVarName;
		GetValueFromValueOrName(valueString);
		outFloat = std::stof(valueString);
		return outFloat != NAN;
	}

	bool Program::SetVar(const std::string& name, const std::string& inValueOrName)
	{
		std::string valueOrVarName = inValueOrName;
		GetValueFromValueOrName(valueOrVarName);
		std::pair<VariableIterator, bool> ret = variables.insert_or_assign(name, valueOrVarName);
		return ret.first != variables.end();
	}
}