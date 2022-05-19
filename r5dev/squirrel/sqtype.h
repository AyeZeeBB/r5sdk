#ifndef SQTYPE_H
#define SQTYPE_H

#define SQ_OK (1)
#define SQ_ERROR (-1)
#define SQ_FAILED(res) (res<0)
#define SQ_SUCCEEDED(res) (res>=0)

typedef char SQChar;
typedef float SQFloat;
typedef long SQInteger;
typedef unsigned long SQUnsignedInteger;

typedef SQUnsignedInteger SQBool;
typedef SQInteger SQRESULT;

enum class SQCONTEXT : SQInteger
{
	SERVER = 0,
	CLIENT,
	UI,
	NONE
};

const static std::string SQVM_TYPE_T[4] =
{
	"SERVER",
	"CLIENT",
	"UI",
	"NONE"
};

const static std::string SQVM_LOG_T[4] =
{
	"Script(S):",
	"Script(C):",
	"Script(U):",
	"Script(X):"
};

const static std::string SQVM_ANSI_LOG_T[4] =
{
	"\033[38;2;151;149;187mScript(S):",
	"\033[38;2;151;149;163mScript(C):",
	"\033[38;2;151;123;136mScript(U):",
	"\033[38;2;151;149;163mScript(X):"
};

const static std::string SQVM_WARNING_ANSI_LOG_T[4] =
{
	"\033[38;2;151;149;187mScript(S):\033[38;2;255;255;000m",
	"\033[38;2;151;149;163mScript(C):\033[38;2;255;255;000m",
	"\033[38;2;151;123;136mScript(U):\033[38;2;255;255;000m",
	"\033[38;2;151;149;163mScript(X):\033[38;2;255;255;000m"
};

const static std::string SQVM_ERROR_ANSI_LOG_T[4] =
{
	"\033[38;2;151;149;187mScript(S):\033[38;2;255;000;000m",
	"\033[38;2;151;149;163mScript(C):\033[38;2;255;000;000m",
	"\033[38;2;151;123;136mScript(U):\033[38;2;255;000;000m",
	"\033[38;2;151;149;163mScript(X):\033[38;2;255;000;000m"
};
#endif // SQTYPE_H
