/*
* Alexei Cioina
* CISP 430 - MW 7:00 p.m.
* Group Project 7 (Expressions)
* October 10, 2014
* It can evaluate multiple expression from a text file. Must be one expression per line.
* Example: ( 502  *  ( 1034  -  2394  +  456 ) * ( 356 - 409 ) )  /  ( ( 50 - 68) - (8029-9047) * 2 ^ 2   )
* The expression can contain spaces and 'big' integer numbers.
* This program can correctly evaluate 2^2^2^2.
* It can handle unary minus: -1 + -2
* It can handle variables: x=12^2
*/
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string.h>
#include <limits>

using namespace std;

const int ARRAY_LENGHT = 3000;
const int UNARY_MINUS_PRIORITY = 5;

typedef struct Digit
{
	char format;
	Digit * prev;
	Digit * next;
};

typedef struct Expression
{
	char * oper;//dinamic string
	long long value;
	Expression * prev;
	Expression * next;
};

static void pushDigit(Digit *&, char);
static Digit popDigit(Digit *&, bool &);
static  Digit* addDigit(Digit*&, char);
static void convertNumberToBase(long long, unsigned char, char *);
static int getRang(char);
static int evaluateExpression(Expression *&, Expression *&, Expression *&, Expression *&,
	char *, char*, char*, const int, const int, long long&);
static int evaluateExpressionsFromFile(ifstream&);
static int readFileSize(ifstream&);
static int readFile(ifstream&, char*, const int);
static int separateCommentLine(char*, const int, const int);
static void stringCopy(char *&, char *);
static int stringLength(char *);
static void enqueue(Expression *&, char *, long long);
static Expression dequeue(Expression *&, bool &);
static void pushOperator(Expression *&, char*);
static Expression popOperator(Expression *&, bool &);
static  Expression* addOperator(Expression*&, char *, long long);
static long long pown_l(long long, unsigned);
static long long compute(char, long long, long long);
static void traverseQueue(Expression *&, char*);
static bool parseNumber(char*, Expression *&, Expression *&, Expression *&);
static void deleteNodeFromHead(Expression *&, Expression *&, const int);
static void clearDoublyLinkedList(Expression *&, Expression *&);
static Expression* FindEqual(Expression *&, char *);
static Expression* FindGreater(Expression *&, char *);
static Expression* addDoublyLinkedNode(Expression*&, char *);

static long long strtolonglong(char *, bool&);
static void testConversion(char *);
static void printNumber(char *);

