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
#include "CPlainWriter.hh"

#include <EventData.hh>

#include <cstddef>
#include <iomanip>
#include <ios>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>

namespace cola {

  CPlainWriter::CPlainWriter(const std::string& filename, bool write_coord) : write_coord_(write_coord) {
    out_.open(filename, std::ios::out | std::ios::trunc);
    if (!out_.is_open()) {
      throw std::runtime_error("CPlainWriter: cannot open file " + filename);
    }
    out_ << std::scientific << std::setprecision(6);
  }

  CPlainWriter::~CPlainWriter() {
    if (out_.is_open()) {
      out_.close();
    }
  }

  void CPlainWriter::operator()(std::unique_ptr<EventData>&& data) {
    if (!data) {
      return;
    }

    if (!global_header_written_) {
      WriteGlobalHeader(data->ini_state);
      global_header_written_ = true;
    }

    WriteEventHeader(data->ini_state);

    const auto& particles = data->particles;
    for (std::size_t i = 0; i < particles.size(); ++i) {
      WriteParticle(particles[i], i);
    }

    out_ << std::flush;
  }

  // ---------------------------------------------------------------------------
  // Global header: parameters identical for every event.
  // ---------------------------------------------------------------------------
  void CPlainWriter::WriteGlobalHeader(const EventIniState& ini) {
    out_ << "# COLA plain-text output\n";
    out_ << "# ---- Constant run parameters ----\n";
    out_ << "#   projectile_PDG = " << ini.pdg_code_a << "\n";
    out_ << "#   target_PDG     = " << ini.pdg_code_b << "\n";
    out_ << "#   energy[GeV]    = " << ini.energy << "\n";
    out_ << "#   pz_a[GeV]      = " << ini.pz_a << "\n";
    out_ << "#   pz_b[GeV]      = " << ini.pz_b << "\n";
    out_ << "#\n";
    out_ << "# Columns: idx  PDG  status";
    if (write_coord_) {
      out_ << "  E[GeV]  px[GeV]  py[GeV]  pz[GeV]"
              "  x[fm]  y[fm]  z[fm]  t[fm/c]";
    }
    out_ << "\n";
  }

  // ---------------------------------------------------------------------------
  // Per-event header: parameters that change event by event.
  // ---------------------------------------------------------------------------
  void CPlainWriter::WriteEventHeader(const EventIniState& ini) {
    out_ << "\n# ---- Event ----\n";
    out_ << "#   b[fm]             = " << ini.b << "\n";
    out_ << "#   npart             = " << ini.num_part << "\n";
    out_ << "#   ncoll             = " << ini.num_coll << "\n";
    out_ << "#   cross_section[mb] = " << ini.sect_nn << "\n";
    out_ << "#\n";
  }

  void CPlainWriter::WriteParticle(const Particle& particle, std::size_t index) {
    out_ << std::setw(6) << index << std::setw(11) << particle.pdg_code << std::setw(2)
         << static_cast<int>(particle.p_class);

    if (write_coord_) {
      const auto& mom = particle.momentum;
      const auto& pos = particle.position;
      out_ << std::setw(14) << mom.e << std::setw(14) << mom.x << std::setw(14) << mom.y << std::setw(14) << mom.z
           << std::setw(14) << pos.x << std::setw(14) << pos.y << std::setw(14) << pos.z << std::setw(14)
           << pos.e;  // e is the time component
    }
    out_ << "\n";
  }

}  // namespace cola