#pragma once

#include "common.h"

#include <memory>
#include <vector>

// This is a description of a formula that can calculate and update arithmetic expressions with the following supported features:
// - Simple binary operations and numbers, including parentheses: For example, "1+2*3", "2.5*(2+3.5/7)".
// - Cell values are used as variables: For example, "A1+B2*C3".
// The cells mentioned in the formula can contain either formulas or text. 
// If they contain text, but represent a number, they should be treated as numbers. 
// An empty cell or a cell with empty text is interpreted as the number zero.
class FormulaInterface {
public:
    using Value = std::variant<double, FormulaError>;

    virtual ~FormulaInterface() = default;

    // Please note that in the `Evaluate()` method, a reference to the table is passed as an argument. 
    // This method returns the computed value of the formula for the provided table or an error.
    // If the calculation of any cell mentioned in the formula results in an error, that specific error is returned. 
    // If there are multiple such errors, any one of them is returned.
    virtual Value Evaluate(const SheetInterface& sheet) const = 0;

    // This method returns the expression that describes the formula. 
    // It does not contain spaces or unnecessary parentheses.
    virtual std::string GetExpression() const = 0;

   // This method returns a list of cells that are directly involved in the formula's calculation. 
   // The list is sorted in ascending order and does not contain duplicate cells.
    virtual std::vector<Position> GetReferencedCells() const = 0;
};

// Parses the provided expression and returns a formula object. 
// It throws a FormulaException if the formula is syntactically incorrect.
std::unique_ptr<FormulaInterface> ParseFormula(std::string expression);
