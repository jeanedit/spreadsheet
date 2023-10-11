#pragma once

#include "common.h"
#include "formula.h"

#include <functional>
#include <unordered_set>

class Sheet;

class Cell : public CellInterface {
public:
    Cell(SheetInterface& sheet);
    ~Cell();

    void Set(std::string text) override;
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;

    bool IsReferenced() const;

private:
    class Impl;
    class EmptyImpl;
    class TextImpl;
    class FormulaImpl;

    SheetInterface& sheet_;
    std::unique_ptr<Impl> impl_;

    mutable std::unordered_set<const Cell*> dependent_cells_;
    mutable std::unordered_set<const Cell*> referenced_cells_;

    /* functions */
    bool IsCircularDependent(const Cell* source, const std::unique_ptr<Impl>& current, std::unordered_set<const Cell*>& visited) const;
    bool IsCacheValid() const;
    void InvalidateCache() const;
    void InvalidateDependentCache() const;
    void CreateEmptyCells(const std::unique_ptr<Impl>& impl);
    void RemoveInvalidLinks();
    void AddNewLinks();
  };
