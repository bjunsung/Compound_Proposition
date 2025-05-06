#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <iomanip>

//단순명제 class
class SimpleProposition {
public:
	static bool calcLogicalNot(bool p) { return !p; }
	static bool calcLogicalAnd(bool p, bool q) { return p && q; }
	static bool calcLogicalOr(bool p, bool q) { return p || q; }
	static bool calcLogicalImplies(bool p, bool q) { return !(p && !q); }
	static bool calcLogicalEquivalent(bool p, bool q) { return p == q; }
	static std::string converter(char logicalConnective, bool needExplan) {
		switch (logicalConnective) {
		case '~': return needExplan ? "~ (negation)" : "~";
		case '&': return needExplan ? "· (conjunction)" : "·";
		case 'V': return needExplan ? "V (disjunction)" : "V";
		case '>': return needExplan ? "-> (material implication)" : "->";
		case '<': return needExplan ? "<-> (biconditional)" : "<->";
		default: return std::string(1, logicalConnective);
		}
	}
};



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

	void setKeys() {
		for (const auto& pair : truthMap)
			keys.push_back(pair.first);
	}

	void setPropositionValueByStep(int num) {
		int size = (int)keys.size();
		for (int i = 0; i < size; ++i)
			truthMap[keys[i]] = num / (int)pow(2.0, (double)size - (i + 1)) % 2 == 0;
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
				std::cout << "idx__" << key;
			else
				std::cout << "___" << key;
		}
	}

	int getKeySize() {
		return (int)keys.size();
	}

	bool getPropositionValue(std::string p) {
		if (p == "True")
			return true;
		else if (p == "False")
			return false;
		else
			return truthMap.at(p);
	}
};


class ComplexPropEvaluator {
private:
	std::vector<char> postfix;
	TruthAssignment* truthData;

	std::string pop(std::stack<std::string>& stack) {
		if (stack.empty())
			throw std::runtime_error("Invalid Compound Proposition");
		std::string s = stack.top();
		stack.pop();
		return s;
	}

	bool evaluateSimplePropositon(bool p, char op) {
		return SimpleProposition::calcLogicalNot(p);
	}

	bool evaluateSimplePropositon(bool p, bool q, char op) {
		switch (op) {
		case '&': return SimpleProposition::calcLogicalAnd(p, q);
		case 'V': return SimpleProposition::calcLogicalOr(p, q);
		case '>': return SimpleProposition::calcLogicalImplies(p, q);
		case '<': return SimpleProposition::calcLogicalEquivalent(p, q);
		default: throw std::runtime_error(std::string("Invalid Logical Connective : ") + op); //" "는 const char*임 -> " " + char 는 포인터 + 정수로 해석, std::string()을 사용해서 객체로 변환해줌 
		}
	}

public:
	ComplexPropEvaluator() {}
	ComplexPropEvaluator(std::vector<char> postfix, TruthAssignment* truthData){
		this->postfix = postfix;
		this->truthData = truthData;
	}

	bool calc_postfix() {
		std::stack<std::string> evalStack;  //단순명제나 그 명제들의 연산 결과를 string type으로 stack에 넣음
		for (char token : postfix) {
			if (token != 'V' && ('A' <= token && token <= 'Z') || ('a' <= token && token <= 'z')) {
				std::string s(1, token);
				evalStack.push(s);
			}
			else if (token == '~') { // case Logical Connective is '~' 
				std::string p = pop(evalStack);
				bool truthValue = truthData->getPropositionValue(p);
				bool evaluatedTruthValue = evaluateSimplePropositon(truthValue, '~');
				evalStack.push(evaluatedTruthValue ? "True" : "False");
			}
			else { // case Logical Connective except '~' 
				std::string q = pop(evalStack);
				std::string p = pop(evalStack);
				bool p_value = truthData->getPropositionValue(p);
				bool q_value = truthData->getPropositionValue(q);
				bool evaluatedTruthValue = evaluateSimplePropositon(p_value, q_value, token);
				evalStack.push(evaluatedTruthValue ? "True" : "False");
			}
		}
		if ((int)evalStack.size() != 1)
			throw std::runtime_error("Invalid Complex Proposition");
		std::string result = evalStack.top();
		return result == "True";
	}

	char getMajorLogicalConnective() {
		return postfix.back();
	}
};

