/*
 *  UCF COP3330 Fall 2021 Assignment 6 Solution
 *  Copyright 2021 Dynie Mesoneuvre
 */


#include "std_lib_facilities.h"

//turned struct Token into class
class Token {
    //make code public
    public:
	char kind;
	double value;
	string name;

	//creating tokens for char, double, and string
	Token(char ch) :kind(ch), value(0) { }
	Token(char ch, double val) :kind(ch), value(val) { }
    Token(char ch, string n) :kind(ch), name(n) { }
};

class Token_stream {
	bool full;
	Token buffer;
public:
	Token_stream() :full(0), buffer(0) { }

	Token get();
	void unget(Token t) { buffer = t; full = true; }

	void ignore(char);
};

const char let = 'L';
const char quit = 'Q';
const char con = 'C';
const char print = ';';
const char number = '8';
const char name = 'a';


Token Token_stream::get()
{
	if (full) {full = false;return buffer;}
	char ch;
	cin >> ch;
	switch (ch)
    {
        case '(':
        case ')':
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case ';':
        case '=':
            return Token(ch);
        case '.':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {	cin.unget();
        double val;
        cin >> val;
        return Token(number, val);
        }
         default:


        //if statement to handle underscore case
		if (isalpha(ch) || ch == '_')
		{
			string s;

			s += ch;


			while (cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '_'))

			s += ch;

			cin.unget();

			//consistent returns based on what s is
			if (s == "let") return Token(let);
			if (s == "quit") return Token(quit);
			return Token(name, s);
		}

        //error message for wrong input/mismatch
		error("Bad token");

	}
}

void Token_stream::ignore(char c)
{
	if (full && c == buffer.kind) {
		full = false;
		return;
	}
	full = false;

	char ch;
	while (cin >> ch)
		if (ch == c) return;
}

//turned struct Variable into class
class Variable {
    //make code public
    public:
	string name;
	double value;
	//added boolean to keep track of constants
    bool is_const;
	Variable(string n, double v, bool b) :name(n), value(v), is_const(b) { }
};

vector<Variable> names;

double get_value(string s)
{

	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) return names[i].value;
	    error("get: undefined name ", s);
}


void set_value(string s, double d)
{
 for (int i = 0; i <= names.size(); ++i)
 {

  if (names[i].name == s && names[i].is_const == false)
  {
   names[i].value = d;
   return;
  }
 }

 error("set: undefined name ", s);
}

bool is_declared(string s)
{
 for (int i = 0; i < names.size(); ++i)
 {
  if (names[i].name == s && names[i].is_const == true)
   error("Error, try again");
  else if (names[i].name == s && names[i].is_const == false)
   return true;
 }

 return false;
}

Token_stream ts;

double expression();

double primary()
{
	Token t = ts.get();
	switch (t.kind) {
	case '(':
	{	double d = expression();
	    t = ts.get();
	    if (t.kind != ')') error("'(' expected");
        return d;
	}
	case '-':
		return -primary();
     case '+':
        return primary();
	case number:
		return t.value;
    case name:
        //changed to retrieve token to be used through the rest of the program
    {   Token t2 = ts.get();
        if (t2.kind == '=') {
            double d = expression();
            return d;
        }
        else {
            ts.unget(t2);
        }
    }

	default:
		error("primary expected");
	}

}

double term()
{
	double left = primary();
	while (true) {
		Token t = ts.get();
		switch (t.kind) {
		case '*':
			left *= primary();
			break;
		case '/':
		{	double d = primary();
		if (d == 0) error("divide by zero");
		left /= d;
		break;
		}
		default:
		    //changed putback to unget
			ts.unget(t);
			return left;
		}
	}
}

double expression()
{
	double left = term();
    Token t = ts.get();

	while (true) {
		switch (t.kind) {
		case '+':
			left += term();
            t = ts.get();
			break;
		case '-':
			left -= term();
            t = ts.get();
			break;
		default:
			ts.unget(t);
			return left;
		}
	}
}

double declaration()
{
 int val;
 bool isC;

 Token t = ts.get();

 if (t.kind == 'C')
 {
  isC = true;
  t = ts.get();
 }
 else
  isC = false;

 if (t.kind != 'a')
  error("name expected in declaration");

//checks for duplicated assignments
 string name = t.name;
 if (is_declared(name))
 {
  cout << name + ", inputted twice, would you like to change it? yes or no. ";
  cin.clear();
  cin.ignore(10000, '\n'); \
  string ans;
  getline(cin, ans);
  if (ans == "no" || ans == "No")
   error(name, "remains the same. ");
  if (ans == "yes" ||ans == "Yes"  )
  {
   cout << "New Value: ";
   cin >> val;
   set_value(name, val);
   double d = val;
   return d;

}

}

//if it's missing
 Token t2 = ts.get();
 if (t2.kind != '=')
  error("= missing in declaration of ", name);

 double d = expression();
 names.push_back(Variable(name, d, isC));

 return d;
}

double statement()
{
	Token t = ts.get();
	switch(t.kind) {
	case let:
		return declaration();
    case con:
        return declaration();
	default:
		ts.unget(t);
		return expression();
	}
}

void clean_up_mess()
{
	ts.ignore(print);
}

const string prompt = "> ";
const string result = "= ";

void calculate()
{
	while (true) try {
		cout << prompt;
		Token t = ts.get();
		while (t.kind == print) t = ts.get();
		if (t.kind == quit) return;
		ts.unget(t);
		cout << result << statement() << endl;
	}
	catch (runtime_error& e) {
		cerr << e.what() << endl;
		clean_up_mess();
	}
}

int main()

try {
	calculate();
	return 0;
}
catch (exception& e) {
	cerr << "exception: " << e.what() << endl;
	char c;
	while (cin >> c && c != ';');
	return 1;
}
catch (...) {
	cerr << "exception\n";
	char c;
	while (cin >> c && c != ';');
	return 2;
}
