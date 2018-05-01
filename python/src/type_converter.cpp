//
//  type_converter.cpp
//  blocksci
//
//  Created by Harry Kalodner on 7/4/17.
//
//

#include "type_converter.hpp"

#include <blocksci/scripts/script_variant.hpp>

#include <pybind11/pybind11.h>

using namespace blocksci;

blocksci::AnyScript BasicTypeConverter::operator()(const blocksci::Address &val) {
    return val.getScript();
}

std::string getTypeName(const char *text, const std::type_info *const *types) {
    std::string signature;
    size_t type_depth = 0, char_index = 0, type_index = 0;
    while (true) {
        char c = text[char_index++];
        if (c == '\0')
            break;

        if (c == '{') {
            ++type_depth;
        } else if (c == '}') {
            --type_depth;
        } else if (c == '%') {
            const std::type_info *t = types[type_index++];
            if (!t)
                pybind11::pybind11_fail("Internal error while parsing type signature (1)");
            if (auto tinfo = pybind11::detail::get_type_info(*t)) {
                pybind11::handle th((PyObject *) tinfo->type);
                signature +=
                    th.attr("__module__").cast<std::string>() + "." +
                    th.attr("__qualname__").cast<std::string>(); // Python 3.3+, but we backport it to earlier versions
            } else {
                std::string tname(t->name());
                pybind11::detail::clean_type_id(tname);
                signature += tname;
            }
        } else {
            signature += c;
        }
    }
    return signature;
}