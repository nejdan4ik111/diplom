/*#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

// Класс представляет ошибку формулы
class FormulaError::FormulaError(Category category) 
    : category_(category) {}

// Метод возвращает категорию ошибки формулы
FormulaError::Category FormulaError::GetCategory() const {
    return category_;
}

// Оператор сравнения ошибок формулы
bool FormulaError::operator==(FormulaError rhs) const {
    return category_ == rhs.category_;
}

// Метод возвращает строковое представление ошибки формулы
std::string_view FormulaError::ToString() const {
    switch (category_) {
        case Category::Ref:
            return "#REF!";
        case Category::Value:
            return "#VALUE!";
        case Category::Div0:
            return "#DIV/0!";
    }
    return "";
}

// Оператор вывода ошибки формулы в поток
std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << fe.ToString();
}

namespace {

// Класс, представляющий формулу
class Formula : public FormulaInterface {
public:
    // Конструктор принимает выражение формулы
    explicit Formula(std::string expression)
        try : ast_(ParseFormulaAST(expression)) {}
        catch (const std::exception& e) {
            std::throw_with_nested(FormulaException(e.what()));
    }

    // Метод вычисляет значение формулы
    Value Evaluate(const SheetInterface& sheet) const override {
        // Функция для получения значения ячейки
        const std::function<double(Position)> args = [&sheet](const Position p)->double {
            if (!p.IsValid()) throw FormulaError(FormulaError::Category::Ref);

            const auto* cell = sheet.GetCell(p);
            if (!cell) return 0;
            if (std::holds_alternative<double>(cell->GetValue())) return std::get<double>(cell->GetValue());
            if (std::holds_alternative<std::string>(cell->GetValue())) {
                auto value = std::get<std::string>(cell->GetValue());
                double result = 0;
                if (!value.empty()) {
                    std::istringstream in(value);
                    if (!(in >> result) || !in.eof()) throw FormulaError(FormulaError::Category::Value);
                }
                return result;
            }
            throw FormulaError(std::get<FormulaError>(cell->GetValue()));
        };

        try {
            return ast_.Execute(args);
        }
        catch (FormulaError& e) {
            return e;
        }
    }
    
    // Метод возвращает список позиций ячеек, на которые ссылается формула
    std::vector<Position> GetReferencedCells() const override {
        std::vector<Position> cells;
        for (auto cell : ast_.GetCells()) {
            if (cell.IsValid()) cells.push_back(cell);
        }
        cells.resize(std::unique(cells.begin(), cells.end()) - cells.begin());
        return cells;
    }

    // Метод возвращает строковое представление формулы
    std::string GetExpression() const override {
        std::ostringstream out;
        ast_.PrintFormula(out);
        return out.str();
    }

private:
    const FormulaAST ast_;
};

}  // namespace

// Функция разбирает выражение и возвращает указатель на объект формулы
std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    try {
        return std::make_unique<Formula>(std::move(expression));
    }
    catch (...) {
        throw FormulaException("");
    }
}*/
#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

FormulaError::FormulaError(Category category) 
    : category_(category) {}

FormulaError::Category FormulaError::GetCategory() const {
    return category_;
}

bool FormulaError::operator==(FormulaError rhs) const {
    return category_ == rhs.category_;
}

std::string_view FormulaError::ToString() const {
    switch (category_) {
        case Category::Ref:
            return "#REF!";
        case Category::Value:
            return "#VALUE!";
        case Category::Div0:
            return "#DIV/0!";
        }
    return "";
}

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << fe.ToString();
}

namespace {

class Formula : public FormulaInterface {
public:
    explicit Formula(std::string expression)
        try : ast_(ParseFormulaAST(expression)) {}
        catch (const std::exception& e) {
            std::throw_with_nested(FormulaException(e.what()));
    }

    Value Evaluate(const SheetInterface& sheet) const override {
        const std::function<double(Position)> args = [&sheet](const Position p)->double {
            if (!p.IsValid()) throw FormulaError(FormulaError::Category::Ref);

            const auto* cell = sheet.GetCell(p);
            if (!cell) return 0;
            if (std::holds_alternative<double>(cell->GetValue())) return std::get<double>(cell->GetValue());
            if (std::holds_alternative<std::string>(cell->GetValue())) {
                auto value = std::get<std::string>(cell->GetValue());
                double result = 0;
                if (!value.empty()) {
                    std::istringstream in(value);
                    if (!(in >> result) || !in.eof()) throw FormulaError(FormulaError::Category::Value);
                }
                return result;
            }
            throw FormulaError(std::get<FormulaError>(cell->GetValue()));
        };

        try {
            return ast_.Execute(args);
        }
        catch (FormulaError& e) {
            return e;
        }
    }
    
    std::vector<Position> GetReferencedCells() const override {
        std::vector<Position> cells;
        for (auto cell : ast_.GetCells()) {
            if (cell.IsValid()) cells.push_back(cell);
        }
        cells.resize(std::unique(cells.begin(), cells.end()) - cells.begin());
        return cells;
    }

    std::string GetExpression() const override {
        std::ostringstream out;
        ast_.PrintFormula(out);
        return out.str();
    }

private:
    const FormulaAST ast_;
};

}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    try {
        return std::make_unique<Formula>(std::move(expression));
    }
    catch (...) {
        throw FormulaException("");
    }
}