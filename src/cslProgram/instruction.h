#pragma once

#ifndef CSLPROGRAM_INSTRUCTION_H
#define CSLPROGRAM_INSTRUCTION_H

#include <string>

namespace cslProgram
{
	class Program;

	enum EInstructionResult
	{
		Success,
		Fail,

		// returned by conditional instructions
		CondTrue,
		CondFalse
	};

	class Instruction
	{
	protected:
		std::string srcLine; // line from source script that this instruction was created from. Printed for debugging errors

	public:
		Instruction() {}
		virtual EInstructionResult Execute(Program* context) const = 0;
	};

	class PrintInstruction : public Instruction
	{
	protected:
		std::string line; // parsing should make sure this is not empty

	public:
		PrintInstruction(const std::string& inLine)
		{
			line = inLine;
		}

		virtual EInstructionResult Execute(Program* context) const override;
	};

	class SetVarInstruction : public Instruction
	{
	protected:
		// parsing should make sure these are not empty, and name is one word
		std::string name;
		std::string value;

	public:
		SetVarInstruction(const std::string& inName, const std::string& inValue)
		{
			name = inName;
			value = inValue;
		}

		virtual EInstructionResult Execute(Program* context) const override;
	};

	class RunFuncInstruction : public Instruction
	{
	protected:
		std::string name; // parsing should make sure this is not empty and is one word

	public:
		RunFuncInstruction(const std::string& inName)
		{
			name = inName;
		}

		virtual EInstructionResult Execute(Program* context) const override;
	};
}

#endif