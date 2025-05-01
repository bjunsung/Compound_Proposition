#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <unordered_map>

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
	void printTruthMap() {
		std::string truthValue;
		for (std::string proposition : keys) {
			if (truthMap[proposition] == true) truthValue = "T";
			else truthValue = "F";
			std::cout << truthValue << " | ";
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
	int getSize() {
		return (int)keys.size();
	}
};

//단순 명제 논리 연산 class
class LogicalOperator {
private:
	bool p, q;
public:
	bool logicalNot(bool p) { return !p; }
	bool logicalAnd(bool p, bool q) { return p && q; }
	bool logicalOr(bool p, bool q) { return p || q; }
	bool logicalImplies(bool p, bool q) { return !(p && !q); }
	bool logicalEquivalent(bool p, bool q) { return p == q; }
};


//복합명제 class
class CompoundPropositionOperator {
private:
	std::vector<char> postfix;
	std::stack<char> operator_stack;
	TruthAssignment truthData;
	char validOperator[11] = { '~', '&', '^', '>', '=', '[', '{', '(', ')', '}', ']' };
	void popStackTilLeftBracket(char bracketRight) {
		char bracketLeft;
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
		if (operator_stack.top() == '~') {
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
	bool calcLogicalOperation(bool p, bool q, char op) {
		switch (op) {
		case '&': return p && q;
		case '^': return p || q;
		case '>': return !p || q;
		case '=': return p == q;
		default: throw std::runtime_error(std::string("Invalid Operator : ") + op);
		}
	}
	bool isValidOperator(char op) {
		for (char sample : validOperator)
			if (op == sample)
				return true;
		return false;
	}
public:
	CompoundPropositionOperator() {}
	CompoundPropositionOperator(std::string inputLine) {
		try {
			infixToPostfix(inputLine);
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
			else if (('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z')) {
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

	std::vector<char> clone()
	{
		std::vector<char> cloned;
		for (char proposition : postfix)
			cloned.push_back(proposition);
		return cloned;
	}
	bool calc_postfix() {
		std::vector<char> cloned = this->clone();
		std::stack<std::string> stack;
		for (char c : cloned) {
			if (('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z')) {
				std::string s(1, c);
				stack.push(s);
			}
			else if (c == '~') {
				if (stack.empty())
					throw std::runtime_error("Invalid Compound Proposition");
				std::string p = stack.top();
				stack.pop();
				bool truthValue;
				if (p == "True")
					truthValue = true;
				else if (p == "False")
					truthValue = false;
				else
					truthValue = !truthData.getPropositionValue(p);
				if (truthValue)
					stack.push("True");
				else
					stack.push("False");
			}
			else { // Truth Operator except '~' 
				if (stack.empty())
					throw std::runtime_error("Invalid Compound Proposition");
				std::string q = stack.top();
				stack.pop();
				if (stack.empty())
					throw std::runtime_error("Invalid Compound Proposition");
				std::string p = stack.top();
				stack.pop();
				bool p_value;
				if (p == "True")
					p_value = true;
				else if (p == "False")
					p_value = false;
				else
					p_value = truthData.getPropositionValue(p);
				bool q_value;
				if (q == "True")
					q_value = true;
				else if (q == "False")
					q_value = false;
				else
					q_value = truthData.getPropositionValue(q);
				bool truthValue = calcLogicalOperation(p_value, q_value, c);
				if (truthValue)
					stack.push("True");
				else
					stack.push("False");
			}

		}
		if ((int)stack.size() != 1)
			throw std::runtime_error("Invalid Compound Proposition");
		std::string result = stack.top();
		if (result == "True") return true;
		else return false;
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

	int getSize() {
		return truthData.getSize();
	}
	void printTruthMap() {
		truthData.printTruthMap();
	}
	void printAllPropositions() {
		truthData.printAllPropositions();
	}
	static void printAllLogicalOperator() {
		std::cout << "Logical Operators : ";
		for (char op : CompoundPropositionOperator().validOperator) {
			std::cout << op;
			if (op != ']')
				std::cout << ", ";
			else
				std::cout << std::endl;
		}
	}
};

//제곱을 리턴하는 함수
int myPow(int base, int exponent) {
	int result = 1;
	for (int i = 0; i < exponent; ++i)
		result *= base;
	return result;
}





int main()
{
	CompoundPropositionOperator::printAllLogicalOperator();
	//(P&Q)^R 이거 왜 오류 뜨지?
	std::cout << "Example Input : 1. (P&Q)^R	2. ~[C^(A^~D)]&(A>~C)" << std::endl;
	std::cout << "Input Your Compound Proposition here >> ";
	std::string line;
	std::getline(std::cin, line);

	CompoundPropositionOperator myCompoundProposition(line);
	std::cout << "<TRUTH TABLE>" << std::endl;
	myCompoundProposition.printAllPropositions();
	std::cout << "___";
	for (char c : line) {
		if (c == ' ') continue;
		std::cout << c;
	}
	std::cout << std::endl;
	bool truthValueOfCompoundProposition;
	std::string truthValue;
	for (int i = 0; i < pow(2, myCompoundProposition.getSize()); ++i) {
		truthValueOfCompoundProposition = myCompoundProposition.getTruthValueForEachCase(i);
		std::cout << i + 1 << " | ";
		myCompoundProposition.printTruthMap();
		if (truthValueOfCompoundProposition == true) truthValue = "T";
		else truthValue = "F";
		std::cout << " " << truthValue << " " << std::endl;
	}

}

