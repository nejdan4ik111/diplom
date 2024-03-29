#include "sheet.h"
#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

Sheet::~Sheet() {}

// Устанавливает текст в ячейку по указанной позиции.
void Sheet::SetCell(Position pos, std::string text) {
    if (!pos.IsValid()) throw InvalidPositionException("Invalid position");
    
    const auto& cell = cells_.find(pos);

    // Если ячейки с указанной позицией не существует, добавляем ее в хранилище.
    if (cell == cells_.end()) {
        cells_.emplace(pos, std::make_unique<Cell>(*this));
    }

    cells_.at(pos)->Set(std::move(text));
}

// Возвращает константный указатель на ячейку по указанной позиции.
const CellInterface* Sheet::GetCell(Position pos) const {
    return GetCellPtr(pos);
}

// Возвращает указатель на ячейку по указанной позиции.
CellInterface* Sheet::GetCell(Position pos) {
    return GetCellPtr(pos);
}

// Очищает содержимое ячейки по указанной позиции.
void Sheet::ClearCell(Position pos) {
    if (!pos.IsValid()) throw InvalidPositionException("Invalid position");

    const auto& cell = cells_.find(pos);
    if (cell != cells_.end() && cell->second != nullptr) {
        cell->second->Clear();

        // Если ячейка больше не используется другими ячейками, освобождаем память.
        if (!cell->second->IsReferenced()) {
            cell->second.reset();
        }
    }
}

// Возвращает размер таблицы, т.е. количество строк и столбцов, образованных непустыми ячейками.
Size Sheet::GetPrintableSize() const {
    Size result{ 0, 0 };
    
    for (auto it = cells_.begin(); it != cells_.end(); ++it) {
        if (it->second != nullptr) {
            const int col = it->first.col;
            const int row = it->first.row;
            result.rows = std::max(result.rows, row + 1);
            result.cols = std::max(result.cols, col + 1);
        }
    }

    return { result.rows, result.cols };
}

// Печатает значения ячеек в указанный поток вывода.
void Sheet::PrintValues(std::ostream& output) const {
    Size size = GetPrintableSize();
    for (int row = 0; row < size.rows; ++row) {
        for (int col = 0; col < size.cols; ++col) {
            if (col > 0) output << "\t";
            const auto& it = cells_.find({ row, col });
            if (it != cells_.end() && it->second != nullptr && !it->second->GetText().empty()) {
                // Выводим значение ячейки разными вариантами в зависимости от типа значения.
                std::visit([&](const auto value) { output << value; }, it->second->GetValue());
            }
        }
        output << "\n";
    }
}

// Печатает тексты ячеек в указанный поток вывода.
void Sheet::PrintTexts(std::ostream& output) const {
    Size size = GetPrintableSize();
    for (int row = 0; row < size.rows; ++row) {
        for (int col = 0; col < size.cols; ++col) {
            if (col > 0) output << "\t";
            const auto& it = cells_.find({ row, col });
            if (it != cells_.end() && it->second != nullptr && !it->second->GetText().empty()) {
                output << it->second->GetText();
            }
        }
        output << "\n";
    }
}

// Возвращает константный указатель на ячейку по указанной позиции.
const Cell* Sheet::GetCellPtr(Position pos) const {
    if (!pos.IsValid()) throw InvalidPositionException("Invalid position");

    const auto cell = cells_.find(pos);
    if (cell == cells_.end()) {
        return nullptr;
    }

    return cells_.at(pos).get();
}

// Возвращает указатель на ячейку по указанной позиции.
Cell* Sheet::GetCellPtr(Position pos) {
    return const_cast<Cell*>(
        static_cast<const Sheet&>(*this).GetCellPtr(pos));
}

// Создает экземпляр класса Sheet.
std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}