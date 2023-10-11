#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>
#include <stack>
using namespace std::literals;

class Cell::Impl {
public:
	enum class CellType {
		EMPTY,
		TEXT,
		FORMULA
	};

	explicit Impl(CellType type)
		:type_(type)
	{
	}

	virtual ~Impl() = default;

	virtual CellInterface::Value GetValue() const = 0;
	virtual std::string GetText() const = 0;

	virtual bool IsCacheValid() const{
		return true;
	}

	virtual void InvalidateCache() {};

	virtual std::vector<Position> GetReferencedCells() const {
		return {};
	};

private:
	CellType type_;
};

class Cell::EmptyImpl : public Impl {
public:
	EmptyImpl() 
	: Impl(Impl::CellType::EMPTY) 
	{
	}

	virtual CellInterface::Value GetValue() const {
		return 0.0;
	}

	virtual std::string GetText() const {
		return "";
	}
};

class Cell::TextImpl : public Impl {
public:
	TextImpl(std::string text)
		: Impl(Impl::CellType::TEXT),
		text_(std::move(text))
	{
	}

	virtual CellInterface::Value GetValue() const {
		return text_.front() == ESCAPE_SIGN ? text_.substr(1) : text_;
	}

	virtual std::string GetText() const {
		return text_;
	}
private:
	std::string text_;
};

class Cell::FormulaImpl : public Impl {
public:
	FormulaImpl(std::string text, SheetInterface& sheet)
		: Impl(Impl::CellType::FORMULA),
		formula_(ParseFormula(text)),
		sheet_(sheet)
	{
	}

	virtual CellInterface::Value GetValue() const {
		if (cache_.has_value()) {
			return *cache_;
		}

		auto evaluation = formula_->Evaluate(sheet_);
		if (std::holds_alternative<double>(evaluation)) {
			cache_ = std::get<double>(evaluation);
		}

		return std::visit([this](auto value) {
			return CellValue(value);
			}, evaluation);
	}

	virtual std::string GetText() const {
		return "=" + formula_->GetExpression();
	}
	virtual bool IsCacheValid() const override {
		return cache_.has_value();
	}

	virtual void InvalidateCache() override {
		cache_.reset();
	}

	virtual std::vector<Position> GetReferencedCells() const override{
		return formula_->GetReferencedCells();
	};

private:
	CellInterface::Value CellValue(double value) const {
		return value;
	}

	CellInterface::Value CellValue(FormulaError fe) const {
		return fe;
	}
	std::unique_ptr<FormulaInterface> formula_;
	SheetInterface& sheet_;
	mutable std::optional<double> cache_;
};


Cell::Cell(SheetInterface& sheet):sheet_(sheet) {}

Cell::~Cell() {}

void Cell::Set(std::string text) {
	std::unique_ptr<Impl> temp_impl;
	if (text.size() > 1 && text.front() == FORMULA_SIGN) {
		temp_impl= std::make_unique<FormulaImpl>(std::move(text.substr(1)), sheet_);
	}
	else if (!text.empty()) {
		temp_impl = std::make_unique<TextImpl>(text);
	}
	else {
		temp_impl = std::make_unique<EmptyImpl>();
	}

	CreateEmptyCells(temp_impl);

	if (std::unordered_set<const Cell*> visited; IsCircularDependent(this, temp_impl, visited)) {
		throw CircularDependencyException("Setting Cell caused circular dependency");
	}

	impl_ = std::move(temp_impl);

	RemoveInvalidLinks();
	AddNewLinks();

	if (IsCacheValid()) {
		InvalidateCache();
		InvalidateDependentCache();
	}
}

void Cell::CreateEmptyCells(const std::unique_ptr<Impl>& impl) {
	for (Position pos : impl->GetReferencedCells()) {
		const Cell* cell = dynamic_cast<const Cell*>(sheet_.GetCell(pos));
		if (!cell) {
			sheet_.SetCell(pos, ""s);
		}
	}
}


void Cell::RemoveInvalidLinks() {
	for (auto* ref_cell : referenced_cells_) {
		ref_cell->dependent_cells_.erase(this);
	}
	referenced_cells_.clear();
}

void Cell::AddNewLinks() {
	for (Position pos : GetReferencedCells()) {
		const Cell* cell = dynamic_cast<const Cell*>(sheet_.GetCell(pos));
		dependent_cells_.insert(cell);
		cell->referenced_cells_.insert(this);
	}

}

void Cell::Clear() {
	impl_.reset();
}

Cell::Value Cell::GetValue() const {
	return impl_->GetValue();
}

std::string Cell::GetText() const {
	return impl_->GetText();
}

std::vector<Position> Cell::GetReferencedCells() const {
	return impl_->GetReferencedCells();
}

bool Cell::IsReferenced() const {
	return dependent_cells_.empty();
}

bool Cell::IsCacheValid() const {
	return impl_->IsCacheValid();
}


void Cell::InvalidateCache() const {
	impl_->InvalidateCache();
}

void Cell::InvalidateDependentCache() const {
	std::unordered_set<const Cell*> visited;
	std::stack<const Cell*> to_visit;
	for (const Cell* cell : dependent_cells_) {
		to_visit.push(cell);
	}

	while (!to_visit.empty()) {
		const Cell* cell = to_visit.top();
		to_visit.pop();
		if (visited.count(cell)) {
			continue;
		}
		visited.insert(cell);
		if (cell->IsCacheValid()) {
			cell->InvalidateCache();
		}
		for (const Cell* dep_cell : cell->dependent_cells_) {
			to_visit.push(dep_cell);
		}
	}
}

bool Cell::IsCircularDependent(const Cell* source, const std::unique_ptr<Impl>& current, 
								std::unordered_set<const Cell*>& visited) const {
	for (Position pos : current->GetReferencedCells()) {
		Cell* cell = dynamic_cast<Cell*>(sheet_.GetCell(pos));
		if (source == cell) {
			return true;
		}
		if (visited.count(cell)) {
			continue;
		}
		visited.insert(cell);
		return cell->IsCircularDependent(source, cell->impl_, visited);
	}
	return false;
}
