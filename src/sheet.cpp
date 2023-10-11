#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

Sheet::~Sheet() {}

void Sheet::SetCell(Position pos, std::string text) {
	if (!pos.IsValid()) {
		throw InvalidPositionException("Invalid position"s);
	}

	OptionalTableResize(pos);

	if (!table_[pos.row][pos.col]) {
		table_[pos.row][pos.col] = std::make_unique<Cell>(*this);

	}
	table_[pos.row][pos.col]->Set(std::move(text));
}

const CellInterface* Sheet::GetCell(Position pos) const {
	return const_cast<Sheet*>(this)->GetCell(pos);
}

CellInterface* Sheet::GetCell(Position pos) {
	if (!pos.IsValid()) {
		throw InvalidPositionException("Invalid position"s);
	}

	if (static_cast<size_t>(pos.row) >= table_.size()) {
		return nullptr;
	}

	if (static_cast<size_t>(pos.col) >= table_.at(pos.row).size()) {
		return nullptr;
	}

	return table_[pos.row][pos.col].get();

}

void Sheet::ClearCell(Position pos) {
	if (GetCell(pos)) {
		table_[pos.row][pos.col].reset();
	}
}

Size Sheet::GetPrintableSize() const {
	Size result;
	for (int row = 0; static_cast<size_t>(row) < table_.size(); ++row) {
		for (int col = 0; static_cast<size_t>(col) < table_.at(row).size(); ++col) {
			if (table_[row][col]) {
				result.rows = std::max(result.rows, row + 1);
				result.cols = std::max(result.cols, col + 1);
			}
		}
	}
	return result;
}

void Sheet::PrintValues(std::ostream& output) const {
	auto cell_value = [&output](const std::unique_ptr<CellInterface>& cell) {
		std::visit([&output](const auto& value) {
			output << value;
			}, cell->GetValue());
	};
	PrintTable(output, cell_value);
}

void Sheet::PrintTexts(std::ostream& output) const {
	auto cell_text = [&output](const std::unique_ptr<CellInterface>& cell) {
		output <<  cell->GetText();
	};
	PrintTable(output, cell_text);
}

void Sheet::OptionalTableResize(Position pos) {
	if (static_cast<size_t>(pos.row) >= table_.size()) {
		table_.resize(static_cast<size_t>(pos.row) + 1);
	}

	if (static_cast<size_t>(pos.col) >= table_.at(pos.row).size()) {
		table_[pos.row].resize(static_cast<size_t>(pos.col) + 1);
	}
}

void Sheet::PrintTable(std::ostream& output, const PrintFunction& print_function) const {
	Size printable_size = GetPrintableSize();
	for (int row = 0; row < printable_size.rows; ++row) {
		for (int col = 0; col < printable_size.cols; ++col) {
			if (col != 0) {
				output << '\t';
			}
			if (static_cast<size_t>(col) < table_[row].size() && table_[row][col]) {
				print_function(table_[row][col]);
			}
		}
		output << '\n';
	}
}

std::unique_ptr<SheetInterface> CreateSheet() {
	return std::make_unique<Sheet>();
}