//복합명제 class
class ComplexProposition {
private:
	std::vector<char> postfix;
	std::stack<char> connective_statck;
	TruthAssignment truthData;
	ComplexPropEvaluator evaluator;
	//valid logical connective : '~' (negation), '&' means '·'(conjunction), 'V' Big alphavet V means '∨'(disjunction), '>' means "->", '→'(material implication), '<' means "<->", '↔' 
	const static char validConnective[11];
	bool isValidConnective(char op) {
		for (char sample : validConnective)
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
	void popStackTilLeftBracket(char bracketRight) {
		char bracketLeft = ' ';
		switch (bracketRight) {
		case ')': bracketLeft = '('; break;
		case '}': bracketLeft = '{'; break;
		case ']': bracketLeft = '[';
		}
		while (true) {
			if (connective_statck.empty()) {
				std::string brkLeft(1, bracketLeft);
				std::string brkRight(1, bracketRight);
				throw std::runtime_error("Bracket Mismatch Error : matching '" + brkLeft + "' not found for '" + brkRight + "'");
			}
			char op = connective_statck.top();
			connective_statck.pop();
			if (op == bracketLeft)
				break;
			else
				postfix.push_back(op);
		}
		if (!connective_statck.empty() && connective_statck.top() == '~') {
			postfix.push_back('~');
			connective_statck.pop();
		}
	}
	void popStackTilEmpty() {
		while (!connective_statck.empty()) {
			char op = connective_statck.top();
			connective_statck.pop();
			postfix.push_back(op);
		}
	}
	bool getTruthValueForEachCase(int i) {
		truthData.setPropositionValueByStep(i);
		return evaluator.calc_postfix();
	}
	void infixToPostfix(std::string inputLine)
	{
		for (char token : inputLine) {
			if (token == ' ') continue;
			else if (token != 'V' && ('A' <= token && token <= 'Z') || ('a' <= token && token <= 'z')) {
				truthData.addProposition(token);
				postfix.push_back(token);
				if (!connective_statck.empty() && connective_statck.top() == '~') {
					postfix.push_back('~');
					connective_statck.pop();
				}
			}
			else if (!isValidConnective(token)) {
				std::string str_token(1, token);
				throw std::runtime_error("Invalid Connective Detected : " + str_token);
			}
			else if (token == ']' || token == '}' || token == ')') {
				popStackTilLeftBracket(token);
			}
			else
				connective_statck.push(token);
		}
		popStackTilEmpty();
		truthData.setKeys();
		evaluator = ComplexPropEvaluator(postfix, &truthData);
	}

public:
	ComplexProposition() {}
	ComplexProposition(std::string inputLine) {
		
		inputLine = inputLinePreprocessing(inputLine);
		infixToPostfix(inputLine);
	}
	static void printAllValidConnective() {
		std::cout << "Logical Connections : ";
		for (char op : validConnective) {
			if (op == ']')
				std::cout << op;
			else
				std::cout << SimpleProposition::converter(op, true) << ", ";
		}
		std::cout << std::endl;
	}
	void printTruthTable(std::string line) {
		std::cout << "*  *  *  *  *" << std::endl << "<TRUTH TABLE>" << std::endl << "*  *  *  *  *" << std::endl;
		truthData.printAllPropositions();
		std::cout << "___" << line << std::endl;
		for (int i = 0; i < pow(2, truthData.getKeySize()); ++i) {
			bool truthValueOfCompoundProposition = getTruthValueForEachCase(i); 
			std::cout << std::setw(2) << i + 1 << " | ";
			truthData.printTruthValue();
			std::cout << " " << (truthValueOfCompoundProposition ? "T" : "F") << " " << std::endl;
		}
		std::cout << "Major Logical Connective : " << SimpleProposition::converter(evaluator.getMajorLogicalConnective(), true) << std::endl;
		std::cout << "*  *  *  *  *" << std::endl;
	}
	const static std::string ExampleComplexProposition[3];
};

const char ComplexProposition::validConnective[11] = { '~', '&', 'V', '>', '<', '[', '{', '(', ')', '}', ']' };
const std::string ComplexProposition::ExampleComplexProposition[3] = { "(P·Q)VR", "~[CV(AV~D)]·(A->~C)", "P<->Q" };

void printTruthTable()
{
	ComplexProposition::printAllValidConnective();
	std::cout << "Example Input : 1. (P·Q)VR\t2. ~[CV(AV~D)]·(A->~C)\t3. P<->Q" << std::endl;
	std::cout << "Input Your Complex Proposition here(you can type a number 1, 2, or 3 to use the examples above)>> ";
	std::string line;
	std::getline(std::cin, line);
	try {
		int ex = std::stoi(line);
		if (ex < 1 || ex > 3) throw std::runtime_error("Example number must be 1, 2, or 3");
		line = ComplexProposition::ExampleComplexProposition[ex - 1];
	}
	catch (std::invalid_argument) {}
	catch (std::out_of_range) {}
	ComplexProposition myCompoundProposition(line);
	myCompoundProposition.printTruthTable(line);
}

int main()
{
	while (true)
	{
		int n;
		std::cout << "input number here(1.print truth table 2. stop)>>";
		std::cin >> n;
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //입력버퍼 지우기
		if (n == 1) {
			try { printTruthTable(); }
			catch (const std::runtime_error& e) {
				std::cout << e.what() << std::endl;
			}
		}
		else break;
	}
}


