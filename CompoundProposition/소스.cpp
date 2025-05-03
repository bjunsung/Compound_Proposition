#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <unordered_map>


//단순명제 class
class LogicalOperator {
private:
	static bool p, q;
public:
	static bool logicalNot(bool p) { return !p; }
	static bool logicalAnd(bool p, bool q) { return p && q; }
	static bool logicalOr(bool p, bool q) { return p || q; }
	static bool logicalImplies(bool p, bool q) { return !(p && !q); }
	static bool logicalEquivalent(bool p, bool q) { return p == q; }
};



int myPow(int, int);

//진리값(true, false) 저장 class
class TruthAssignment {
private:
	std::unordered_map<std::string, bool> truthMap;
	std::vector<std::string> keys;
public:
	void addProposition(char proposition) {
		std::string s(1, proposition);
		setPropositionValue(s, true);
	}
	void setPropositionValue(std::string proposition, bool value) {
		truthMap[proposition] = value;
	}
	bool getPropositionValue(std::string proposition) {
		return truthMap.at(proposition);
	}
	void setKeys() {
		for (const auto& pair : truthMap)
			keys.push_back(pair.first);
	}
	void setPropositionValueByStep(int num) {
		int size = (int)keys.size();
		for (int i = 0; i < size; ++i)
			truthMap[keys[i]] = num / myPow(2, size - (i + 1)) % 2 == 0;
	}
	void printTruthValue() {
		std::string truthValue;
		for (std::string proposition : keys) {
			truthValue = truthMap[proposition] ? "T" : "F";
			std::cout << truthValue + " | ";
		}
	}
	void printAllPropositions() {
		for (std::string key : keys) {
			if (key == keys.front())
				std::cout << "idx_" << key;
			else
				std::cout << "___" << key;
		}
	}
	int getKeySize() {
		return (int)keys.size();
	}
};



