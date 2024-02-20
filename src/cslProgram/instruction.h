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
		Instruction(const std::string& inSrc) : srcLine(inSrc) {}
		virtual EInstructionResult Execute(Program* context) const = 0;
		virtual bool IsConditional() const { return false; }
	};

	class PrintInstruction : public Instruction
	{
	protected:
		std::string line; // parsing should make sure this is not empty

	public:
		PrintInstruction(const std::string& inSrc, const std::string& inLine) :
			Instruction(inSrc),
			line(inLine) {}

		virtual EInstructionResult Execute(Program* context) const override;
	};

	class SetVarInstruction : public Instruction
	{
	protected:
		// parsing should make sure these are not empty, and name is one word
		std::string name;
		std::string value;

	public:
		SetVarInstruction(const std::string& inSrc, const std::string& inName, const std::string& inValue) :
			Instruction(inSrc),
			name(inName),
			value(inValue) {}

		virtual EInstructionResult Execute(Program* context) const override;
	};

	class RunFuncInstruction : public Instruction
	{
	protected:
		std::string name; // parsing should make sure this is not empty and is one word

	public:
		RunFuncInstruction(const std::string& inSrc, const std::string& inName) :
			Instruction(inSrc),
			name(inName) {}

		virtual EInstructionResult Execute(Program* context) const override;
	};

	class IsGreaterConditional : public Instruction
	{
	protected:
		std::string lVar; // parsing should make sure these are not empty and 1 word
		std::string rVar;

	public:
		IsGreaterConditional(const std::string& inSrc, const std::string& inLVar, const std::string& inRVar) :
			Instruction(inSrc),
			lVar(inLVar),
			rVar(inRVar) {}

		virtual EInstructionResult Execute(Program* context) const override;
		virtual bool IsConditional() const override { return true; }
	};

	class IsGreaterEqualConditional : public Instruction
	{
	protected:
		std::string lVar; // parsing should make sure these are not empty and 1 word
		std::string rVar;

	public:
		IsGreaterEqualConditional(const std::string& inSrc, const std::string& inLVar, const std::string& inRVar) :
			Instruction(inSrc),
			lVar(inLVar),
			rVar(inRVar) {}

		virtual EInstructionResult Execute(Program* context) const override;
		virtual bool IsConditional() const override { return true; }
	};
}

#endif