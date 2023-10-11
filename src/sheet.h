#pragma once

#include "cell.h"
#include "common.h"
#include <ostream>
#include <functional>

class Sheet : public SheetInterface {
public:
    using Table = std::vector<std::vector<std::unique_ptr<CellInterface>>>;
    ~Sheet();

    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

private:
    using PrintFunction = std::function<void(const std::unique_ptr<CellInterface>&)>;
    Table table_;

    /* Auxiliary functions */
	void OptionalTableResize(Position pos);
    void PrintTable(std::ostream& output, const PrintFunction& print_function) const;
};