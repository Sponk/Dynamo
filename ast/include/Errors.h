#ifndef ERRORS_H
#define ERRORS_H

#include <string>
#include <exception>

#define SYNTAX_ASSERT(x, line, colStart, colEnd, msg) if(!(x)) throw dynamo::SyntaxException((msg), (line), (colStart), (colEnd));

namespace dynamo
{
class SyntaxException : public std::runtime_error
{
	std::string m_text;
	unsigned int m_line, m_colStart, m_colEnd;
public:
	SyntaxException(const std::string& text, 
			unsigned int line, 
			unsigned int colStart, 
			unsigned int colEnd):
			
			m_text(text),
			m_line(line),
			m_colStart(colStart),
			m_colEnd(colEnd),
			std::runtime_error(("Error:" + std::to_string(line) + ":" + std::to_string(colStart) + ": " + text).c_str()) {}
};
}

#endif
