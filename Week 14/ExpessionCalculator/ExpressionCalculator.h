#include <iostream>
#include <exception>
#include "MyString/MyString.h"
#include "MyString/StringView.h"

const char AND = '^';
const char OR = 'v';
const char IMPL = '>'; //=>
const char IFF = '='; // <=>
const char XOR = '+';
const char NEG = '!';

class ExpressionCalculator
{

	class BooleanExpr
	{
	public:
		bool vars[26] = { false }; // which variables are in the expression
		size_t countVars = 0;


		struct BooleanInterpretation
		{
			bool values[26] = { false }; //values assigned to every variable
			BooleanInterpretation()
			{	}
			void setValue(char ch, bool value)
			{
				if (ch < 'A' || ch > 'Z')
					throw "error";

				values[ch - 'A'] = value;
			}
			bool getValue(char ch) const
			{
				if (ch < 'A' || ch > 'Z')
					throw "error";
				return 	values[ch - 'A'];
			}
		};

		virtual bool eval(const BooleanInterpretation& interpret) const = 0;

		virtual BooleanExpr* clone() const = 0;
		virtual ~BooleanExpr() = default;

	};
	class Variable : public BooleanExpr
	{
		char ch;
	public:
		Variable(char ch) : ch(ch)
		{
			vars[ch - 'A'] = true;
			countVars = 1;
		} ///only CAPITALS

		bool eval(const BooleanInterpretation& interpret) const override
		{
			return interpret.getValue(ch);
		}
		BooleanExpr* clone() const override
		{
			return new Variable(*this);
		}

	};

	class BinaryOperation : public BooleanExpr
	{
		BooleanExpr* left;
		char op;
		BooleanExpr* right;
	public:
		BinaryOperation(BooleanExpr* left, char op, BooleanExpr* right) : left(left), op(op), right(right)
		{
			for (int i = 0; i < 26; i++)
			{
				vars[i] = left->vars[i] || right->vars[i];
				if (vars[i])
					countVars++;

			}
		}

		bool eval(const BooleanInterpretation& interpret) const override
		{

			switch (op)
			{
			case OR: {return  left->eval(interpret) || right->eval(interpret); }
			case AND: {return left->eval(interpret) && right->eval(interpret); }
			case IMPL: {return !left->eval(interpret) || right->eval(interpret); }
			case IFF: {bool first = left->eval(interpret); bool second = right->eval(interpret); return first == second; }
			case XOR: {bool first = left->eval(interpret); bool second = right->eval(interpret); return first != second;}

			default: return false;
				break;
			}
		}

		BooleanExpr* clone() const override
		{
			return new BinaryOperation(left->clone(), op, right->clone());
		}



		~BinaryOperation()
		{
			delete left;
			delete right;
		}
	};

	class UnaryOperation : public BooleanExpr
	{
		char ch; //!
		BooleanExpr* expr;

	public:
		UnaryOperation(char ch, BooleanExpr* expr) : ch(ch), expr(expr)
		{
			for (int i = 0; i < 26; i++)
			{
				vars[i] = expr->vars[i];
				if (vars[i])
					countVars++;
			}
		}

		BooleanExpr* clone() const override
		{
			return new UnaryOperation(ch, expr->clone());
		}

		bool eval(const BooleanInterpretation& interpret) const
		{
			if (ch == '!')
			{
				return !expr->eval(interpret);
			}
			else
			{
				// 
				return false;
			}
		}
		~UnaryOperation()
		{
			delete expr;
		}
	};

	BooleanExpr::BooleanInterpretation convertFromNumber(size_t number, bool vars[26]) const;
	bool checkAll(BooleanExpr* expr, bool value) const;

	BooleanExpr* expr;
	void copyFrom(const ExpressionCalculator& other);
	void free();
	
public:
	ExpressionCalculator(const MyString& str);
	ExpressionCalculator(const ExpressionCalculator& other);
	ExpressionCalculator& operator=(const ExpressionCalculator& other);
	~ExpressionCalculator();

	bool isTautology() const;
	bool isContradiction() const;

	friend ExpressionCalculator::BooleanExpr* parse(const StringView& str);

};
