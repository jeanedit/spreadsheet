#pragma once

#include <iosfwd>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

// Cell position. Zero-based indexing.
struct Position {
    int row = 0;
    int col = 0;

    bool operator==(Position rhs) const;
    bool operator<(Position rhs) const;

    bool IsValid() const;
    std::string ToString() const;

    static Position FromString(std::string_view str);

    static const int MAX_ROWS = 16384;
    static const int MAX_COLS = 16384;
    static const Position NONE;
};

struct Size {
    int rows = 0;
    int cols = 0;

    bool operator==(Size rhs) const;
    Size operator= (Size rhs);
};

// Describes errors that can occur during formula calculations.
class FormulaError {
public:
    enum class Category {
        Ref,    // A link to a cell with an incorrect position
        Value,  // A cell cannot be interpreted as a number
        Div0,  // Division by zero occurred during the calculation
    };

    FormulaError(Category category) 
        :category_(category) {}

    Category GetCategory() const {
        return category_;
    }

    bool operator==(FormulaError rhs) const {
        return category_ == rhs.category_;
    }

    std::string_view ToString() const {
		using namespace std::string_view_literals;
		switch (category_) {
		case Category::Ref:
			return "#REF!"sv;
		case Category::Value:
			return "#VALUE!"sv;
		case Category::Div0:
			return "#DIV/0!"sv;
		}

		return ""sv;
    }

private:
    Category category_;
};

std::ostream& operator<<(std::ostream& output, FormulaError fe);

// An exception thrown when attempting to pass an incorrect position to a method
class InvalidPositionException : public std::out_of_range {
public:
    using std::out_of_range::out_of_range;
};

// An exception thrown when attempting to set a syntactically incorrect formula
class FormulaException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

// An exception thrown when attempting to set a formula that results in a cyclic dependency between cells
class CircularDependencyException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class CellInterface {
public:
    // Either the cell's text, the formula's value, or an error message from the formula
    using Value = std::variant<std::string, double, FormulaError>;

    virtual ~CellInterface() = default;

    virtual void Set(std::string text) = 0;
    // Returns the visible value of the cell. 
    // For a text cell, it's the text (without escape characters). 
    // For a formula, it's the numeric value of the formula or an error message
    virtual Value GetValue() const = 0;
    // Returns the internal text of the cell as if we started editing it. 
    // For a text cell, it's the text (possibly containing escape characters). 
    // For a formula, it's its expression.
    virtual std::string GetText() const = 0;

    // Returns a list of cells that are directly involved in the formula. 
    // The list is sorted in ascending order and does not contain duplicate cells. 
    // For a text cell, the list is empty.
    virtual std::vector<Position> GetReferencedCells() const = 0;
};

inline constexpr char FORMULA_SIGN = '=';
inline constexpr char ESCAPE_SIGN = '\'';

// Table interface
class SheetInterface {
public:
    virtual ~SheetInterface() = default;

//This code snippet describes a method for setting the content of a cell in a table. Here are the key points:
// - If the text starts with "=", it's interpreted as a formula.
// - If the formula is syntactically incorrect, a `FormulaException` is thrown, and the cell's value is not changed.
// - If the formula leads to a circular dependency (e.g., it uses the current cell), a `CircularDependencyException` is thrown, and the cell's value is not changed.
// - There are clarifications about formula notation:
  // - If the text contains only "=", it is not considered a formula.
  // - If the text starts with an apostrophe ('), the apostrophe is omitted when displaying the cell's value using the `GetValue()` method. 
  // This is useful if you want to start text with "=", but you don't want it to be interpreted as a formula.
    virtual void SetCell(Position pos, std::string text) = 0;

    // This method returns the value of the cell. 
    // If the cell is empty, it may return nullptr.
    virtual const CellInterface* GetCell(Position pos) const = 0;
    virtual CellInterface* GetCell(Position pos) = 0;

    // This method clears the cell. 
    // Subsequent calls to GetCell() for this cell will return either nullptr or an object with empty text.
    virtual void ClearCell(Position pos) = 0;

    // This method calculates the size of the area that participates in printing. 
    // It is determined as the bounding rectangle of all cells with non-empty text.
    virtual Size GetPrintableSize() const = 0;

    // This method outputs the entire table to the given stream. 
    // Columns are separated by a tab character, and a newline character is added after each row. 
    // To convert cells to a string, the methods GetValue() or GetText() are used accordingly. 
    // An empty cell is represented as an empty string in either case.
    virtual void PrintValues(std::ostream& output) const = 0;
    virtual void PrintTexts(std::ostream& output) const = 0;
};

// Creates a ready-to-use empty table
std::unique_ptr<SheetInterface> CreateSheet();
