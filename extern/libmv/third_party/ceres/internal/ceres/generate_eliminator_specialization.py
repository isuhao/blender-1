// Ceres Solver - A fast non-linear least squares minimizer
// Copyright 2010, 2011, 2012 Google Inc. All rights reserved.
// http://code.google.com/p/ceres-solver/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of Google Inc. nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
#
# Copyright 2011 Google Inc. All Rights Reserved.
# Author: sameeragarwal@google.com (Sameer Agarwal)
#
# Script for explicitly generating template specialization of the
# SchurEliminator class. It is a rather large class
# and the number of explicit instantiations is also large. Explicitly
# generating these instantiations in separate .cc files breaks the
# compilation into separate compilation unit rather than one large cc
# file which takes 2+GB of RAM to compile.
#
# This script creates two sets of files.
#
# 1. schur_eliminator_x_x_x.cc
# where, the x indicates the template parameters and
#
# 2. schur_eliminator.cc
#
# that contains a factory function for instantiating these classes
# based on runtime parameters.
#
# The list of tuples, specializations indicates the set of
# specializations that is generated.

# Set of template specializations to generate
SPECIALIZATIONS = [(2, 2, 2),
                   (2, 2, 3),
                   (2, 2, 4),
                   (2, 2, "Dynamic"),
                   (2, 3, 3),
                   (2, 3, 4),
                   (2, 3, 9),
                   (2, 3, "Dynamic"),
                   (2, 4, 3),
                   (2, 4, 4),
                   (2, 4, "Dynamic"),
                   (4, 4, 2),
                   (4, 4, 3),
                   (4, 4, 4),
                   (4, 4, "Dynamic"),
                   ("Dynamic", "Dynamic", "Dynamic")]

SPECIALIZATION_FILE = """// Copyright 2011 Google Inc. All Rights Reserved.
// Author: sameeragarwal@google.com (Sameer Agarwal)
//
// Template specialization of SchurEliminator.
//
// ========================================
// THIS FILE IS AUTOGENERATED. DO NOT EDIT.
// THIS FILE IS AUTOGENERATED. DO NOT EDIT.
// THIS FILE IS AUTOGENERATED. DO NOT EDIT.
// THIS FILE IS AUTOGENERATED. DO NOT EDIT.
//=========================================
//
// This file is generated using generate_eliminator_specializations.py.
// Editing it manually is not recommended.

#include "ceres/schur_eliminator_impl.h"
#include "ceres/internal/eigen.h"

namespace ceres {
namespace internal {

template class SchurEliminator<%s, %s, %s>;

}  // namespace internal
}  // namespace ceres

"""

FACTORY_FILE_HEADER = """// Copyright 2011 Google Inc. All Rights Reserved.
// Author: sameeragarwal@google.com (Sameer Agarwal)
//
// ========================================
// THIS FILE IS AUTOGENERATED. DO NOT EDIT.
// THIS FILE IS AUTOGENERATED. DO NOT EDIT.
// THIS FILE IS AUTOGENERATED. DO NOT EDIT.
// THIS FILE IS AUTOGENERATED. DO NOT EDIT.
//=========================================
//
// This file is generated using generate_template_specializations.py.
// Editing it manually is not recommended.

#include "ceres/linear_solver.h"
#include "ceres/schur_eliminator.h"
#include "ceres/internal/eigen.h"

namespace ceres {
namespace internal {

SchurEliminatorBase*
SchurEliminatorBase::Create(const LinearSolver::Options& options) {
#ifndef CERES_RESTRICT_SCHUR_SPECIALIZATION
"""

FACTORY_CONDITIONAL = """  if ((options.row_block_size == %s) &&
      (options.e_block_size == %s) &&
      (options.f_block_size == %s)) {
    return new SchurEliminator<%s, %s, %s>(options);
  }
"""

FACTORY_FOOTER = """
#endif
  VLOG(1) << "Template specializations not found for <"
          << options.row_block_size << ","
          << options.e_block_size << ","
          << options.f_block_size << ">";
  return new SchurEliminator<Dynamic, Dynamic, Dynamic>(options);
}

}  // namespace internal
}  // namespace ceres
"""


def SuffixForSize(size):
  if size == "Dynamic":
    return "d"
  return str(size)


def SpecializationFilename(prefix, row_block_size, e_block_size, f_block_size):
  return "_".join([prefix] + map(SuffixForSize, (row_block_size,
                                                 e_block_size,
                                                 f_block_size)))


def Specialize():
  """
  Generate specialization code and the conditionals to instantiate it.
  """
  f = open("schur_eliminator.cc", "w")
  f.write(FACTORY_FILE_HEADER)

  for row_block_size, e_block_size, f_block_size in SPECIALIZATIONS:
    output = SpecializationFilename("generated/schur_eliminator",
                                    row_block_size,
                                    e_block_size,
                                    f_block_size) + ".cc"
    fptr = open(output, "w")
    fptr.write(SPECIALIZATION_FILE % (row_block_size,
                                      e_block_size,
                                      f_block_size))
    fptr.close()

    f.write(FACTORY_CONDITIONAL % (row_block_size,
                                   e_block_size,
                                   f_block_size,
                                   row_block_size,
                                   e_block_size,
                                   f_block_size))
  f.write(FACTORY_FOOTER)
  f.close()


if __name__ == "__main__":
  Specialize()