static void printNumber(char * n)
{
	bool isError;
	long long value = strtolonglong(n, isError);
	if (isError)return;
	cout << endl << n << endl;
	cout << value << endl;
}
static void testConversion(char * filePath)
{
    ifstream file(filePath, ios::in | ios::binary | ios::ate);
	evaluateExpressionsFromFile(file);
}
/*
static void testConversion()
{
	char n[25];
	char *err;
	long long value;
	errno_t result;
	
	bool isError;
	result = strcpy_s(n, sizeof(n), "-9223372036854775809");
	value = strtolonglong(n, isError);
	if (isError)
		cout << "ERROR: " << n << " is not an int64 number" << endl;
	else
		cout << value << endl;
	result = strcpy_s(n, sizeof(n), "9223372036854775808");
	value = strtolonglong(n, isError);
	if (isError)
		cout << "ERROR: " << n << " is not an int64 number" << endl;
	else
		cout << value << endl;
	result = strcpy_s(n, sizeof(n), "--2");
	value = strtolonglong(n, isError);
	if (isError)
		cout << "ERROR: " << n << " is not an int64 number" << endl;
	else
		cout << value << endl;

	cout << "min";
	convertNumberToBase(std::numeric_limits<long long>::min(), 10, n);
	printNumber(n);
	cout << "max";
	convertNumberToBase(std::numeric_limits<long long>::max(), 10, n);
	printNumber(n);

	value = strtoll(n, &err, 10);
	if (err == n || *err != '\0' || errno == ERANGE)
		cout << "ERROR: " << n << " is not an int64 number" << endl;
	else
		cout << value << endl;

	result = strcpy_s(n, sizeof(n), "-9999999999999999999");
	if (result != 0) 
		cout << "Error copying string" << endl;
	else {
		value = strtoll(n, &err, 10);
		if (err == n || *err != '\0' || errno == ERANGE)
			cout << "ERROR: " << n << " is not an int64 number" << endl;
		else
			cout << value << endl;
	}

	// This will NOT trigger an assertion error
	//strncpy_s(small_buffer, sizeof(small_buffer), large_string, _TRUNCATE);
	
	//result = strcpy_s(n, sizeof(n), "-9999999999999999999888888888888");//Buffer is too small
	result = strcpy_s(n, sizeof(n), "abc");
	if (result != 0) 
		cout << "Error copying string" << endl;
	else {
		value = strtoll(n, &err, 10);
		if (err == n || *err != '\0' || errno == ERANGE)
			cout << "ERROR: " << n << " is not an int64 number" << endl;
		else
			cout << value << endl;
	}
}
*/
static long long strtolonglong(char * number, bool &isError)
{
	long long result = 0, l = stringLength(number);
	isError = true;
	if (l == 0 || l > 20)
	{
		cout << "ERROR: the string " << number << " is to big."  << endl;
		return result;
	}
	int i = (number[0] == '-') ? 1 : 0;
	if (l == 1 && i == 1)
	{
		cout << "ERROR: - is not a number" << endl;
		return result;
	}
	long long sign = (number[0] == '-') ? -1 : 1;
	l--;
	long long b = 1;
	result = 0;
	long long prev = -1;
	while (l >= i)
	{
		if (number[l] < 48 || number[l] > 57)
		{
			cout << "ERROR: " << number[l] << " is not a number" << endl;
			return result;
		}
		result += b*(number[l] - 48);
		if (result < prev)
		{
			if (sign > 0)
			{
				cout << "ERROR: " << number << " overflowed" << endl;
				return result;
			}
		}
		prev = result;
		b *= 10;
		l--;
	}

	result = sign*result;
	if (sign < 0 && result > 0)
	{
		cout << "ERROR: " << number << " overflowed" << endl;
		return result;
	}
	isError = false;
	return result;
}
//-----------------------------------------------------------------------------------------------
static Expression* addDoublyLinkedNode(Expression*& next, char * name)
{
	Expression* newNode = new Expression;
	int n = stringLength(name);
	newNode->oper = new char[n + 1];
	stringCopy(newNode->oper, name);
	newNode->next = NULL;
	newNode->prev = NULL;
	next = newNode;
	return next;
}
//-----------------------------------------------------------------------------------------------
static Expression* FindEqual(Expression *&head, char *name)
{
	Expression *curr = head;
	if (head == NULL) return NULL;
	while (curr != NULL)
	{
		if (strcmp(curr->oper, name) == 0) return curr;
		curr = curr->next;
	}
	return NULL;
}
//-----------------------------------------------------------------------------------------------
static Expression* FindGreater(Expression *&head, char *name)
{
	Expression *curr = head;
	if (head == NULL) return NULL;
	while (curr != NULL)
	{
		if (strcmp(curr->oper, name) > 0) return curr;
		curr = curr->next;
	}
	return NULL;
}
//-----------------------------------------------------------------------------------------------
static void deleteNodeFromHead(Expression *&head, Expression *&tail, const int itemAt)
{
	Expression *temp = NULL, *curr = head;

	if (itemAt < 0) return;
	if (head == NULL) return;

	if (itemAt == 0)
	{
		temp = head;
		head = head->next;
		if (head != NULL)
		{
			head->prev = NULL;
			if (head->next != NULL)
			{
				head->next->prev = head;
			}
		}
	}
	else
	{
		int i = 1;
		while ((curr->next != NULL) && (i < itemAt))
		{
			curr = curr->next;
			i++;
		}
		if (curr->next != NULL)
		{
			temp = curr->next;
			curr->next = curr->next->next;
			if (curr->next != NULL)
			{
				curr->next->prev = curr;
			}
		}
	}

	if (temp != NULL)
	{
		if (temp == tail)
		{
			tail = temp->prev;
		}
		delete[] temp->oper;
		delete temp;
	}
}
//----------------------------------------------------------------------------------
static void clearDoublyLinkedList(Expression *&head, Expression *&tail)
{
	while (head != NULL)
	{
		deleteNodeFromHead(head, tail, 0);
	}
}
//----------------------------------------------------------------------------------
static void enqueue(Expression *&head, char * oper, long long value)
{
	Expression  * tmp, *newNode;
	if (head == NULL)
	{
		newNode = addOperator(head, oper, value);
		head->next = newNode;
		head->prev = newNode;
	}
	else
	{
		tmp = head->prev;
		newNode = addOperator(head->prev, oper, value);
		newNode->next = head;
		newNode->prev = tmp;
		tmp->next = newNode;
	}
}
//----------------------------------------------------------------------------------
static Expression dequeue(Expression *&head, bool &isValue)
{
	isValue = false;
	Expression result;
	result.next = NULL;
	result.prev = NULL;
	Expression * tmp, *temp = head;
	if (head == NULL) return result;
	if (head != head->next && head != head->prev)
	{
		tmp = head->prev;
		head = head->next;
		head->prev = tmp;
		tmp->next = head;
	}
	else
	{
		head = NULL;
	}
	isValue = true;
	if (temp->oper)
	{
		result.oper = new char[stringLength(temp->oper) + 1];
		stringCopy(result.oper, temp->oper);
	}
	else
	{
		result.oper = NULL;
	}
	result.value = temp->value;
	delete[] temp->oper;
	delete temp;
	return result;
}
//----------------------------------------------------------------------------------
static void pushOperator(Expression *&head, char * word, long long value)
{
	Expression  * tmp, *newNode;
	if (head == NULL)
	{
		newNode = addOperator(head, word, value);
		head->next = newNode;
		head->prev = newNode;
	}
	else
	{
		tmp = head->prev;
		newNode = addOperator(head->prev, word, value);
		newNode->next = head;
		newNode->prev = tmp;
		tmp->next = newNode;
	}
}
//----------------------------------------------------------------------------------
static Expression popOperator(Expression *&head, bool &isValue)
{
	isValue = false;
	Expression result;
	result.next = NULL;
	result.prev = NULL;
	if (head == NULL) return result;
	Expression * tmp, *temp = head->prev;
	if (head != head->prev)
	{
		tmp = head->prev;
		head->prev = tmp->prev;
		tmp->next = head;
	}
	else
	{
		head = NULL;
	}
	isValue = true;
	if (temp->oper)
	{
		result.oper = new char[stringLength(temp->oper) + 1];
		stringCopy(result.oper, temp->oper);
	}
	else
	{
		result.oper = NULL;
	}
	result.value = temp->value;
	delete[] temp->oper;
	delete temp;
	return result;
}
//----------------------------------------------------------------------------------
static void stringCopy(char *&A, char *B)
{
	int i = 0;
	while (B[i] != NULL)
	{
		A[i] = B[i];
		i++;
	}
	A[i] = NULL;
}
//----------------------------------------------------------------------------------
static int stringLength(char *A)
{
	int i = 0;
	while (A[i] != NULL)
	{
		i++;
	}
	return i;
}
//----------------------------------------------------------------------------------
static  Expression* addOperator(Expression*& next, char * name, long long value)
{
	Expression* newNode = new Expression;
	if (name)
	{
		newNode->oper = new char[stringLength(name) + 1];
		stringCopy(newNode->oper, name);
	}
	else
	{
		newNode->oper = NULL;
	}
	newNode->value = value;
	newNode->next = NULL;
	newNode->prev = NULL;
	next = newNode;
	return next;
}
//----------------------------------------------------------------------------------
static void convertNumberToBase(long long n, unsigned char base, char * formatedNumber)
{
	Digit *head = NULL;
	formatedNumber[0] = NULL;
	if (base < 2 || base > 16)
	{
		cout << "ERROR: base must be beween 2 and 16 inclusive" << endl;
		return;
	}
	formatedNumber[0] = (base == 10) ? ((n < 0) ? '-' : NULL) : NULL;
	int i = (base == 10) ? ((n < 0) ? 1 : 0) : 0;
	unsigned long long number = (base == 10) ? ((n < 0) ? -n : n) : n;
	do
	{
		char mod = number % base;
		pushDigit(head, (mod < 10) ? 48 + mod : 55 + mod);
		number /= base;
	} while (number>0);

	bool isValue;
	do
	{
		Digit node = popDigit(head, isValue);
		if (isValue) formatedNumber[i++] = node.format;
	} while (isValue);
	formatedNumber[i] = NULL;
}
//----------------------------------------------------------------------------------
static void pushDigit(Digit *&head, char word)
{
	Digit  * tmp, *newNode;
	if (head == NULL)
	{
		newNode = addDigit(head, word);
		head->next = newNode;
		head->prev = newNode;
	}
	else
	{
		tmp = head->prev;
		newNode = addDigit(head->prev, word);
		newNode->next = head;
		newNode->prev = tmp;
		tmp->next = newNode;
	}
}
//----------------------------------------------------------------------------------
static Digit popDigit(Digit *&head, bool &isValue)
{
	isValue = false;
	Digit result;
	result.next = NULL;
	result.prev = NULL;
	if (head == NULL) return result;
	Digit * tmp, *temp = head->prev;
	if (head != head->prev)
	{
		tmp = head->prev;
		head->prev = tmp->prev;
		tmp->next = head;
	}
	else
	{
		head = NULL;
	}
	isValue = true;
	result.format = temp->format;
	delete temp;
	return result;
}
//----------------------------------------------------------------------------------
static  Digit* addDigit(Digit*& next, char name)
{
	Digit* newNode = new Digit;
	newNode->format = name;
	newNode->next = NULL;
	newNode->prev = NULL;
	next = newNode;
	return next;
}
//-----------------------------------------------------------------------------------------------
static int evaluateExpressionsFromFile(ifstream& inputFile)
{
	Expression * stackHead, *queueHead, *varHead, *varTail;
	bool isValue;
	char a[ARRAY_LENGHT], word[ARRAY_LENGHT], secondVar[ARRAY_LENGHT];
	int fileSize = 0, currentPos = 0, n, count = 0;
	long long result;
	varHead = varTail = NULL;

	fileSize = readFileSize(inputFile);
	if (fileSize == 0)
	{
		cout << "The file is empty or does not exist." << endl;
		return 0;
	}
	do
	{
		currentPos = readFile(inputFile, a, ARRAY_LENGHT);

		int i = 0;
		while (a[i] != NULL)
		{
			int j = i;
			while ((a[j] != '\n') && (a[j] != NULL))
			{
				j++;
			}

			int k = separateCommentLine(a, i, j);
			if (k < 0)
			{
				n = evaluateExpression(stackHead, queueHead, varHead, varTail, a, word, secondVar, i, j, result);
			}
			else
			{
				n = evaluateExpression(stackHead, queueHead, varHead, varTail, a, word, secondVar, i, k, result);
			}
			if (n < 0)
			{
				do
				{
					popOperator(stackHead, isValue);

				} while (isValue);
				do
				{
					dequeue(queueHead, isValue);

				} while (isValue);
			}
			else
			{
				cout << "Result = " << result << endl;
				count++;
			}
			cout << "=============================================" << endl;

			if (a[j] != NULL)
			{
				i = j + 1;
			}
			else
			{
				i = j;
			}
		}

	} while ((currentPos < fileSize) && (currentPos > 0));

	inputFile.close();
	clearDoublyLinkedList(varHead, varTail);
	return count;
}
//-----------------------------------------------------------------------------------------------
static int getRang(char c)
{
	int result = -1;
	switch (c)
	{
	case '=':  	result = 0;
		break;
	case '(':  	result = 1;
		break;
	case '+':
	case '-':  	result = 2;
		break;
	case '*':
	case '/': 	result = 3;
		break;
	case '^':	result = 4;
		break;
	default:
		break;
	}
	return result;
}
//--------------------------------------------------------------------------------------------
static int evaluateExpression(Expression *& head,
	Expression *&polishNotation,
	Expression *&varHead,
	Expression *&varTail,
	char * charArray,
	char* result,
	char* secondVar,
	const int begin,
	const int end,
	long long& rez)
{
	Expression oper, top;
	char op[2];
	bool isValue, isClosedPar = false;
	int i = begin, n, j, w, k, rang;
	int ch = begin;

	j = n = w = 0;
	head = polishNotation = NULL;
	result[0] = NULL;
	op[1] = NULL;

	while (ch < end)
	{
		cout << charArray[ch];
		ch++;
	}
	cout << endl;

	while (i < end)
	{
		if ((charArray[i] != '\n') && (charArray[i] != '\r') &&
			(charArray[i] != '^') && (charArray[i] != '*') && (charArray[i] != '/') &&
			(charArray[i] != '+') && (charArray[i] != '-') &&
			(charArray[i] != '(') && (charArray[i] != ')') && (charArray[i] != '=') &&
			(charArray[i] != NULL))
		{
			j++;
		}
		else
		{
			if (j > 0)
			{
				w = 0;
				k = i - j;
				while (k <= i)
				{
					if ((charArray[k] != '\n') && (charArray[k] != '\r') &&
						(charArray[k] != '\t') && (charArray[k] != ' ') &&
						(charArray[k] != '^') && (charArray[k] != '*') && (charArray[k] != '/') &&
						(charArray[k] != '+') && (charArray[k] != '-') &&
						(charArray[k] != '(') && (charArray[k] != ')') && (charArray[k] != '=') &&
						(charArray[k] != NULL))
					{
						result[w] = charArray[k];
						result[w + 1] = NULL;
						w++;
					}
					else
					{
						if (w > 0)
						{
							while (k < i)
							{
								result[w] = charArray[k];
								result[w + 1] = NULL;
								w++;
								k++;
							}
							n++;
							w = 0;
						}
					}
					k++;
				}

				k = 0;
				while (result[k] != NULL)
				{
					k++;
				}
				k--;
				while (k >= 0)
				{
					if ((result[k] == '\t') || (result[k] == ' '))
					{
						result[k] = NULL;
					}
					else
					{
						k = 0;
					}
					k--;
				}
				if (result[0])
				{
					if (!parseNumber(result, polishNotation, varHead, varTail))return -1;
				}
				j = 0;
			}
			rang = getRang(charArray[i]);
			if (rang >= 0)
			{
				op[0] = charArray[i];
				if (!isClosedPar && !result[0] && (charArray[i] == '-' || charArray[i] == '+'))
				{
					enqueue(polishNotation, NULL, 0);
					pushOperator(head, op, UNARY_MINUS_PRIORITY);
				}
				else
					if (head == NULL)
					{
					pushOperator(head, op, rang);
					}
					else if (charArray[i] == '(')
					{
						pushOperator(head, op, rang);
					}
					else if (head->prev->value < rang)
					{
						pushOperator(head, op, rang);
					}
					else if (head->prev->oper[0] == '^' &&  charArray[i] == '^')
					{
						pushOperator(head, op, rang);
					}
					else
					{
						do
						{
							oper = popOperator(head, isValue);
							if (isValue)
							{
								enqueue(polishNotation, oper.oper, oper.value);
							}
							if (head)
							{
								if (head->prev->value < rang) isValue = false;
							}

						} while (isValue && head);
						pushOperator(head, op, rang);
					}
					isClosedPar = false;
			}
			else if (charArray[i] != '\r' && charArray[i] != '\n')
			{
				if (charArray[i] != ')')
				{
					cout << "ERROR: unknown operator " << charArray[i] << endl;
					return -1;
				}
				isClosedPar = true;
				bool foundPar = false;
				do
				{
					oper = popOperator(head, isValue);
					if (isValue)
					{
						if (oper.oper)
						{

							if (oper.oper[0] == '(')
							{
								isValue = false;
								foundPar = true;
							}
						}
						if (isValue)
						{
							enqueue(polishNotation, oper.oper, oper.value);
						}

					}
				} while (isValue);
				if (!foundPar)
				{
					cout << "ERROR: open and closed parantheses do not much " << endl;
					return -1;
				}
			}

			result[0] = NULL;
		}
		i++;
	}
	if (j > 0)//This is the last word
	{
		w = 0;
		k = i - j;
		while (k < i)
		{
			if ((charArray[k] != '\n') && (charArray[k] != '\r') &&
				(charArray[k] != '\t') && (charArray[k] != ' ') &&
				(charArray[k] != '^') && (charArray[k] != '*') && (charArray[k] != '/') &&
				(charArray[k] != '+') && (charArray[k] != '-') &&
				(charArray[k] != '(') && (charArray[k] != ')') && (charArray[k] != '=') &&
				(charArray[k] != NULL))
			{
				result[w] = charArray[k];
				result[w + 1] = NULL;
				w++;
			}
			else
			{
				if (w > 0)
				{
					while (k < i)
					{
						result[w] = charArray[k];
						result[w + 1] = NULL;
						w++;
						k++;
					}

					n++;
					w = 0;
				}
			}
			k++;
		}
		if (w > 0)
		{
			n++;
		}
		k = 0;
		while (result[k] != NULL)
		{
			k++;
		}
		k--;
		while (k >= 0)
		{
			if ((result[k] == '\t') || (result[k] == ' '))
			{
				result[k] = NULL;
			}
			else
			{
				k = 0;
			}
			k--;
		}
		if (result[0])
		{
			if (!parseNumber(result, polishNotation, varHead, varTail))return -1;
		}
	}
	do
	{
		oper = popOperator(head, isValue);
		if (isValue) enqueue(polishNotation, oper.oper, oper.value);

	} while (isValue);

	traverseQueue(polishNotation, result);
	if(result[0] != NULL)
		cout << "RPN: " << result << endl;

	if (n == 0)
	{
		if (head || polishNotation)
		{
			cout << "ERROR: incorrect expression" << endl;
		}
		return -1;
	}

	long long op1, op2;
	Expression * found;
	result[0] = NULL;
	secondVar[0] = NULL;
	k = 0;
	do
	{
		oper = dequeue(polishNotation, isValue);
		if (isValue)
		{
			if (oper.oper)
			{
				if (getRang(oper.oper[0]) >= 0)
				{
					if (k == 2)
					{
						if (oper.oper[0] == '=')
						{
							found = FindEqual(varHead, result);
							if (!found)
							{
								cout << "ERROR: invalid operand: NOT FOUND " << oper.oper << endl;
								return -1;
							}
							found->value = op2;
							op1 = op2;
						}
						else	op1 = compute(oper.oper[0], op1, op2);
					}
					else if (head)
					{
						top = popOperator(head, isValue);
						if (isValue)
						{
							if (top.oper)
							{
								if (getRang(top.oper[0]) >= 0)
								{
									cout << "ERROR: invalid operand " << top.oper << endl;
									return -1;
								}
								else
								{
									found = FindEqual(varHead, top.oper);
									if (!found)
									{
										cout << "ERROR: invalid operand: NOT FOUND " << top.oper << endl;
										return -1;
									}
									if (oper.oper[0] == '=')
									{
										found->value = op1;
									}
									else op1 = compute(oper.oper[0], found->value, op1);
								}
							}
							else
							{
								if (oper.oper[0] == '=')
								{
									cout << "ERROR: invalid operand for = " << top.value << endl;
									return -1;
								}
								else op1 = compute(oper.oper[0], top.value, op1);
							}
						}
					}
					else
					{
						cout << "ERROR: no operand for operator " << oper.oper << endl;
						return -1;
					}
					k = 1;
				}
				else
				{
					k++;
					found = FindEqual(varHead, oper.oper);
					if (!found)
					{
						cout << "ERROR: invalid operand: NOT FOUND " << oper.oper << endl;
						return -1;
					}
					if (k == 1)
					{
						stringCopy(result, oper.oper);
						op1 = found->value;
					}
					else if (k == 2)
					{
						stringCopy(secondVar, oper.oper);
						op2 = found->value;
					}
					else
					{
						if (result[0])
						{
							pushOperator(head, result, op1);
							result[0] = NULL;
						}
						else pushOperator(head, NULL, op1);
						op1 = op2;
						if (secondVar[0]) stringCopy(result, secondVar);
						stringCopy(secondVar, oper.oper);
						op2 = oper.value;
						k = 2;
					}

				}
			}
			else
			{
				k++;
				if (k == 1)
				{
					op1 = oper.value;
				}
				else if (k == 2)
				{
					op2 = oper.value;
				}
				else
				{
					if (result[0])
					{
						pushOperator(head, result, op1);
						result[0] = NULL;
					}
					else pushOperator(head, NULL, op1);
					op1 = op2;
					if (secondVar[0])
					{
						stringCopy(result, secondVar);
						secondVar[0] = NULL;
					}
					op2 = oper.value;
					k = 2;
				}
			}
		}
	} while (isValue);

	if (head || k == 2)
	{
		cout << "ERROR: the expression was not fully evaluated " << endl;
		return -1;
	}

	rez = op1;
	return n;
}
//--------------------------------------------------------------------------------------------
static bool parseNumber(char* result, Expression *&polishNotation, Expression *&head, Expression *&tail)
{
	Expression dummy;
	Expression * newNode = &dummy, *tmp, *found;
	char * err;
	long long  value;
	if (result[0] < 48 || result[0] > 57)
	{
		enqueue(polishNotation, result, 0);
		found = FindEqual(head, result);//IMPORTANT:  is case sensitive
		if (found)
		{
			return true;
		}
		if (head == NULL)//this is the first node	
		{
			newNode = addDoublyLinkedNode(newNode->next, result);
			head = newNode;
			tail = newNode;
		}
		else
		{
			found = FindGreater(head, result);//IMPORTANT:  is case sensitive
			if (found == NULL)//here we add a brand new name to the tail
			{
				newNode = addDoublyLinkedNode(tail->next, result);
				newNode->prev = tail;
				tail = newNode;//we have a new tail!
			}
			else if (found == head)//here we insert the name before the head
			{
				newNode = addDoublyLinkedNode(head->prev, result);
				newNode->next = found;
				head = newNode;//we have a new head!
			}
			else//here we insert a name in asccending order between the head and tail
			{
				tmp = found->prev;
				newNode = addDoublyLinkedNode(found->prev, result);
				newNode->next = found;
				newNode->prev = tmp;
				tmp->next = newNode;
			}

		}

		return true;
	}
	bool isError;
	value = strtolonglong(result, isError);
	if (isError)return false;

	enqueue(polishNotation, NULL, value);
	return true;
}
static void traverseQueue(Expression *&head, char* postfixNotation)
{
	Expression *curr = head;
	int i = 0, j;
	char number[65];
	postfixNotation[0] = NULL;
	if (head == NULL) return;

	while (head != curr->next)
	{

		if (curr->oper)
		{
			if (getRang(curr->oper[0]) >= 0)
			{
				postfixNotation[i++] = curr->oper[0];
			}
			else
			{
				j = 0;
				while (curr->oper[j])
				{
					postfixNotation[i] = curr->oper[j];
					i++;
					j++;
				}
				postfixNotation[i++] = ' ';
			}
		}
		else
		{
			convertNumberToBase(curr->value, 10, number);
			j = 0;
			while (number[j])
			{
				postfixNotation[i] = number[j];
				j++;
				i++;
			}
			postfixNotation[i++] = ' ';
		}
		curr = curr->next;
	}
	if (curr->oper)
	{
		postfixNotation[i++] = curr->oper[0];
	}
	else
	{
		convertNumberToBase(curr->value, 10, number);
		int j = 0;
		while (number[j])
		{
			postfixNotation[i] = number[j];
			j++;
			i++;
		}
	}
	postfixNotation[i] = NULL;
}
//----------------------------------------------------------------------------------
static long long compute(char c, long long a, long long b)
{
	long long result = NULL;
	switch (c)
	{
	case '+':	result = a + b;
		break;
	case '-':  	result = a - b;
		break;
	case '*':	result = a * b;
		break;
	case '/': 	result = a / b;
		break;
	case '^':	result = pown_l(a, b);
		break;
	default: cout << "ERROR: wrong operation " << c << endl;
		break;
	}
	return result;
}
//--------------------------------------------------------------------------------------------
static long long pown_l(long long x, unsigned n)
{
	long long  p = 1;
	for (unsigned i = 0; i < n; i++) p *= x;
	return p;
}
//--------------------------------------------------------------------------------------------
static int separateCommentLine(char* charArray, const int begin, const int end)
{
	int i = begin;
	while ((charArray[i] != '/') && (i < end))
	{
		i++;
	}
	if (i == end)
	{
		return -1;
	}
	else if (charArray[i + 1] == '/')
	{
		return i;
	}
	else
	{
		return -1;
	}
}
//--------------------------------------------------------------------------------------------
static int readFileSize(ifstream& inf)
{
	long l, m, size = 0;
	if (inf.is_open())
	{
		inf.seekg(0, ios::beg);
		l = inf.tellg();
		inf.seekg(0, ios::end);
		m = inf.tellg();
		size = (m - l);
		inf.seekg(0, ios::beg);
	}
	return size;
}
//--------------------------------------------------------------------------------------------
static int readFile(ifstream& inf, char* charArray, const int arrayLenght)
{
	long pos = -1;
	if (inf.is_open())
	{
		long l, m, size;
		l = inf.tellg();
		inf.seekg(0, ios::end);
		m = inf.tellg();
		size = m - l;
		inf.seekg(0, ios::beg);
		inf.seekg(l);
		if (size < arrayLenght)
		{
			inf.read(reinterpret_cast<char*>(charArray), size);
			charArray[size] = NULL;
		}
		else
		{
			inf.read(reinterpret_cast<char*>(charArray), (arrayLenght - 1));
			charArray[arrayLenght] = NULL;
			int j = arrayLenght - 1;
			while ((charArray[j] != '\n') && (j > 0))
			{
				j--;
			}
			if (j > 0)
			{
				charArray[j] = NULL;
				inf.seekg(0, ios::beg);
				inf.seekg(l + ((j + 1)));
			}
			else
			{
				if (inf.tellg() > 0)
				{
					cout << "ERROR: The row lenght is biger than " << arrayLenght << " elements." << endl;
					exit(0);
				}
			}
		}
		pos = inf.tellg();
	}
	return (pos);
}

