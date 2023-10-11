#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>
#include <regex>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << fe.ToString();
}

namespace {
	double CellValueHandler(std::string text) {
        static const std::regex double_pattern("^(-?)(0|([1-9][0-9]*))(\\.[0-9]+)?$");
        std::smatch double_match;
        if (std::regex_match(text, double_match, double_pattern)) {
            return std::stod(text);
        }

        throw FormulaError(FormulaError::Category::Value);
	}

	double CellValueHandler(double value) {
		return value;
	}

	double CellValueHandler(FormulaError fe) {
		throw fe;
	}
}

namespace {
class Formula : public FormulaInterface {
public:
// Реализуйте следующие методы:
    explicit Formula(std::string expression)
        :ast_(ParseFormulaAST(expression)),referenced_cells_(ast_.GetCells().begin(),ast_.GetCells().end())
    {
        std::sort(referenced_cells_.begin(), referenced_cells_.end());
        referenced_cells_.erase(std::unique(referenced_cells_.begin(), referenced_cells_.end()), 
                                referenced_cells_.end());
    }

    Value Evaluate(const SheetInterface& sheet) const override {
        try {
            auto cell_value = [&sheet](Position pos) {
                if (auto* cell = sheet.GetCell(pos)) {
                    return std::visit([](auto value) {
                        return CellValueHandler(value);
                        }, cell->GetValue());
                }
                return 0.0;
            };
            return ast_.Execute(cell_value);
        }
        catch (const FormulaError& formula_error) {
            return formula_error;
        }
    }
    std::vector<Position> GetReferencedCells() const override {
        return referenced_cells_;
    }

    std::string GetExpression() const override {
        std::ostringstream os;
        ast_.PrintFormula(os);
        return os.str();
    }

private:
    FormulaAST ast_;
    std::vector<Position> referenced_cells_;
};
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    try {
        return std::make_unique<Formula>(std::move(expression));
    } 
    catch(std::exception& e){
        throw FormulaException(e.what());
    }

}