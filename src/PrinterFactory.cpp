#include "PrinterFactory.h"
#include "StdOutPrinter.h"

const std::unordered_map<std::type_index, std::function<std::shared_ptr<IPrinter>()>> PrinterFactory::Creators = {
    {typeid(IPrinter::StdOutTag), [](){ return std::make_shared<StdOutPrinter>(); }}
};

std::shared_ptr<IPrinter> PrinterFactory::GetPrinter(const IPrinter::PrinterTag& printer_type)
{
    auto& printer_impl = Printers[typeid(printer_type)];
    auto printer_impl_strong = printer_impl.lock();

    if (!printer_impl_strong)
    {
        printer_impl_strong = Creators.at(typeid(printer_type))();
        printer_impl = printer_impl_strong;
    }
    return printer_impl_strong;
}