//복합명제 class
class CompoundProposition {
private:
	std::vector<char> postfix;
	std::stack<char> operator_stack;
	TruthAssignment truthData;
	char validOperator[11] = { '~', '&', 'V', '>', '<', '[', '{', '(', ')', '}', ']' };
	void popStackTilLeftBracket(char bracketRight) {
		char bracketLeft = ' ';
		switch (bracketRight) {
		case ')': bracketLeft = '('; break;
		case '}': bracketLeft = '{'; break;
		case ']': bracketLeft = '[';
		}
		while (true) {
			if (operator_stack.empty()) {
				std::string brkLeft(1, bracketLeft);
				std::string brkRight(1, bracketRight);
				throw std::runtime_error("Bracket Mismatch Error : matching '" + brkLeft + "' not found for '" + brkRight + "'");
			}
			char op = operator_stack.top();
			operator_stack.pop();
			if (op == bracketLeft)
				break;
			else
				postfix.push_back(op);
		}
		if (!operator_stack.empty() && operator_stack.top() == '~') {
			postfix.push_back('~');
			operator_stack.pop();
		}
	}
	void popStackTilEmpty() {
		while (!operator_stack.empty()) {
			char op = operator_stack.top();
			operator_stack.pop();
			postfix.push_back(op);
		}
	}
	bool calcLogicalOperation(bool p, char op) {
		return LogicalOperator().logicalNot(p);
	}
	bool calcLogicalOperation(bool p, bool q, char op) {
		switch (op) {
		case '&': return LogicalOperator().logicalAnd(p, q);
		case 'V': return LogicalOperator().logicalOr(p, q);
		case '>': return LogicalOperator().logicalImplies(p, q);
		case '<': return LogicalOperator().logicalEquivalent(p, q);
		default: throw std::runtime_error(std::string("Invalid Operator : ") + op);
		}
	}
	bool isValidOperator(char op) {
		for (char sample : validOperator)
			if (op == sample)
				return true;
		return false;
	}
	bool isASCII(char c) {
		return !((unsigned char)c > 127);
	}
	std::string inputLinePreprocessing(std::string inputLine) {
		for (int i = 0; i < inputLine.size(); ++i) {
			if (!isASCII(inputLine.at(i))) { // · to &
				inputLine[i] = '&';
				inputLine.erase(i + 1, 1);
			}
			else if (inputLine.at(i) == '<') // <-> to <
				inputLine.erase(i + 1, 2);
			else if (inputLine.at(i) == '-') // -> to >
				inputLine.erase(i, 1);
		}
		return inputLine;
	}
	bool getTruthValueForEachCase(int i) {
		truthData.setPropositionValueByStep(i);
		try {
			return calc_postfix();
		}
		catch (const std::runtime_error& e) {
			std::cout << e.what() << std::endl;
			exit(1);
		}
	}
	void infixToPostfix(std::string inputLine)
	{
		for (char c : inputLine) {
			if (c == ' ') continue;
			else if (c != 'V' && ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z')) {
				truthData.addProposition(c);
				postfix.push_back(c);
				if (!operator_stack.empty() && operator_stack.top() == '~') {
					postfix.push_back('~');
					operator_stack.pop();
				}
			}
			else if (!isValidOperator(c)) {
				std::string str_c(1, c);
				throw std::runtime_error("Invalid Operator Detected : " + str_c);
			}
			else if (c == ']' || c == '}' || c == ')') {
				try {
					popStackTilLeftBracket(c);
				}
				catch (const std::runtime_error& e) {
					std::cout << e.what() << std::endl;
				}
			}
			else
				operator_stack.push(c);
		}
		popStackTilEmpty();
		truthData.setKeys();
	}
	std::vector<char> clone_postfix()
	{
		std::vector<char> cloned;
		for (char proposition : postfix)
			cloned.push_back(proposition);
		return cloned;
	}
	std::string pop(std::stack<std::string>& stack) {
		if (stack.empty())
			throw std::runtime_error("Invalid Compound Proposition");
		std::string s = stack.top();
		stack.pop();
		return s;
	}
	bool getPropositionValue(const std::string& p) {
		bool truthValue;
		if (p == "True")
			truthValue = true;
		else if (p == "False")
			truthValue = false;
		else
			truthValue = truthData.getPropositionValue(p);
		return truthValue;
	}
	bool calc_postfix() {
		std::vector<char> cloned_postfix = this->clone_postfix();
		std::stack<std::string> evalStack;  //단순명제나 그 명제들의 연산 결과를 string type으로 stack에 넣음
		for (char c : cloned_postfix) {
			if (c != 'V' && ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z')) {
				std::string s(1, c);
				evalStack.push(s);
			}
			else if (c == '~') { // case Truth Operator is '~' 
				std::string p = pop(evalStack);
				bool truthValue = getPropositionValue(p);
				bool calculatedTruthValue = calcLogicalOperation(truthValue, '~');
				evalStack.push(calculatedTruthValue ? "True" : "False");
			}
			else { // case Truth Operator except '~' 
				std::string q = pop(evalStack);
				std::string p = pop(evalStack);
				bool p_value = getPropositionValue(p);
				bool q_value = getPropositionValue(q);
				bool truthValue = calcLogicalOperation(p_value, q_value, c);
				evalStack.push(truthValue ? "True" : "False");
			}
		}
		if ((int)evalStack.size() != 1)
			throw std::runtime_error("Invalid Compound Proposition");
		std::string result = evalStack.top();
		return result == "True";
	}

public:
	CompoundProposition() {}
	CompoundProposition(std::string inputLine) {
		inputLine = inputLinePreprocessing(inputLine);
		try {
			infixToPostfix(inputLine);
		}
		catch (const std::runtime_error& e) {
			std::cout << e.what() << std::endl;
			exit(1);
		}
	}
	static void printAllLogicalOperator() {
		std::cout << "Logical Operators : ";
		for (char op : CompoundProposition().validOperator) {
			switch (op) {
			case '~': { std::cout << op << "(negation), "; break; }
			case '&': { std::cout << "·(conjunction), "; break; }
			case 'V': { std::cout << op << "(disjunction), "; break; }
			case '>': { std::cout << "->(material implication), "; break; }
			case '<': { std::cout << "<->(biconditional), "; break; }
			case ']': { std::cout << op; break; }
			default: std::cout << op << ", ";
			}
		}
		std::cout << std::endl;
	}
	void printTruthTable(std::string line) {
		std::cout << "*  *  *  *  *" << std::endl << "<TRUTH TABLE>" << std::endl << "*  *  *  *  *" << std::endl;
		truthData.printAllPropositions();
		std::cout << "___" << line << std::endl;
		for (int i = 0; i < pow(2, truthData.getKeySize()); ++i) {
			bool truthValueOfCompoundProposition = getTruthValueForEachCase(i);
			std::cout << i + 1 << " | ";
			truthData.printTruthValue();
			std::cout << " " << (truthValueOfCompoundProposition ? "T" : "F") << " " << std::endl;
		}
	}
};



int main()
{
	CompoundProposition::printAllLogicalOperator();
	std::cout << "Example Input : 1. (P·Q)VR	2. ~[CV(AV~D)]·(A->~C)	3. P<->Q" << std::endl;
	std::cout << "Input Your Compound Proposition here >> ";
	std::string line;
	std::getline(std::cin, line);
	CompoundProposition myCompoundProposition(line);
	myCompoundProposition.printTruthTable(line);
}



//제곱을 리턴하는 함수
int myPow(int base, int exponent) {
	int result = 1;
	for (int i = 0; i < exponent; ++i)
		result *= base;
	return result;
}