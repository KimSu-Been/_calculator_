#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <cmath>
using namespace std;

//연산
class Calculator {
public:
	stack<double> valueStack;

	virtual double evaluate(const string& postfixExpression) = 0;
	virtual bool executeOperator(const char& anOperator) = 0;
};

//연산 세부 구현
class PostfixCalculator : public Calculator {
private:
	stack<char> tempStack;

public:
	double evaluate(const string& postfixExpression) {
		if (postfixExpression.size() == 0) {
			cout << "! 수식이 없습니다." << endl;
			throw "error";
		}

		const char* expression = postfixExpression.c_str();
		char token;
		for (int current = 0; current < postfixExpression.size(); current++) {
			token = expression[current];
			if (token == '\0') {
				break;
			}

			if (token == ' ') {
				if (!tempStack.empty()) {
					vector<char> tempValue(this->tempStack.size());
					for (int i = tempValue.size() - 1; i >= 0; i--) {
						tempValue[i] = this->tempStack.top();
						this->tempStack.pop();
					}
					string temp(tempValue.begin(), tempValue.end());
					int tokenvalue = stoi(temp);
					this->valueStack.push(tokenvalue);
				}
				continue;
			}

			if (isdigit(token)) {//숫자인 경우
				this->tempStack.push(token);
			}
			else {//연산자의 경우
				bool check = this->executeOperator(token);
				if (!check) {
					throw "error";
				}
			}
		}

		if (this->valueStack.size() == 1) {
			double result = this->valueStack.top();
			this->valueStack.pop();
			return result;
		}
		else {
			throw "error";
		}

	}


	bool executeOperator(const char& anOperator) {
		if (this->valueStack.size() < 2) {
			throw "error";
		}

		double operand1 = this->valueStack.top();
		this->valueStack.pop();
		double operand2 = this->valueStack.top();
		this->valueStack.pop();

		double calculated = 0;

		switch (anOperator) {
		case '*':
			calculated = operand2 * operand1;
			break;
		case '/':
			if (operand1 == 0) {
				cout << "! 0으로 나눌 수 없습니다." << endl;
				return false;
			}
			else {
				calculated = operand2 / operand1;
			}
			break;
		case '+':
			calculated = operand2 + operand1;
			break;
		case '-':
			calculated = operand2 - operand1;
			break;
		default:
			return false;
			break;
		}

		this->valueStack.push(calculated);
		return true;
	}
};

//변환
class Infix2Postfix {//후위연산으로 변환
public:
	stack<char> operatorStack;
	string infixExpression;
	string postfixExpression;
	PostfixCalculator* postfixCalculator = new PostfixCalculator();

	//infix표현에서 스택에 넣을 때 연산자 우선순위 결정
	int inComingPrecedence(const char& aToken) {
		switch (aToken) {
		case '(':
			return 20;
		case ')':
			return 19;
		case '*':
			return 13;
		case '/':
			return 13;
		case '+':
			return 12;
		case '-':
			return 12;
		default:
			return -1;
		}
	}

	//postfix표현에서 스택에서 꺼낼때 연산자 우선순위 결정
	int inStackPrecedence(const char& aToken) {
		switch (aToken) {
		case '(':
			return 0;
		case ')':
			return 19;
		case '*':
			return 13;
		case '/':
			return 13;
		case '+':
			return 12;
		case '-':
			return 12;
		default:
			return -1;
		}
	}

	virtual bool infixToPostfix() = 0;

	double evaluates(const string& infixExpression) {
		this->infixExpression = infixExpression;
		if (infixExpression.size() == 0) {
			cout << "! 수식이 없습니다." << endl;
			throw "error";
		}

		bool infixChech = this->infixToPostfix();
		if (infixChech) {
			return this->postfixCalculator->evaluate(this->postfixExpression);
		}
	}
};

//변환 세부
class Conversion : public Infix2Postfix {
	//2진수를 10진수로 바꾸기
	string binaryToDecimal(const string& binaryV) {
		const char* binary = binaryV.c_str();
		int resultInt = 0;

		for (int i = 0; i < binaryV.size(); i++) {
			if (binary[i] > '1') {
				cout << "! 잘못된 2진수 입니다." << endl;
				throw "error";
			}
			else {
				int n = binary[i] - '0';
				int k = binaryV.size() - (i + 1);
				resultInt += n * pow(2, k);

			}
		}

		string result = to_string(resultInt);

		return result;
	}

