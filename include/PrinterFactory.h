#pragma once

#include "base_interfaces/IPrinter.h"

#include <memory>
#include <unordered_map>
#include <typeindex>

class PrinterFactory
{
private:
    static inline std::unordered_map<std::type_index, std::weak_ptr<IPrinter>> Printers;
    static const std::unordered_map<std::type_index, std::function<std::shared_ptr<IPrinter>()>> Creators;

public:
    static std::shared_ptr<IPrinter> GetPrinter(const IPrinter::PrinterTag& printer_type = IPrinter::StdOutTag{});
};
