#include "program.h"

#include "common/common.h"
#include "common/stringUtils.h"

namespace cslProgram
{
	#pragma region Typedefs

	typedef std::list<const Instruction*>::const_iterator InstructionIterator;
	typedef std::unordered_map<std::string, const Function*>::iterator FunctionIterator;
	typedef std::unordered_map<std::string, std::string>::iterator VariableIterator;
	typedef Instruction* (*ExtractInstructionFunc)(const std::vector<std::string>&, const std::string&);

	#pragma endregion
	
	#pragma region Misc

	inline bool IsCondResult(const EInstructionResult result)
	{
		return result == EInstructionResult::CondTrue || result == EInstructionResult::CondFalse;
	}

	InstructionIterator SafeAdvance(InstructionIterator iter, InstructionIterator end, unsigned int n)
	{
		while (n > 0 && iter != end)
		{
			--n;
			++iter;
		}

		return iter;
	}

	void DeleteFuncInstructions(const Function* func)
	{
		if (func == nullptr) return;
		for (const Instruction* instruction : func->instructions)
		{
			if (instruction == nullptr) continue;
			delete instruction;
		}
	}

	#pragma endregion

	#pragma region Instruction Extraction Functions

	Instruction* ExtractPrintInstruction(const std::vector<std::string>& words, const std::string& src)
	{
		return new PrintInstruction(src, words[0]);
	}

	Instruction* ExtractSetVarInstruction(const std::vector<std::string>& words, const std::string& src)
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

	Instruction* ExtractRunFuncInstruction(const std::vector<std::string>& words, const std::string& src)
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

	Instruction* ExtractIsGreaterConditional(const std::vector<std::string>& words, const std::string& src)
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

	const std::unordered_map<std::string, ExtractInstructionFunc> s_extractionInstructionFuncs =
	{
		{ "Print", ExtractPrintInstruction },
		{ "SetVar", ExtractSetVarInstruction },
		{ "RunFunc", ExtractRunFuncInstruction },
		{ "IsGreater", ExtractIsGreaterConditional }
	};

	#pragma endregion

	#pragma region Parsing functions

	bool IsValidFunctionLine(const std::vector<std::string>& words)
	{
		if (words.size() != 1) return false;
		if (stringUtils::hasSpace(words[0])) return false;

		return true;
	}

	bool IsValidNonFunctionLine(const std::vector<std::string>& words)
	{
		if (words.size() < 2) return false;
		if (stringUtils::hasSpace(words[0])) return false;

		return true;
	}

	// trims copy of inLine, splits by ',' and trims each word (removes empties)
	void GetFormattedWords(const std::string& inLine, std::vector<std::string>& words)
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

	bool GetNextFunction(std::istream& source, Function*& pFunc, std::string& funcName)
	{
		assert(pFunc == nullptr);

		std::string rawline;

		bool foundFunc = false; // first find function name, ignore whitespace/empty lines
		while (std::getline(source, rawline))
		{
			PRINTF("Parsing line: %s\n", rawline.c_str());
			stringUtils::trim(rawline);

			if (rawline.empty())
			{
				continue;
			}

			std::vector<std::string> words;
			GetFormattedWords(rawline, words);

			if (IsValidFunctionLine(words))
			{
				funcName = words[0];
				foundFunc = true;
				break;
			}
			else if (IsValidNonFunctionLine(words))
			{
				PRINTF("Compilation error: Line outside function: %s\n", rawline.c_str());
				return false;
			}
			else
			{
				PRINTF("Invalid line: %s\n", rawline.c_str());
				return false;
			}
		}

		if (foundFunc == false)
		{
			// empty file, so success is true but pFunc is nullptr
			return true;
		}

		Function* newFunction = new Function();

		bool failed = false;

		unsigned int isAfterConditional = 0; // used to catch lack of instructions after conditional (need 2)
											 // also used to catch nested conditionals
		Instruction* pLastConditional = nullptr;

		std::streampos oldPos = source.tellg();
		while (std::getline(source, rawline))
		{
			PRINTF("Parsing line: %s\n", rawline.c_str());
			stringUtils::trim(rawline);

			if (rawline.empty())
			{
				oldPos = source.tellg();
				continue;
			}

			std::vector<std::string> words;
			GetFormattedWords(rawline, words);

			if (IsValidFunctionLine(words))
			{
				source.seekg(oldPos); // reached next function, reset source pos so next read can see this function name
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

				if (pNewInstruction->IsConditional())
				{
					pLastConditional = pNewInstruction;

					if (isAfterConditional > 0)
					{
						PRINTF("Compilation Error: No nested conditionals allowed: %s\n", rawline.c_str());
						failed = true;
						break;
					}

					isAfterConditional = 2;
				}
				else if (isAfterConditional > 0)
				{
					--isAfterConditional;
				}

				PRINTF("Valid Instruction: %s\n", rawline.c_str());
				newFunction->instructions.push_back(pNewInstruction);
			}
			else
			{
				PRINTF("Unknown instruction: %s in line: %s\n", cmd.c_str(), rawline.c_str());
				failed = true;
				break;
			}
		}

		if (isAfterConditional > 0) // there were < 2 instructions after conditional
		{
			PRINTF("Compilation error: Not enough instructions after conditional: %s\n", pLastConditional->GetSrcLine());
			failed = true;
		}

		if (failed)
		{
			DeleteFuncInstructions(newFunction);
			delete newFunction;
			return false;
		}

		pFunc = newFunction;
		return true;
	}

	#pragma endregion

	// Main Run function:
	bool Program::RunFunctionInternal(const Function* function)
	{
		assert(function != nullptr);

		InstructionIterator iter = function->instructions.begin();
		const InstructionIterator end = function->instructions.end();

		EInstructionResult result = EInstructionResult::Success;
		
		// iterate over instructions
		while (iter != end)
		{
			result = (*iter)->Execute(this);

			if (IsCondResult(result)) // if we just ran a conditional instruction, skip first instruction after this if false, skip second after this if true
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
	
	#pragma region Construction Destruction

	Program::Program(std::istream& source)
	{
		m_init = false;
		PRINTF("Beginning parse and compile\n");
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
				break; // success + null function = reached end of file
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
		PRINTF("Finished parse and compile\n\n\n");
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

	#pragma endregion

	#pragma region Public Functions to iteract with program

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

	#pragma endregion
}