	//16진수를 10진수로 바꾸기
	string hexToDecimal(const string& hexV) {
		const char* hex = hexV.c_str();
		int resultInt = 0;

		for (int i = 0; i < hexV.size(); i++) {
			int k = hexV.size() - (i + 1);
			if (hex[i] >= '0' && hex[i] <= '9') {
				int n = (hex[i] - '0');
				resultInt += n * pow(16, k);
			}
			else if (hex[i] >= 'a' && hex[i] <= 'f') {
				int n = hex[i] - ('a' - 10);
				resultInt += n * pow(16, k);
			}
			else {
				cout << "! 잘못된 16진수 입니다." << endl;
				throw "error";
			}
		}
		string result = to_string(resultInt);

		return result;

	}

	bool infixToPostfix() {
		int sizeV = infixExpression.size();
		vector<char> postfixVector(sizeV);

		char currentToken, poppedToken, topToken;
		const char* expression = infixExpression.c_str();
		int p = 0;
		for (int i = 0; i < infixExpression.size(); i++) {
			currentToken = expression[i];
			if (isdigit(currentToken)) {//숫자일 경우
				if (currentToken == '0') {
					//2진수
					if (i != infixExpression.size() - 1 && expression[i + 1] == 'b') {
						i = i + 2;
						vector<char> binaryVactor(0);
						while (isdigit(expression[i])) {
							int k = binaryVactor.size();
							binaryVactor.resize(++k);
							currentToken = expression[i];
							binaryVactor[k - 1] = currentToken;
							i++;
						}
						string binaryV(binaryVactor.begin(), binaryVactor.end());
						string decimals = binaryToDecimal(binaryV);
						const char* decimal = decimals.c_str();
						for (int j = 0; j < decimals.size(); j++) {
							postfixVector[p++] = decimal[j];
						}
						i--;

					}
					//16진수
					else if (i != infixExpression.size() - 1 && expression[i + 1] == 'x') {
						i = i + 2;
						vector<char> hexVactor(0);
						while ((expression[i] >= 'a' && expression[i] <= 'f') || isdigit(expression[i])) {
							int k = hexVactor.size();
							hexVactor.resize(++k);
							currentToken = expression[i];
							hexVactor[k - 1] = currentToken;
							i++;
						}
						string binaryV(hexVactor.begin(), hexVactor.end());
						string hexs = hexToDecimal(binaryV);
						const char* hex = hexs.c_str();
						for (int j = 0; j < hexs.size(); j++) {
							postfixVector[p++] = hex[j];
						}
						i--;

					}
					else {//0 일반 숫자
						postfixVector[p++] = currentToken;
					}
				}
				else {//일반 숫자
					postfixVector[p++] = currentToken;
				}
			}
			else {//연산자의 경우
				postfixVector.resize(++sizeV);
				postfixVector[p++] = ' ';

				if (currentToken == ')') {//괄호 있을때
					poppedToken = operatorStack.top();
					operatorStack.pop();
					while (!operatorStack.empty() && poppedToken != '(') {
						postfixVector[p++] = poppedToken;
						poppedToken = operatorStack.top();
						operatorStack.pop();
					}

					if (poppedToken == NULL) {
						cout << "! 오른쪽 괄호가 없습니다." << endl;
						return false;
					}
				}
				else {
					int inComing = this->inComingPrecedence(currentToken);
					if (inComing < 0) {
						cout << "! 알 수 없는 연산자입니다." << endl;
						return false;
					}

					if (!operatorStack.empty()) {
						topToken = operatorStack.top();
						while (!operatorStack.empty() && inStackPrecedence(topToken) >= inComing) {
							poppedToken = operatorStack.top();
							operatorStack.pop();
							postfixVector[p++] = poppedToken;
						}
					}

					operatorStack.push(currentToken);
				}
			}
		}

		while (!operatorStack.empty()) {
			poppedToken = operatorStack.top();
			operatorStack.pop();
			if (poppedToken == '(') {
				cout << "! 오른쪽 괄호가 없습니다." << endl;
				return false;
			}
			postfixVector.resize(++sizeV);
			postfixVector[p++] = ' ';
			postfixVector[p++] = poppedToken;
		}
		string temp(postfixVector.begin(), postfixVector.end());
		this->postfixExpression = temp;
		return true;
	}

};


//실행
int main() {

	cout << "계산기를 실행합니다.\n" << endl;
	string infix;
	cout << ">>16진수의 경우, 소문자만 사용합니다." << endl;
	cout << ">>수식을 입력하세요(종료하려면 q를 입력하세요):" << endl;
	getline(cin, infix);

	Infix2Postfix* calculator = new Conversion();
	while (infix != "q") {
		try {
			double result = calculator->evaluates(infix);
			cout << "계산 결과:" << result << endl;
		}
		catch (...) {
			cout << "! 계산 중 오류가 발생하였습니다.\n" << endl;
		}

		cout << "\n>>수식을 입력하세요(종료하려면 q를 입력하세요):" << endl;
		getline(cin, infix);
	}


	cout << "\n계산기를 종료합니다." << endl;
	return 0;
}