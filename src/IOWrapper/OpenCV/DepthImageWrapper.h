/**
 * This file is part of DSO.
 *
 * Copyright 2016 Technical University of Munich and Intel.
 * Developed by Jakob Engel <engelj at in dot tum dot de>,
 * for more information see <http://vision.in.tum.de/dso>.
 * If you use this code, please cite the respective publications as
 * listed on the above website.
 *
 * DSO is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DSO is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DSO. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstdio>
#include <filesystem>

#include "FullSystem/HessianBlocks.h"
#include "IOWrapper/Output3DWrapper.h"
#include "util/MinimalImage.h"

namespace dso::IOWrap {
class DepthImageWrapper : public Output3DWrapper {
public:
  DepthImageWrapper(std::filesystem::path output_directory,
                    std::filesystem::path timestamp_output);
  ~DepthImageWrapper() noexcept;

  void pushDepthImageFloat(MinimalImageF *image, FrameHessian *KF) override;

private:
  std::filesystem::path output_directory_;
  std::FILE *timestamp_output_file_;
  std::size_t count_ = 0;
};
} // namespace dso::IOWrap
