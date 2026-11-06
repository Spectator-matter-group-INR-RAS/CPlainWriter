/*
 * MIT License
 *
 * Copyright (c) 2026 Savva Savenkov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "CPlainWriterFactory.hh"

#include "CPlainWriter.hh"

#include <COLA.hh>

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace cola {

  std::unique_ptr<VFilter> CPlainWriterFactory::Create(const std::unordered_map<std::string, std::string>& param_map) {
    // The output filename is obligatory, matching CRoot's behaviour.
    auto file_it = param_map.find("file_name");
    if (file_it == param_map.end()) {
      throw std::invalid_argument("CPlainWriterFactory: missing obligatory parameter 'file_name'");
    }
    const std::string& filename = file_it->second;

    // 'write_coord' is optional, defaulting to false.
    bool write_coord = false;
    auto coord_it = param_map.find("write_coord");
    if (coord_it != param_map.end()) {
      write_coord = (std::stoi(coord_it->second) != 0);
    }

    return std::make_unique<CPlainWriter>(filename, write_coord);
  }

}  // namespace cola